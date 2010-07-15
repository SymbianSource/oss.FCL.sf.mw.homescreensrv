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
* Description: EUnit unit test class for CAiEventHandler
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
#include "ut_aieventhandler.h"
#include "aiuicontrollermanager_stub.h"

#include "aipluginfactory.h"
#include "aieventhandler.h"
#include "aifwpublisherinfo.h"
#include "aistatemanager.h"

// Constants
_LIT( KProfile, "Profile" );
_LIT( KDevStat, "DeviceStatus" );
_LIT( KUnknown, "Unknown" );

const TInt KProfileUid( 0x10275101 );
const TInt KDeviceStatusUid( 0x102750F8 );
const TInt KUnknownUid( 0xBABE2BED );

_LIT8( KNs1, "namespace1" );
_LIT8( KNs2, "namespace2" );

// ======== LOCAL FUNCTIONS ========
    
// ======== MEMBER FUNCTIONS =======
// ----------------------------------------------------------------------------
// UT_AiEventHandler::NewL()
//
// ----------------------------------------------------------------------------
//
UT_AiEventHandler* UT_AiEventHandler::NewL()
    {
    UT_AiEventHandler* self = UT_AiEventHandler::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// UT_AiEventHandler::NewLC()
//
// ----------------------------------------------------------------------------
//
UT_AiEventHandler* UT_AiEventHandler::NewLC()
    {
    UT_AiEventHandler* self = new ( ELeave ) UT_AiEventHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// UT_AiEventHandler::~UT_AiEventHandler()
//
// ----------------------------------------------------------------------------
//
UT_AiEventHandler::~UT_AiEventHandler()
    {    
    }

// ----------------------------------------------------------------------------
// UT_AiEventHandler::UT_AiEventHandler()
//
// ----------------------------------------------------------------------------
//
UT_AiEventHandler::UT_AiEventHandler()
    {
    }

// ----------------------------------------------------------------------------
// UT_AiEventHandler::ConstructL()
//
// ----------------------------------------------------------------------------
//
void UT_AiEventHandler::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// ----------------------------------------------------------------------------
// UT_AiEventHandler::SetupL()
//
// ----------------------------------------------------------------------------
//
void UT_AiEventHandler::SetupL()
    {
    iUiMgr = CAiUiControllerManager::NewL();
    
    iFactory = CAiPluginFactory::NewL( *iUiMgr );
    iFactory->ConstructL();
    
    iStateManager = CAiStateManager::NewL( *iFactory );
    
    iHandler = CAiEventHandler::NewL( *iFactory );

    iWait = new ( ELeave ) CActiveSchedulerWait;
    
    iResult = KErrNone;
    }

// ----------------------------------------------------------------------------
// UT_AiEventHandler::Teardown()
//
// ----------------------------------------------------------------------------
//
void UT_AiEventHandler::Teardown()
    {    
    delete iHandler;
    iHandler = NULL;
    
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
    }

//------------------------------------------------------------------------------
// UT_AiEventHandler::TestHandlePluginEventL()
//
//------------------------------------------------------------------------------
//
void UT_AiEventHandler::TestHandlePluginEventL()
    {
    THsPublisherInfo profile( 
        TUid::Uid( KProfileUid ), KProfile, KNs1 );
    
    THsPublisherInfo devstat( 
        TUid::Uid( KDeviceStatusUid ), KDevStat, KNs2 );
    
    THsPublisherInfo unknown(
        TUid::Uid( KUnknownUid ), KUnknown, KNs1 );
    
    _LIT( KProfileEvent, "Profile/SwitchProfileByIndex" );
    _LIT( KDeviceStatusEvent, "DeviceStatus/DummyEvent" );
    _LIT( KUnknownEvent, "Unknown/DummyEvent" );
    _LIT( KIvalidEvent, "еедaдеp39469568sdfsdfs" );
    _LIT( KInvalidEvent2, "--\\\afsd////еедaдеp39469568sdfsdfs" );
    
    TAiFwPublisherInfo profileInfo( profile, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iFactory->LoadPlugin( profileInfo );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( profile ) != NULL, ETrue );

    TAiFwPublisherInfo devstatInfo( devstat, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iFactory->LoadPlugin( devstatInfo );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );    
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( devstat ) != NULL, ETrue );
    
    iHandler->HandlePluginEvent( KProfileEvent );
    iHandler->HandlePluginEvent( KDeviceStatusEvent );
    iHandler->HandlePluginEvent( KUnknownEvent );
    iHandler->HandlePluginEvent( KIvalidEvent );
    iHandler->HandlePluginEvent( KInvalidEvent2 );

    iHandler->HandlePluginEventL( profile, KProfileEvent );
    iHandler->HandlePluginEventL( devstat, KDeviceStatusEvent );
    iHandler->HandlePluginEventL( unknown, KUnknownEvent );
    
    profileInfo = TAiFwPublisherInfo( profile, TAiFwCallback( CallBack, this ), EAiFwPluginShutdown );
    iFactory->DestroyPlugin( profileInfo );
    devstatInfo = TAiFwPublisherInfo( devstat, TAiFwCallback( CallBack, this ), EAiFwPluginShutdown );
    iFactory->DestroyPlugin( devstatInfo );
    }

//------------------------------------------------------------------------------
// UT_AiEventHandler::TestHasMenuItemL()
//
//------------------------------------------------------------------------------
//
void UT_AiEventHandler::TestHasMenuItemL()
    {
    THsPublisherInfo profile( 
        TUid::Uid( KProfileUid ), KProfile, KNs1 );
    
    THsPublisherInfo devstat( 
        TUid::Uid( KDeviceStatusUid ), KDevStat, KNs2 );
    
    THsPublisherInfo unknown(
        TUid::Uid( KUnknownUid ), KUnknown, KNs1 );

    _LIT( KMenuitem, "Some_menuitem" );
    
    TAiFwPublisherInfo profileInfo( profile, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iFactory->LoadPlugin( profileInfo );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( profile ) != NULL, ETrue );

    TAiFwPublisherInfo devstatInfo( devstat, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iFactory->LoadPlugin( devstatInfo );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( devstat ) != NULL, ETrue );
    
    iHandler->HasMenuItemL( profile, KMenuitem );
    iHandler->HasMenuItemL( devstat, KMenuitem );
    iHandler->HasMenuItemL( unknown, KMenuitem );
    
    profileInfo = TAiFwPublisherInfo( profile, TAiFwCallback( CallBack, this ), EAiFwPluginShutdown );
    iFactory->DestroyPlugin( profileInfo );
    
    devstatInfo = TAiFwPublisherInfo( devstat, TAiFwCallback( CallBack, this ), EAiFwPluginShutdown );
    iFactory->DestroyPlugin( devstatInfo );    
    }

//------------------------------------------------------------------------------
// UT_AiEventHandler::TestRefreshContentL()
//
//------------------------------------------------------------------------------
//
void UT_AiEventHandler::TestRefreshContentL()
    {
    THsPublisherInfo profile( 
        TUid::Uid( KProfileUid ), KProfile, KNs1 );
    
    THsPublisherInfo devstat( 
        TUid::Uid( KDeviceStatusUid ), KDevStat, KNs2 );
    
    THsPublisherInfo unknown(
        TUid::Uid( KUnknownUid ), KUnknown, KNs1 );

    _LIT( KProfileContent, "Profile/ActiveProfileName" );
    _LIT( KDeviceStatusContent, "DeviceStatus/NetworkIdentity" );
    _LIT( KUnknownContent, "Unknown/DummyContentId" );
    
    _LIT( KNetworkIdentity, "NetworkIdentity" );
    
    TAiFwPublisherInfo profileInfo( profile, TAiFwCallback( CallBack, this ) , EAiFwPluginStartup );
    iFactory->LoadPlugin( profileInfo );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );           
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( profile ) != NULL, ETrue );

    TAiFwPublisherInfo devstatInfo( devstat, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iFactory->LoadPlugin( devstatInfo );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );           
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( devstat ) != NULL, ETrue );
    
    iHandler->RefreshContent( KProfileContent );
    iHandler->RefreshContent( KDeviceStatusContent );
    iHandler->RefreshContent( KUnknownContent );

    iHandler->RefreshContent( profile, KProfileContent );
    iHandler->RefreshContent( devstat, KDeviceStatusContent );
    iHandler->RefreshContent( unknown, KUnknownContent );
    
    iHandler->RefreshContent( devstat, KNetworkIdentity );
    
    profileInfo = TAiFwPublisherInfo( profile, TAiFwCallback( CallBack, this ), EAiFwPluginShutdown );
    iFactory->DestroyPlugin( profileInfo );
    
    devstatInfo = TAiFwPublisherInfo( devstat, TAiFwCallback( CallBack, this ), EAiFwPluginShutdown );
    iFactory->DestroyPlugin( devstatInfo );        
    }

