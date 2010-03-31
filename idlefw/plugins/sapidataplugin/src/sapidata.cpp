/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Homescreen Data plug-in publisher
*
*/

// System includes
#include <ecom/ecom.h>
#include <liwservicehandler.h>
#include <aipluginsettings.h>
#include <utf.h>

// User includes
#include <aicpscommandbuffer.h>
#include "sapidata.h"
#include "sapidatapluginconst.h"
#include "sapidataobserver.h"
#include "sapidataplugin.h"

const TUint KDisableNotification = 0x2000;
// ======== MEMBER FUNCTIONS ========
	
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------------------------
//
CContentItem* CContentItem::NewL()
    {
    CContentItem* self = new (ELeave) CContentItem();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CContentItem::CContentItem()
    {
    }
    
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------------------------
//
void CContentItem::ConstructL()
    { 
    iId = KErrNotFound;
    iName = NULL;
	iContentType = NULL;
	iType = NULL;
    }

// ---------------------------------------------------------------------------
// Destructor
// Deletes all data created to heap
// ---------------------------------------------------------------------------
//
CContentItem::~CContentItem()
	{
	delete iName;
	delete iContentType;
	delete iType;
	}
	
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------------------------
//
CSapiData* CSapiData::NewL(CSapiDataPlugin* aPlugin)
    {
    CSapiData* self = new (ELeave) CSapiData();
    CleanupStack::PushL( self );
    self->ConstructL(aPlugin);
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
CSapiData::CSapiData()
    {
    }
    
// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave
// ---------------------------------------------------------------------------
//
void CSapiData::ConstructL(CSapiDataPlugin* aPlugin)
    { 
     iPlugin = aPlugin;
	 iUpdateNeeded = EFalse;
    }
    
// ---------------------------------------------------------------------------
// Destructor
// Deletes all data created to heap
// ---------------------------------------------------------------------------
//
CSapiData::~CSapiData()
    {
    delete iCommandName;
	delete iPublisher;
	delete iContentType;
	delete iContentId;
	delete iStartupReason;
	
	if(iPubObserver)
		{
		TRAP_IGNORE(iPubObserver->ReleaseL() );
		delete iPubObserver;
		iPubObserver = NULL;
		}
    if(iContentObserver)
        {
        TRAP_IGNORE(iContentObserver->ReleaseL() );
        delete iContentObserver;
        iContentObserver = NULL;
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
    iCpsExecute = NULL;
    iMenuItems.ResetAndDestroy();
    iMenuTriggers.ResetAndDestroy();
    iItemList.ResetAndDestroy();
    // not owned
    iPlugin = NULL;
    }

// ---------------------------------------------------------------------------
// ConfigureL
// ---------------------------------------------------------------------------
//
void CSapiData::ConfigureL(RAiSettingsItemArray& aConfigurations )
    {
    HBufC8* serviceName = NULL;
    HBufC8* interfaceName = NULL;

    TInt count = aConfigurations.Count();
    
    for(TInt i = 0;i<count;i++)
       {
       MAiPluginConfigurationItem& confItem = ( aConfigurations[i] )->AiPluginConfigurationItem();
       // if owner is plugin then it (key,value) is for plugin configurations items
       if(confItem.Owner() == KPlugin())
           {
           if(confItem.Name() ==  KService())
               {
               serviceName = CnvUtfConverter::ConvertFromUnicodeToUtf8L(confItem.Value());
               }
           else if( confItem.Name() == KInterface() )
                 {
                 interfaceName = CnvUtfConverter::ConvertFromUnicodeToUtf8L(confItem.Value());
                 }
           else if( confItem.Name() == KCommand() )
                 {
                 iCommandName  = CnvUtfConverter::ConvertFromUnicodeToUtf8L(confItem.Value());
                 }
           else if( confItem.Name()  == KPublisher16() ) 
				  {
				  iPublisher = confItem.Value().AllocL();
				  }
           }
       else if ( confItem.Name() == KContentType16() )
           { 
           HBufC* objectId = confItem.Owner().AllocLC();
           objectId->Des().Delete(0, objectId->LocateReverse(KPluginNameSeprator) + 1);
           
           CContentItem *conItem = CContentItem::NewL();
           conItem->iName = objectId->AllocL();
           conItem->iContentType =  confItem.Value().AllocL();
           conItem->iType = iPlugin->GetTypeL( *objectId ).AllocL();
           conItem->iId = iPlugin->GetIdL(*objectId  );
           iItemList.AppendL( conItem );
           
           CleanupStack::PopAndDestroy(objectId);
           }
       }
    
    iItemCount = iItemList.Count();  
    
    if( !serviceName || !interfaceName || !iCommandName  
    		|| !iContentId || !iPublisher || !iItemCount )
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
    // Interface name 
    RCriteriaArray criteriaArray;
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
    delete interfaceName;
    delete serviceName;

	//Gets the menu items from the publisher registry    
    GetMenuItemsL();
 
    iContentObserver = CSapiDataObserver::NewL( iInterface, this );   
    iPubObserver = CSapiDataObserver::NewL( iInterface, this );
    }

// ---------------------------------------------------------------------------
// SetContentIdL
// ---------------------------------------------------------------------------
//
void CSapiData::SetContentIdL(const TDesC8& aId)
	{
	iContentId = CnvUtfConverter::ConvertToUnicodeFromUtf8L(aId);
	}

// ---------------------------------------------------------------------------
// SetStartupReasonL
// ---------------------------------------------------------------------------
//
void CSapiData::SetStartupReasonL(const TDesC8& aStartupReason)
    {
    delete iStartupReason;
    iStartupReason = NULL;
    iStartupReason = aStartupReason.AllocL();
    ChangePublisherStatusL( aStartupReason );
    }

// ---------------------------------------------------------------------------
// GetMenuItemsL
// ---------------------------------------------------------------------------
//
void CSapiData::GetMenuItemsL()
	{
	if(iInterface && iItemCount > 0)
		{
		CLiwGenericParamList* outParamList = &iServiceHandler->OutParamListL();
		CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
		filter->InsertL( KPublisherId, TLiwVariant(iPublisher ));
		//append filter to input param
        ExecuteCommandL( KPubData, filter, outParamList );
        CleanupStack::PopAndDestroy( filter );
    	//extracts data map
    	TInt pos = 0;
    	outParamList->FindFirst( pos, KResults );
    	if( pos != KErrNotFound )
    		// results present
    		{
    		//extract iterator on results list
    		TLiwVariant variant = (*outParamList)[pos].Value();
    		variant.PushL();
    		CLiwIterable* iterable = variant.AsIterable();
    		iterable->Reset();
    		
    	    CLiwDefaultMap *map = CLiwDefaultMap::NewLC();
    		//considering publisher is unique reading only first entry
    		if( iterable->NextL( variant ) )
    			{
    			//extract content map
    			variant.Get( *map );
    			if( map->FindL( KDataMap, variant) )
    				{
    				 variant.Get( *map );
    				}
    			}
    		iterable->Reset();
    		variant.Reset();
    		if (  map->FindL( KMenuItems, variant ) )
				{
				CLiwDefaultMap *menuMap = CLiwDefaultMap::NewLC();
				variant.Get( *menuMap );
				for ( TInt i = 0; i < menuMap->Count(); i++)
					{
					menuMap->FindL(menuMap->AtL(i), variant );
					HBufC8* menuItem = variant.AsData().AllocLC();
					if ( menuItem->Length()> 0 )
						{
						iMenuTriggers.AppendL( menuItem );
						CleanupStack::Pop( menuItem );
						HBufC* triggerName = CnvUtfConverter::ConvertToUnicodeFromUtf8L(menuMap->AtL(i));
						CleanupStack::PushL( triggerName );
						iMenuItems.AppendL( triggerName );
						CleanupStack::Pop( triggerName );
						}
					else
						{
						CleanupStack::PopAndDestroy( menuItem );	
						}
					variant.Reset();
					}
				CleanupStack::PopAndDestroy( menuMap );
				}
    		CleanupStack::PopAndDestroy( map );
    		CleanupStack::PopAndDestroy( &variant );
    		}
    	outParamList->Reset();
		}
	}

// ---------------------------------------------------------------------------
// CreateFilterL
// ---------------------------------------------------------------------------
//
CLiwDefaultMap* CSapiData::CreateFilterLC(const TDesC& aContentType,
		const TDesC& aContentId)
    {
    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
    filter->InsertL( KPublisherId, TLiwVariant(iPublisher ));
    filter->InsertL( KContentId, TLiwVariant(aContentId ));
    filter->InsertL( KContentType, TLiwVariant(aContentType ));
    return filter;
    }

// ---------------------------------------------------------------------------
// CreateFilterL
// ---------------------------------------------------------------------------
//
CLiwDefaultMap* CSapiData::CreateFilterLC(const TDesC& aContentType)
    {
    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
    filter->InsertL( KPublisherId, TLiwVariant(iPublisher ));
    filter->InsertL( KContentId, TLiwVariant(iContentId ));
    filter->InsertL( KContentType, TLiwVariant(aContentType ));
    return filter;
    }

// ---------------------------------------------------------------------------
// CreateFilterL
// ---------------------------------------------------------------------------
//
CLiwDefaultMap* CSapiData::CreateFilterLC()
    {
    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
    filter->InsertL( KPublisherId, TLiwVariant(iPublisher ));
    filter->InsertL( KContentId, TLiwVariant(iContentId ));
    filter->InsertL( KContentType, TLiwVariant(KAll));
    return filter;
    }


// ---------------------------------------------------------------------------
// Update
// ---------------------------------------------------------------------------
//
TBool CSapiData::CanUpdate( TDesC& aPublisher, TDesC& aContentType,
    		TDesC& aContentId)
	{
    TBool res = EFalse;
    if( aPublisher == *iPublisher  
	   && aContentId == *iContentId  )
	   {
	   if ( aContentType != KAll )
		   {
		   for (TInt i = 0; i < iItemCount ; i++)
			   {
			   if ( aContentType == iItemList[i]->iContentType )
				   {
				   res = ETrue;
				   break;
				   }
			   }
		   }
	   else
		   {
		   res = ETrue;
		   }
	   }
    else
	   {
	   res = EFalse;
	   }
    return res;
	}

// ---------------------------------------------------------------------------
// RemoveL
// ---------------------------------------------------------------------------
//
void CSapiData::RemoveL( MAiContentObserver* aObserver, TDesC& aContentType  )
	{
	for(TInt index = 0; index < iItemCount; index++)
		{
		if ( aContentType == iItemList[index]->iContentType )
			{
			iPlugin->Clean( aObserver, iItemList[index]->iId );
			}
		}
	}


// ---------------------------------------------------------------------------
// HasMenuItem
// ---------------------------------------------------------------------------
//
TBool CSapiData::HasMenuItem(const TDesC& aMenuItem )
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
// PublishL
// ---------------------------------------------------------------------------
//
void CSapiData::PublishL( MAiContentObserver* aObserver, const TDesC& aContentType ) 
    {
	CLiwGenericParamList* outParamList = &iServiceHandler->OutParamListL();
    //Create filter criteria for requested entries in form of LIW map:
    CLiwDefaultMap* filter = CreateFilterLC( aContentType );
    ExecuteCommandL( KCpData, filter, outParamList );
    CleanupStack::PopAndDestroy( filter );
    
	TInt pos = 0;
	outParamList->FindFirst( pos, KResults );
	if( pos != KErrNotFound )
		// results present
		{
		//extract iterator on results list
		TLiwVariant variant = (*outParamList)[pos].Value();
		variant.PushL();
		CLiwIterable* iterable = variant.AsIterable();
		iterable->Reset();
		CLiwDefaultMap *map = CLiwDefaultMap::NewLC();
		
		while( iterable->NextL( variant ) )
			{
			//extract content map
			if( variant.Get( *map ) &&
					// Find the data map 
					map->FindL( KDataMap, variant) )
				{
				 CLiwDefaultMap *datamap = CLiwDefaultMap::NewLC();
				 if ( variant.Get( *datamap ) )
					 {
					 PublishDataL(aObserver, datamap);
					 }
				 CleanupStack::PopAndDestroy( datamap );
				}
			}
		CleanupStack::PopAndDestroy( map );
		CleanupStack::PopAndDestroy( &variant );
		}
	outParamList->Reset();
    }

void CSapiData::PublishDataL(MAiContentObserver* aObserver, CLiwDefaultMap* aDataMap )
	{
    for(TInt pIndex = 0; pIndex < iItemCount; pIndex++)
	    {
	    // result name to find
		TLiwVariant variant;
		HBufC8* itemName = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*iItemList[pIndex]->iName);
		CleanupStack::PushL( itemName );
		if ( aDataMap->FindL( *itemName, variant ) )
		    {
			TPtrC valPtr;
			if(  iItemList[pIndex]->iType->Des() == KText )
				{
				valPtr.Set( variant.AsDes() );
				iPlugin->PublishTextL( aObserver, iItemList[pIndex]->iId, valPtr  );
				}
			else if( iItemList[pIndex]->iType->Des() == KImage )
				{
				TInt handle = KErrBadHandle;
                TUint uintHandle = 0;
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
					iPlugin->PublishImageL(aObserver, iItemList[pIndex]->iId, valPtr );
					}
				else
					{
					TInt maskHandle = KErrBadHandle;
					TUint uintmaskHandle = 0;
					//Look for image mask
					HBufC8* maskKey = HBufC8::NewLC( itemName->Length() + KMask().Length() );
					TPtr8 maskKeyPtr = maskKey->Des();
					maskKeyPtr.Append( *itemName );
					maskKeyPtr.Append( KMask );
					if ( aDataMap->FindL( maskKeyPtr, variant ) )
						{
                        if ( variant.Get( uintmaskHandle ) )
                            {
                            maskHandle = uintmaskHandle;
                            }
                        else if ( !variant.Get( maskHandle ) )
                            {
                            maskHandle = KErrBadHandle;
                            }                           
						}
					CleanupStack::PopAndDestroy( maskKey );
					iPlugin->PublishImageL(aObserver, iItemList[pIndex]->iId, handle, maskHandle );
					}
				}
		    }
			variant.Reset();
			CleanupStack::PopAndDestroy( itemName );
	    }
    }
// ---------------------------------------------------------------------------
// ExecuteCommandL
// ---------------------------------------------------------------------------
//
void CSapiData::ExecuteCommandL(const TDesC& aRegistry, CLiwDefaultMap* aInFilter, 
		CLiwGenericParamList* aOutParamList)
	{
    if( iInterface == NULL )
        {
        User::Leave( KErrNotSupported );
        }
    CLiwGenericParamList* inParamList  = &iServiceHandler->InParamListL();
    
    TLiwGenericParam type( KType, TLiwVariant( aRegistry ) );
    inParamList->AppendL( type );
    
    //append filter to input param
    TLiwGenericParam item( KFilter, TLiwVariant( aInFilter ));
    inParamList->AppendL( item );
    
    // execute service.It is assumed that iInterface is already initiated
    iInterface->ExecuteCmdL( *iCommandName, *inParamList, *aOutParamList);
    type.Reset();
    item.Reset();
    inParamList->Reset();
	}

// ---------------------------------------------------------------------------
// ExecuteActionL
// ---------------------------------------------------------------------------
//
void CSapiData::ExecuteActionL(const TDesC& aObjectId, const TDesC& aTrigger )
   {
    if( iInterface == NULL )
        {
        User::Leave( KErrNotSupported );
        }
   HBufC8* triggerName = HBufC8::NewLC( KSAPIContentNameMaxLength );
  
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
	   filter = CreateFilterLC( KWidget() );
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
			  filter = CreateFilterLC( KWidget() );
		   }
	   else
		   {
		   //Create filter criteria for requested entries in form of LIW map:
		   filter = CreateFilterLC( aObjectId );
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
// RegisterPublisherObserverL
// ---------------------------------------------------------------------------
//
void CSapiData::RegisterPublisherObserverL()
    {
    if ( iItemCount > 0)
    	{
		CLiwDefaultMap* pubRegFilter = CreateFilterLC( KAll(), KAll() );
		pubRegFilter->InsertL( KOperation, TLiwVariant( KUpdate ) );
		iPubObserver->RegisterL( pubRegFilter, KPubData() );
		CleanupStack::PopAndDestroy( pubRegFilter );
		}
    }

// ---------------------------------------------------------------------------
// RegisterContentObserverL
// ---------------------------------------------------------------------------
//
void CSapiData::RegisterContentObserverL()
    {
    if ( iItemCount > 0)
    	{
		CLiwDefaultMap* conRegFilter = CreateFilterLC();
		conRegFilter->InsertL( KOperation, TLiwVariant( KAddUpdateDelete ) );
		iContentObserver->RegisterL( conRegFilter, KCpData() );
		CleanupStack::PopAndDestroy( conRegFilter );
		}
    }

// ---------------------------------------------------------------------------
// RefreshL
// ---------------------------------------------------------------------------
//
void CSapiData::RefreshL( TDesC& aPublisher,
                          TDesC& aContentType, 
                          TDesC& aContentId,
                          TDesC& aOperation,
                          CLiwDefaultMap* aDataMap )
    {
     if ( CanUpdate( aPublisher, aContentType, aContentId ) )
    	 {
    	 iPlugin->RefreshL( aContentType, aOperation, aDataMap );
    	 }
    }

// ---------------------------------------------------------------------------
// IsPluginActive
// ---------------------------------------------------------------------------
//
TBool CSapiData::IsPluginActive()
    {
    return iPlugin->IsActive();
    }

// ---------------------------------------------------------------------------
// ChangePublisherStatusL
// ---------------------------------------------------------------------------
//
void CSapiData::ChangePublisherStatusL(const TDesC8& aStatus)
    {
    if( iCpsExecute == NULL )
        {
        User::Leave( KErrNotSupported );
        }

    if ( aStatus == KResume && iUpdateNeeded )
        {
        iPlugin->PublishL();
        iUpdateNeeded = EFalse;
        }
    CLiwDefaultMap* filter = CreateFilterLC( KWidget() );
    // Add execute command triggers. Idle framework will execute 
    iCpsExecute->AddCommand( *iContentId, KPubData, filter, aStatus );
    CleanupStack::PopAndDestroy( filter );
    
    }

// ---------------------------------------------------------------------------
// ChangePublisherStatusL
// ---------------------------------------------------------------------------
//
void CSapiData::ChangePublisherStatusL(CLiwDefaultList* aActionsList)
    {
    if( iInterface == NULL )
        {
        User::Leave( KErrNotSupported );
        }
    
    CLiwGenericParamList* inParamList  = &iServiceHandler->InParamListL();
    CLiwGenericParamList* outParamList = &iServiceHandler->OutParamListL();
    
    TLiwGenericParam pluginId( KPluginId, TLiwVariant( iContentId ) );
    inParamList->AppendL( pluginId );
    TLiwGenericParam type( KType, TLiwVariant( KPubData ) );
    inParamList->AppendL( type );
     
    CLiwDefaultMap* filter = CreateFilterLC( KWidget() );
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
// TriggerActiveL
// ---------------------------------------------------------------------------
//
void CSapiData::TriggerActiveL()
    {
    if(iInterface)
        {
        CLiwGenericParamList* inParamList  = &iServiceHandler->InParamListL();
        CLiwGenericParamList* outParamList = &iServiceHandler->OutParamListL();
        
        TLiwGenericParam type( KType, TLiwVariant( KPubData ) );
        inParamList->AppendL( type );
        
        CLiwDefaultMap* filter = CreateFilterLC( KAll(), KAll() );
        filter->InsertL(KActionTrigger, TLiwVariant( KActive() ));
        
        TLiwGenericParam item( KFilter, TLiwVariant( filter ));
        inParamList->AppendL( item );
        iInterface->ExecuteCmdL( KExecuteAction, *inParamList, *outParamList, KDisableNotification );
        
        CleanupStack::PopAndDestroy( filter );
        inParamList->Reset();
        outParamList->Reset();
       }
    else
       {
       User::Leave( KErrNotSupported );
       }
   }
// ---------------------------------------------------------------------------
// UpdatePublisherStatusL
// ---------------------------------------------------------------------------
//
void CSapiData::UpdatePublisherStatusL( TDesC& aPublisher )
	{
	 if ( aPublisher == iPublisher )
        {
       // Resend the plugin status to publisher
        CLiwDefaultList* actionsToLaunch = CLiwDefaultList::NewLC();
        actionsToLaunch->AppendL( TLiwVariant( KActive ));
         if( iStartupReason->Length() != 0 )
             {
             actionsToLaunch->AppendL( TLiwVariant( *iStartupReason ));
             }
		 if ( iPlugin->IsActive() )
			 {
		     actionsToLaunch->AppendL( TLiwVariant( KResume ));
			 }
		 else
			 {
		     actionsToLaunch->AppendL(TLiwVariant( KSuspend ));
			 }
		  // forward the network status if it uses.
		if ( iPlugin->NetworkStatus() == CSapiDataPlugin::EOnline )
			{
		    actionsToLaunch->AppendL(TLiwVariant( KOnLine ));
			}
		else if ( iPlugin->NetworkStatus() == CSapiDataPlugin::EOffline )
			{
		    actionsToLaunch->AppendL(TLiwVariant( KOffLine ));
			}
	 
	     ChangePublisherStatusL( actionsToLaunch );
	     CleanupStack::PopAndDestroy( actionsToLaunch );
		 }
	}

// ---------------------------------------------------------------------------
// ResolveSkinItemId
// ---------------------------------------------------------------------------
//
TBool CSapiData::ResolveSkinIdAndMifId( const TDesC& aPath, TAknsItemID& aItemId,
		TInt& abitmapId, TInt& aMaskId, TDes& aFilename )
   {
   // Syntax: skin( <major> <minor> ):mif(filename bimapId maskId) 
   TInt error = KErrNotFound;
   TInt pos = aPath.FindF( KSkin );
   if( pos != KErrNotFound )
	   {
	   // Skip skin token
	   pos += KSkin().Length();
	   
	   // Initialize lexer
	  TLex lex( aPath.Mid( pos ) );
	  lex.SkipSpace();
	   
	   // Check left parenthesis
	  if (lex.Get() == KLeftParenthesis )
		   {
		   //lex.SkipSpace();
		   
		   TInt majorId( 0 );        
		   TInt minorId( 0 );

		   // Resolve major id        
		   error = lex.Val( majorId );
		   
		   // Resolve minor id
		   lex.SkipSpace();
		   error |= lex.Val( minorId );
		   
		   // initilize skin item id object
		   aItemId.Set( majorId, minorId );
		   }
	   }

   if( (error == KErrNone && aPath.FindF( KColon ) != KErrNotFound ) 
		 || ( error == KErrNotFound ) )
	   {
	   error = KErrNotFound;
	   pos = aPath.FindF( KMif );
	   if ( pos != KErrNotFound )
		   {
		   pos += KMif().Length();
		   // Initialize lexer
		   TLex lex( aPath.Mid( pos ) );
		   lex.SkipSpace();
		   
		   // Check left parenthesis
		   if (lex.Get() == KLeftParenthesis )
			   {
			   lex.SkipSpaceAndMark();
			   lex.SkipCharacters();
			   // Resolve MifFile name
			   aFilename.Copy(lex.MarkedToken());
			   if( aFilename.Length()!= 0)
				   {
				   // Resolve major id  
				   lex.SkipSpace();
				   error = lex.Val( abitmapId );
				   
				   // Resolve minor id
				   lex.SkipSpace();
				   error |= lex.Val( aMaskId );
				   }
			   else
				   {
				   error = KErrNotFound;
				   }
			   }
		   }
	   }
   return (error == KErrNone );
   }

// ---------------------------------------------------------------------------
// SetUpdateNeeded
// ---------------------------------------------------------------------------
//
void CSapiData::SetUpdateNeeded(TBool aStatus)
	{
	iUpdateNeeded = aStatus;
	}

// ---------------------------------------------------------------------------
// SetCommandBuffer
// ---------------------------------------------------------------------------
//
void CSapiData::SetCommandBuffer(TAny* aAny)
    {
    iCpsExecute = reinterpret_cast <MAiCpsCommandBuffer* > ( aAny );
    }

// End of file
