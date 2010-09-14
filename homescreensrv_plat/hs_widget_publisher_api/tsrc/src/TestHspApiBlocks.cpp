/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ?Description
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "TestHspApi.h"
#include <hswidgetpublisher.h>
#include <hswidget.h>
#include <hsdataobserver.h>
#include <liwservicehandler.h>
#include <liwvariant.h>
#include <liwgenericparam.h>
#include <liwcommon.h>
#include "waitactive.h"
#include <hsexception.h>

_LIT8( KResults, "results" );

_LIT8( KType, "type" );
_LIT( KCpData, "cp_data" );
_LIT8( KContentIdFormat, "%s:%s");
_LIT8( KPublisherId, "publisher" );
_LIT8( KContentType, "content_type" );
_LIT8( KContentId, "content_id" );
_LIT( KHsPublisher, "ai3templatedwidget" );
_LIT( KHsWidgetPublisher, "hswidgetpublisher");
_LIT( KAll, "all" );
_LIT8( KFilter, "filter" );
_LIT( KContentAndPublisher_Registry, "cp_data:publisher");
_LIT8( KRequestNotification, "RequestNotification" );
_LIT( KPublisher, "publisher" );
_LIT8( KActionTrigger, "action_trigger" );
_LIT8( KCPService, "Service.ContentPublishing" );
_LIT8( KActive, "active");
_LIT8( KSelected, "selected");
_LIT( KFakeContentId, "616" );

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
LOCAL_C HBufC* StdStringTo16bitDescLC( const std::string& aSrcString )
    {
    TPtrC8 srcDescriptor = (TUint8*) aSrcString.c_str();
    HBufC* dstDescriptor = HBufC::NewLC( srcDescriptor.Length() );
    dstDescriptor->Des().Copy( srcDescriptor );
    return dstDescriptor;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestHspApi::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CTestHspApi::Delete() 
    {
    mServiceInterface->Close();
    delete mServiceHandler;    
    }
    
// -----------------------------------------------------------------------------
// CTestHspApi::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CTestHspApi::RunMethodL( 
    CStifItemParser& aItem ) 
    {
   
    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "TestHspApiConstrL", CTestHspApi::TestHspApiConstrL ),
        ENTRY( "TestHspWidgetAddWidgetItem1L", CTestHspApi::TestHspWidgetAddWidgetItem1L ),
        ENTRY( "TestHspWidgetAddWidgetItem2L", CTestHspApi::TestHspWidgetAddWidgetItem2L ),
        ENTRY( "TestHspWidgetAddWidgetItem3L", CTestHspApi::TestHspWidgetAddWidgetItem3L ),
        ENTRY( "TestHspApiDeleteWidgetL", CTestHspApi::TestHspApiDeleteWidgetL ), 
        ENTRY( "TestHspApiSetDataObserver1L", CTestHspApi::TestHspApiSetDataObserver1L ),
		ENTRY( "TestHspApiSetDataObserver2L", CTestHspApi::TestHspApiSetDataObserver2L ),
        ENTRY( "TestHspApiGetWidgetL", CTestHspApi::TestHspApiGetWidgetL ),
        ENTRY( "TestHspApiWidgetRemoveItem1L", CTestHspApi::TestHspApiWidgetRemoveItem1L ),
		ENTRY( "TestHspApiWidgetRemoveItem2L", CTestHspApi::TestHspApiWidgetRemoveItem2L ),
        ENTRY( "TestHspApiExceptionL", CTestHspApi::TestHspApiExceptionL ),
        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CTestHspApi::ExtractItemCountL(
      const CLiwGenericParamList& aInParamList )
    {
    TInt result( KErrNotFound );
    TInt count (0);
    TInt pos( 0 );
    aInParamList.FindFirst( pos, KResults );
    if( pos != KErrNotFound )
      // results present - extract and return text
         {
         TLiwVariant variant = aInParamList[pos].Value();
         CLiwIterable* iterable = variant.AsIterable();
         iterable->Reset();

         while(iterable->NextL( variant ))
            {
            count++;
            }

         iterable->Reset();
         variant.Reset();

         result = count;
         }

    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CTestHspApi::ExtractItemL( const CLiwGenericParamList& aInParamList,
		const TDesC8& aItemName, RBuf& aItemValue )
	{
    TInt result( KErrNotFound );
    TInt count (0);
    TInt pos( 0 );
    aInParamList.FindFirst( pos, KResults );
    if( pos != KErrNotFound )
      // results present - extract and return text
         {
         TLiwVariant variant = aInParamList[pos].Value();
         CLiwIterable* iterable = variant.AsIterable();
         
         TLiwVariant item;
         while ( iterable->NextL( item ) )
        	 {
        	 CLiwDefaultMap *map = CLiwDefaultMap::NewLC();
        	 item.Get( *map );
             TLiwVariant dataMapVariant;
             TBool found = map->FindL( _L8("data_map"), dataMapVariant );
             if ( found )
            	 {
				 CLiwDefaultMap *dataMap = CLiwDefaultMap::NewLC();
				 dataMapVariant.Get( *dataMap );
				 TLiwVariant item1;
				 found = dataMap->FindL( aItemName, item1 );
				 if ( found )
					 {
					 TPtrC ptr;
					 item1.Get( ptr );
					 aItemValue.Create( ptr );
					 result = KErrNone;
					 }
				 item1.Reset();
				 dataMapVariant.Reset();
				 CleanupStack::PopAndDestroy( dataMap );
            	 }
        	 CleanupStack::PopAndDestroy( map );
        	 }         
        
         iterable->Reset();
         variant.Reset();
         }

    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CLiwGenericParamList* CTestHspApi::GetListL( std::string& aContentType, 
		std::string& aIdentifier, std::string& aWidgetName )
	{
	TBool res( EFalse );
	
	CLiwGenericParamList* inParam = &(mServiceHandler->InParamListL());
	CLiwGenericParamList* outParam = &(mServiceHandler->OutParamListL());

	TLiwGenericParam type(KType, TLiwVariant(KCpData));
	inParam->AppendL(type);
	CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();

	TPtrC8 contentType = ((TUint8*)aContentType.c_str());
	
	const TInt KSAPIContentNameMaxLength = 255;
	
	TBuf8<KSAPIContentNameMaxLength> contentId;
	contentId = (TUint8*)aIdentifier.c_str();
	
	HBufC* publisherName = StdStringTo16bitDescLC( aIdentifier + ":hswidgetpublisher" );
	cpdatamap->InsertL( KPublisherId, TLiwVariant( *publisherName ));
	cpdatamap->InsertL( KContentType, TLiwVariant( KAll ));
	cpdatamap->InsertL( KContentId, TLiwVariant( KFakeContentId ));
	
	TLiwGenericParam filter( KFilter, TLiwVariant( cpdatamap ));
	inParam->AppendL( filter );
	mServiceInterface->ExecuteCmdL( _L8("GetList"),
		*inParam, *outParam);
	
	CleanupStack::PopAndDestroy( publisherName );
	CleanupStack::PopAndDestroy( cpdatamap );
	
	filter.Reset();
	type.Reset();	

	inParam->Reset();
	
	return outParam;
	}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CTestHspApi::HandleNotifyL(
    TInt /*aCmdId*/,
    TInt /*aEventId*/,
    CLiwGenericParamList& aEventParamList,
    const CLiwGenericParamList& /*aInParamList*/)
	{
	iActionStatus = KErrNone;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CLiwGenericParamList* CTestHspApi::ExecuteActionL( std::string& aContentType, 
		std::string& aIdentifier, std::string& aWidgetName )
	{
	TBool res( EFalse );
	
	// ----------------------------
	// register for notification
	// ----------------------------
	
	CLiwGenericParamList* inParam = &(mServiceHandler->InParamListL());
	CLiwGenericParamList* outParam = &(mServiceHandler->OutParamListL());
	CLiwDefaultMap* reqFilter = CLiwDefaultMap::NewLC();
	
	//reqFilter->InsertL( KPublisherId, TLiwVariant( KHSPPublisher ));
	reqFilter->InsertL( KPublisherId, TLiwVariant( KAll ));
	reqFilter->InsertL( KContentType, TLiwVariant( KAll ));
	reqFilter->InsertL( KContentId, TLiwVariant( KAll ));
	//reqFilter->InsertL( KOperation, TLiwVariant( KOperationExecute ));
	
	inParam->AppendL(TLiwGenericParam(KType, TLiwVariant(KContentAndPublisher_Registry)));
	inParam->AppendL(TLiwGenericParam(KFilter, TLiwVariant(reqFilter)));
	
	mServiceInterface->ExecuteCmdL( KRequestNotification,
		*inParam, *outParam, 0, this );
	
	outParam->Reset();
	inParam->Reset();
	CleanupStack::PopAndDestroy( reqFilter );
	
	// --------------
	// execute action
	// --------------
	
	TLiwGenericParam type(KType, TLiwVariant(KPublisher));
	inParam->AppendL(type);
	CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();

	TPtrC8 contentType = ((TUint8*)aContentType.c_str());
	
	const TInt KSAPIContentNameMaxLength = 255;
	
	TBuf8<KSAPIContentNameMaxLength> contentId;
	contentId = (TUint8*)aIdentifier.c_str();
	
	cpdatamap->InsertL( KPublisherId, TLiwVariant( KHsWidgetPublisher ));
	cpdatamap->InsertL( KContentType, TLiwVariant( KHsPublisher ));
	cpdatamap->InsertL( KContentId, TLiwVariant( contentId ));		
	cpdatamap->InsertL( KActionTrigger, TLiwVariant( KActive ));
	
	TLiwGenericParam filter( KFilter, TLiwVariant( cpdatamap ));
	inParam->AppendL( filter );
	mServiceInterface->ExecuteCmdL( _L8("ExecuteAction"),
		*inParam, *outParam );
	
	CleanupStack::PopAndDestroy( cpdatamap );
	
	filter.Reset();
	type.Reset();	

	inParam->Reset();
	
	return outParam;
	}
	
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CLiwGenericParamList* CTestHspApi::ExecuteActionSelectedL( std::string& aContentType, 
		std::string& aIdentifier, std::string& aWidgetName )
	{
	TBool res( EFalse );
	
	// ----------------------------
	// register for notification
	// ----------------------------
	
	CLiwGenericParamList* inParam = &(mServiceHandler->InParamListL());
	CLiwGenericParamList* outParam = &(mServiceHandler->OutParamListL());
	CLiwDefaultMap* reqFilter = CLiwDefaultMap::NewLC();
	
	//reqFilter->InsertL( KPublisherId, TLiwVariant( KHSPPublisher ));
	reqFilter->InsertL( KPublisherId, TLiwVariant( KAll ));
	reqFilter->InsertL( KContentType, TLiwVariant( KAll ));
	reqFilter->InsertL( KContentId, TLiwVariant( KAll ));
	//reqFilter->InsertL( KOperation, TLiwVariant( KOperationExecute ));
	
	inParam->AppendL(TLiwGenericParam(KType, TLiwVariant(KContentAndPublisher_Registry)));
	inParam->AppendL(TLiwGenericParam(KFilter, TLiwVariant(reqFilter)));
	
	mServiceInterface->ExecuteCmdL( KRequestNotification,
		*inParam, *outParam, 0, this );
	
	outParam->Reset();
	inParam->Reset();
	CleanupStack::PopAndDestroy( reqFilter );
	
	// --------------
	// execute action
	// --------------
	
	TLiwGenericParam type(KType, TLiwVariant(KPublisher));
	inParam->AppendL(type);
	CLiwDefaultMap* cpdatamap = CLiwDefaultMap::NewLC();

	TPtrC8 contentType = ((TUint8*)aContentType.c_str());
	
	const TInt KSAPIContentNameMaxLength = 255;
	
	TBuf8<KSAPIContentNameMaxLength> contentId;
	contentId.Format( KContentIdFormat,
		((TUint8*)aIdentifier.c_str()), 
		((TUint8*)aWidgetName.c_str()) );
	
	cpdatamap->InsertL( KPublisherId, TLiwVariant( KHsWidgetPublisher ));
	cpdatamap->InsertL( KContentType, TLiwVariant( KHsPublisher ));
	cpdatamap->InsertL( KContentId, TLiwVariant( contentId ));		
	cpdatamap->InsertL( KActionTrigger, TLiwVariant( KSelected ));
	
	TLiwGenericParam filter( KFilter, TLiwVariant( cpdatamap ));
	inParam->AppendL( filter );
	mServiceInterface->ExecuteCmdL( _L8("ExecuteAction"),
		*inParam, *outParam );
	
	CleanupStack::PopAndDestroy( cpdatamap );
	
	filter.Reset();
	type.Reset();	

	inParam->Reset();
	
	return outParam;
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CTestHspApi::WaitL(TInt aMicroSec)
   {
   CWaitActive* wait = CWaitActive::NewL();
    wait->Wait(aMicroSec);
    delete wait;
   }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
MLiwInterface* CTestHspApi::GetServiceInterfaceL()
	{
	CLiwGenericParamList* inParam = CLiwGenericParamList::NewL();
	CleanupStack::PushL( inParam );
	CLiwGenericParamList* outParam = CLiwGenericParamList::NewL();
	CleanupStack::PushL( outParam );
//	CLiwCriteriaItem* crit = CLiwCriteriaItem::NewL( KLiwCmdAsStr, 
//		KCPInterface, KCPService );
	CLiwCriteriaItem* crit = CLiwCriteriaItem::NewL( KLiwCmdAsStr, 
			_L8("IContentPublishing"), KCPService );
	
	crit->SetServiceClass( TUid::Uid(KLiwClassBase) );
	
	RCriteriaArray a;
	a.AppendL(crit);    
	
	mServiceHandler->AttachL(a);
	mServiceHandler->ExecuteServiceCmdL( *crit, *inParam, *outParam ); 
	
	delete crit;
	a.Reset();
	
	// find service interface
	TInt pos = 0;
	MLiwInterface* msgInterface = NULL;
	//outParam->FindFirst( pos, KCPInterface );
	outParam->FindFirst( pos, _L8("IContentPublishing") );
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


// from Observer interface

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CTestHspApi::handleItemEvent( std::string aWidgetName, std::string aTemplateItemName,
		IHsDataObserver::EItemEvent aAction )
	{
	TPtrC8 itemName = ((TUint8*)aTemplateItemName.c_str());
	
	iItemActionStatus = KErrNone;
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CTestHspApi::handleEvent( std::string aWidgetName,
		IHsDataObserver::EEvent aAction )
	{
	TPtrC8 widgetName = ((TUint8*)aWidgetName.c_str());
	
	iActionStatus = KErrNone;
	}


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt CTestHspApi::TestHspApiConstrL( CStifItemParser& aItem )
    {
    Hs::HsWidgetPublisher* mHSPApi = new Hs::HsWidgetPublisher(this);

    if ( mHSPApi )
    	{
    	delete mHSPApi;
    	return KErrNone;
    	}
    else
    	{
    	return KErrGeneral;
    	}
    }
    

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CTestHspApi::AddTemplate( Hs::HsWidgetPublisher* aHspApi, std::string& aContentType, 
		std::string& aIdentifier, std::string& aWidgetName )
	{
	Hs::HsWidget& templ = aHspApi->createHsWidget( aContentType, aWidgetName, aIdentifier);
	std::string itemName1( "item_name_1" );
	std::string itemValue1( "myimage1_image" );
	templ.setItem( itemName1, itemValue1 );
	std::string itemName2( "item_name_2" );
	templ.setItem( itemName2, 1 );
	std::string itemName3( "item_name_3" );
	std::string itemValue3( "value_3" );
	templ.setItem( itemName3, itemValue3 );
	
	templ.setItem( itemName2, 1 );
	templ.setItem( itemName2, itemValue3 );
	
	SendFakeActivateEvent( aIdentifier );
	WaitL( 1000000 );
	
	aHspApi->publishHsWidget( templ );
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CTestHspApi::SendFakeActivateEvent( std::string& aIdentifier )
    {
    CLiwGenericParamList* inParam = &(mServiceHandler->InParamListL());
    CLiwGenericParamList* outParam = &(mServiceHandler->OutParamListL());

    TLiwGenericParam type(KType, TLiwVariant(KCpData));
    inParam->AppendL(type);
    
    CLiwDefaultMap* map = CLiwDefaultMap::NewLC();
    HBufC* publisherName = StdStringTo16bitDescLC( aIdentifier + ":hswidgetpublisher" );
    map->InsertL( KPublisherId, TLiwVariant( *publisherName ) );
    map->InsertL( KContentType, TLiwVariant( _L("hswidget") ) );
    map->InsertL( KContentId, TLiwVariant( KFakeContentId ) );
    map->InsertL( KType, TLiwVariant( _L("publisher") ) );
    map->InsertL( KActionTrigger, TLiwVariant( _L8("active") ) );
    map->InsertL( _L8("action_trigger16"), TLiwVariant( _L("active") ) );
    map->InsertL( _L8("operation"), TLiwVariant( _L("execute") ) );
    
    TLiwGenericParam filter( KFilter, TLiwVariant( map ) );
    inParam->AppendL( filter );
    
    mServiceInterface->ExecuteCmdL( _L8("ExecuteAction"), *inParam, *outParam);
    
    CleanupStack::PopAndDestroy( publisherName );
    CleanupStack::PopAndDestroy( map );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CTestHspApi::AddExistingTemplate( Hs::HsWidgetPublisher* aHspApi, std::string& aContentType, 
		std::string& aIdentifier, std::string& aWidgetName )
	{
	Hs::HsWidget& templ = aHspApi->createHsWidget( aContentType, aWidgetName, aIdentifier);
	std::string itemName1( "item_name_1" );
	std::string itemValue1( "myimage1_image" );
	templ.setItem( itemName1, itemValue1 );
	Hs::HsWidget& temp2 = aHspApi->createHsWidget( aContentType, aWidgetName, aIdentifier);
	aHspApi->publishHsWidget( templ );
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CTestHspApi::AddAndRemoveTemplateItem1( Hs::HsWidgetPublisher* aHspApi, std::string& aContentType, 
		std::string& aIdentifier, std::string& aWidgetName )
	{
	Hs::HsWidget& templ = aHspApi->createHsWidget( aContentType, aWidgetName, aIdentifier);
	std::string itemName1( "item_name_1" );
	std::string itemValue1( "myimage1_image" );
	templ.setItem( itemName1, itemValue1 );
	std::string itemName2( "item_name_2" );
	templ.setItem( itemName2, 1 );
	std::string itemName3( "item_name_3" );
	std::string itemValue3( "value_3" );
	templ.setItem( itemName3, itemValue3 );
	templ.removeItem( itemName3 );
	
    SendFakeActivateEvent( aIdentifier );
    WaitL( 1000000 );
	
	aHspApi->publishHsWidget( templ );
	}
	
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CTestHspApi::AddAndRemoveTemplateItem2( Hs::HsWidgetPublisher* aHspApi, std::string& aContentType, 
		std::string& aIdentifier, std::string& aWidgetName )
	{
	Hs::HsWidget& templ = aHspApi->createHsWidget( aContentType, aWidgetName, aIdentifier);
	std::string itemName1( "item_name_1" );
	std::string itemValue1( "myimage1_image" );
	templ.setItem( itemName1, itemValue1 );
	std::string itemName2( "item_name_2" );
	templ.setItem( itemName2, 1 );
	std::string itemName3( "item_name_3" );
	templ.removeItem( itemName3 );
	aHspApi->publishHsWidget( templ );
	}


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt CTestHspApi::TestHspWidgetAddWidgetItem1L( CStifItemParser& aItem )
	{
	TInt err( KErrNone );
	
	Hs::HsWidgetPublisher* mHSPApi = new Hs::HsWidgetPublisher( this );
	
	std::string contentType( "ct" );
	std::string identifier( "id" );
	std::string widgetName( "wn" );
	
	AddTemplate( mHSPApi, contentType, identifier, widgetName );
	
	CLiwGenericParamList* res = GetListL( contentType, identifier, widgetName );
	TInt count = ExtractItemCountL( *res );
	
	delete mHSPApi;
	
	if ( count != 4 )
		{
		err = KErrGeneral;
		}	

	return err;
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt CTestHspApi::TestHspWidgetAddWidgetItem2L( CStifItemParser& aItem )
	{
	TInt err( KErrNone );
	
	Hs::HsWidgetPublisher* mHSPApi = new Hs::HsWidgetPublisher( this );
	
	std::string contentType( "ct" );
	std::string identifier( "id" );
	std::string widgetName( "wn" );
	
	AddTemplate( mHSPApi, contentType, identifier, widgetName );
	
	CLiwGenericParamList* res = GetListL( contentType, identifier, widgetName );
	
	// check if item was really removed
	RBuf value;
	err = ExtractItemL( *res, _L8("item_name_3"), value );
	
	if ( value.Compare( _L("value_3") ) )
		{
		err = KErrGeneral;
		}
	
	value.Close();
	
	delete mHSPApi;
	return err;
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt CTestHspApi::TestHspWidgetAddWidgetItem3L( CStifItemParser& aItem )
	{
	TInt err( KErrNone );
	
	Hs::HsWidgetPublisher* mHSPApi = new Hs::HsWidgetPublisher( this );
	
	std::string contentType( "ct" );
	std::string identifier( "id" );
	std::string widgetName( "wn" );
	
	TInt reason( KErrNone );
	try
		{
		// this should throw an exception with KAlreadyExists reason
		AddExistingTemplate( mHSPApi, contentType, identifier, widgetName );
		}
	catch ( Hs::HsException& e )
		{
		reason = e.getReason();
		}
	
	if ( KErrAlreadyExists != reason )
		{
		err = reason;
		}
	
	delete mHSPApi;
	return err;
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt CTestHspApi::TestHspApiDeleteWidgetL( CStifItemParser& aItem )
	{
	TInt err( KErrNone );
	
	Hs::HsWidgetPublisher* mHSPApi = new Hs::HsWidgetPublisher( this );
	
	std::string contentType( "ct" );
	std::string identifier( "id" );
	std::string widgetName( "wn" );
	
	AddTemplate( mHSPApi, contentType, identifier, widgetName );
	
	CLiwGenericParamList* res = GetListL( contentType, identifier, widgetName );
	TInt count = ExtractItemCountL( *res );
	
	if ( count != 4 )
		{
		err = KErrGeneral;
		}	
	
	res->Reset();
	
	mHSPApi->removeHsWidget( contentType, widgetName, identifier );
	
	res = GetListL( contentType, identifier, widgetName );
	count = ExtractItemCountL( *res );
	res->Reset();
	
	if ( count != KErrNotFound )
		{
		err = KErrGeneral;
		}
	
	delete mHSPApi;

	return err;
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt CTestHspApi::TestHspApiSetDataObserver1L( CStifItemParser& aItem )
	{
	TInt err( KErrNone );
	
	Hs::HsWidgetPublisher* mHSPApi = new Hs::HsWidgetPublisher( this );
	
	std::string contentType( "ct" );
	std::string identifier( "id" );
	std::string widgetName( "wn" );
	
	AddTemplate( mHSPApi, contentType, identifier, widgetName );
	
	ExecuteActionL( contentType, identifier, widgetName );
	
	WaitL( 1000000 );
	
	CLiwGenericParamList* res = GetListL( contentType, identifier, widgetName );
	TInt count = ExtractItemCountL( *res );
	
	if ( count != 4 )
		{
		err = KErrGeneral;
		}
	
	res->Reset();
	
	mHSPApi->removeHsWidget( contentType, widgetName, identifier );
	
	res = GetListL( contentType, identifier, widgetName );
	count = ExtractItemCountL( *res );
	res->Reset();
	
	if ( count != KErrNotFound )
		{
		err = KErrGeneral;
		}
	
	delete mHSPApi;

	return err;
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt CTestHspApi::TestHspApiSetDataObserver2L( CStifItemParser& aItem )
	{
	TInt err( KErrNone );
	
	Hs::HsWidgetPublisher* mHSPApi = new Hs::HsWidgetPublisher( this );
	
	std::string contentType( "ct" );
	std::string identifier( "id" );
	std::string widgetName( "wn" );
	
	AddTemplate( mHSPApi, contentType, identifier, widgetName );
	
	ExecuteActionSelectedL( contentType, identifier, widgetName );
	
	WaitL( 1000000 );
	
	CLiwGenericParamList* res = GetListL( contentType, identifier, widgetName );
	TInt count = ExtractItemCountL( *res );
	
	if ( count != 4 )
		{
		err = KErrGeneral;
		}
	
	res->Reset();
	
	mHSPApi->removeHsWidget( contentType, widgetName, identifier );
	
	res = GetListL( contentType, identifier, widgetName );
	count = ExtractItemCountL( *res );
	res->Reset();
	
	if ( count != KErrNotFound )
		{
		err = KErrGeneral;
		}
	
	delete mHSPApi;

	return err;
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt CTestHspApi::TestHspApiGetWidgetL( CStifItemParser& aItem )
	{
	TInt err( KErrNone );
	
	Hs::HsWidgetPublisher* mHSPApi = new Hs::HsWidgetPublisher( this );
	
	std::string contentType1( "ct1" );
	std::string identifier1( "id1" );
	std::string widgetName1( "wn1" );
	
	AddTemplate( mHSPApi, contentType1, identifier1, widgetName1 );
	
	Hs::HsWidget& templ1 = mHSPApi->getHsWidget( contentType1, widgetName1, identifier1 );
	
	if (! &templ1 )
		{
		err = KErrGeneral;
		}

	std::string contentType2( "ct2" );
	std::string identifier2( "id2" );
	std::string widgetName2( "wn2" );

	try
		{
		Hs::HsWidget& templ2 = mHSPApi->getHsWidget( contentType2, widgetName2, identifier2 );
		if ( &templ2 )
			{
			err = KErrCancel;
			}
		}
	catch ( ... )
		{
		
		}

	delete mHSPApi;

	return err;
	}


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt CTestHspApi::TestHspApiWidgetRemoveItem1L( CStifItemParser& aItem )
	{
	TInt err( KErrNone );
	
	Hs::HsWidgetPublisher* mHSPApi = new Hs::HsWidgetPublisher( this );
	
	std::string contentType( "ct" );
	std::string identifier( "id" );
	std::string widgetName( "wn" );
	
	AddAndRemoveTemplateItem1( mHSPApi, contentType, identifier, widgetName );
	
	CLiwGenericParamList* res = GetListL( contentType, identifier, widgetName );
	
	// check if item was really removed
	RBuf value;
	TInt ret = ExtractItemL( *res, _L8("item_name_3"), value );
	
	if ( ret != KErrNotFound )
		{
		err = ret;
		}

	delete mHSPApi;
	return err;
	}
	
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt CTestHspApi::TestHspApiWidgetRemoveItem2L( CStifItemParser& aItem )
	{
	TInt err( KErrNone );
	
	Hs::HsWidgetPublisher* mHSPApi = new Hs::HsWidgetPublisher( this );
	
	std::string contentType( "ct" );
	std::string identifier( "id" );
	std::string widgetName( "wn" );
	
	TInt reason( KErrNone );
	
	try
		{
		AddAndRemoveTemplateItem2( mHSPApi, contentType, identifier, widgetName );
		}
	catch( Hs::HsException& exception )
        {
        reason = exception.getReason();
        }
     
     if ( KErrNotFound != reason )
    	 {
    	 err = reason;
    	 }
		 
	delete mHSPApi;
	return err;
	}


// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CTestHspApi::ThrowHspException()
	{
	throw Hs::HsException( KErrGeneral );
	}

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
TInt CTestHspApi::TestHspApiExceptionL( CStifItemParser& aItem )
	{
	TInt err( KErrNone );
	TInt reason( KErrNone );
    try
        {
        ThrowHspException();
        }
     catch( Hs::HsException& exception )
        {
        reason = exception.getReason();
        }
     
     if ( KErrGeneral != reason )
    	 {
    	 err = reason;
    	 }
     return err;
	}
