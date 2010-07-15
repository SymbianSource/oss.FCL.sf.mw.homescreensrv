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
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/teunitassertioninfo.h>

// User includes
#include "aifw_stub.h"
#include "aiuicontroller_stub.h"

#include "ut_aiuicontrollermanager.h"

#include "aiuicontrollermanager.h"

// Constants


// ======== LOCAL FUNCTIONS ========
// ----------------------------------------------------------------------------
// CAiUiControllerStub
//
// ----------------------------------------------------------------------------
//
CAiUiControllerStub* AsCAiUiControllerStub( CAiUiController* aController )
    {
    return static_cast< CAiUiControllerStub* >( aController );
    }

// ======== MEMBER FUNCTIONS =======
// ----------------------------------------------------------------------------
// UT_AiUiControllerManager::NewL()
//
// ----------------------------------------------------------------------------
//
UT_AiUiControllerManager* UT_AiUiControllerManager::NewL()
    {
    UT_AiUiControllerManager* self = UT_AiUiControllerManager::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// UT_AiUiControllerManager::NewLC()
//
// ----------------------------------------------------------------------------
//
UT_AiUiControllerManager* UT_AiUiControllerManager::NewLC()
    {
    UT_AiUiControllerManager* self = new ( ELeave ) UT_AiUiControllerManager();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// UT_AiUiControllerManager::~UT_AiUiControllerManager()
//
// ----------------------------------------------------------------------------
//
UT_AiUiControllerManager::~UT_AiUiControllerManager()
    {    
    }

// ----------------------------------------------------------------------------
// UT_AiUiControllerManager::UT_AiUiControllerManager()
//
// ----------------------------------------------------------------------------
//
UT_AiUiControllerManager::UT_AiUiControllerManager()
    {
    }

// ----------------------------------------------------------------------------
// UT_AiUiControllerManager::ConstructL()
//
// ----------------------------------------------------------------------------
//
void UT_AiUiControllerManager::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// ----------------------------------------------------------------------------
// UT_AiUiControllerManager::SetupL()
//
// ----------------------------------------------------------------------------
//
void UT_AiUiControllerManager::SetupL()
    {
    iAiFw = CAiFw::NewL();
    
    iUiMgr = CAiUiControllerManager::NewL( iAiFw );
    
    // Get rid of real UI controllers
    iUiMgr->UiControllers().ResetAndDestroy();
    
    CAiUiControllerStub* stub1 = CAiUiControllerStub::NewL();
    iUiMgr->UiControllers().Append( stub1 );
    
    CAiUiControllerStub* stub2 = CAiUiControllerStub::NewL();
    iUiMgr->UiControllers().Append( stub2 );
    
    stub1->iMain = ETrue;
    iUiMgr->iMainUiController = stub1;    
    }

// ----------------------------------------------------------------------------
// UT_AiUiControllerManager::Teardown()
//
// ----------------------------------------------------------------------------
//
void UT_AiUiControllerManager::Teardown()
    {
    delete iUiMgr;
    iUiMgr = NULL;
    
    delete iAiFw;
    iAiFw = NULL;
    }

// ----------------------------------------------------------------------------
// UT_AiUiControllerManager::TestGetUiControllersL()
//
// ----------------------------------------------------------------------------
//
void UT_AiUiControllerManager::TestGetUiControllersL()
    {
    EUNIT_ASSERT_EQUALS( iUiMgr->UiControllers().Count() == 2, ETrue );    
    }

// ----------------------------------------------------------------------------
// UT_AiUiControllerManager::TestActivateUIL()
//
// ----------------------------------------------------------------------------
//
void UT_AiUiControllerManager::TestActivateUIL()
    {
    for ( TInt i = 0; i < iUiMgr->UiControllers().Count(); i++ )
        {
        CAiUiControllerStub* stub = 
            AsCAiUiControllerStub( iUiMgr->UiControllers()[i] );
                
        EUNIT_ASSERT_EQUALS( stub->iActive, EFalse );
        }
    
    iUiMgr->ActivateUI();
    
    for ( TInt i = 0; i < iUiMgr->UiControllers().Count(); i++ )
        {
        CAiUiControllerStub* stub =
            AsCAiUiControllerStub( iUiMgr->UiControllers()[i] );
        
        EUNIT_ASSERT_EQUALS( stub->iActive, ETrue );
        }    
    }

// ----------------------------------------------------------------------------
// UT_AiUiControllerManager::TestGetMainUiControllerL()
//
// ----------------------------------------------------------------------------
//
void UT_AiUiControllerManager::TestGetMainUiControllerL()
    {    
    MAiMainUiController* main( &iUiMgr->MainUiController() );
    
    CAiUiControllerStub* stub( NULL );
    
    TInt count( 0 );
    
    for ( TInt i = 0; i < iUiMgr->UiControllers().Count(); i++ )
        {
        CAiUiControllerStub* temp = 
            AsCAiUiControllerStub( iUiMgr->UiControllers()[i] );
        
        if ( temp->iMain )
            {
            stub = temp; 
            count++;
            }
        }
    
    EUNIT_ASSERT_EQUALS( count, 1 );
    EUNIT_ASSERT_EQUALS( stub == main, ETrue );    
    }

// ----------------------------------------------------------------------------
// UT_AiUiControllerManager::TestIsMainUiControllerL()
//
// ----------------------------------------------------------------------------
//
void UT_AiUiControllerManager::TestIsMainUiControllerL()
    {
    for ( TInt i = 0; i < iUiMgr->UiControllers().Count(); i++ )
        {
        CAiUiControllerStub* stub = 
            AsCAiUiControllerStub( iUiMgr->UiControllers()[i] );
        
        if ( stub->iMain )
            {
            EUNIT_ASSERT_EQUALS( iUiMgr->IsMainUiController( *stub ), ETrue );
            }
        else
            {
            EUNIT_ASSERT_EQUALS( iUiMgr->IsMainUiController( *stub ), EFalse );
            }
        }                 
    }

// ----------------------------------------------------------------------------
// UT_AiUiControllerManager::TestRunApplicationL()
//
// ----------------------------------------------------------------------------
//
void UT_AiUiControllerManager::TestRunApplicationL()
    {
    iUiMgr->RunApplicationL();

    TInt count( 0 );
    
    for ( TInt i = 0; i < iUiMgr->UiControllers().Count(); i++ )
        {
        CAiUiControllerStub* stub = 
            AsCAiUiControllerStub( iUiMgr->UiControllers()[i] );
        
        if ( stub->iRunning )
            {
            count++;
            }               
        }    

    EUNIT_ASSERT_EQUALS( count, 1 );
    
    CAiUiControllerStub* main = 
        static_cast< CAiUiControllerStub* >( &iUiMgr->MainUiController() );
    
    CAiUiControllerStub* running( NULL );
        
    for ( TInt i = 0; i < iUiMgr->UiControllers().Count(); i++ )
        {
        CAiUiControllerStub* stub =
            AsCAiUiControllerStub( iUiMgr->UiControllers()[i] );
        
        if ( stub->iRunning )
            {
            running = stub;
            break;                       
            }               
        }    
    
    EUNIT_ASSERT_EQUALS( running != NULL, ETrue );
    EUNIT_ASSERT_EQUALS( main != NULL, ETrue );
    EUNIT_ASSERT_EQUALS( running == main, ETrue );                 
    }

// ----------------------------------------------------------------------------
// UT_AiUiControllerManager::TestLoadUIDefinitionL()
//
// ----------------------------------------------------------------------------
//
void UT_AiUiControllerManager::TestLoadUIDefinitionL()
    {
    for ( TInt i = 0; i < iUiMgr->UiControllers().Count(); i++ )
        {
        CAiUiControllerStub* stub =
            AsCAiUiControllerStub( iUiMgr->UiControllers()[i] );

        EUNIT_ASSERT_EQUALS( stub->iUiDefinition, EFalse );
        }        
    
    iUiMgr->LoadUIDefinition();
    
    for ( TInt i = 0; i < iUiMgr->UiControllers().Count(); i++ )
        {
        CAiUiControllerStub* stub =
            AsCAiUiControllerStub( iUiMgr->UiControllers()[i] );

        EUNIT_ASSERT_EQUALS( stub->iUiDefinition, ETrue );
        }        
    }

// ----------------------------------------------------------------------------
// UT_AiUiControllerManager::TestGetCoeEnvL()
//
// ----------------------------------------------------------------------------
//
void UT_AiUiControllerManager::TestGetCoeEnvL()
    {
    CCoeEnv* env( &iUiMgr->CoeEnv() );
    
    EUNIT_ASSERT_EQUALS( env != NULL, ETrue );
    }

// ----------------------------------------------------------------------------
// UT_AiUiControllerManager::TestDestroySecondaryUiControllersL()
//
// ----------------------------------------------------------------------------
//
void UT_AiUiControllerManager::TestDestroySecondaryUiControllersL()
    {
    TInt count( 0 );
    
    for ( TInt i = 0; i < iUiMgr->UiControllers().Count(); i++ )
        {
        CAiUiControllerStub* stub = 
            AsCAiUiControllerStub( iUiMgr->UiControllers()[i] );
                       
        if ( !stub->iMain )
            {
            count++;
            }
        }    

    EUNIT_ASSERT_EQUALS( count > 0, ETrue );
    
    iUiMgr->DestroySecondaryUiControllers();
    
    count = 0;
    
    for ( TInt i = 0; i < iUiMgr->UiControllers().Count(); i++ )
        {
        CAiUiControllerStub* stub = 
            AsCAiUiControllerStub( iUiMgr->UiControllers()[i] );
              
        if ( !stub->iMain )
            {
            count++;
            }        
        }    
    
    EUNIT_ASSERT_EQUALS( count == 0, ETrue );
    
    for ( TInt i = 0; i < iUiMgr->UiControllers().Count(); i++ )
        {
        CAiUiControllerStub* stub = 
            AsCAiUiControllerStub( iUiMgr->UiControllers()[i] );
              
        EUNIT_ASSERT_EQUALS( stub->iMain, ETrue );
        }        
    }

// ----------------------------------------------------------------------------
// UT_AiUiControllerManager::TestExitMainControllerL()
//
// ----------------------------------------------------------------------------
//
void UT_AiUiControllerManager::TestExitMainControllerL()
    {
    TInt count( 0 );
    
    for ( TInt i = 0; i < iUiMgr->UiControllers().Count(); i++ )
        {
        CAiUiControllerStub* stub = 
            AsCAiUiControllerStub( iUiMgr->UiControllers()[i] );
                       
        if ( stub->iMain )
            {
            count++;
            
            EUNIT_ASSERT_EQUALS( stub->iExit, EFalse );
            }
        }
    
    EUNIT_ASSERT_EQUALS( count > 0, ETrue );
    
    TInt count2( 0 );
    
    iUiMgr->ExitMainController();

    for ( TInt i = 0; i < iUiMgr->UiControllers().Count(); i++ )
        {
        CAiUiControllerStub* stub = 
            AsCAiUiControllerStub( iUiMgr->UiControllers()[i] );
                       
        if ( stub->iMain )
            {
            count2++;
            EUNIT_ASSERT_EQUALS( stub->iExit, ETrue );
            }
        }    
    
    EUNIT_ASSERT_EQUALS( count == count2, ETrue );
    }

// ----------------------------------------------------------------------------
// UT_AiUiControllerManager::TestSetStateHandlerL()
//
// ----------------------------------------------------------------------------
//
void UT_AiUiControllerManager::TestSetStateHandlerL()
    {
    for ( TInt i = 0; i < iUiMgr->UiControllers().Count(); i++ )
        {
        CAiUiControllerStub* stub = 
            AsCAiUiControllerStub( iUiMgr->UiControllers()[i] );
        
        EUNIT_ASSERT_EQUALS( stub->iStateHandler, EFalse );
        }
    
    MAiFwStateHandler* handler = reinterpret_cast< MAiFwStateHandler* >( this );
    
    iUiMgr->SetStateHandler( *handler );

    for ( TInt i = 0; i < iUiMgr->UiControllers().Count(); i++ )
        {
        CAiUiControllerStub* stub = 
            AsCAiUiControllerStub( iUiMgr->UiControllers()[i] );
        
        EUNIT_ASSERT_EQUALS( stub->iStateHandler, ETrue );
        }        
    }

//------------------------------------------------------------------------------
// Test case table
//
//------------------------------------------------------------------------------
//
EUNIT_BEGIN_TEST_TABLE(
    UT_AiUiControllerManager,
    "Unit test suite for CAiUiControllerManager",
    "UNIT" )

    EUNIT_TEST(
        "GetUiControllers",
        "CAiUiControllerManager",
        "",
        "FUNCTIONALITY",
        SetupL, TestGetUiControllersL, Teardown )

    EUNIT_TEST(
        "ActivateUi",
        "CAiUiControllerManager",
        "",
        "FUNCTIONALITY",
        SetupL, TestActivateUIL, Teardown )
    
    EUNIT_TEST(
        "GetMainUiController",
        "CAiUiControllerManager",
        "",
        "FUNCTIONALITY",
        SetupL, TestGetMainUiControllerL, Teardown )
    
    EUNIT_TEST(
        "IsMainUiController",
        "CAiUiControllerManager",
        "",
        "FUNCTIONALITY",
        SetupL, TestIsMainUiControllerL, Teardown )
    
    EUNIT_TEST(
        "RunApplication",
        "CAiUiControllerManager",
        "",
        "FUNCTIONALITY",
        SetupL, TestRunApplicationL, Teardown )
    
    EUNIT_TEST(
        "LoadUiDefinition",
        "CAiUiControllerManager",
        "",
        "FUNCTIONALITY",
        SetupL, TestLoadUIDefinitionL, Teardown )
    
    EUNIT_TEST(
        "GetCoeEnv",
        "CAiUiControllerManager",
        "",
        "FUNCTIONALITY",
        SetupL, TestGetCoeEnvL, Teardown )
    
    EUNIT_TEST(
        "DestroySecondaryUiControllers",
        "CAiUiControllerManager",
        "",
        "FUNCTIONALITY",
        SetupL, TestDestroySecondaryUiControllersL, Teardown )
    
    EUNIT_TEST(
        "ExitMainController",
        "CAiUiControllerManager",
        "",
        "FUNCTIONALITY",
        SetupL, TestExitMainControllerL, Teardown )
    
    EUNIT_TEST(
        "SetStateHandler",
        "CAiUiControllerManager",
        "",
        "FUNCTIONALITY",
        SetupL, TestSetStateHandlerL, Teardown )

EUNIT_END_TEST_TABLE

// End of file
