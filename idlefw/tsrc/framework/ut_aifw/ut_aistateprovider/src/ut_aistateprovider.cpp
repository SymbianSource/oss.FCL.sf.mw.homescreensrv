/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: EUnit unit test class for CAiStateProvider
*
*/

// System includes
#include <e32base.h>
#include <e32cmn.h> 
#include <bautils.h>
#include <babitflags.h>
#include <AknDef.h>
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/teunitassertioninfo.h>

// User includes
#include <hspublisherinfo.h>
#include "ut_aistateprovider.h"

#include "aistateprovider.h"
#include "aifwpublisherinfo.h"

// Constants
const TInt KUid( 0xBABE2BED );
_LIT( KTestPublisher, "TestPublisher" );
_LIT8( KNs, "TestNamespace" );

// ======== LOCAL FUNCTIONS ========
    
// ======== MEMBER FUNCTIONS =======
// ----------------------------------------------------------------------------
// UT_AiStateProvider::NewL()
//
// ----------------------------------------------------------------------------
//
UT_AiStateProvider* UT_AiStateProvider::NewL()
    {
    UT_AiStateProvider* self = UT_AiStateProvider::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// UT_AiStateProvider::NewLC()
//
// ----------------------------------------------------------------------------
//
UT_AiStateProvider* UT_AiStateProvider::NewLC()
    {
    UT_AiStateProvider* self = new ( ELeave ) UT_AiStateProvider();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// UT_AiStateProvider::~UT_AiStateProvider()
//
// ----------------------------------------------------------------------------
//
UT_AiStateProvider::~UT_AiStateProvider()
    {    
    }

// ----------------------------------------------------------------------------
// UT_AiStateProvider::UT_AiStateProvider()
//
// ----------------------------------------------------------------------------
//
UT_AiStateProvider::UT_AiStateProvider()
    {
    }

// ----------------------------------------------------------------------------
// UT_AiStateProvider::ConstructL()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateProvider::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// ----------------------------------------------------------------------------
// UT_AiStateProvider::NotifyStateChange()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateProvider::NotifyStateChange( TAiFwState aState )
    {
    if ( iIgnoreNotifyStateChange )
        {
        return;
        }
    
    EUNIT_ASSERT_EQUALS( aState, iExpectedState );
    
    if ( iExpectedState == EAiFwUiStartup )
        {
        // Need to ignore until state provider is fully constructed
        iIgnoreNotifyStateChange = ETrue;
        }
    }

// ----------------------------------------------------------------------------
// UT_AiStateProvider::NotifyLoadPlugin()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateProvider::NotifyLoadPlugin( const TAiFwPublisherInfo& aInfo )
    {
    //TODO: ok?
    THsPublisherInfo info( TUid::Uid( KUid ), KTestPublisher, KNs );
    
    EUNIT_ASSERT_EQUALS( aInfo.Info(), info );
    
    EUNIT_ASSERT_EQUALS( aInfo.Reason(), EAiFwSystemStartup );
    }

// ----------------------------------------------------------------------------
// UT_AiStateProvider::NotifyDestroyPlugin()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateProvider::NotifyDestroyPlugin( const TAiFwPublisherInfo& aInfo )
    {
    //TODO: ok?
    THsPublisherInfo info( TUid::Uid( KUid ), KTestPublisher, KNs );
    
    EUNIT_ASSERT_EQUALS( aInfo.Info(), info );
    
    EUNIT_ASSERT_EQUALS( aInfo.Reason(), EAiFwSystemShutdown );    
    }

// ----------------------------------------------------------------------------
// UT_AiStateProvider::NotifyUpdatePlugins()
//
// ----------------------------------------------------------------------------
//
/*void UT_AiStateProvider::NotifyUpdatePlugins()
    {
    iPluginsUpdated = ETrue;
    }*/

// ----------------------------------------------------------------------------
// UT_AiStateProvider::NotifyReloadPlugins()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateProvider::NotifyReloadPlugins()
    {
    iPluginsUpdated = ETrue;
    }

// ----------------------------------------------------------------------------
// UT_AiStateProvider::NotifyReleasePlugins()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateProvider::NotifyReleasePlugins( const RArray<TUid>& )
    {
    }

// ----------------------------------------------------------------------------
// UT_AiStateProvider::SetupL()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateProvider::SetupL()
    {
    CCoeEnv* env( CCoeEnv::Static() );
    
    iExpectedState = EAiFwUiStartup;
    
    iProvider = CAiStateProvider::NewL( *this );
    iProvider->StartL( *env );
    
    iIgnoreNotifyStateChange = EFalse;
    }

// ----------------------------------------------------------------------------
// UT_AiStateProvider::Teardown()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateProvider::Teardown()
    {
    iExpectedState = EAiFwUiShutdown;
    
    iProvider->Stop();
    
    delete iProvider;
    iProvider = NULL;
    }

// ----------------------------------------------------------------------------
// UT_AiStateProvider::TestStartStopL()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateProvider::TestStartStopL()
    {
    CCoeEnv* env( CCoeEnv::Static() );
    
    EUNIT_ASSERT_EQUALS( iProvider->iStarted, ETrue );
            
    iExpectedState = EAiFwUiShutdown;
    
    iProvider->Stop();
    
    EUNIT_ASSERT_EQUALS( iProvider->iStarted, EFalse );
    EUNIT_ASSERT_EQUALS( iProvider->iCoeEnv == NULL, ETrue );        
    EUNIT_ASSERT_EQUALS( iProvider->iEcomObserver == NULL, ETrue );
    EUNIT_ASSERT_EQUALS( iProvider->iBackupRestoreObserver == NULL, ETrue );
    EUNIT_ASSERT_EQUALS( iProvider->iLightObserver == NULL, ETrue );

    iExpectedState = EAiFwUiStartup;
    
    iProvider->StartL( *env );
            
    EUNIT_ASSERT_EQUALS( iProvider->iStarted, ETrue );
    EUNIT_ASSERT_EQUALS( iProvider->iCoeEnv != NULL, ETrue );        
    EUNIT_ASSERT_EQUALS( iProvider->iEcomObserver != NULL, ETrue );
    EUNIT_ASSERT_EQUALS( iProvider->iBackupRestoreObserver != NULL, ETrue );
    EUNIT_ASSERT_EQUALS( iProvider->iLightObserver != NULL, ETrue );
    
    iIgnoreNotifyStateChange = EFalse;
    iExpectedState = (TAiFwState)0;
    // Should not get any notification because already started
    iProvider->StartL( *env );

    EUNIT_ASSERT_EQUALS( iProvider->iStarted, ETrue );
    EUNIT_ASSERT_EQUALS( iProvider->iCoeEnv != NULL, ETrue );        
    EUNIT_ASSERT_EQUALS( iProvider->iEcomObserver != NULL, ETrue );
    EUNIT_ASSERT_EQUALS( iProvider->iBackupRestoreObserver != NULL, ETrue );
    EUNIT_ASSERT_EQUALS( iProvider->iLightObserver != NULL, ETrue );
    
    iExpectedState = EAiFwUiShutdown;
    
    iProvider->Stop();
    
    EUNIT_ASSERT_EQUALS( iProvider->iStarted, EFalse );
    EUNIT_ASSERT_EQUALS( iProvider->iCoeEnv == NULL, ETrue );        
    EUNIT_ASSERT_EQUALS( iProvider->iEcomObserver == NULL, ETrue );
    EUNIT_ASSERT_EQUALS( iProvider->iBackupRestoreObserver == NULL, ETrue );
    EUNIT_ASSERT_EQUALS( iProvider->iLightObserver == NULL, ETrue );
    
    iExpectedState = (TAiFwState)0;
    // Should not get any notification because already stopped
    iProvider->Stop();

    EUNIT_ASSERT_EQUALS( iProvider->iStarted, EFalse );
    EUNIT_ASSERT_EQUALS( iProvider->iCoeEnv == NULL, ETrue );        
    EUNIT_ASSERT_EQUALS( iProvider->iEcomObserver == NULL, ETrue );
    EUNIT_ASSERT_EQUALS( iProvider->iBackupRestoreObserver == NULL, ETrue );
    EUNIT_ASSERT_EQUALS( iProvider->iLightObserver == NULL, ETrue );    
    }

// ----------------------------------------------------------------------------
// UT_AiStateProvider::TestFgBgStateL()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateProvider::TestFgBgStateL()
    {
    TWsEvent event;
    
    iExpectedState = EAiFwForeground;       
    
    event.SetType( KAknFullOrPartialForegroundGained );
    
    iProvider->MonitorWsMessage( event );
    
    iExpectedState = EAiFwBackground;
    
    event.SetType( KAknFullOrPartialForegroundLost );
    
    iProvider->MonitorWsMessage( event );
    
    event.SetType( 0 );
    
    iProvider->MonitorWsMessage( event );
    }

// ----------------------------------------------------------------------------
// UT_AiStateProvider::TestLightStateL()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateProvider::TestLightStateL()
    {
    iExpectedState = EAiFwBacklightOn;
    
    iProvider->LightStatusChanged( 
        CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOn );
    
    iExpectedState = EAiFwBacklightOff;

    iProvider->LightStatusChanged( 
        CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightOff );
    
    iProvider->LightStatusChanged( 
        CHWRMLight::EPrimaryDisplay, CHWRMLight::ELightStatusUnknown );    
    
    iProvider->LightStatusChanged( 
        CHWRMLight::ENoTarget, CHWRMLight::ELightStatusUnknown );       
    }

// ----------------------------------------------------------------------------
// UT_AiStateProvider::TestSkinChangeL()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateProvider::TestSkinChangeL()
    {
    iProvider->SkinContentChanged();
    iProvider->SkinPackageChanged( ( TAknsSkinStatusPackageChangeReason) 0 );
    
    iExpectedState = EAiFwGeneralThemeChange;
    
    iProvider->SkinConfigurationChanged(
        EAknsSkinStatusConfigurationDeployed );

    iProvider->SkinConfigurationChanged(
        EAknsSkinStatusConfigurationMerged );    
    }

// ----------------------------------------------------------------------------
// UT_AiStateProvider::TestEcomRegistryChangeL()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateProvider::TestEcomRegistryChangeL()
    {
    iPluginsUpdated = EFalse;
    
    iProvider->NotifyEcomRegistryChanged();
    
    EUNIT_ASSERT_EQUALS( iPluginsUpdated, ETrue );
    }

// ----------------------------------------------------------------------------
// UT_AiStateProvider::TestLoadDestroyPluginL()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateProvider::TestLoadDestroyPluginL()
    {
    //TODO: ok?
    THsPublisherInfo hsInfo( TUid::Uid( KUid ), KTestPublisher, KNs );
    TAiFwPublisherInfo info( hsInfo, TAiFwCallback(), EAiFwSystemStartup );
    
    iProvider->LoadPlugin( info );
    
    iProvider->DestroyPlugin( info/*, EAiFwSystemShutdown*/ );    
    }

// ----------------------------------------------------------------------------
// UT_AiStateProvider::TestChangePluginStateL()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateProvider::TestChangePluginStateL()
    {
    iExpectedState = EAiFwOffline;
    
    iProvider->ChangePluginState( EAiFwOffline );
    
    iExpectedState = EAiFwOnline;
    
    iProvider->ChangePluginState( EAiFwOnline );
    
    iProvider->ChangePluginState( EAiFwBacklightOn );
    iProvider->ChangePluginState( EAiFwBackupRestoreStart );
    }

// ----------------------------------------------------------------------------
// UT_AiStateProvider::TestBackupRestoreStateChangeL()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateProvider::TestBackupRestoreStateChangeL()
    {    
    iIgnoreNotifyStateChange = ETrue;
    
    EUNIT_ASSERT_EQUALS( 
        CAiStateProvider::BackupRestoreEvent( iProvider ), KErrNone );
    }

// ----------------------------------------------------------------------------
// UT_AiStateProvider::SwiUidListEventL()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateProvider::SwiUidListEventL()
    {    
    TInt ret = iProvider->SwiUidListEvent( iProvider );
    }

//------------------------------------------------------------------------------
// Test case table
//
//------------------------------------------------------------------------------
//
EUNIT_BEGIN_TEST_TABLE(
    UT_AiStateProvider,
    "Unit test suite for CAiStateProvider",
    "UNIT" )

    EUNIT_TEST(
        "Start/Stop provider",
        "CAiStateProvider",
        "",
        "FUNCTIONALITY",
        SetupL, TestStartStopL, Teardown )

    EUNIT_TEST(
        "Fg/Bg state change",
        "CAiStateProvider",
        "",
        "FUNCTIONALITY",
        SetupL, TestFgBgStateL, Teardown )
        
    EUNIT_TEST(
        "Light state change",
        "CAiStateProvider",
        "",
        "FUNCTIONALITY",
        SetupL, TestLightStateL, Teardown )

    EUNIT_TEST(
        "Skin change",
        "CAiStateProvider",
        "",
        "FUNCTIONALITY",
        SetupL, TestSkinChangeL, Teardown )

    EUNIT_TEST(
        "Ecom registry change",
        "CAiStateProvider",
        "",
        "FUNCTIONALITY",
        SetupL, TestEcomRegistryChangeL, Teardown )

    EUNIT_TEST(
        "Load/Destroy plugin",
        "CAiStateProvider",
        "",
        "FUNCTIONALITY",
        SetupL, TestLoadDestroyPluginL, Teardown )

    EUNIT_TEST(
        "Change plugin state",
        "CAiStateProvider",
        "",
        "FUNCTIONALITY",
        SetupL, TestChangePluginStateL, Teardown )

    EUNIT_TEST(
        "Backup/Restore state",
        "CAiStateProvider",
        "",
        "FUNCTIONALITY",
        SetupL, TestBackupRestoreStateChangeL, Teardown )
        
    EUNIT_TEST(
        "Swi Uid List Event",
        "CAiStateProvider",
        "",
        "FUNCTIONALITY",
        SetupL, SwiUidListEventL, Teardown )
        
EUNIT_END_TEST_TABLE

// End of file
