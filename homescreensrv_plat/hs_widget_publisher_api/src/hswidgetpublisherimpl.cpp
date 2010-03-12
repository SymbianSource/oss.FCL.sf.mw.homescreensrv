/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: CPSApiDll.cpp - main DLL source
*
*/

#include <utf.h>

#include "hswidgetpublisherimpl.h"
#include "hswidget.h"
#include "hswidgetitem.h"
#include "hsglobals.h"
#include "hsexception.h"
#include "hsserviceerrno.h"

const TInt KSAPIContentNameMaxLength = 255;
const TInt KUnicodeSize = 4;

using namespace Hs;
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
HsWidgetPublisherImpl::HsWidgetPublisherImpl()
	{
	mServiceInterface = 0;
	TInt err;
	TRAP( err,
		mServiceHandler = CLiwServiceHandler::NewL();
	    )
	if( err != KErrNone )
		{
		throw HsException( err );
		}
    RProcess process;
    iWidgetUid = process.Identity().iUid;
    process.Close();
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
HsWidgetPublisherImpl::~HsWidgetPublisherImpl()
	{
   	TRAP_IGNORE( UnegisterNotificationL() );

    if( mServiceInterface )
    	{
    	mServiceInterface->Close();
    	}
   	if( mServiceHandler )
    	{
    	mServiceHandler->Reset();
    	delete mServiceHandler;
    	}
   	
   	const int count = mWidgets.size();
    for( int index = 0; index < count; index++ )
        {
        HsWidget* const widget = mWidgets.at( index );
        mWidgets[index] = NULL;
        delete widget;
        }
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HsWidget& HsWidgetPublisherImpl::CreateWidget( std::string& aTemplateName, 
	std::string& aWidgetName, std::string& aIdentifier,
	std::string& aDescription, std::string& aIconLocation)
	{
	if( CheckIfWidgetExist( aTemplateName, aWidgetName, aIdentifier, EFalse ) )
		{
		throw HsException( KErrAlreadyExists );
		}
	HsWidget* widget= new HsWidget( aTemplateName, aWidgetName, 
		aIdentifier, aDescription, aIconLocation );
	if( !widget)
		{
		throw HsException( KErrNoMemory );
		}
	mWidgets.push_back( widget);
	TInt err( KErrNone );
	TRAP( err, 
		PublishWidgetActionsL( *widget );
        RegisterNotificationL( *widget );
		)
	if( err != KErrNone )
		{
		throw HsException( err );
		}
	
	return *widget;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt HsWidgetPublisherImpl::HandleNotifyL(
    TInt /*aCmdId*/,
    TInt /*aEventId*/,
    CLiwGenericParamList& aEventParamList,
    const CLiwGenericParamList& /*aInParamList*/)
	{
	if( !mHsDataObserver )
		 {
		 return 1;
		 }
	
	TInt pos(0);	
    aEventParamList.FindFirst(pos, KChangeInfo);
    if (pos != KErrNotFound)
        {
        // Get list of maps
        TLiwVariant variant = (aEventParamList)[pos].Value();
        variant.PushL();
        const CLiwList* changeMapsList = variant.AsList();
        
		TBuf8<KSAPIContentNameMaxLength> operation;
		TBuf8<KSAPIContentNameMaxLength> trigger;
        // Iter through list content
        for (TInt index = 0; index < changeMapsList->Count(); ++index)
            {
             if ( changeMapsList->AtL(index, variant) ) 
            	 {
                 const CLiwMap* map  = variant.AsMap();
                 HsWidget* widget = GetWidgetL( map );
                 if( !widget )
                	 {
                	 break;
                	 }
                 if ( map->FindL(KOperation, variant) )
                     {// Check what triggered a notification
                     variant.Get(operation);
                     }
                 variant.Reset();
                 if ( operation == KOperationExecute )
                	 {
                	 if( map->FindL( KActionTrigger, variant) )
						 {
						 variant.Get(trigger );
						 }
                	 if( !HandleWidgetActionL( trigger, *widget ) )
                		 {
                		 break;
                		 }
                	 HandleWidgetItemActionL( trigger, *widget );
                	 }
                 }
            }
        CleanupStack::PopAndDestroy(1);
        }
    return 0;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
int HsWidgetPublisherImpl::HandleWidgetActionL( const TDesC8& aActionDes,
	HsWidget& aWidget )
	{
	int ret( 1 );//not found
	IHsDataObserver::EEvent action( IHsDataObserver::EUnknown );
	TranslateObserverAction( aActionDes, action );
	if( action == IHsDataObserver::EActivate ||
		action == IHsDataObserver::EDeactivate ||
		action == IHsDataObserver::EResume ||
		action == IHsDataObserver::ESuspend )
		{
		TInt observerError = KErrNone;
		try
			{
			mHsDataObserver->handleEvent(
				aWidget.getWidgetName(), action );
			}
		catch( HsException& exception )
			{
			observerError = exception.getReason();
			}
		
		if ( action == IHsDataObserver::EDeactivate )
		    {
		    // remove widget data when widget removed from screen
		    CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();
		    InsertWidgetDataIdentifiersL( aWidget, cpdatamap, KAll );
		    // removal may fail if the client has already removed the data
		    TRAP_IGNORE( RemoveFromCpsL( cpdatamap, KCpData ) );
		    mWidgetContentIds.erase( aWidget.getIdentifier() );
		    CleanupStack::PopAndDestroy( cpdatamap );
		    }
		
		User::LeaveIfError( observerError );
		ret = 0;
		}
	return ret;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HsWidgetPublisherImpl::TranslateObserverAction( const TDesC8& aActionDes,
	IHsDataObserver::EEvent& aAction )
	{
	if( !aActionDes.Compare( KActive() ) )
		{
		aAction = IHsDataObserver::EActivate;
		}
	if( !aActionDes.Compare( KDeActive() ) )
		{
		aAction = IHsDataObserver::EDeactivate;
		}
	if( !aActionDes.Compare( KSuspend() ) )
		{
		aAction = IHsDataObserver::ESuspend;
		}
	if( !aActionDes.Compare( KResume() ) )
		{
		aAction = IHsDataObserver::EResume;
		}
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
int HsWidgetPublisherImpl::HandleWidgetItemActionL( const TDesC8& aActionDes,
		HsWidget& aWidget )
	{
	int ret( 1 );
	TBuf8<KSAPIContentNameMaxLength> itemName( aActionDes );
	std::string strItemName( (char*) itemName.PtrZ() );
	if( aWidget.checkIfWidgetItemExist(  strItemName ) )
		{
		std::string widgetName( aWidget.getWidgetName() );
		try
			{
			mHsDataObserver->handleItemEvent( widgetName,
				strItemName, 
				IHsDataObserver::ESelect );
			}
		catch( HsException& exception )
			{
			User::Leave( exception.getReason() );
			}
		ret = 0;
		}
	else if (aActionDes.Compare(KSelected) == 0)
	    {
        std::string widgetName( aWidget.getWidgetName() );
	    int count = aWidget.itemsCount();
	    
        for (int index = 0; index < count; index++)
            {
            HsWidgetItem* const item = aWidget.getWidgetItem( index );
            try
                {
                mHsDataObserver->handleItemEvent( widgetName,
                    item->getItemName(), 
                    IHsDataObserver::ESelect );
                }
            catch( HsException& exception )
                {
                User::Leave( exception.getReason() );
                }
            }
	        ret = 0;
	    }
	return ret;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
MLiwInterface* HsWidgetPublisherImpl::GetServiceInterfaceL( )
	{
	CLiwGenericParamList* inParam = CLiwGenericParamList::NewL();
	CleanupStack::PushL( inParam );
	CLiwGenericParamList* outParam = CLiwGenericParamList::NewL();
	CleanupStack::PushL( outParam );
	CLiwCriteriaItem* crit = CLiwCriteriaItem::NewL( KLiwCmdAsStr, 
		KCPInterface, KCPService );
	crit->SetServiceClass( TUid::Uid(KLiwClassBase) );
	
	RCriteriaArray a;
	a.AppendL(crit);    
	mServiceHandler->AttachL(a);
	mServiceHandler->ExecuteServiceCmdL( *crit, *inParam, *outParam ); 
	delete crit;
	a.Reset();
	
	// find service interface
	TInt pos = 0;
	MLiwInterface* msgInterface( 0 );
	outParam->FindFirst( pos, KCPInterface );
	if ( pos != KErrNotFound ) 
	    {
	    msgInterface = (*outParam)[pos].Value().AsInterface(); 
	    }
	
	outParam->Reset();
	inParam->Reset();
	CleanupStack::PopAndDestroy( outParam );
	CleanupStack::PopAndDestroy( inParam );
	
	return msgInterface;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HsWidgetPublisherImpl::SetDataObserver( IHsDataObserver* aDataObserver )
	{
	__ASSERT_ALWAYS( aDataObserver, User::Panic( KStartUpFailed, 0) );
	mHsDataObserver = aDataObserver;
	TInt err( KErrNone );
	TRAP( err,
		mServiceInterface = GetServiceInterfaceL();
		);
	if( err != KErrNone )
		{
		throw HsException( err );
		}
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HsWidgetPublisherImpl::RegisterNotificationL( HsWidget& aWidget )
	{
	CLiwGenericParamList* inParam = &(mServiceHandler->InParamListL());
	CLiwGenericParamList* outParam = &(mServiceHandler->OutParamListL());
	CLiwDefaultMap* reqFilter = CLiwDefaultMap::NewLC();
	
	HBufC* publisherName = StdStringToUnicodeLC( GetPublisherNameL( aWidget ) );
	
	reqFilter->InsertL( KPublisherId, TLiwVariant( *publisherName ) );
	reqFilter->InsertL( KContentType, TLiwVariant( KAll ) );
	reqFilter->InsertL( KContentId, TLiwVariant( KAll ));
	reqFilter->InsertL( KOperation, TLiwVariant( KOperationExecute ));
	
	inParam->AppendL(TLiwGenericParam(KType, TLiwVariant( 
		KContentAndPublisher_Registry)));
	inParam->AppendL(TLiwGenericParam(KFilter, TLiwVariant(reqFilter)));
	
	mServiceInterface->ExecuteCmdL( KRequestNotification,
		*inParam, *outParam, 0, this );
	TInt ret = ObtainErrorCode( *outParam );
	
	outParam->Reset();
	inParam->Reset();
	CleanupStack::PopAndDestroy( publisherName );
	CleanupStack::PopAndDestroy( reqFilter );
	User::LeaveIfError( ret );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HsWidgetPublisherImpl::UnegisterNotificationL()
	{
	CLiwGenericParamList* inParam = &(mServiceHandler->InParamListL());
	CLiwGenericParamList* outParam = &(mServiceHandler->OutParamListL());

	mServiceInterface->ExecuteCmdL( KRequestNotification,
		*inParam, *outParam, KLiwOptCancel, this ); 
	TInt ret= ObtainErrorCode( *outParam );

	outParam->Reset();
	inParam->Reset();	
	User::LeaveIfError( ret );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HsWidgetPublisherImpl::PublishWidget( HsWidget& aWidget )
	{
	TRAPD( err,
		PublishWidgetDataL( aWidget );
		);
	if( err != KErrNone )
		{
		throw HsException( err );
		}
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HsWidgetPublisherImpl::PublishWidgetDataL( HsWidget& aWidget )
    {
    CLiwGenericParamList* inParam = &(mServiceHandler->InParamListL());
    CLiwGenericParamList* outParam = &(mServiceHandler->OutParamListL());
    TLiwGenericParam type(KType, TLiwVariant(KCpData));
    
    int count = aWidget.itemsCount();
    for( int i = 0; i < count; i++ )
        {
        inParam->AppendL(type);
        CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();

        HsWidgetItem* const widgetItem = aWidget.getWidgetItem( i );
        
        // insert widget data identifiers
        HBufC* itemName = StdStringToUnicodeLC( widgetItem->getItemName());
        InsertWidgetDataIdentifiersL( aWidget, cpdatamap, *itemName );
        CleanupStack::PopAndDestroy( itemName );
        
        // insert widget item
        CLiwDefaultMap* datamap = CLiwDefaultMap::NewLC();
        InsertWidgetItemL( *widgetItem, datamap );
        cpdatamap->InsertL( KDataMap, TLiwVariant( datamap ) );
        
        // insert widget access control list
        InsertWidgetACLL( cpdatamap );
        
        // insert item triggers
        CLiwDefaultMap* triggermap = CLiwDefaultMap::NewLC();
        InsertItemTriggerL( *widgetItem, triggermap );
        cpdatamap->InsertL( KActionMap, TLiwVariant( triggermap ) );
        CleanupStack::PopAndDestroy( triggermap );

        // add to CPS
        TLiwGenericParam item( KItem, TLiwVariant( cpdatamap ));
        inParam->AppendL( item );
        mServiceInterface->ExecuteCmdL( KAdd,
                *inParam, *outParam);
        TInt ret= ObtainErrorCode( *outParam );
        
        CleanupStack::PopAndDestroy( datamap );
        CleanupStack::PopAndDestroy( cpdatamap );
        
        item.Reset();
        outParam->Reset();
        inParam->Reset();
        User::LeaveIfError( ret );
        }
    type.Reset();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HsWidgetPublisherImpl::PublishWidgetActionsL( HsWidget& aWidget )
	{
	CLiwGenericParamList* inParam = &(mServiceHandler->InParamListL());
	CLiwGenericParamList* outParam = &(mServiceHandler->OutParamListL());

	TLiwGenericParam type(KType, TLiwVariant(KPublisher));
	inParam->AppendL(type);
	CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();
	InsertWidgetIdentifiersL( aWidget, cpdatamap );
	
	CLiwDefaultMap* mapAction = CLiwDefaultMap::NewLC();
	
	CLiwDefaultMap* activateAction = CLiwDefaultMap::NewLC();
	activateAction->InsertL( KPluginId, TLiwVariant( KCASpaAppLauncherPlugin ) );
	
	CLiwDefaultMap* activate = CLiwDefaultMap::NewLC();
	activate->InsertL( KType, TLiwVariant( KActionValueLaunchApplication ) );
	activate->InsertL( KLaunchMethod, TLiwVariant( KLaunchMethodValueCmdLine ) );
	activate->InsertL( KApplicationUid, TLiwVariant( iWidgetUid ) );
	activate->InsertL( KApaCommand, TLiwVariant( KApaCommandBackground ) );
	
	activateAction->InsertL( KData, TLiwVariant( activate ) );
	
	mapAction->InsertL(KActive, TLiwVariant( activateAction ));
    mapAction->InsertL(KDeActive, TLiwVariant(KTriggerMap));
    mapAction->InsertL(KSuspend, TLiwVariant(KTriggerMap));
    mapAction->InsertL(KResume, TLiwVariant(KTriggerMap));
    mapAction->InsertL(KSelected, TLiwVariant(activateAction));
    
    cpdatamap->InsertL( KActionMap, TLiwVariant( mapAction ) );
    
	CLiwDefaultMap* datamap = CLiwDefaultMap::NewLC();
	InsertWidgetInfoL( aWidget, datamap );
	cpdatamap->InsertL( KDataMap, TLiwVariant( datamap ) );
    
	InsertWidgetACLL( cpdatamap );
	
	TLiwGenericParam item( KItem, TLiwVariant( cpdatamap ));	    
	inParam->AppendL( item );
	
	mServiceInterface->ExecuteCmdL( KAdd, *inParam, *outParam);
	
	TInt ret= ObtainErrorCode( *outParam );

	CleanupStack::PopAndDestroy( datamap );
	CleanupStack::PopAndDestroy( activate );
	CleanupStack::PopAndDestroy( activateAction );
	CleanupStack::PopAndDestroy( mapAction );
	CleanupStack::PopAndDestroy( cpdatamap );
	
	item.Reset();
	type.Reset();	
	outParam->Reset();
	inParam->Reset();
	User::LeaveIfError( ret );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HsWidgetPublisherImpl::RemoveWidget( std::string& aTemplateName, 
	std::string& aWidgetName,
	std::string& aIdentifier )
	{
	HsWidget* ret( 0 );
	int count = mWidgets.size();
	for (int index = 0; index < count; index++)
		{
		HsWidget* const widget = mWidgets.at( index );
		if ( !aTemplateName.compare( widget->getTemplateName() ) )
			{
			if( !aWidgetName.compare( widget->getWidgetName() ) )
				{
				if( !aIdentifier.compare( widget->getIdentifier() ) )
					{
					ret = widget;
					TRAPD( removalErr, RemoveWidgetL( *widget ) );
					if ( removalErr != KErrNone )
					    {
					    throw HsException( removalErr );
					    }
					mWidgetContentIds.erase( widget->getIdentifier() );
					mWidgets.erase( mWidgets.begin() + index );
					break;
					}
				}
			}
		}
	if( !ret )
		{
		throw HsException( KErrNotFound );
		}
	delete ret;
	}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HsWidgetPublisherImpl::RemoveWidgetL( HsWidget& aWidget )
	{
	// remove widget data
    CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();
    
    TRAPD( err,
            InsertWidgetDataIdentifiersL( aWidget, cpdatamap, KAll );
            RemoveFromCpsL( cpdatamap, KCpData ) );
    if ( err != KErrNotFound )
        {
        User::LeaveIfError( err );
        }
    CleanupStack::PopAndDestroy( cpdatamap );
    cpdatamap = NULL;
	
	// remove widget definition (actions and template info)
    cpdatamap = CLiwDefaultMap::NewLC();
    InsertWidgetIdentifiersL( aWidget, cpdatamap );
    RemoveFromCpsL( cpdatamap, KPublisher );
    CleanupStack::PopAndDestroy( cpdatamap );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HsWidgetPublisherImpl::RemoveFromCpsL( CLiwDefaultMap* aCpDataMap, 
	const TDesC& aType )
	{
	__ASSERT_ALWAYS( aCpDataMap, User::Invariant() );
	CLiwGenericParamList* inParam = &(mServiceHandler->InParamListL());
	CLiwGenericParamList* outParam = &(mServiceHandler->OutParamListL());
	
	TLiwGenericParam type( KType, TLiwVariant( aType ) );
	inParam->AppendL( type );
	
	TLiwGenericParam item( KItem, TLiwVariant( aCpDataMap ));	    
	inParam->AppendL( item );
	
	mServiceInterface->ExecuteCmdL( KDelete,
		*inParam, *outParam);
	TInt ret= ObtainErrorCode( *outParam );
	
	item.Reset();
	type.Reset();	
	inParam->Reset();
	outParam->Reset();
	User::LeaveIfError( ret );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HsWidget& HsWidgetPublisherImpl::GetWidget( std::string& aTemplateName, 
	std::string& aWidgetName,
	std::string& aIdentifier )
	{
	HsWidget* ret( 0 );
	int count = mWidgets.size();
	for (int index = 0; index < count; index++)
		{
		HsWidget* const widget = mWidgets.at( index );
		if ( !aTemplateName.compare( widget->getTemplateName() ) )
			{
			if( !aWidgetName.compare( widget->getWidgetName() ) )
				{
				if( !aIdentifier.compare( widget->getIdentifier() ) )
					{
					ret = widget;
					break;
					}
				}
			}
		}
	if( !ret )
		{
		throw HsException( KErrNotFound );
		}
	return *ret; 
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HsWidget* HsWidgetPublisherImpl::GetWidgetL( const CLiwMap* aMap )
	{
	// identifier length + colon char + KHsWidgetPublisher string length
	const TInt maxPublisherNameLength =
        KSAPIContentNameMaxLength + 1 + KHsWidgetPublisher().Length();

	HsWidget* ret ( 0 );
	TLiwVariant variant;
    variant.PushL();
    
    if ( !aMap->FindL( KPublisherId, variant ) )
        {
        User::Leave( KErrNotFound );
        }
    HBufC* publisherName = HBufC::NewLC( maxPublisherNameLength );
        { // braces for scope only
        TPtr16 publisherNameDes = publisherName->Des();
        variant.Get( publisherNameDes );
        }
    variant.Reset();
    std::string strIdentifier( GetWidgetIdentifierFromPublisherNameL(
            *publisherName ) );
    CleanupStack::PopAndDestroy( publisherName );
    publisherName = 0;
    
	TBuf<KSAPIContentNameMaxLength> contentIdDesc;
    if ( !aMap->FindL( KContentId, variant ) )
         {
         User::Leave( KErrNotFound );
         }
    variant.Get( contentIdDesc );
    CleanupStack::PopAndDestroy( &variant );
    std::wstring contentId( ToWideStringL( contentIdDesc ) );
	
	std::string strTemplate;
	std::string strWidgetName;
		
	if( CheckIfWidgetExist(strTemplate, strWidgetName, strIdentifier, ETrue) )
		{
		ret = &GetWidget( strTemplate, strWidgetName, strIdentifier);
		
		// verify that content id does not change
		__ASSERT_DEBUG( !mWidgetContentIds.count( ret->getIdentifier() ) ||
		        mWidgetContentIds[ret->getIdentifier()] == contentId,
		        User::Invariant() );

		mWidgetContentIds[ret->getIdentifier()] = contentId;
		}
	
	return ret;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
bool HsWidgetPublisherImpl::CheckIfWidgetExist( std::string& aTemplateName, 
	std::string& aWidgetName,
	std::string& aIdentifier,
	TBool aAdjustToIdentifier )
	{
	bool widgetExist( false );
	int count = mWidgets.size();
	for (int index = 0; index < count; index++)
		{
		HsWidget* const widget = mWidgets.at( index );
		if( !aIdentifier.compare( widget->getIdentifier() ) )
			{
			if( !aTemplateName.compare( widget->getTemplateName() ) )
				{
				if( !aWidgetName.compare( widget->getWidgetName() ) )
					{
					widgetExist = true;
					break;
					}
				}
			if( aAdjustToIdentifier )
				{
				aTemplateName = widget->getTemplateName();
				aWidgetName = widget->getWidgetName();
				widgetExist = true;
				break;
				}
			}
		}
	return widgetExist;	
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
std::string HsWidgetPublisherImpl::ToStringL(const TDesC& aText)
    {
    HBufC8* text = HBufC8::NewL( aText.Length() + 1 /*for ending zero*/ );
    TPtr8 dest( text->Des() );
    CnvUtfConverter::ConvertFromUnicodeToUtf8( dest, aText );    
    std::string ret((const char*)dest.PtrZ());
    delete text;
    return ret;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
std::wstring HsWidgetPublisherImpl::ToWideStringL( const TDesC& aText ) const
    {
    return std::wstring(
            reinterpret_cast<const wchar_t*>( aText.Ptr() ), aText.Length() );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HsWidgetPublisherImpl::InsertWidgetIdentifiersL( HsWidget& aWidget,
	CLiwDefaultMap* aDataMap )
    {
    HBufC* publisherName = StdStringToUnicodeLC( GetPublisherNameL( aWidget ) );
    aDataMap->InsertL( KPublisherId, TLiwVariant( *publisherName ));
    aDataMap->InsertL( KContentType, TLiwVariant( KHSTemplate ));
    aDataMap->InsertL( KContentId, TLiwVariant( KAll ));
    CleanupStack::PopAndDestroy( publisherName );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HsWidgetPublisherImpl::InsertWidgetDataIdentifiersL( HsWidget& aWidget,
	CLiwDefaultMap* aDataMap, const TDesC& aContentType )
    {
    WidgetContentIdMapType::const_iterator contentIdIter =
            mWidgetContentIds.find( aWidget.getIdentifier() );
    if ( contentIdIter == mWidgetContentIds.end() )
        {
        User::Leave( KErrNotFound );
        }
    TPtrC16 contentId = reinterpret_cast<const TUint16*>(
            contentIdIter->second.c_str() );
    
    HBufC* publisherName = StdStringToUnicodeLC( GetPublisherNameL( aWidget ) );
    
    aDataMap->InsertL( KPublisherId, TLiwVariant( *publisherName ) );
    aDataMap->InsertL( KContentType, TLiwVariant( aContentType ) );
    aDataMap->InsertL( KContentId, TLiwVariant( contentId ) );
    
    CleanupStack::PopAndDestroy( publisherName );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HsWidgetPublisherImpl::InsertWidgetInfoL( HsWidget& aWidget,
	CLiwDefaultMap* aDataMap )
    {
    __ASSERT_ALWAYS( aDataMap, User::Invariant() );

    HBufC* templateType = StdStringToUnicodeLC( aWidget.getTemplateName() );
    HBufC* widgetName = StdStringToUnicodeLC( aWidget.getWidgetName() );
	    
	aDataMap->InsertL( KTemplateType, TLiwVariant( *templateType ) );
	aDataMap->InsertL( KWidgetName, TLiwVariant( *widgetName ) );
    aDataMap->InsertL( KWidgetMaxInstance, TLiwVariant( 1 ) );
    
    // this is needed by chswiplugin:
    aDataMap->InsertL( KWidgetUid, TLiwVariant( iWidgetUid ));
    
    const std::string& widgetDescriptionStd = aWidget.getDescription();
    if ( widgetDescriptionStd.length() )
        {
        HBufC* widgetDescription = StdStringToUnicodeLC( widgetDescriptionStd );
        aDataMap->InsertL( KWidgetDesc, TLiwVariant( *widgetDescription ) );
        CleanupStack::PopAndDestroy( widgetDescription );
        }
    const std::string& widgetIconLocStd = aWidget.getIconLocation();
    if ( widgetIconLocStd.length() )
        {
        HBufC* widgetIconLoc = StdStringToUnicodeLC( widgetIconLocStd );
        aDataMap->InsertL( KWidgetIcon, TLiwVariant( *widgetIconLoc ) );
        CleanupStack::PopAndDestroy( widgetIconLoc );
        }

	CleanupStack::PopAndDestroy( widgetName );
	CleanupStack::PopAndDestroy( templateType );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HsWidgetPublisherImpl::InsertWidgetItemL ( HsWidgetItem& aWidgetItem,
	CLiwDefaultMap* aDataMap )
	{
    TPtrC8 itemName = ((TUint8*)aWidgetItem.getItemName().c_str());
    if( aWidgetItem.isStringValue() )
        {
        TPtrC8 itemValue = ((TUint8*)aWidgetItem.getItemValue().c_str());
        HBufC* value = HBufC::NewLC( KUnicodeSize * itemValue.Length() );
        TPtr dest( value->Des() );
        CnvUtfConverter::ConvertToUnicodeFromUtf8( dest, itemValue );
        
        aDataMap->InsertL( itemName, TLiwVariant(*value ));
        CleanupStack::PopAndDestroy(value);
        }
    else
        {
        int itemValue = aWidgetItem.getItemValueInt();
        aDataMap->InsertL( itemName, TLiwVariant( TInt32( itemValue ) ));
        }
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt HsWidgetPublisherImpl::ObtainErrorCode( 
	const CLiwGenericParamList& aOutParamList )
	{
    TInt errorCode( KErrNone );
    TInt32 serviceErrorCode;
    TInt posErr( 0 );
    aOutParamList.FindFirst( posErr, LIW::EGenericParamError );
	if( posErr != KErrNotFound )
		{
		// error code found - extract and handle
		aOutParamList[posErr].Value().Get( serviceErrorCode );
		errorCode = TranslateServiceError( serviceErrorCode );
		}
    return errorCode;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt HsWidgetPublisherImpl::TranslateServiceError( TInt32 aServiceErrorCode )
	{
	TInt ret( KErrNone );

    switch ( aServiceErrorCode )
        {
        case SErrNone: 
        	// KErrNone and KErrCancel are translated into SErrNone 
        	ret = KErrNone;
        	break;

        case SErrNotFound:
            ret = KErrNotFound;
            break;

        case SErrNoMemory:
            ret = KErrNoMemory;
            break;

        case SErrServiceInUse:
            ret = KErrInUse;
            break;

        case SErrServiceNotSupported:
            ret = KErrNotSupported;
            break;

        case SErrBadArgumentType:
            ret = KErrBadName;
            break;

        case SErrInvalidServiceArgument:
            ret = KErrArgument;
            break;

        case SErrAccessDenied:
            ret = KErrPermissionDenied;
            break;            

        default:
            ret = KErrGeneral;
            break;
        }
	
	return ret;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HsWidgetPublisherImpl::InsertItemTriggerL( HsWidgetItem& aWidgetItem,
	CLiwDefaultMap* aTriggerMap )
	{
	CLiwDefaultMap* activateAction = CLiwDefaultMap::NewLC();
	activateAction->InsertL( KPluginId, TLiwVariant( KCASpaAppLauncherPlugin ) );
	
	CLiwDefaultMap* activate = CLiwDefaultMap::NewLC();
	activate->InsertL( KType, TLiwVariant( KActionValueLaunchApplication ) );
	activate->InsertL( KLaunchMethod, TLiwVariant( KLaunchMethodValueCmdLine ) );
	activate->InsertL( KApplicationUid, TLiwVariant( iWidgetUid ) );
	activate->InsertL( KApaCommand, TLiwVariant( KApaCommandBackground ) );
	
	activateAction->InsertL( KData, TLiwVariant( activate ) );

    TPtrC8 itemName = ((TUint8*)aWidgetItem.getItemName().c_str());
    aTriggerMap->InsertL( itemName, TLiwVariant( activateAction ));

	CleanupStack::PopAndDestroy( activate );
	CleanupStack::PopAndDestroy( activateAction );
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void HsWidgetPublisherImpl::InsertWidgetACLL( CLiwDefaultMap* aDataMap )
    {
    CLiwDefaultMap* aclMap = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* writeMap = CLiwDefaultMap::NewLC();
    CLiwDefaultMap* deleteMap = CLiwDefaultMap::NewLC();
    
    RBuf widgetUid;
    widgetUid.CleanupClosePushL();
    widgetUid.CreateL(10);
    widgetUid.Append( KHexPrefix );
    widgetUid.AppendNum( (TUint)iWidgetUid, EHex );
    
    RBuf widgetCHUids;
    widgetCHUids.CleanupClosePushL();
    widgetCHUids.CreateL(21);
    widgetCHUids.Append( widgetUid );
    widgetCHUids.Append( KColon );
    widgetCHUids.Append( KContentHarvesterUid );
    
    writeMap->InsertL( KUIDs, TLiwVariant( widgetUid ) );
    deleteMap->InsertL( KUIDs, TLiwVariant( widgetCHUids ) );
    
    aclMap->InsertL( KACLWrite, TLiwVariant( writeMap ) );
    aclMap->InsertL( KACLDelete, TLiwVariant( deleteMap ) );
    
    aDataMap->InsertL( KAccessList, TLiwVariant( aclMap ) );
    
    CleanupStack::PopAndDestroy( &widgetCHUids );
    CleanupStack::PopAndDestroy( &widgetUid );
    CleanupStack::PopAndDestroy( deleteMap );
    CleanupStack::PopAndDestroy( writeMap );
    CleanupStack::PopAndDestroy( aclMap );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
std::string HsWidgetPublisherImpl::GetPublisherNameL( HsWidget& aWidget )
    {
    return aWidget.getIdentifier() + ':' + ToStringL( KHsWidgetPublisher() );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
std::string HsWidgetPublisherImpl::GetWidgetIdentifierFromPublisherNameL(
        const TDesC& aPublisherName )
    {
    TInt colonPos = aPublisherName.LocateReverse(':');
    User::LeaveIfError( colonPos );
    return ToStringL( aPublisherName.Left( colonPos ) );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HBufC* HsWidgetPublisherImpl::Utf8ToUnicodeLC( const TDesC8& aSrcString ) const
    {
    HBufC* destString = HBufC::NewLC( aSrcString.Length() );
    TPtr destStringDes = destString->Des();
    CnvUtfConverter::ConvertToUnicodeFromUtf8( destStringDes, aSrcString );
    return destString;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
HBufC* HsWidgetPublisherImpl::StdStringToUnicodeLC(
        const std::string& aSrcString ) const
    {
    TPtrC8 srcDescriptor = reinterpret_cast<const TUint8*>(
            aSrcString.c_str() );
    return Utf8ToUnicodeLC( srcDescriptor );
    }