//------------------------------------------------------------------------------
// UT_AiEventHandler::TestSuspendContentL()
//
//------------------------------------------------------------------------------
//
void UT_AiEventHandler::TestSuspendContentL()
    {
    THsPublisherInfo profile( 
        TUid::Uid( KProfileUid ), KProfile, KNs1 );
    
    THsPublisherInfo devstat( 
        TUid::Uid( KDeviceStatusUid ), KDevStat, KNs2 );
    
    THsPublisherInfo unknown(
        TUid::Uid( KUnknownUid ), KUnknown, KNs1 );

    _LIT( KProfileContent, "ActiveProfileName" );
    _LIT( KDeviceStatusContent, "NetworkIdentity" );
    _LIT( KUnknownContent, "DummyContentId" );       

    TAiFwPublisherInfo profileInfo( profile, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iFactory->LoadPlugin( profileInfo );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );               
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( profile ) != NULL, ETrue );

    TAiFwPublisherInfo devstatInfo( devstat, TAiFwCallback( CallBack, this ), EAiFwPluginStartup );
    iFactory->LoadPlugin( devstatInfo );
    iWait->Start();
    EUNIT_ASSERT_EQUALS( iResult, KErrNone );               
    EUNIT_ASSERT_EQUALS( iFactory->PluginByInfo( devstat ) != NULL, ETrue );

    iHandler->SuspendContent( profile, KProfileContent );
    iHandler->SuspendContent( devstat, KDeviceStatusContent );
    iHandler->SuspendContent( unknown, KUnknownContent );
             
    profileInfo = TAiFwPublisherInfo( profile, TAiFwCallback( CallBack, this ), EAiFwPluginShutdown );
    iFactory->DestroyPlugin( profileInfo );
    
    devstatInfo = TAiFwPublisherInfo( devstat, TAiFwCallback( CallBack, this ), EAiFwPluginShutdown );
    iFactory->DestroyPlugin( devstatInfo );        
    }

