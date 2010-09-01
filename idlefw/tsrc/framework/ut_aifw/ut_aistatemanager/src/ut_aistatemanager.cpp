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
* Description: EUnit unit test class for CAiStateManager
*
*/

// System includes
#include <e32base.h>
#include <e32cmn.h> 
#include <bautils.h>
#include <babitflags.h>
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/teunitassertioninfo.h>

// User includes
#include "ut_aistatemanager.h"

#include "aipluginfactory_stub.h"
#include "aiuicontroller_stub.h"
#include "aiuicontrollermanager_stub.h"

#include "aistatemanager.h"
#include "aifwpublisherinfo.h"

// Constants
_LIT( KProfile, "Profile" );
_LIT( KDummy, "dummy" );

const TInt KUid( 0x10275101 );
const TInt KNone( 0xBABE2BDED );    
const TInt KDestroyDelay( 600000 );

_LIT8( KNs, "namespace" );
    
// ======== LOCAL FUNCTIONS ========
    
// ======== MEMBER FUNCTIONS =======
// ----------------------------------------------------------------------------
// UT_AiStateManager::NewL()
//
// ----------------------------------------------------------------------------
//
UT_AiStateManager* UT_AiStateManager::NewL()
    {
    UT_AiStateManager* self = UT_AiStateManager::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// UT_AiStateManager::NewLC()
//
// ----------------------------------------------------------------------------
//
UT_AiStateManager* UT_AiStateManager::NewLC()
    {
    UT_AiStateManager* self = new ( ELeave ) UT_AiStateManager();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// UT_AiStateManager::~UT_AiStateManager()
//
// ----------------------------------------------------------------------------
//
UT_AiStateManager::~UT_AiStateManager()
    {    
    }

// ----------------------------------------------------------------------------
// UT_AiStateManager::UT_AiStateManager()
//
// ----------------------------------------------------------------------------
//
UT_AiStateManager::UT_AiStateManager()
    {
    }

// ----------------------------------------------------------------------------
// UT_AiStateManager::ConstructL()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateManager::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// ----------------------------------------------------------------------------
// UT_AiStateManager::SetupL()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateManager::SetupL()
    {
    iUiMgr = CAiUiControllerManager::NewL();    
    iFactory = CAiPluginFactory::NewL( *iUiMgr );
    iFactory->ConstructL();
    iStateManager = CAiStateManager::NewL( *iFactory );
    iMgr = CAiStateManager::NewL( *iFactory );       
    iWait = new ( ELeave ) CActiveSchedulerWait;
    iPeriodic = CPeriodic::NewL( CActive::EPriorityStandard );
    }

// ----------------------------------------------------------------------------
// UT_AiStateManager::Teardown()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateManager::Teardown()
    {        
    delete iMgr;
    iMgr = NULL;
    
    delete iFactory;
    iFactory = NULL;
    
    delete iUiMgr;
    iUiMgr = NULL;      
    
    delete iStateManager;
    iStateManager = NULL;
    
    if ( iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    delete iWait;
    iWait = NULL;
    
    if ( iPeriodic->IsActive() )
        {
        iPeriodic->Cancel();
        }
    delete iPeriodic;
    iPeriodic = NULL;
    }

// ----------------------------------------------------------------------------
// UT_AiStateManager::TestLoadAndDestroyPluginL()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateManager::TestLoadAndDestroyPluginL()
    {
    THsPublisherInfo hsInfo( TUid::Uid( KUid ), KProfile, KNs );

    TAiFwPublisherInfo info( hsInfo, TAiFwCallback( CallBack, this ), EAiFwSystemStartup );
    iMgr->NotifyLoadPlugin( info );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );
    EUNIT_ASSERT_EQUALS( iMgr->iFactory.Publishers().Count(), 1 );

    info = TAiFwPublisherInfo( hsInfo, TAiFwCallback( CallBack, this ), EAiFwSystemShutdown );
    iMgr->NotifyDestroyPlugin( info );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iFactory->Publishers().Count(), 1 );
    
    TAiFwPublisherInfo info2( hsInfo, TAiFwCallback( CallBack, this ), EAiFwPageStartup );
    iMgr->NotifyLoadPlugin( info2 );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );
    EUNIT_ASSERT_EQUALS( iFactory->Publishers().Count(), 1 );
    
    info2 = TAiFwPublisherInfo( hsInfo, TAiFwCallback( CallBack, this ), EAiFwPageShutdown );
    iMgr->NotifyDestroyPlugin( info2 );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iFactory->Publishers().Count(), 1 );

    TAiFwPublisherInfo info3( hsInfo, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iMgr->NotifyLoadPlugin( info3 );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );
    EUNIT_ASSERT_EQUALS( iFactory->Publishers().Count(), 1 );
    
    info3 = TAiFwPublisherInfo( hsInfo, TAiFwCallback( CallBack, this ), EAiFwPluginShutdown );
    iMgr->NotifyDestroyPlugin( info3 );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    
    THsPublisherInfo unknown( TUid::Uid( KNone ), KDummy, KNs );
    TAiFwPublisherInfo info4( unknown, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iMgr->NotifyLoadPlugin( info4 );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNotSupported );
    
    info4 = TAiFwPublisherInfo( unknown, TAiFwCallback( CallBack, this ), EAiFwPluginShutdown );
    iMgr->NotifyDestroyPlugin( info4 );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    
    //iMgr->DestroyPlugins();
    EUNIT_ASSERT_EQUALS( iFactory->Publishers().Count(), 1 );
    }

