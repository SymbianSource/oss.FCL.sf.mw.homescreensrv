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
* Description:  State Provider
*
*/

// System includes
#include <AknDef.h>
#include <connect/sbdefs.h>
#include <e32property.h>
#include <swi/swispubsubdefs.h>
#include <swi/swiutils.h>
#include <sacls.h> // KSWIUidsCurrentlyBeingProcessed

// User includes
#include <aipspropertyobserver.h>
#include <aiutility.h>
#include <aifwdefs.h>
#include "aiecomobserver.h"
#include "aistateobserver.h"

#include "aistateprovider.h"

#include "debug.h"

// Constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CAiStateProvider::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAiStateProvider* CAiStateProvider::NewL( MAiStateObserver& aObserver )     
    {
    CAiStateProvider* self = 
        CAiStateProvider::NewLC( aObserver );
    
    CleanupStack::Pop( self );
    
    return self;
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAiStateProvider* CAiStateProvider::NewLC( MAiStateObserver& aObserver )     
    {
    CAiStateProvider* self = 
        new ( ELeave ) CAiStateProvider( aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    
    return self;
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::~CAiStateProvider()
// C++ default destructor.
// ----------------------------------------------------------------------------
//
CAiStateProvider::~CAiStateProvider()
    {  
    Stop();
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::CAiStateProvider()
// C++ default constructor.
// ----------------------------------------------------------------------------
//
CAiStateProvider::CAiStateProvider( MAiStateObserver& aObserver )        
    : iObserver( aObserver )
    {      
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::ConstructL()
// 2nd phase constructor
// ----------------------------------------------------------------------------
//
void CAiStateProvider::ConstructL()
    {
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::StartL()
// 
// ----------------------------------------------------------------------------
//
void CAiStateProvider::StartL( CCoeEnv& aCoeEnv )
    {
    if ( !iStarted )
        {        
        iStarted = ETrue;
                        
        iObserver.NotifyStateChange( EAiFwUiStartup );
        
        iLightObserver = CHWRMLight::NewL( this );
        
        iBackupRestoreObserver = AiUtility::CreatePSPropertyObserverL(
            TCallBack( BackupRestoreEvent, this ),
            KUidSystemCategory, conn::KUidBackupRestoreKey );

        iSwiUidListObserver = AiUtility::CreatePSPropertyObserverL(
                TCallBack( SwiUidListEvent, this ),
                KUidSystemCategory, KSWIUidsCurrentlyBeingProcessed );

        User::LeaveIfError( iSkinSrv.Connect( this ) );
           
        iEcomObserver = CAiEcomObserver::NewL();
        iEcomObserver->AddObserverL( this );
        
        iCoeEnv = &aCoeEnv;
        
        iCoeEnv->AddMessageMonitorObserverL( *this );
        }
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::Stop()
// 
// ----------------------------------------------------------------------------
//
void CAiStateProvider::Stop()
    {
    if ( iStarted )
        {
        iStarted = EFalse;
        
        iObserver.NotifyStateChange( EAiFwUiShutdown );
        
        if ( iCoeEnv )
            {
            iCoeEnv->RemoveMessageMonitorObserver( *this );        
            }
                
        iCoeEnv = NULL;
        
        delete iEcomObserver;
        iEcomObserver = NULL;
        
        iSkinSrv.Close();
               
        Release( iBackupRestoreObserver );
        iBackupRestoreObserver = NULL;

        Release( iSwiUidListObserver );
        iSwiUidListObserver = NULL;

        delete iLightObserver;      
        iLightObserver = NULL;
        }           
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::MonitorWsMessage()
// 
// ----------------------------------------------------------------------------
//
void CAiStateProvider::MonitorWsMessage( const TWsEvent& aEvent )
    {
    TInt type( aEvent.Type() );
    
    if ( type == KAknFullOrPartialForegroundGained )
        {
        iObserver.NotifyStateChange( EAiFwForeground );
        }
    else if ( type == KAknFullOrPartialForegroundLost )
        {
        iObserver.NotifyStateChange( EAiFwBackground );
        }
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::LightStatusChanged()
// 
// ----------------------------------------------------------------------------
//
void CAiStateProvider::LightStatusChanged( TInt aTarget, 
    CHWRMLight::TLightStatus aStatus )
    {
    if ( aTarget == CHWRMLight::EPrimaryDisplay )
        {
        if ( aStatus == CHWRMLight::ELightOn )
            {
            iObserver.NotifyStateChange( EAiFwBacklightOn );
            }
        else if ( aStatus == CHWRMLight::ELightOff )
            {
            iObserver.NotifyStateChange( EAiFwBacklightOff );
            }
        }
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::SkinContentChanged()
// 
// ----------------------------------------------------------------------------
//
void CAiStateProvider::SkinContentChanged()
    {    
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::SkinConfigurationChanged()
// 
// ----------------------------------------------------------------------------
//
void CAiStateProvider::SkinConfigurationChanged(
    const TAknsSkinStatusConfigurationChangeReason aReason )
    {
    if ( aReason == EAknsSkinStatusConfigurationDeployed )
        {
        iObserver.NotifyStateChange( EAiFwGeneralThemeChange );
        }
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::SkinPackageChanged()
// 
// ----------------------------------------------------------------------------
//
void CAiStateProvider::SkinPackageChanged(
    const TAknsSkinStatusPackageChangeReason /*aReason*/ )
    {    
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::NotifyEcomRegistryChanged()
// 
// ----------------------------------------------------------------------------
//
void CAiStateProvider::NotifyEcomRegistryChanged()
    {
    __PRINTS( "CAiStateProvider::NotifyEcomRegistryChanged" );    
    
    iObserver.NotifyReloadPlugins();
    
    __PRINTS( "CAiStateProvider::NotifyEcomRegistryChanged - done" );    
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::LoadPlugin()
// 
// ----------------------------------------------------------------------------
//
void CAiStateProvider::LoadPlugin( const TAiFwPublisherInfo& aInfo )     
    {
    iObserver.NotifyLoadPlugin( aInfo );
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::DestroyPlugin()
// 
// ----------------------------------------------------------------------------
//
void CAiStateProvider::DestroyPlugin( const TAiFwPublisherInfo& aInfo )    
    {
    iObserver.NotifyDestroyPlugin( aInfo );
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::ChangePluginState()
// 
// ----------------------------------------------------------------------------
//
void CAiStateProvider::ChangePluginState( TAiFwState aState )    
    {
    if ( aState == EAiFwOnline || aState == EAiFwOffline )
        {
        iObserver.NotifyStateChange( aState );
        }
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::BackupRestoreEvent()
// 
// ----------------------------------------------------------------------------
//
/* static */ TInt CAiStateProvider::BackupRestoreEvent( TAny* aAny )
    {
    CAiStateProvider* self = static_cast< CAiStateProvider* >( aAny );

    const TUint mask( conn::KBURPartTypeMask ^ conn::EBURNormal );
    
    TInt value( 0 );
    
    if ( self->iBackupRestoreObserver->Get( value ) == KErrNone )
        {
        if ( value & mask )
            {
            // Any type of backup or restore operation
            self->iObserver.NotifyStateChange( EAiFwBackupRestoreStart );
            }
        else
            {
            self->iObserver.NotifyStateChange( EAiFwBackupRestoreEnd ); 
            }
        }
    
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::SwiUidLIstEvent()
// 
// ----------------------------------------------------------------------------
//
/* static */ TInt CAiStateProvider::SwiUidListEvent( TAny* aAny )
    {
    CAiStateProvider* self = static_cast< CAiStateProvider* >( aAny );

    RArray< TUid > list;
    
    if ( Swi::GetAllUids( list ) == KErrNone )
        {
        self->iObserver.NotifyReleasePlugins( list );
        }

    list.Reset();
    
    return KErrNone;
    }

// End of file
