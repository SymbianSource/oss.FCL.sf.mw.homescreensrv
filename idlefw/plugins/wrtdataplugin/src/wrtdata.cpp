/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
* Profile plug-in publisher
*
*/


// INCLUDE FILES
#include <ecom/ecom.h>
#include <liwservicehandler.h>
#include <aipluginsettings.h>
#include <apgcli.h>
#include <apgicnfl.h> 
#include <bautils.h> 
#include <aicpscommandbuffer.h>

#include "wrtdata.h"
#include "wrtdatapluginconst.h"
#include "wrtdataobserver.h"
#include "wrtdataplugin.h"

// ======== MEMBER FUNCTIONS ========
	
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------------------------
//
CWrtData* CWrtData::NewL(CWrtDataPlugin* aPlugin)
    {
    CWrtData* self = new (ELeave) CWrtData();
    CleanupStack::PushL( self );
    self->ConstructL(aPlugin);
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CWrtData::CWrtData()
    : iAppUid( KNullUid )
    {
    }
    
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------------------------
//
void CWrtData::ConstructL(CWrtDataPlugin* aPlugin)
    { 
     iPlugin = aPlugin;
    }
    
// ---------------------------------------------------------------------------
// Destructor
// Deletes all data created to heap
// ---------------------------------------------------------------------------
//
CWrtData::~CWrtData()
    {
    if(iObserver)
        {      
        delete iObserver;
        iObserver = NULL;
        }
    if ( iContentId )
    	{
    	delete iContentId;
    	iContentId = NULL;
    	}
    // not owned
    iInterface = NULL;
    iServiceHandler = NULL;
    iCpsExecute = NULL;
    iPlugin = NULL;
    }

// ---------------------------------------------------------------------------
// CWrtData::ConfigureL
// ---------------------------------------------------------------------------
//
void CWrtData::ConfigureL(RAiSettingsItemArray& aConfigurations )
    {
    TInt count = aConfigurations.Count();
    for(TInt i = 0; i<count; i++ )
       {
       MAiPluginConfigurationItem& confItem = ( aConfigurations[i] )->AiPluginConfigurationItem();
       // if owner is plugin then it (key,value) is for plugin configurations items
       if(confItem.Owner() == KPlugin() && confItem.Name() == KPubData())
           {
           iContentId = confItem.Value().AllocL();
		   }
       }
    if( iContentId->Des().Length() == 0 ) 
        {
        // No service to offer without plugin configurations 
        User::Leave( KErrNotSupported );
        }
    iObserver = CWrtDataObserver::NewL( iInterface, this );   
    }

// ---------------------------------------------------------------------------
// CWrtData::RegisterL
// ---------------------------------------------------------------------------
//
void CWrtData::RegisterL()
    {
    CLiwDefaultMap* filter = CreateFilterLC();
    filter->InsertL( KOperation, TLiwVariant( KAddUpdateDelete ) );
    iObserver->RegisterL(filter);
    CleanupStack::PopAndDestroy( filter );
    }

// ---------------------------------------------------------------------------
// CWrtData::UpdatePublisherStatusL
// ---------------------------------------------------------------------------
//
void CWrtData::UpdatePublisherStatusL()
    {
     // Resent the plugin status to publisher
    CLiwDefaultList* actions= CLiwDefaultList::NewLC();
    actions->AppendL( TLiwVariant( KActive ));
     if ( iPlugin->IsActive() )
         {
         actions->AppendL( TLiwVariant( KResume ) );
         }
     else
         {
         actions->AppendL( TLiwVariant( KSuspend ));
         }
      // forward the network status if it uses.
    if ( iPlugin->NetworkStatus() == CWrtDataPlugin::EOnline )
        {
        actions->AppendL( TLiwVariant( KOnLine ));
        }
    else if ( iPlugin->NetworkStatus() == CWrtDataPlugin::EOffline )
        {
        actions->AppendL( TLiwVariant(  KOffLine));
        }

    ReSendNotificationL( actions );
    CleanupStack::PopAndDestroy( actions );
    }

// ---------------------------------------------------------------------------
// CWrtData::PublishInitialDataL
// ---------------------------------------------------------------------------
//
void CWrtData::PublishInitialDataL( MAiContentObserver* aObserver )
    {
    // Show loading animation
    iPlugin->ShowLoadingIcon( aObserver );

    TBuf<KWRTContentValueMaxLength> appName;
    TBuf<KWRTAppUidLenth> appUidStr;
    GetWidgetNameAndUidL( appName, appUidStr );

    // Publish widget's name
    if ( appName.Length() > 0 )
       {
       iPlugin->PublishTextL( aObserver, CWrtDataPlugin::EDefaultText, appName );
       }

    // Publish widget's apparc image. This might fail if there is application
    // list population ongoing in AppFW and then we have to try again later
    if ( ResolveUid ( appUidStr, iAppUid ) )
        {
        TRAPD( err, PublishDefaultImageL( aObserver ) );
        if ( KErrNone != err )
            {
            iPlugin->StartTimer();
            }
        }
    }

// ---------------------------------------------------------------------------
// CWrtData::PublishDefaultImageL
// ---------------------------------------------------------------------------
//
void CWrtData::PublishDefaultImageL( MAiContentObserver* aObserver )
    {
    // Publish widget's apparc image
    TInt handle = KErrNotFound;
    TInt mask = KErrNotFound;
    // create icon from application UID
    CreateIconFromUidL( handle, mask, iAppUid );
    // Publish apparc image 
    iPlugin->PublishImageL( aObserver,
                                CWrtDataPlugin::EDefaultImage,
                                handle,
                                mask );
    }


// ---------------------------------------------------------------------------
// CWrtData::PublishL
// ---------------------------------------------------------------------------
//
void CWrtData::PublishL( MAiContentObserver* aObserver, CLiwDefaultMap* aDataMap ) 
    {
    TLiwVariant variant;
    iPlugin->HideLoadingIcon(aObserver);
    if ( aDataMap->FindL( KImage1, variant ) )
        {
        TInt handle = KErrBadHandle;
        TUint uintHandle = 0;
        TPtrC16 valPtr;
        if ( variant.Get( uintHandle ) )
            {
            handle = uintHandle;
            }
        else if ( !variant.Get( handle ) )
            {
            handle = KErrBadHandle;
            }
        // read as a image handle
        if( handle == KErrBadHandle )
            {
            // no handle, so read as image path
            variant.Get( valPtr );
            iPlugin->PublishImageL(aObserver, CWrtDataPlugin::EImage1, valPtr );
            }
        else
            {
            TInt maskHandle = KErrBadHandle;
            //Look for image mask
            if ( aDataMap->FindL( KImageMask, variant ) )
               {
               variant.Get( maskHandle );                           
               }
            iPlugin->PublishImageL(aObserver, CWrtDataPlugin::EImage1, handle, maskHandle );
            }
        }
    variant.Reset();
    }

// ---------------------------------------------------------------------------
// CWrtData::RefreshL
// ---------------------------------------------------------------------------
//
void CWrtData::RefreshL( TDesC& aContentId, TDesC& aOperation, 
        CLiwDefaultMap* aDataMap )
    {
     if ( aContentId == iContentId )
         {
         iPlugin->RefreshL( aOperation, aDataMap);
         }
    }

// ---------------------------------------------------------------------------
// CWrtData::ExecuteActionL
// ---------------------------------------------------------------------------
//
void CWrtData::ExecuteActionL(const TDesC& aObjectId, const TDesC& aTrigger )
   {
   HBufC8* triggerName = HBufC8::NewLC( KWRTContentNameMaxLength );
  
   CLiwGenericParamList* inParamList  = &iServiceHandler->InParamListL();
   CLiwGenericParamList* outParamList = &iServiceHandler->OutParamListL();

   // use the first item configuration to create the filter
   CLiwDefaultMap* filter  = CreateFilterLC();

   triggerName->Des().Copy(aTrigger);
   if ( aObjectId == KPubData )
       {
       // this trigger belongs to publisher registery.
       // in such case it is assumed that all the items in the widgets
       // belongs to same publisher, type and id.
       TLiwGenericParam cptype( KType, TLiwVariant( KPubData ) );
       inParamList->AppendL( cptype );
       cptype.Reset();
       }
   else
       {
        //append type to inparam list
       TLiwGenericParam cptype( KType, TLiwVariant( KCpData ) );
       inParamList->AppendL( cptype );
       cptype.Reset();
       }

    filter->InsertL( KActionTrigger, TLiwVariant( triggerName->Des() ) );
   //append filter to input param
    TLiwGenericParam item( KFilter, TLiwVariant( filter ) );
    inParamList->AppendL( item );
    iInterface->ExecuteCmdL( KExecuteAction,  *inParamList, *outParamList );
    
    item.Reset();
    CleanupStack::PopAndDestroy( filter );
    CleanupStack::PopAndDestroy( triggerName );
    outParamList->Reset();
    inParamList->Reset();
    }

// ---------------------------------------------------------------------------
// CWrtData::IsPluginActive
// ---------------------------------------------------------------------------
//
TBool CWrtData::IsPluginActive()
    {
    return iPlugin->IsActive();
    }

// ---------------------------------------------------------------------------
// CWrtData::CreateFilterL
// ---------------------------------------------------------------------------
//
CLiwDefaultMap* CWrtData::CreateFilterLC()
    {
    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
    filter->InsertL( KPublisherId, TLiwVariant( KWRTPublisher ));
    filter->InsertL( KContentType, TLiwVariant( KTemplateWidget ));
    filter->InsertL( KContentId, TLiwVariant( iContentId ));
    return filter;
    }

// ---------------------------------------------------------------------------
// CWrtData::ExecuteCommandL
// ---------------------------------------------------------------------------
//
void CWrtData::ExecuteCommandL(CLiwDefaultMap* aInFilter, CLiwDefaultMap* aOutDataMap, const TDesC16& aRegistry  )
    {
    CLiwGenericParamList* inParamList  = &iServiceHandler->InParamListL();
    CLiwGenericParamList* outParamList = &iServiceHandler->OutParamListL();
    
    TLiwGenericParam type( KType, TLiwVariant( aRegistry ) );
    inParamList->AppendL( type );
    
    //append filter to input param
     TLiwGenericParam item( KFilter, TLiwVariant( aInFilter ));
     inParamList->AppendL( item );
     
    // execute service.It is assumed that iInterface is already initiatedd
    if(iInterface)
        {
        iInterface->ExecuteCmdL( KGetList, *inParamList, *outParamList);
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    type.Reset();
    item.Reset();
    inParamList->Reset();
    
    //extracts data map
    TInt pos = 0;
    outParamList->FindFirst( pos, KResults );
    if( pos != KErrNotFound )
        // results present
        {
        //extract iterator on results list
        TLiwVariant variant = (*outParamList)[pos].Value();
        CLiwIterable* iterable = variant.AsIterable();
        iterable->Reset();
        
        //get next result
        if( iterable->NextL( variant ) )
            {
            //extract content map
            CLiwDefaultMap *map = CLiwDefaultMap::NewLC();
            variant.Get( *map );
            if( map->FindL( KDataMap, variant) )
                {
                 variant.Get( *aOutDataMap );
                }
            CleanupStack::PopAndDestroy( map );
            }
        iterable->Reset();
        variant.Reset();
        }
    outParamList->Reset();
    }

// ---------------------------------------------------------------------------
// NotifyPublisherL
// ---------------------------------------------------------------------------
//
void CWrtData::NotifyPublisherL(const TDesC8& aStatus)
    {
    if( iCpsExecute == NULL )
        {
        User::Leave( KErrNotSupported );
        }
    
    CLiwDefaultMap* filter = CreateFilterLC();
    // Add execute command triggers. Idle framework will execute 
    iCpsExecute->AddCommand( iPluginId, KPubData, filter, aStatus );
    CleanupStack::PopAndDestroy( filter );
    }

// ---------------------------------------------------------------------------
// CWrtData::GetWidgetNameAndUidL
// ---------------------------------------------------------------------------
//
void CWrtData::GetWidgetNameAndUidL(TDes& aName, TDes& aAppUID )
    {
    CLiwDefaultMap *outDataMap = CLiwDefaultMap::NewLC();
    //Create filter criteria for requested entries in form of LIW map:
    CLiwDefaultMap* filter = CreateFilterLC();
    ExecuteCommandL( filter, outDataMap, KPubData  );
    CleanupStack::PopAndDestroy( filter );
  
    TLiwVariant variant;
    if ( outDataMap->FindL(KWidgetInfo, variant) )
        {
        CLiwDefaultMap* widgetInfoMap = CLiwDefaultMap::NewLC();
        variant.Get( *widgetInfoMap );
        variant.Reset();
        if ( widgetInfoMap->FindL( KWidgetName, variant ) )
            {
            aName.Copy(variant.AsDes());
            variant.Reset();
            if ( widgetInfoMap->FindL( KWidgetIcon, variant ) )
               {
               aAppUID.Copy(variant.AsDes());
               }
            }
        CleanupStack::PopAndDestroy( widgetInfoMap );
        }
    variant.Reset();
    CleanupStack::PopAndDestroy( outDataMap );
    }

// ---------------------------------------------------------------------------
// CWrtData::ResolveUid
// ---------------------------------------------------------------------------
//
TBool CWrtData::ResolveUid(const TDesC& aUidDes, TUid& aUid )
    {
    // Syntax: uid(0x12345678)
    TInt error = KErrNotFound;
    TInt pos = aUidDes.FindF( KUid );
    if( pos == 0 )
        {
        // Skip skin token
        pos += KUid().Length();

        // Initialize lexer
        TLex lex( aUidDes.Mid( pos ) );

        // Check left parenthesis
        if ( lex.Get() == KLeftParenthesis )
            {
            lex.SkipSpaceAndMark();
            lex.SkipCharacters();
            
            TPtrC mtoken = lex.MarkedToken();
            pos = mtoken.FindF( KHexPrefix );
            if ( pos == 0 )
                {
                TLex lex( mtoken.Mid( KHexPrefix().Length() ) );
                TUint id = 0;
                error = lex.Val( id, EHex );
                aUid = TUid::Uid( (TInt)id );
                }
            else
                {
                TInt id( 0 );
                error = lex.Val( id );
                aUid.iUid = id;
                }
            }
        }
    return ( error == KErrNone );
    }

// ---------------------------------------------------------------------------
// CWrtData::CreateIconFromUidL
// ---------------------------------------------------------------------------
//
void CWrtData::CreateIconFromUidL( TInt& aHandle, TInt& aMaskHandle, const TUid& aAppUid ) 
    {
    RApaLsSession lsSession;
    User::LeaveIfError( lsSession.Connect() );
    CleanupClosePushL( lsSession ); // lsSession (1)
    
    CArrayFixFlat<TSize>* sizeArray = new(ELeave) CArrayFixFlat<TSize>( 5 );
    CleanupStack::PushL( sizeArray );

    User::LeaveIfError( lsSession.GetAppIconSizes( aAppUid, *sizeArray ) );

    if ( sizeArray->Count() ) 
        {
        // There are other icon sizes
        TInt idx = 0;
        TInt size( sizeArray->At(idx).iWidth * sizeArray->At(idx).iHeight );
        for ( TInt i = 1; i < sizeArray->Count(); i++ ) 
            {
            if ( ( sizeArray->At(i).iWidth * sizeArray->At(i).iHeight ) > size )
                {
                idx = i;
                size =  sizeArray->At(idx).iWidth * sizeArray->At(idx).iHeight;
                }
            }

        CApaMaskedBitmap* appBitMap = CApaMaskedBitmap::NewLC();
        User::LeaveIfError( lsSession.GetAppIcon( aAppUid, sizeArray->At(idx), *appBitMap ) );
        aHandle = appBitMap->Handle();
        aMaskHandle = appBitMap->Mask()->Handle();
        CleanupStack::PopAndDestroy( appBitMap );
        }

    CleanupStack::PopAndDestroy( sizeArray );
    CleanupStack::PopAndDestroy( &lsSession );
    }


// ---------------------------------------------------------------------------
// ReSendNotificationL
// ---------------------------------------------------------------------------
//
void CWrtData::ReSendNotificationL(CLiwDefaultList* aActionsList)
    {
    if( iInterface == NULL )
        {
        User::Leave( KErrNotSupported );
        }
    
    CLiwGenericParamList* inParamList  = &iServiceHandler->InParamListL();
    CLiwGenericParamList* outParamList = &iServiceHandler->OutParamListL();

    TLiwGenericParam type( KType, TLiwVariant( KPubData ) );
    inParamList->AppendL( type );
     
    CLiwDefaultMap* filter = CreateFilterLC();
    // add list of action triggers to execute
    filter->InsertL(KActionTrigger, TLiwVariant(aActionsList) );
    
    TLiwGenericParam item( KFilter, TLiwVariant( filter ));
    inParamList->AppendL( item );
    iInterface->ExecuteCmdL( KExecuteAction, *inParamList, *outParamList);
    CleanupStack::PopAndDestroy( filter );
    outParamList->Reset();
    inParamList->Reset();

    }

// ---------------------------------------------------------------------------
// SetCommandBuffer
// ---------------------------------------------------------------------------
//
void CWrtData::SetCommandBuffer(TAny* aAny, const TDesC8& aNameSpace )
    {
    iPluginId.Copy(aNameSpace);
    iCpsExecute = reinterpret_cast <MAiCpsCommandBuffer* > ( aAny );
    if ( iCpsExecute )
        {
        iInterface = iCpsExecute->CpsInterface();
        iServiceHandler = iCpsExecute->ServiceHandler();
        }
    }

// End of file

