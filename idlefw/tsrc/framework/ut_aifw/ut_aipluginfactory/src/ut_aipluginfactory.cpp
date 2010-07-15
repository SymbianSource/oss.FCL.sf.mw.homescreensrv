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
* Description: EUnit unit test class for CAiPluginFactory
*
*/

// System includes
#include <e32base.h>
#include <e32cmn.h> 
#include <bautils.h>
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/teunitassertioninfo.h>

// User includes
#include <hspublisherinfo.h>
#include "ut_aipluginfactory.h"
#include "aiuicontrollermanager_stub.h"

#include "aipluginfactory.h"
#include "aifwpublisherinfo.h"
#include "aistatemanager.h"

// Constants
_LIT( KProfile, "Profile" );
_LIT( KDevstat, "DeviceStatus");
_LIT( KUnknown, "Unknown" );
_LIT( KOrganizer, "Organizer" );

const TInt KProfileUid( 0x10275101 );
const TInt KDevstatUid( 0x102750F8 );
const TInt KUnknownUid( 0xBABE2BED );
const TInt KOrganizerUid( 0x102750FE );
const TInt KDestroyDelay( 600000 );
_LIT8( KNs1, "namespace1" );
_LIT8( KNs2, "namespace2" );

// ======== LOCAL FUNCTIONS ========
    
// ======== MEMBER FUNCTIONS =======
// ----------------------------------------------------------------------------
// UT_AiPluginFactory::NewL()
//
// ----------------------------------------------------------------------------
//
UT_AiPluginFactory* UT_AiPluginFactory::NewL()
    {
    UT_AiPluginFactory* self = UT_AiPluginFactory::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// UT_AiPluginFactory::NewLC()
//
// ----------------------------------------------------------------------------
//
UT_AiPluginFactory* UT_AiPluginFactory::NewLC()
    {
    UT_AiPluginFactory* self = new ( ELeave ) UT_AiPluginFactory();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// UT_AiPluginFactory::~UT_AiPluginFactory()
//
// ----------------------------------------------------------------------------
//
UT_AiPluginFactory::~UT_AiPluginFactory()
    {    
    }

// ----------------------------------------------------------------------------
// UT_AiPluginFactory::UT_AiPluginFactory()
//
// ----------------------------------------------------------------------------
//
UT_AiPluginFactory::UT_AiPluginFactory()
    {
    }

// ----------------------------------------------------------------------------
// UT_AiPluginFactory::ConstructL()
//
// ----------------------------------------------------------------------------
//
void UT_AiPluginFactory::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// ----------------------------------------------------------------------------
// UT_AiPluginFactory::SetupL()
//
// ----------------------------------------------------------------------------
//
void UT_AiPluginFactory::SetupL()
    {
    iUiMgr = CAiUiControllerManager::NewL();
    
    iFactory = CAiPluginFactory::NewL( *iUiMgr );
    iFactory->ConstructL();
    
    iStateManager = CAiStateManager::NewL( *iFactory );
    
    iWait = new ( ELeave ) CActiveSchedulerWait;
    
    iPeriodic = CPeriodic::NewL( CActive::EPriorityStandard );
    
    iResult = KErrNone;
    }

// ----------------------------------------------------------------------------
// UT_AiPluginFactory::Teardown()
//
// ----------------------------------------------------------------------------
//
void UT_AiPluginFactory::Teardown()
    {    
    delete iFactory;
    iFactory = NULL;

    delete iUiMgr;
    iUiMgr = NULL;
    
    delete iStateManager;
    iStateManager = NULL;
    
    if ( iPeriodic->IsActive() )
        {
        iPeriodic->Cancel();
        }
    delete iPeriodic;
    iPeriodic = NULL;
    
    if ( iWait->IsStarted() )
        {
        iWait->AsyncStop();
        }
    delete iWait;
    iWait = NULL;
    }

// ----------------------------------------------------------------------------
// UT_AiPluginFactory::TestCreateDestroyPluginL()
//
// ----------------------------------------------------------------------------
//
void UT_AiPluginFactory::TestCreateDestroyPluginL()
    {
    THsPublisherInfo data( 
        TUid::Uid( KOrganizerUid ), KOrganizer, KNs1 );

    THsPublisherInfo profile( 
        TUid::Uid( KProfileUid ), KProfile, KNs2 );

    THsPublisherInfo devstat( 
        TUid::Uid( KDevstatUid ), KDevstat, KNs2 );

    THsPublisherInfo missing( 
        TUid::Uid( KProfileUid ), KProfile, KNullDesC8 );

    THsPublisherInfo unknown( 
        TUid::Uid( KUnknownUid ), KUnknown, KNs1 );

    // create 2 same without waiting first to finnish. Second should return
    // KErrAlreadyExists
    TAiFwPublisherInfo info( data, TAiFwCallback( CallBack, this ), EAiFwSystemStartup );
    iFactory->LoadPlugin( info );
    iFactory->LoadPlugin( info );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrAlreadyExists );
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( data ) != NULL, ETrue );
    
    // wait for destroy to finnish
    iFactory->DestroyPlugin( info );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( data ) == NULL, ETrue );
    EUNIT_ASSERT_EQUALS( iFactory->Publishers().Count(), 0 );
    
    // create 2 same and wait first to finnish. Second should return
    // KErrAlreadyExists
    iFactory->LoadPlugin( info );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( data ) != NULL, ETrue );

    iFactory->LoadPlugin( info );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrAlreadyExists );
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( data ) != NULL, ETrue );

    // Test missing namespace
    TAiFwPublisherInfo missingInfo( missing, TAiFwCallback( CallBack, this ), EAiFwSystemStartup );
    iFactory->LoadPlugin( missingInfo );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNotSupported );
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( missing ) == NULL, ETrue );

    // Test unknown
    TAiFwPublisherInfo unknownInfo( unknown, TAiFwCallback( CallBack, this ), EAiFwSystemStartup );
    iFactory->LoadPlugin( unknownInfo );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNotSupported );
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( unknown ) == NULL, ETrue );

    //profile
    TAiFwPublisherInfo profileInfo( profile, TAiFwCallback( CallBack, this ), EAiFwSystemStartup );
    iFactory->LoadPlugin( profileInfo );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( profile ) != NULL, ETrue );

    // Create devstat
    TAiFwPublisherInfo devstatInfo( devstat, TAiFwCallback( CallBack, this ), EAiFwSystemStartup );
    iFactory->LoadPlugin( devstatInfo );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( devstat ) != NULL, ETrue );

    // Test that Devstat is recycled by creating exactly same plugin again
    iFactory->LoadPlugin( devstatInfo );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( devstat ) != NULL, ETrue );

    // wait for destroy to finnish
    iFactory->DestroyPlugin( info );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( data ) == NULL, ETrue );

    // Test profile recycle, it won't be deleted
    iFactory->DestroyPlugin( profileInfo );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( profile ) != NULL, ETrue );

    // Test devstat recycle, it won't be deleted
    iFactory->DestroyPlugin( devstatInfo );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( devstat ) != NULL, ETrue );

    // Try destroying unknown
    iFactory->DestroyPlugin( unknownInfo );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( unknown ) == NULL, ETrue );

    iFactory->Publishers().ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// UT_AiPluginFactory::TestCreateDestroyPluginL()