// ----------------------------------------------------------------------------
// UT_AiStateManager::TestStateChangeL()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateManager::TestStateChangeL()
    {   
    THsPublisherInfo hsInfo( TUid::Uid( KUid ), KProfile, KNs );
    TAiFwPublisherInfo info( hsInfo, TAiFwCallback( CallBack, this ), EAiFwSystemStartup );
    
    iMgr->NotifyLoadPlugin( info );
    iWait->Start();
    
    EUNIT_ASSERT_EQUALS( iMgr->iCurrentState, CAiStateManager::ESuspended );

    TBitFlags32 clear;
        
    EUNIT_ASSERT_EQUALS( iMgr->iFlags.iFlags, clear.iFlags );
    
    EUNIT_ASSERT_EQUALS( iMgr->iHalt, EFalse );
    
    iMgr->NotifyStateChange( EAiFwUiStartup );
    
    EUNIT_ASSERT_EQUALS( iMgr->iFlags.IsClear( CAiStateManager::EShutdown ), ETrue );
    
    iMgr->NotifyStateChange( EAiFwForeground );
    
    EUNIT_ASSERT_EQUALS( iMgr->iFlags.IsSet( CAiStateManager::EIsForeground ), ETrue );
            
    iMgr->NotifyStateChange( EAiFwBacklightOn );
    
    EUNIT_ASSERT_EQUALS( iMgr->iFlags.IsSet( CAiStateManager::EIsLightsOn ), ETrue );
    
    EUNIT_ASSERT_EQUALS( iMgr->iCurrentState, CAiStateManager::EAlive );
    
    info = TAiFwPublisherInfo( hsInfo, TAiFwCallback( CallBack, this ), EAiFwSystemShutdown );
    iMgr->NotifyDestroyPlugin( info );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    info = TAiFwPublisherInfo( hsInfo, TAiFwCallback( CallBack, this ), EAiFwSystemStartup );
    iMgr->NotifyLoadPlugin( info );
    iWait->Start();
    
    iMgr->NotifyStateChange( EAiFwOnline );

    info = TAiFwPublisherInfo( hsInfo, TAiFwCallback( CallBack, this ), EAiFwSystemShutdown );
    iMgr->NotifyDestroyPlugin( info );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    info = TAiFwPublisherInfo( hsInfo, TAiFwCallback( CallBack, this ), EAiFwSystemStartup );
    iMgr->NotifyLoadPlugin( info );
    iWait->Start();
    
    iMgr->NotifyStateChange( EAiFwBacklightOff );
    iMgr->NotifyStateChange( EAiFwBacklightOn );
    iMgr->NotifyStateChange( EAiFwBacklightOff );
    
    EUNIT_ASSERT_EQUALS( iMgr->iCurrentState, CAiStateManager::ESuspended );
    
    info = TAiFwPublisherInfo( hsInfo, TAiFwCallback( CallBack, this ), EAiFwSystemShutdown );
    iMgr->NotifyDestroyPlugin( info );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    info = TAiFwPublisherInfo( hsInfo, TAiFwCallback( CallBack, this ), EAiFwSystemStartup );
    iMgr->NotifyLoadPlugin( info );
    iWait->Start();
    
    iMgr->NotifyStateChange( EAiFwBacklightOn );
    iMgr->NotifyStateChange( EAiFwForeground );
    
    iMgr->NotifyStateChange( EAiFwBacklightOff );
    iMgr->NotifyStateChange( EAiFwBacklightOn );
    
    EUNIT_ASSERT_EQUALS( iMgr->iCurrentState, CAiStateManager::EAlive );
    
    iMgr->NotifyStateChange( EAiFwBacklightOff );
    
    EUNIT_ASSERT_EQUALS( iMgr->iCurrentState, CAiStateManager::ESuspended );
            
    iMgr->NotifyStateChange( EAiFwBackground );
    
    EUNIT_ASSERT_EQUALS( iMgr->iCurrentState, CAiStateManager::ESuspended );
    
    iMgr->NotifyStateChange( EAiFwBacklightOn );
    
    EUNIT_ASSERT_EQUALS( iMgr->iCurrentState, CAiStateManager::ESuspended );
    
    iMgr->NotifyStateChange( EAiFwForeground );
    
    EUNIT_ASSERT_EQUALS( iMgr->iCurrentState, CAiStateManager::EAlive );
    
    iMgr->NotifyStateChange( EAiFwOnline );
    
    EUNIT_ASSERT_EQUALS( iMgr->iFlags.IsSet( CAiStateManager::EIsOnline ), ETrue );

    iMgr->NotifyStateChange( EAiFwOffline );
    
    EUNIT_ASSERT_EQUALS( iMgr->iFlags.IsClear( CAiStateManager::EIsOnline ), ETrue );

    iMgr->NotifyStateChange( EAiFwOnline );
    
    EUNIT_ASSERT_EQUALS( iMgr->iFlags.IsSet( CAiStateManager::EIsOnline ), ETrue );
    
    iMgr->NotifyStateChange( EAiFwGeneralThemeChange );

    EUNIT_ASSERT_EQUALS( iMgr->iCurrentState, CAiStateManager::EAlive );
    
    iMgr->NotifyStateChange( EAiFwBackupRestoreStart );

    EUNIT_ASSERT_EQUALS( iMgr->iHalt, ETrue );
    
    EUNIT_ASSERT_EQUALS( iMgr->iCurrentState, CAiStateManager::EAlive );

    iMgr->NotifyStateChange( EAiFwBackupRestoreEnd );

    EUNIT_ASSERT_EQUALS( iMgr->iHalt, EFalse );
    EUNIT_ASSERT_EQUALS( iMgr->iCurrentState, CAiStateManager::EAlive );
        
    iMgr->NotifyStateChange( (TAiFwState) 0xFFFF );
    iMgr->NotifyStateChange( (TAiFwState) 0xFFFF );
    
    EUNIT_ASSERT_EQUALS( iMgr->iHalt, EFalse );
    EUNIT_ASSERT_EQUALS( iMgr->iCurrentState, CAiStateManager::EAlive );
    EUNIT_ASSERT_EQUALS( iMgr->iFlags.IsSet( CAiStateManager::EIsOnline ), ETrue );
                
    iMgr->NotifyStateChange( EAiFwUiShutdown );
    EUNIT_ASSERT_EQUALS( iMgr->iFlags.IsSet( CAiStateManager::EShutdown ), ETrue );
    
    info = TAiFwPublisherInfo( hsInfo, TAiFwCallback( CallBack, this ), EAiFwSystemShutdown );
    iMgr->NotifyDestroyPlugin( info );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    }

