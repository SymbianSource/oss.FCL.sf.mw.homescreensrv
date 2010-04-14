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
* Description:  Plug-in main class
*
*/


#include <liwservicehandler.h>
#include <liwvariant.h>
#include <liwgenericparam.h>
#include <s32mem.h>
#include "sapidata.h"
#include "sapidataobserver.h"
#include "sapidatapluginconst.h"
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CSapiDataObserver::CSapiDataObserver ()
    {

    }
        
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSapiDataObserver ::~CSapiDataObserver ()
    {
    iInterface = NULL;
    iData = NULL;
    }

// ---------------------------------------------------------------------------
// Register for notifications
// ---------------------------------------------------------------------------
//
void CSapiDataObserver::RegisterL( CLiwDefaultMap* aFilter, const TDesC& aRegistry )
    {
    CLiwGenericParamList* inParamList = CLiwGenericParamList::NewL();
    CleanupStack::PushL( inParamList );
    CLiwGenericParamList* outParamList = CLiwGenericParamList::NewL();
    CleanupStack::PushL( outParamList );
        
    // Fill in input list for RequestNotification command
    inParamList->AppendL(TLiwGenericParam(KType,TLiwVariant(aRegistry)));
    inParamList->AppendL(TLiwGenericParam(KFilter ,TLiwVariant(aFilter)));
        
    TRAP_IGNORE( iInterface->ExecuteCmdL(KRequestNotification,
                *inParamList,
                *outParamList,
                0,
                this ) );
    
    CleanupStack::PopAndDestroy( outParamList );
    CleanupStack::PopAndDestroy( inParamList ); 
    }

// ---------------------------------------------------------------------------
// Factory method construction
// ---------------------------------------------------------------------------
//
CSapiDataObserver * CSapiDataObserver::NewL( MLiwInterface* aInterface, CSapiData* aData )
    {
    CSapiDataObserver * self = new (ELeave) CSapiDataObserver();
    CleanupStack::PushL( self );
    self->ConstructL( aInterface, aData );
    CleanupStack::Pop( self );
    return self;    
    }
        
// ---------------------------------------------------------------------------
// 2n phase constructor
// ---------------------------------------------------------------------------
//
void CSapiDataObserver::ConstructL( MLiwInterface* aInterface, CSapiData* aData )
    {
    iData = aData;
    iInterface = aInterface;
    }

// ---------------------------------------------------------------------------
// Handles Published content notification
// ---------------------------------------------------------------------------
//
TInt CSapiDataObserver::HandleNotifyL(
        TInt aErrorCode,
        TInt /*aEventId*/,
        CLiwGenericParamList& aEventParamList,
        const CLiwGenericParamList& /*aInParamList*/ )
    {
     
    // Is plugin active to refresh the published data
	TInt count(0);
	TInt pos(0);
	const TLiwGenericParam* param(NULL);
	CLiwDefaultList* listOfMaps = CLiwDefaultList::NewLC();
	param = aEventParamList.FindFirst(pos,KChangeInfo);
	if( param )
		{
		 User::LeaveIfError( param->Value().Get( *listOfMaps ) );
		 count = listOfMaps->Count();
		}
	TLiwVariant variant;
	// Extract the data from the map 
	for(TInt i = 0;i < count; i++)
		{
		listOfMaps->AtL(i,variant);
		HBufC* operation = NULL;

		CLiwDefaultMap *map = CLiwDefaultMap::NewLC();
		variant.Get( *map );
		TBool found;
		found = map->FindL( KOperation, variant );
		if (found)
		   {
		   operation = variant.AsDes().AllocLC();
		   }
		variant.Reset();
		if( operation && ( operation->Des() != KOperationExecute ) )
		   {
		   // Nothing to update for execute action
		   HBufC* publisher = NULL; 
		   HBufC* contentType = NULL;
		   HBufC* contentId = NULL;
		   found = map->FindL( KPublisherId, variant );
		   if (found)
			   {
			   publisher = variant.AsDes().AllocLC();
			   }
		   variant.Reset();
		   found = map->FindL( KFLAG, variant );
		   if ( found)
			   {
			   // notification from publisher registry
			  if ( operation->Des() !=  KOperationDelete )
				  {
				  iData->UpdatePublisherStatusL( *publisher );
				  }
			   }
		   else if ( iData->IsPluginActive() ) 
		       {                                              
                // notification from content registry
                found = map->FindL( KContentType, variant );
                if( found )
                    {
                    contentType = variant.AsDes().AllocLC();
                    }
                variant.Reset();
                
                // content id
                found = map->FindL( KContentId, variant );
                if( found )
                    {
                    contentId = variant.AsDes().AllocLC();
                    }
                variant.Reset();
                                
                // Data map. Optional.
                CLiwDefaultMap* dataMap = NULL;
                
                found = map->FindL( KDataMap, variant);
                if( found )
                    {
                    TPtrC8 data = variant.AsData();                    
                    RDesReadStream datastrm ( data );
                    CleanupClosePushL( datastrm );
                    dataMap = CLiwDefaultMap::NewLC( datastrm );
                    // There is no CLiwDefaultMap::NewL( RReadStream )
                    // so, must do some work with cleanup stack here.
                    CleanupStack::Pop( dataMap );
                    CleanupStack::PopAndDestroy(); // datastrm                    
                    dataMap->PushL();
                    }                
                
                iData->RefreshL( *publisher,
                                 *contentType,
                                 *contentId,
                                 *operation,
                                 dataMap );
                
                if( dataMap )
                    {
                    CleanupStack::PopAndDestroy( dataMap );
                    }
                
                if( contentId )
                    {
                    CleanupStack::PopAndDestroy( contentId );
                    }
                
                if( contentType )
                    {
                    CleanupStack::PopAndDestroy( contentType );
                    }			  
                }
		   else
			   {
			   // remember update if plugin is in suspend mode
			   iData->SetUpdateNeeded( ETrue );
			   }
			variant.Reset();
			 if ( publisher )
			   {
			   CleanupStack::PopAndDestroy( publisher );
			   }
		   }
		if ( operation )
			{
			CleanupStack::PopAndDestroy( operation );
			}
		CleanupStack::PopAndDestroy( map );
		}
	CleanupStack::PopAndDestroy( listOfMaps );
	
    return aErrorCode;
    }