//
// ----------------------------------------------------------------------------
//
void UT_AiPluginFactory::TestCreateDestroyAllL()
    {
    THsPublisherInfo data( 
    TUid::Uid( KOrganizerUid ), KOrganizer, KNs1 );
    
    TAiFwPublisherInfo info( data, TAiFwCallback( CallBack, this ), EAiFwSystemStartup );
    iFactory->LoadPlugin( info );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( data ) != NULL, ETrue );
    
    //destroy using uid
    iFactory->DestroyPlugin( info.Info().Uid() );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( data ) == NULL, ETrue );
    
    iFactory->LoadPlugin( info );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( data ) != NULL, ETrue );
    
    iFactory->DestroyAllPlugins();
    iPeriodic->Start( KDestroyDelay*2, KDestroyDelay*2, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( data ) == NULL, ETrue );
    }
// ----------------------------------------------------------------------------
// UT_AiPluginFactory::TestPluginByInfoL()
//
// ----------------------------------------------------------------------------
//
void UT_AiPluginFactory::TestPluginByInfoL()
    {
    THsPublisherInfo profile1( 
        TUid::Uid( KProfileUid ), KProfile, KNs1 );
    
    THsPublisherInfo profile2( 
        TUid::Uid( KProfileUid ), KProfile, KNs2 );

    THsPublisherInfo missing( 
        TUid::Uid( KProfileUid ), KProfile, KNullDesC8 );

    THsPublisherInfo unknown(
        TUid::Uid( KUnknownUid ), KUnknown, KNs1 );

    TAiFwPublisherInfo profile1Info( profile1, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iFactory->LoadPlugin( profile1Info );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );

    TAiFwPublisherInfo profile2Info( profile2, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iFactory->LoadPlugin( profile2Info );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );

    TAiFwPublisherInfo missingInfo( missing, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iFactory->LoadPlugin( missingInfo );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNotSupported )
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( missing ) == NULL, ETrue );

    TAiFwPublisherInfo unknownInfo( unknown, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iFactory->LoadPlugin( unknownInfo );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNotSupported )
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( unknown ) == NULL, ETrue );

    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( profile1 ) == NULL, ETrue );
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( profile2 ) != NULL, ETrue );

    // Because profile2 is recycled, it won't be deleted
    profile2Info = TAiFwPublisherInfo( profile2, TAiFwCallback( CallBack, this ), EAiFwPluginShutdown );
    iFactory->DestroyPlugin( profile2Info );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( profile2 ) != NULL, ETrue );

    iFactory->Publishers().ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// UT_AiPluginFactory::TestPluginByUidL()