// ----------------------------------------------------------------------------
// UT_AiStateManager::TestUpgradePluginL()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateManager::TestUpgradePluginL()
    {
    THsPublisherInfo hsInfo( TUid::Uid( KUid ), KProfile, KNs );
    TAiFwPublisherInfo info( hsInfo, TAiFwCallback( CallBack, this ), EAiFwSystemStartup );
    
    iMgr->NotifyLoadPlugin( info );
    iWait->Start();
    
    iMgr->NotifyReloadPlugins();
    //iWait->Start();
    
    info = TAiFwPublisherInfo( hsInfo, TAiFwCallback( CallBack, this ), EAiFwSystemShutdown );
    iMgr->NotifyDestroyPlugin( info );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    }

// ----------------------------------------------------------------------------
// UT_AiStateManager::NotifyReleasePlugins
//
// ----------------------------------------------------------------------------
//
void UT_AiStateManager::NotifyReleasePluginsL()
    {
    THsPublisherInfo hsInfo( TUid::Uid( KUid ), KProfile, KNs );
    TAiFwPublisherInfo info( hsInfo, TAiFwCallback( CallBack, this ), EAiFwSystemStartup );
    
    iMgr->NotifyLoadPlugin( info );
    iWait->Start();
    
    RArray<TUid> aUidList;
    aUidList.Append( hsInfo.Uid() );
    iMgr->NotifyReleasePlugins( aUidList );
    CHsContentPublisher* plugin = iMgr->iFactory.PluginByUid( hsInfo.Uid() );
    if ( plugin )
        {
        User::Leave( KErrArgument);
        }
    }

