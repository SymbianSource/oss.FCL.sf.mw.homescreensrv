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
* Plug-in main class
*
*/


// INCLUDE FILES
#include <liwservicehandler.h>
#include <liwvariant.h>
#include <liwgenericparam.h>
#include <s32mem.h>
#include "wrtdata.h"
#include "wrtdataobserver.h"
#include "wrtdatapluginconst.h"

// ---------------------------------------------------------------------------
// Factory method construction
// ---------------------------------------------------------------------------
//
CWrtDataObserver * CWrtDataObserver::NewL( MLiwInterface* aInterface, CWrtData* aData )
    {
    CWrtDataObserver * self = new (ELeave) CWrtDataObserver();
    CleanupStack::PushL( self );
    self->ConstructL( aInterface, aData );
    CleanupStack::Pop( self );
    return self;    
    }


// ---------------------------------------------------------------------------
// 2n phase constructor
// ---------------------------------------------------------------------------
//
void CWrtDataObserver::ConstructL( MLiwInterface* aInterface, CWrtData* aData )
    {
    iData = aData;
    iInterface = aInterface;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CWrtDataObserver::CWrtDataObserver ()
    {    
    }
        
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CWrtDataObserver ::~CWrtDataObserver ()
    {
    TRAP_IGNORE( ReleaseL() );
    
    iInterface = NULL;
    iData = NULL;
    }

// ---------------------------------------------------------------------------
// CWrtDataObserver::HandleNotifyL
// Handles Published content notification
// ---------------------------------------------------------------------------
//
TInt CWrtDataObserver::HandleNotifyL(
        TInt aErrorCode,
        TInt /*aEventId*/,
        CLiwGenericParamList& aEventParamList,
        const CLiwGenericParamList& /*aInParamList*/ )
    {
     
    // Is plugin active to refresh the published data
	iError = aErrorCode;        
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
		HBufC16* operation = NULL;

		CLiwDefaultMap *map = CLiwDefaultMap::NewLC();
		variant.Get( *map );
		if (map->FindL( KOperation, variant ))
		   {
		   operation = variant.AsDes().AllocLC();
	       variant.Reset();
            if( operation->Des() != KOperationExecute )
               {
               // Nothing to update for execute action
               if ( map->FindL( KFLAG, variant ))
                   {
                   // notification from publisher registry
                  if ( operation->Des() !=  KOperationDelete )
                      {
                      iData->UpdatePublisherStatusL();
                      }
                   }
               // ignore update if plugin is in suspend mode
               else if ( iData->IsPluginActive() ) 
                   {
                   HBufC16* contentId = NULL; 
                   CLiwDefaultMap* dataMap = NULL;
                    // Get the data Map if available
                    if ( map->FindL( KDataMap, variant))
                        {
                        TPtrC8 data = variant.AsData();
                        RDesReadStream datastrm ( data );
                        dataMap = CLiwDefaultMap::NewLC(datastrm);
                        }
                    // Get the content Id
                   if ( map->FindL( KContentId, variant ))
                       {
                       contentId = variant.AsDes().AllocLC();
                       // Refresh only if contentId is retrieved
                       iData->RefreshL( *contentId, *operation, dataMap );
                       CleanupStack::PopAndDestroy( contentId );
                       }

                   if ( dataMap )
                        {
                        CleanupStack::PopAndDestroy( dataMap );
                        }
                   }
                 variant.Reset();
               }
            if ( operation )
                {
                CleanupStack::PopAndDestroy( operation );
                }
		     }
		CleanupStack::PopAndDestroy( map );
		}
	CleanupStack::PopAndDestroy( listOfMaps );
	
    return aErrorCode;
    }

// ---------------------------------------------------------------------------
// CWrtDataObserver::RegisterL
// Register for notifications
// ---------------------------------------------------------------------------
//
void CWrtDataObserver::RegisterL( CLiwDefaultMap* aFilter )
    {
    CLiwGenericParamList* inParamList = CLiwGenericParamList::NewL();
    CleanupStack::PushL( inParamList );
    CLiwGenericParamList* outParamList = CLiwGenericParamList::NewL();
    CleanupStack::PushL( outParamList );
        
    // Fill in input list for RequestNotification command
    inParamList->AppendL(TLiwGenericParam(KType,TLiwVariant(KCpData_PubData)));
    inParamList->AppendL(TLiwGenericParam(KFilter ,TLiwVariant(aFilter)));
        
    iError = KErrNone;
    TRAP( iError, iInterface->ExecuteCmdL( 
                KRequestNotification,
                *inParamList,
                *outParamList,
                0,
                this ) );
    
    CleanupStack::PopAndDestroy( outParamList );
    CleanupStack::PopAndDestroy( inParamList ); 
    }

// ---------------------------------------------------------------------------
// CWrtDataObserver ::ReleaseL
// Sing off to notification
// ---------------------------------------------------------------------------
//
void CWrtDataObserver::ReleaseL()
    {
    if( iInterface )
        {
        CLiwGenericParamList* inParamList = CLiwGenericParamList::NewL();
        CleanupStack::PushL( inParamList );
        CLiwGenericParamList* outParamList = CLiwGenericParamList::NewL();
        CleanupStack::PushL( outParamList );
        
        TInt err(KErrNone);
        TRAP(err, iInterface->ExecuteCmdL( 
                KRequestNotification,
                *inParamList,
                *outParamList,
                KLiwOptCancel,
                this ));
        
        CleanupStack::PopAndDestroy( outParamList );
        CleanupStack::PopAndDestroy( inParamList );
        }
    }

// End of file