//
// ----------------------------------------------------------------------------
//
void UT_AiPluginFactory::TestPluginByUidL()
    {
    THsPublisherInfo profile1( 
        TUid::Uid( KProfileUid ), KProfile, KNs1 );

    THsPublisherInfo unknown(
        TUid::Uid( KUnknownUid ), KUnknown, KNs1 );

    TAiFwPublisherInfo profile1Info( profile1, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iFactory->LoadPlugin( profile1Info );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );

    TAiFwPublisherInfo unknownInfo( unknown, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iFactory->LoadPlugin( unknownInfo );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNotSupported );
        
    EUNIT_ASSERT_EQUALS( iFactory->PluginByUid( unknown.Uid() ) == NULL, ETrue );
    EUNIT_ASSERT_EQUALS( iFactory->PluginByUid( profile1.Uid() ) != NULL, ETrue );

    // Because profile1 is recycled, it won't be deleted
    profile1Info = TAiFwPublisherInfo( profile1, TAiFwCallback( CallBack, this ), EAiFwPluginShutdown );
    iFactory->DestroyPlugin( profile1Info );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iFactory->PluginByUid( profile1.Uid() ) != NULL, ETrue );

    iFactory->Publishers().ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// UT_AiPluginFactory::TestPluginByNameL()
//
// ----------------------------------------------------------------------------
//
void UT_AiPluginFactory::TestPluginByNameL()
    {
    THsPublisherInfo profile1( 
        TUid::Uid( KProfileUid ), KProfile, KNs1 );

    THsPublisherInfo unknown(
        TUid::Uid( KUnknownUid ), KUnknown, KNs1 );

    TAiFwPublisherInfo profile1Info( profile1, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iFactory->LoadPlugin( profile1Info );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );

    TAiFwPublisherInfo unknownInfo( unknown, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iFactory->LoadPlugin( unknownInfo );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNotSupported );

    EUNIT_ASSERT_EQUALS( iFactory->PluginByName( unknown.Name() ) == NULL, ETrue );
    EUNIT_ASSERT_EQUALS( iFactory->PluginByName( profile1.Name() ) != NULL, ETrue );

    // Because profile1 is recycled, it won't be deleted
    profile1Info = TAiFwPublisherInfo( profile1, TAiFwCallback( CallBack, this ), EAiFwPluginShutdown );
    iFactory->DestroyPlugin( profile1Info );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iFactory->PluginByName( profile1.Name() ) != NULL, ETrue );

    iFactory->Publishers().ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// UT_AiPluginFactory::TestGetPublishersL()
