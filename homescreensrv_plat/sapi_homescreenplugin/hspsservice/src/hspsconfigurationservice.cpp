/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Interface to HSPS service
*
*/

// SYSTEM INCLUDES
#include <eikenv.h>
#include <eikappui.h>
#include <eikapp.h>
#include <bldvariant.hrh>

#include <hspsdomattribute.h>
#include <mhspsdomlistitem.h>
#include <hspsdomlist.h>
#include <hspsdomdepthiterator.h>
#include <hspsdomnode.h>
#include <hspsdefinitionrepository.h>
#include <hspsresource.h>

#include "hspsconfigurationservice.h"
#include "hspsxmlelements.h"
#include "hspscallback.h"
#include "hspsserviceutilities.h"

#ifdef HSPS_LOG_ACTIVE
#include <hspsodtdump.h>
#include <hspslogbusfile.h>
#endif

// ======== LOCAL CONSTANTS ========



// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CHspsConfigurationService* CHspsConfigurationService::NewL()
    {
    CHspsConfigurationService* self = new (ELeave) CHspsConfigurationService;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CHspsConfigurationService::~CHspsConfigurationService()
    {
    iProperty.Close();
    iPluginIds.Close();
    if(iHspsRequestClient)
        {
        iHspsRequestClient->hspsCancelGetODTUpdate();
        }
    
    if( iCallback )
        {
        delete iCallback;
        iCallback = NULL;
        }
     
    
    delete iHspsRequestClient;
    delete iHspsODT;    
    delete iNotifyParams;
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CHspsConfigurationService::CHspsConfigurationService() :
        iODTVersion( 0 )
    {
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CHspsConfigurationService::ConstructL()
    {
    iHspsODT = ChspsODT::NewL();
    iHspsRequestClient = ChspsRequestClient::NewL( *this  );
    iNotifyParams = ChspsRequestNotificationParams::NewL();
    iPluginIds.Reset();
    }

// -----------------------------------------------------------------------------
// Gets requested configuration.
// -----------------------------------------------------------------------------
EXPORT_C ChspsDomDocument& CHspsConfigurationService::GetDOML()
	{	
	if( !iHspsODT ||
	    !iHspsODT->DomDocument().RootNode() )
		{		
		//Odt is not parsed, leave.
		User::Leave( KErrNotFound );
		}

     if( iODTVersion < 1 )
         {
         GetODTL( iHspsODT->RootUid() );         
         }     
     
     // Get app uid/key
     TInt key = 0;
     GetAppUidL( key );
     if( key < 1 )
         {
         User::Leave( KErrNotFound );
         }
     
     if( iODTVersion < 1 )
         {
         User::LeaveIfError( 
             iProperty.Attach( 
                 KPropertyHspsCat, 
                 key ) 
                 );
         User::LeaveIfError( 
             iProperty.Get( 
                 KPropertyHspsCat, 
                 key, 
                 iODTVersion ) 
                 );         
         }
     else
         {
         // Check whether the ODT needs to be updated
         TInt latestVersion( -1 );
         User::LeaveIfError( 
             iProperty.Get( 
                 KPropertyHspsCat, 
                 key,
                 latestVersion ) 
                 );      
         if( latestVersion != iODTVersion )
             {
             GetODTL( iHspsODT->RootUid() );
             iODTVersion = latestVersion;
             }              
         }
	
	return iHspsODT->DomDocument();
	}

// -----------------------------------------------------------------------------
// Gets application configuration ODT
// -----------------------------------------------------------------------------
EXPORT_C void CHspsConfigurationService::GetODTL( const TInt aAppUid )
	{
#ifdef HSPS_LOG_ACTIVE
	if( iLogBus )
	    {
	    iLogBus->LogText( _L( "CHspsConfigurationService::GetODTL" ) );
	    }
#endif
	
	// Fetch application configuration   
    ThspsServiceCompletedMessage retOdt = iHspsRequestClient->hspsGetODT( 
            aAppUid, 
            *iHspsODT );
    if ( retOdt != EhspsGetODTSuccess )
        {            
#ifdef HSPS_LOG_ACTIVE
        if( iLogBus )
            {
            iLogBus->LogText( _L( "CHspsConfigurationService::GetActiveConfigurationL(): - Failed to find a configuration" ) );
            }
#endif
        
        User::Leave( KErrNotFound );    
        }
    else
        {
        // ODT is now valid.

        /*
        // Start observing ODT changes
        if ( EhspsServiceRequestSheduled != iHspsRequestClient->hspsGetODTUpdate() )
            {
#ifdef HSPS_LOG_ACTIVE
            if( iLogBus )
                {
                iLogBus->LogText( _L( "CHspsConfigurationService::GetActiveConfigurationL(): - Failed to start observing" ) );
                }
#endif
    
            User::Leave( KErrGeneral );
            }
         */   
        }

#ifdef HSPS_LOG_ACTIVE    
    if( iHspsODT && iLogBus )
        {
        ChspsOdtDump::Dump( *iHspsODT, *iLogBus );
        }
#endif    
    }

// -----------------------------------------------------------------------------
// Gets application UID of active configuration
// -----------------------------------------------------------------------------
EXPORT_C void CHspsConfigurationService::GetAppUidL(
        TInt& aAppUid )
    {
    if( !iHspsODT ||
        !iHspsODT->DomDocument().RootNode() )
        {
        //Odt is not parsed, leave.
        User::Leave( KErrNotFound );
        }
    aAppUid = iHspsODT->RootUid();
    }

// -----------------------------------------------------------------------------
// Set log bus.
// -----------------------------------------------------------------------------
#ifdef HSPS_LOG_ACTIVE
EXPORT_C void CHspsConfigurationService::SetLogBus( ChspsLogBus* aLogBus )
    {
    iLogBus = aLogBus;
    }
#else
EXPORT_C void CHspsConfigurationService::SetLogBus( ChspsLogBus* /*aLogBus*/ )
    {
    }
#endif

// -----------------------------------------------------------------------------
// See header for comments.
// -----------------------------------------------------------------------------
EXPORT_C void CHspsConfigurationService::InvalidateODT()
    {
    iODTVersion = 0;
    }

// -----------------------------------------------------------------------------
// Gets application configuration family
// -----------------------------------------------------------------------------
EXPORT_C void CHspsConfigurationService::GetFamilyL(
    TUint32& aFamily )
    {
    if( !iHspsODT ||
        !iHspsODT->DomDocument().RootNode() )
        {
        //Odt is not parsed, leave.
        User::Leave( KErrNotFound );
        }
    aFamily = iHspsODT->Family();
    }

// -----------------------------------------------------------------------------
// Gets application UID of active configuration
// -----------------------------------------------------------------------------
EXPORT_C TInt CHspsConfigurationService::RegisterObserverL(CHspsReqNotifCallback* aCallBack )
    {
    
    if( !aCallBack ) 
        {
        return KErrArgument;
        }
    if ( !iHspsRequestClient )
        {
        // Not suppose tp happen since iHspsRequestClient is created during service attach
        return KErrGeneral;
        }
   
    if( iCallback )
        {
        return KErrInUse;
        }
    
    iHspsRequestClient->hspsCancelGetODTUpdate();
    if( !iCallback )
        {
        iCallback = aCallBack;
        }
    
    // Start observing ODT changes
 
  if ( EhspsServiceRequestSheduled != iHspsRequestClient->hspsGetODTUpdate() )
        {
#ifdef HSPS_LOG_ACTIVE
        if( iLogBus )
            {
            iLogBus->LogText( _L( "CHspsConfigurationService::GetActiveConfigurationL(): - Failed to start observing" ) );
            }
#endif
        
        return KErrGeneral;
        }   
    return KErrNone;  
    }
// -----------------------------------------------------------------------------
// Gets application UID of active configuration
// -----------------------------------------------------------------------------
EXPORT_C void CHspsConfigurationService::UnRegisterObserverL()
    {
    
    if ( !iHspsRequestClient )
        {
        // Not suppose tp happen since iHspsRequestClient is created during service attach
        User::Leave( KErrGeneral );
        }
    iHspsRequestClient->hspsCancelGetODTUpdate();
   
    if(iCallback)
        {
        delete iCallback;
        iCallback = NULL;
        }   
    }
// ---------------------------------------------------------------------------
// Hsps client request service observer
// ---------------------------------------------------------------------------
//
void CHspsConfigurationService::HandlehspsRequestClientMessageL( ThspsServiceCompletedMessage aMessage,
                                                               ChspsRequestNotificationParams& aParams )
    {
    if( aMessage == EhspsGetODTUpdateStatus || aMessage == EhspsGetODTUpdateHot )
        {       
        if(iCallback)
            {          
            if( !ParseNotificationDataL(aParams) ) 
                {
                // Invalidate member ODT for specific events.                                
                if( iNotifyParams->iEvent != EhspsCacheUpdate &&
                    iNotifyParams->iEvent != EhspsClean &&
                    iNotifyParams->iEvent != EhspsNoEvent )
                    {
                    InvalidateODT();
                    }
                
                // Notify.
                iCallback->NotifyResultL( KErrNone, *iNotifyParams, iPluginIds );
                
                // Reset.
                iPluginIds.Reset();
                iNotifyParams->ResetData();
                
                } 
            }
        }
    if(aMessage == EhspsServiceRequestError)
        {
        // Invalidate ODT.
        InvalidateODT();        
        
        if(iCallback)
            {
            iCallback->NotifyResultL( KErrGeneral, *iNotifyParams, iPluginIds);
            iPluginIds.Reset();
            iNotifyParams->ResetData();
            }
        }
    
    iHspsRequestClient->hspsCancelGetODTUpdate();
    if( EhspsServiceRequestSheduled != iHspsRequestClient->hspsGetODTUpdate() )                
        {
        User::Leave( KErrGeneral ); 
        }
     
    }

// ---------------------------------------------------------------------------
// Sends events to observers
// ---------------------------------------------------------------------------
//
TInt CHspsConfigurationService::ParseNotificationDataL( ChspsRequestNotificationParams& aParams )
    {
    TInt err = KErrNone;
    
    // Get client's appuid
    const TInt appUid = iHspsODT->RootUid();
          
    // If this is a broadcast to all clients
    if( aParams.iAppUid == 0 )
        {
        // Send widget was installed, uninstalled or updated notifications only          
        if ( aParams.iEvent != EhspsODTAdded 
                && aParams.iEvent != EhspsClean
                && aParams.iEvent != EhspsODTUpdated )
            {
            // Block other notifications
            err = KErrGeneral;
            }
        }
    else if ( appUid != aParams.iAppUid )
        {
        // Block notifications which were sent to other clients
        err = KErrGeneral;
        }        
      
    if( !err )
        {
        iPluginIds.Reset();
        iNotifyParams->iEvent = aParams.iEvent;
        iNotifyParams->iAppUid = aParams.iAppUid;
        iNotifyParams->iOrigUid = aParams.iOrigUid;
        iNotifyParams->iAppConfUid = aParams.iAppConfUid;
        iNotifyParams->iPluginUid = aParams.iPluginUid;
        iNotifyParams->iCount = aParams.iCount;
        
        for(TInt i=0; i < iNotifyParams->iCount; i++ )
            {
            iPluginIds.Append( aParams.iPluginIds[i] );
            }
           
        iNotifyParams->SetNameL(aParams.Name());
        }
    
    return err;
    }
    



