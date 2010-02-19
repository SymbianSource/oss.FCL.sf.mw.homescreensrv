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
    {
    }
    
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------------------------
//
void CWrtData::ConstructL(CWrtDataPlugin* aPlugin)
    { 
     iPlugin = aPlugin;
     iCommandName = HBufC8::NewL( KWRTContentValueMaxLength );
    }
    
// ---------------------------------------------------------------------------
// Destructor
// Deletes all data created to heap
// ---------------------------------------------------------------------------
//
CWrtData::~CWrtData()
    {
    if( iCommandName )
    	{
    	delete iCommandName;
    	iCommandName = NULL;
    	}
    if(iObserver)
        {
        TRAP_IGNORE(iObserver->ReleaseL() );
        delete iObserver;
        iObserver = NULL;
        }
    if( iInterface )
         {
         iInterface->Close();
         iInterface = NULL;
         }
    if( iServiceHandler )
         {
         iServiceHandler->Reset();
         delete iServiceHandler;
         iServiceHandler = NULL;
         }
    if ( iContentId )
    	{
    	delete iContentId;
    	iContentId = NULL;
    	}
    iMenuItems.ResetAndDestroy();
    iMenuTriggers.ResetAndDestroy();
    // not owned
    iPlugin = NULL;
    }

// ---------------------------------------------------------------------------
// CWrtData::ConfigureL
// ---------------------------------------------------------------------------
//
void CWrtData::ConfigureL(RAiSettingsItemArray& aConfigurations )
    {
    HBufC8* serviceName = HBufC8::NewLC( KWRTContentValueMaxLength );
    HBufC8* interfaceName = HBufC8::NewLC( KWRTContentValueMaxLength );
    
    // Interface name 
    RCriteriaArray criteriaArray;
    
    TInt count = aConfigurations.Count();
   
    for(TInt i = 0;i<count;i++)
       {
       MAiPluginConfigurationItem& confItem = ( aConfigurations[i] )->AiPluginConfigurationItem();
       // if owner is plugin then it (key,value) is for plugin configurations items
       if(confItem.Owner() == KPlugin())
           {
           if(confItem.Name() ==  KService())
               {
               serviceName->Des().Copy(confItem.Value());
               }
           else if( confItem.Name() == KInterface() )
                 {
                 interfaceName->Des().Copy(confItem.Value());
                 }
           else if( confItem.Name() == KCommand() )
                 {
                 iCommandName->Des().Copy(confItem.Value());
                 }
           else if( confItem.Name() == KMenuItem16() )
                 {
                 iMenuItems.AppendL( confItem.Value().AllocL() );
                 }
           else if( confItem.Name() == KPubData() )
				  {
				  iContentId = confItem.Value().AllocL();
				  }
           }
       }
    
    if( !( serviceName->Des().Length() >= 0 && interfaceName->Des().Length() >= 0  
            && iCommandName->Des().Length() >= 0 ) )
        {
        // No service to offer without plugin configurations 
        User::Leave( KErrNotSupported );
        }
    
    iServiceHandler = CLiwServiceHandler::NewL(); 

    // for convenience keep pointers to Service Handler param lists 
    CLiwGenericParamList* inParamList  = &iServiceHandler->InParamListL();
    CLiwGenericParamList* outParamList = &iServiceHandler->OutParamListL();

    CLiwCriteriaItem* criteriaItem = CLiwCriteriaItem::NewLC( KLiwCmdAsStr, *interfaceName , *serviceName );
    criteriaItem->SetServiceClass( TUid::Uid( KLiwClassBase ) );
    criteriaArray.AppendL( criteriaItem );


    // attach Liw criteria
     iServiceHandler->AttachL( criteriaArray );
     iServiceHandler->ExecuteServiceCmdL( *criteriaItem, *inParamList, *outParamList );

     CleanupStack::PopAndDestroy(criteriaItem);
     criteriaArray.Reset();

    // extract CPS interface from output params
    TInt pos( 0 );
    outParamList->FindFirst( pos, *interfaceName );
    if( pos != KErrNotFound )
        {
        //iInterface is MLiwInterface*
        iInterface = (*outParamList)[pos].Value().AsInterface(); 
        User::LeaveIfNull( iInterface );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    inParamList->Reset();
    outParamList->Reset();
    CleanupStack::PopAndDestroy( interfaceName );
    CleanupStack::PopAndDestroy( serviceName );

	//Gets the menu items from the publisher registry    
    GetMenuItemsL();
 
    iObserver = CWrtDataObserver::NewL( iInterface, this );   
    }

// ---------------------------------------------------------------------------
// CWrtData::HasMenuItem
// ---------------------------------------------------------------------------
//
TBool CWrtData::HasMenuItem(const TDesC16& aMenuItem )
    {
    TBool found = EFalse;
    for (TInt i = 0; i < iMenuItems.Count(); i++ )
        {
        if( aMenuItem == iMenuItems[i] )
            {
            found =  ETrue;
            break;
            }
        }
    return found;
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
     ActivateL();
     if ( iPlugin->IsActive() )
         {
         ResumeL();
         }
     else
         {
         SuspendL();
         }
      // forward the network status if it uses.
    if ( iPlugin->NetworkStatus() == CWrtDataPlugin::EOnline )
        {
        OnLineL();
        }
    else if ( iPlugin->NetworkStatus() == CWrtDataPlugin::EOffline )
        {
        OffLineL();
        }
    }

// ---------------------------------------------------------------------------
// CWrtData::PublishDefaultImageL
// ---------------------------------------------------------------------------
//
void CWrtData::PublishDefaultImageL( MAiContentObserver* aObserver )
    {
    TBuf<KWRTAppUidLenth> appUidStr;
    TBuf<KWRTContentValueMaxLength> appName;
    GetWidgetNameAndUidL( appName, appUidStr );
    
    TUid appUid;
    if ( ResolveUid (appUidStr, appUid ) )
        {
#ifdef WRT_PREDEFINED_IMAGE         
        RFs rfs;
        User::LeaveIfError( rfs.Connect() );

        TFileName privatePath;
        rfs.PrivatePath(privatePath);
        privatePath.Insert(0,KDrive);
        privatePath.Append( KImgFolder );
      
        appUidStr.Copy( appUid.Name());
        appUidStr.Delete(0,1);
        appUidStr.Delete( appUidStr.Length() -1, 1);
        privatePath.Append (appUidStr );
        privatePath.Append ( KJPEG );
        if ( BaflUtils::FileExists(rfs,privatePath) )
            {
            // Publish predefined jpeg image
            iPlugin->PublishImageL( aObserver, CWrtDataPlugin::EImage1,privatePath);
            }
        else
            {
            privatePath.Delete( privatePath.Length() - 4 , 4);
            privatePath.Append( KPNG );
            if ( BaflUtils::FileExists(rfs,privatePath) )
               {
               // Publish predefined image
               iPlugin->PublishImageL( aObserver, CWrtDataPlugin::EImage1,privatePath);
               }
            else
                {
#endif                 
                TInt handle = KErrNotFound;
                TInt mask = KErrNotFound;
                CreateIconFromUidL( handle, mask, appUid );
                // Publish widget apparc image
                iPlugin->PublishImageL( aObserver, CWrtDataPlugin::EDefaultImage,handle,mask);
                if ( appName.Length() > 0)
                   {
                   // Publish Widget Name
                   iPlugin->PublishTextL( aObserver, CWrtDataPlugin::EDefaultText, appName);
                   }
#ifdef WRT_PREDEFINED_IMAGE                
                }
            }
        rfs.Close();
#endif        
        }
    
    // Show loading animation
    iPlugin->ShowLoadingIcon(aObserver);
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
   CLiwDefaultMap* filter = NULL;

   triggerName->Des().Copy(aTrigger);
   if ( aObjectId == KPubData )
       {
       // this trigger belongs to publisher registery.
       // in such case it is assumed that all the items in the widgets
       // belongs to same publisher, type and id.
       TLiwGenericParam cptype( KType, TLiwVariant( KPubData ) );
       inParamList->AppendL( cptype );
       cptype.Reset();
       // use the first item configuration to create the filter
       filter = CreateFilterLC();
       }
   else
       {
       if ( aObjectId == KMenuItem16 )
           {
           TInt pos = KErrNotFound;
           for (TInt i = 0; i < iMenuItems.Count(); i++)
               {
               if ( aTrigger == iMenuItems[i] )
                   {
                   pos = i;
                   break;
                   }
               }
           if( pos == KErrNotFound )
               {
               // No such menu items
               CleanupStack::PopAndDestroy( triggerName );
               return; 
               }
              triggerName->Des().Copy( iMenuTriggers[pos]->Des() );
              filter = CreateFilterLC();
           }
       else
           {
           //Create filter criteria for requested entries in form of LIW map:
           filter = CreateFilterLC();
           }
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
    
    CleanupStack::PopAndDestroy( filter );
    CleanupStack::PopAndDestroy( triggerName );
    item.Reset();

    inParamList->Reset();
    outParamList->Reset();  
    
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
// CWrtData::ActivateL
// ---------------------------------------------------------------------------
//
void CWrtData::ActivateL()
    {
    ChangePublisherStatusL( KActive );
    }

// ---------------------------------------------------------------------------
// CWrtData::ResumeL
// ---------------------------------------------------------------------------
//
void CWrtData::ResumeL()
    {
    ChangePublisherStatusL( KResume );
    }

// ---------------------------------------------------------------------------
// CWrtData::SuspendL
// ---------------------------------------------------------------------------
//
void CWrtData::SuspendL()
    {
    ChangePublisherStatusL( KSuspend );
    }

// ---------------------------------------------------------------------------
// CWrtData::DeActivateL
// ---------------------------------------------------------------------------
//
void CWrtData::DeActivateL()
    {
    ChangePublisherStatusL( KDeActive );
    }

// ---------------------------------------------------------------------------
// CWrtData::OnLineL
// ---------------------------------------------------------------------------
//
void CWrtData::OnLineL()
    {
    ChangePublisherStatusL( KOnLine );
    }

// ---------------------------------------------------------------------------
// CWrtData::offLineL
// ---------------------------------------------------------------------------
//
void CWrtData::OffLineL()
    {
    ChangePublisherStatusL( KOffLine );
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
        iInterface->ExecuteCmdL( *iCommandName, *inParamList, *outParamList);
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
// CWrtData::PublisherStatusL
// ---------------------------------------------------------------------------
//
void CWrtData::ChangePublisherStatusL(const TDesC& aStatus)
    {
    if( iContentId == NULL )
       {
       return;
       }
   HBufC8* triggerName = HBufC8::NewLC(KWRTContentNameMaxLength);
   triggerName->Des().Copy(aStatus);
   
   CLiwGenericParamList* inParamList  = &iServiceHandler->InParamListL();
   CLiwGenericParamList* outParamList = &iServiceHandler->OutParamListL();
       
   TLiwGenericParam type( KType, TLiwVariant( KPubData ) );
   inParamList->AppendL( type );
              
   CLiwDefaultMap* filter = CreateFilterLC();
   filter->InsertL(KActionTrigger, TLiwVariant(triggerName->Des()) );
   
   TLiwGenericParam item( KFilter, TLiwVariant( filter ));
   inParamList->AppendL( item );
   
   if(iInterface)
       {
       iInterface->ExecuteCmdL( KExecuteAction, *inParamList, *outParamList);
       }
   else
       {
       User::Leave( KErrNotSupported );
       }
   CleanupStack::PopAndDestroy( filter );
   
   inParamList->Reset();
   outParamList->Reset();
   CleanupStack::PopAndDestroy( triggerName );
   }

// ---------------------------------------------------------------------------
// CWrtData::GetMenuItemsL
// ---------------------------------------------------------------------------
//
void CWrtData::GetMenuItemsL()
	{
	if(iInterface)
		{
		CLiwDefaultMap *outDataMap = CLiwDefaultMap::NewLC();
		CLiwDefaultMap* filter = CreateFilterLC( );
		//append filter to input param
        ExecuteCommandL( filter, outDataMap, KPubData  );
		CleanupStack::PopAndDestroy( filter );
		
		TLiwVariant variant;
		TInt pos = outDataMap->FindL( KMenuItems, variant ) ;
		
		if ( pos  )
			{
			CLiwDefaultMap *menuMap = CLiwDefaultMap::NewLC();
			variant.Get( *menuMap );
			for ( TInt i = 0; i < menuMap->Count(); i++)
				{
				menuMap->FindL(menuMap->AtL(i), variant );
				HBufC8* value = HBufC8::NewL( KWRTContentValueMaxLength );
				CleanupStack::PushL( value );
				TPtr8 valPtr = value->Des();
				variant.Get( valPtr);
				if ( valPtr.Length() > 0 )
					{
					iMenuTriggers.AppendL( value );
					CleanupStack::Pop( value );
					HBufC16* triggerName = HBufC16::NewLC( KWRTContentNameMaxLength );
				    triggerName->Des().Copy( menuMap->AtL(i) );
					iMenuItems.AppendL( triggerName );
					CleanupStack::Pop( triggerName );
					}
				else
					{
					CleanupStack::PopAndDestroy( value );	
					}
				variant.Reset();
				}
			CleanupStack::PopAndDestroy( menuMap );
			}
		variant.Reset();
		CleanupStack::PopAndDestroy( outDataMap );
		}
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
    return (error == KErrNone );
    }

// ---------------------------------------------------------------------------
// CWrtData::CreateIconFromUidL
// ---------------------------------------------------------------------------
//
void CWrtData::CreateIconFromUidL(TInt& aHandle, TInt& aMaskHandle, const TUid& aAppUid ) 
    {
    RApaLsSession lsSession;
    User::LeaveIfError( lsSession.Connect() );
    CleanupClosePushL( lsSession ); // lsSession (1)
    
    CArrayFixFlat<TSize>* sizeArray = new(ELeave) CArrayFixFlat<TSize>( 5 );
    CleanupStack::PushL( sizeArray );
    if ( KErrNone == lsSession.GetAppIconSizes(aAppUid, *sizeArray) )
        {
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
            if ( KErrNone == lsSession.GetAppIcon( aAppUid, sizeArray->At(idx),
                    *appBitMap ) )
                {
                aHandle = appBitMap->Handle();
                aMaskHandle = appBitMap->Mask()->Handle();
                }
            CleanupStack::PopAndDestroy( appBitMap );
            }
        }
    CleanupStack::PopAndDestroy( sizeArray );
    CleanupStack::PopAndDestroy( &lsSession );
    }