//
// ----------------------------------------------------------------------------
//
void UT_AiPluginFactory::TestGetPublishersL()
    {      
    THsPublisherInfo profile1( 
        TUid::Uid( KProfileUid ), KProfile, KNs1 );
    
    THsPublisherInfo profile2( 
        TUid::Uid( KProfileUid ), KProfile, KNs2 );
    
    THsPublisherInfo unknown(
        TUid::Uid( KUnknownUid ), KUnknown, KNs1 );
    
    THsPublisherInfo data( 
        TUid::Uid( KOrganizerUid ), KOrganizer, KNs1 );

    
    EUNIT_ASSERT_EQUALS( iFactory->Publishers().Count(), 0 );
        
    TAiFwPublisherInfo profile1Info( profile1, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iFactory->LoadPlugin( profile1Info );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );
    
    EUNIT_ASSERT_EQUALS( iFactory->Publishers().Count(), 1 );
    
    TAiFwPublisherInfo dataInfo( data, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iFactory->LoadPlugin( dataInfo );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );
        
    EUNIT_ASSERT_EQUALS( iFactory->Publishers().Count(), 2 );
    
    TAiFwPublisherInfo profile2Info( profile2, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iFactory->LoadPlugin( profile2Info );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );
           
    EUNIT_ASSERT_EQUALS( iFactory->Publishers().Count(), 2 );
    
    TAiFwPublisherInfo unknownInfo( unknown, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iFactory->LoadPlugin( unknownInfo );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNotSupported );
    EUNIT_ASSERT_EQUALS( iFactory->Publishers().Count(), 2 );
    
    profile1Info = TAiFwPublisherInfo( profile1, TAiFwCallback( CallBack, this ), EAiFwPluginShutdown );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    iFactory->DestroyPlugin( profile1Info );
    EUNIT_ASSERT_EQUALS( iFactory->Publishers().Count(), 2 );
    
    profile2Info = TAiFwPublisherInfo( profile2, TAiFwCallback( CallBack, this ), EAiFwPluginShutdown );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    iFactory->DestroyPlugin( profile2Info );
    EUNIT_ASSERT_EQUALS( iFactory->Publishers().Count(), 2 );
    
    dataInfo = TAiFwPublisherInfo( data, TAiFwCallback( CallBack, this ), EAiFwPluginShutdown );
    iFactory->DestroyPlugin( dataInfo );
    iPeriodic->Start( KDestroyDelay, KDestroyDelay, TCallBack( TimerCallBack, this ) );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iFactory->Publishers().Count(), 1 );
        
    iFactory->Publishers().ResetAndDestroy();
    EUNIT_ASSERT_EQUALS( iFactory->Publishers().Count(), 0 );
    }

// ----------------------------------------------------------------------------
// UT_AiPluginFactory::TestUpgradePublishersL()
//
// ----------------------------------------------------------------------------
//
void UT_AiPluginFactory::TestUpgradePublishersL()
    {
    RArray< THsPublisherInfo > array;
    CleanupClosePushL( array );

    CleanupStack::PopAndDestroy( &array );
    }

// ----------------------------------------------------------------------------
// CAiPluginFactory::CallBack()
//
// ----------------------------------------------------------------------------
//
void UT_AiPluginFactory::CallBack( TAny* aPtr, TInt aResult )
    {
    UT_AiPluginFactory* self = static_cast< UT_AiPluginFactory* >( aPtr );
    self->iResult = aResult;
    self->iWait->AsyncStop();
    }

// ----------------------------------------------------------------------------
// CAiPluginFactory::TimerCallBack
//
// ----------------------------------------------------------------------------
//
TInt UT_AiPluginFactory::TimerCallBack( TAny* aPtr )
    {
    UT_AiPluginFactory* self = static_cast< UT_AiPluginFactory* >( aPtr );
    
    if ( self->iPeriodic->IsActive() )
        {
        self->iPeriodic->Cancel();
        }
    
    self->iWait->AsyncStop();
    return KErrNone;
    }


//-----------------------------------------------------------------------------
// Test case table
//
//-----------------------------------------------------------------------------
//
EUNIT_BEGIN_TEST_TABLE(
    UT_AiPluginFactory,
    "Unit test suite for CAiPluginFactory",
    "UNIT" )

    EUNIT_TEST(
        "Create/Destroy Plugin",
        "CAiPluginFactory",
        "",
        "FUNCTIONALITY",
        SetupL, TestCreateDestroyPluginL, Teardown )
    
    EUNIT_TEST(
        "Create/Destroy All",
        "CAiPluginFactory",
        "",
        "FUNCTIONALITY",
        SetupL, TestCreateDestroyAllL, Teardown )
    
    EUNIT_TEST(
        "PluginByInfo",
        "CAiPluginFactory",
        "",
        "FUNCTIONALITY",
        SetupL, TestPluginByInfoL, Teardown )
    
    EUNIT_TEST(
        "PluginByUid",
        "CAiPluginFactory",
        "",
        "FUNCTIONALITY",
        SetupL, TestPluginByUidL, Teardown )
    
    EUNIT_TEST(
        "PluginByName",
        "CAiPluginFactory",
        "",
        "FUNCTIONALITY",
        SetupL, TestPluginByNameL, Teardown )
        
    EUNIT_TEST(
        "Get Publishers",
        "CAiPluginFactory",
        "",
        "FUNCTIONALITY",
        SetupL, TestGetPublishersL, Teardown )

    EUNIT_TEST(
        "Upgrade Publishers",
        "CAiPluginFactory",
        "",
        "FUNCTIONALITY",
        SetupL, TestUpgradePublishersL, Teardown )
        
EUNIT_END_TEST_TABLE

// End of file
