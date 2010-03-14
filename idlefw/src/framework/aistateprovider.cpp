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

// User includes
#include <aipspropertyobserver.h>
#include <aiutility.h>
#include <aifwdefs.h>
#include "aiecomobserver.h"
#include "aistateobserver.h"

#include "aistateprovider.h"

// Constants

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CAiStateProvider::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAiStateProvider* CAiStateProvider::NewL( 
    MAiStateObserver& aObserver, CCoeEnv& aCoeEnv )
    {
    CAiStateProvider* self = 
        CAiStateProvider::NewLC( aObserver, aCoeEnv );
    
    CleanupStack::Pop( self );
    
    return self;
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAiStateProvider* CAiStateProvider::NewLC( 
    MAiStateObserver& aObserver, CCoeEnv& aCoeEnv )
    {
    CAiStateProvider* self = 
        new ( ELeave ) CAiStateProvider( aObserver, aCoeEnv );
    
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
    iObserver.NotifyStateChange( EAiFwUiShutdown );
    
    iCoeEnv.RemoveMessageMonitorObserver( *this );
    
    delete iEcomObserver;
    
    iSkinSrv.Close();
           
    Release( iBackupRestoreObserver );
    
    delete iLightObserver;       
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::CAiStateProvider()
// C++ default constructor.
// ----------------------------------------------------------------------------
//
CAiStateProvider::CAiStateProvider( MAiStateObserver& aObserver, 
    CCoeEnv& aCoeEnv )     
    : iObserver( aObserver ), iCoeEnv( aCoeEnv )
    {  
    iObserver.NotifyStateChange( EAiFwUiStartup );
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::ConstructL()
// 2nd phase constructor
// ----------------------------------------------------------------------------
//
void CAiStateProvider::ConstructL()
    {
    iLightObserver = CHWRMLight::NewL( this );
    
    iBackupRestoreObserver = AiUtility::CreatePSPropertyObserverL(
        TCallBack( BackupRestoreEvent, this ),
        KUidSystemCategory, conn::KUidBackupRestoreKey );
    
    User::LeaveIfError( iSkinSrv.Connect( this ) );
       
    iEcomObserver = CAiEcomObserver::NewL();
    iEcomObserver->AddObserverL( this );
    
    iCoeEnv.AddMessageMonitorObserverL( *this );
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
    iObserver.NotifyUpdatePlugins();
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::LoadPlugin()
// 
// ----------------------------------------------------------------------------
//
TInt CAiStateProvider::LoadPlugin( const THsPublisherInfo& aPublisherInfo, 
    TAiFwLoadReason aReason )
    {
    return iObserver.NotifyLoadPlugin( aPublisherInfo, aReason );
    }

// ----------------------------------------------------------------------------
// CAiStateProvider::DestroyPlugin()
// 
// ----------------------------------------------------------------------------
//
void CAiStateProvider::DestroyPlugin( const THsPublisherInfo& aPublisherInfo,
    TAiFwDestroyReason aReason )
    {
    iObserver.NotifyDestroyPlugin( aPublisherInfo, aReason );
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
// CAiStateProvider::OnlineStateInUse()
// 
// ----------------------------------------------------------------------------
//
TBool CAiStateProvider::OnlineStateInUse() const
    {
    return iObserver.OnlineStateInUse();
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

// End of file