// ----------------------------------------------------------------------------
// UT_AiStateManager::CallBack()
//
// ----------------------------------------------------------------------------
//
void UT_AiStateManager::CallBack( TAny* aPtr, TInt aResult )
    {
    UT_AiStateManager* self = static_cast< UT_AiStateManager* >( aPtr );
    self->iResult = aResult;
    self->iWait->AsyncStop();
    }

// ----------------------------------------------------------------------------
// UT_AiStateManager::TimerCallBack
//
// ----------------------------------------------------------------------------
//
TInt UT_AiStateManager::TimerCallBack( TAny* aPtr )
    {
    UT_AiStateManager* self = static_cast< UT_AiStateManager* >( aPtr );
    
    if ( self->iPeriodic->IsActive() )
        {
        self->iPeriodic->Cancel();
        }
    
    self->iWait->AsyncStop();
    return KErrNone;
    }

//------------------------------------------------------------------------------
// Test case table
//
//------------------------------------------------------------------------------
//
EUNIT_BEGIN_TEST_TABLE(
    UT_AiStateManager,
    "Unit test suite for CAiStateManager",
    "UNIT" )

    EUNIT_TEST(
        "Load/Destroy Publisher",
        "CAiStatemManager",
        "",
        "FUNCTIONALITY",
        SetupL, TestLoadAndDestroyPluginL, Teardown )
    
EUNIT_TEST(
        "Notify Release Plugins",
        "CAiStatemManager",
        "",
        "FUNCTIONALITY",
        SetupL, NotifyReleasePluginsL, Teardown )

        
    EUNIT_TEST(
        "State change",
        "CAiStatemManager",
        "",
        "FUNCTIONALITY",
        SetupL, TestStateChangeL, Teardown )

    EUNIT_TEST(
        "Upgrade Publisher",
        "CAiStatemManager",
        "",
        "FUNCTIONALITY",
        SetupL, TestUpgradePluginL, Teardown )
        
EUNIT_END_TEST_TABLE

// End of file