// ----------------------------------------------------------------------------
// CAiPluginFactory::CallBack()
//
// ----------------------------------------------------------------------------
//
void UT_AiEventHandler::CallBack( TAny* aPtr, TInt aResult )
    {
    UT_AiEventHandler* self = static_cast< UT_AiEventHandler* >( aPtr );
    self->iResult = aResult;
    self->iWait->AsyncStop();
    }

//------------------------------------------------------------------------------
// Test case table
//
//------------------------------------------------------------------------------
//
EUNIT_BEGIN_TEST_TABLE(
    UT_AiEventHandler,
    "Unit test suite for CAiEventHandler",
    "UNIT" )

    EUNIT_TEST(
        "HandlePluginEvent",
        "CAiEventHandler",
        "",
        "FUNCTIONALITY",
        SetupL, TestHandlePluginEventL, Teardown )

    EUNIT_TEST(
        "HasMenuItem",
        "CAiEventHandler",
        "",
        "FUNCTIONALITY",
        SetupL, TestHasMenuItemL, Teardown )

    EUNIT_TEST(
        "RefreshContent",
        "CAiEventHandler",
        "",
        "FUNCTIONALITY",
        SetupL, TestRefreshContentL, Teardown )
            
    EUNIT_TEST(
        "SuspendContent",
        "CAiEventHandler",
        "",
        "FUNCTIONALITY",
        SetupL, TestSuspendContentL, Teardown )
             
EUNIT_END_TEST_TABLE

// End of file
