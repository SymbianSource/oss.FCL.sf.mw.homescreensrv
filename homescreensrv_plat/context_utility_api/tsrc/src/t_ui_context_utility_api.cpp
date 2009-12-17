/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  T_ui_context_utility_api class member functions
*
*/



// INCLUDE FILES
#include <Stiftestinterface.h>
#include <aknappui.h>
#include <cfclient.h>
#include "hgcontextutilityimpl.h"
#include "hgcontextutility.h"
#include "hgcontexttypes.h"
#include "hgtestbasemacros.h"
#include "T_ui_context_utility_api.h"
#include "wait.h"
#include "hgtestbasemacros.h"
#include <cvpbkcontactmanager.h>
#include <mvpbkcontactstore.h>
#include <mvpbkcontactfieldtextdata.h>
#include <mvpbkstorecontact.h>
#include <MVPbkContactObserver.h>
#include <MVPbkBatchOperationObserver.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactOperationBase.h>
#include <VPbkEng.rsg>
#include <mdesession.h>
#include <e32math.h>
#include <aknappui.h>
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CT_ui_context_utility_api::CT_ui_context_utility_api
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CT_ui_context_utility_api::CT_ui_context_utility_api()
    {
    iTestDllName = _L("CT_ui_context_utility_api").AllocL();
    }

// -----------------------------------------------------------------------------
// CT_ui_context_utility_api::ConstructL
// Symbian 2nd phase constructor can leave.
//
// Note: If OOM test case uses STIF Logger, then STIF Logger must be created
// with static buffer size parameter (aStaticBufferSize). Otherwise Logger
// allocates memory from heap and therefore causes error situations with OOM
// testing. For more information about STIF Logger construction, see STIF Users
// Guide.
// -----------------------------------------------------------------------------
//
void CT_ui_context_utility_api::ConstructL()
    {
    CHgTestBase::BaseConstructL( KT_ui_context_utility_apiLogPath,
                             KT_ui_context_utility_apiLogFile );
    }

// -----------------------------------------------------------------------------
// CT_ui_context_utility_api::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CT_ui_context_utility_api* CT_ui_context_utility_api::NewL()
    {
    CT_ui_context_utility_api* self = new (ELeave) CT_ui_context_utility_api;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CT_ui_context_utility_api::~CT_ui_context_utility_api()
    {

    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point
// Returns: CTestModuleBase*: Pointer to Test Module object
// -----------------------------------------------------------------------------
//
EXPORT_C CTestModuleBase* LibEntryL()
    {
    return CT_ui_context_utility_api::NewL();
    }

// -----------------------------------------------------------------------------
// SetRequirements handles test module parameters(implements evolution
// version 1 for test module's heap and stack sizes configuring).
// Returns: TInt: Symbian error code.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SetRequirements( CTestModuleParam*& /*aTestModuleParam*/,
                                TUint32& /*aParameterValid*/ )
    {
    return KErrNone;
    }

//// -----------------------------------------------------------------------------
//// CT_ui_context_utility_api::RunTestL
//// Method responsible for enumerating and running test cases (and also setup and teardown activities before
//// and after each test case).
//// -----------------------------------------------------------------------------
TInt CT_ui_context_utility_api::RunTestL(
        CT_ui_context_utility_api::TCallReason aRunReason, 
        TInt aTestToRun, 
        RPointerArray<TTestCaseInfo>& aTestCases,
        TTestResult& aResult )
    {
    TInt _test_case_no = -1;
    //test cases, setup and teardown include
    
    
    if( aRunReason == CHgTestBase::ERunTestCase )
        {
        LoadTestCasesL(_test_case_no, aRunReason, STIF_RUN_SETUP, aTestCases, aResult);
		LoadTestCasesL(_test_case_no, aRunReason, aTestToRun, aTestCases, aResult);
		LoadTestCasesL(_test_case_no, aRunReason, STIF_RUN_TEARDOWN, aTestCases, aResult);
        CreateEnvL();
        _test_case_no = -1;
        TRAPD(
                errCode, 
                LoadTestCasesL(_test_case_no, aRunReason, STIF_RUN_SETUP, aTestCases, aResult);
                LoadTestCasesL(_test_case_no, aRunReason, aTestToRun, aTestCases, aResult);
                LoadTestCasesL(_test_case_no, aRunReason, STIF_RUN_TEARDOWN, aTestCases, aResult);
             )
        DestroyEnv();
        User::LeaveIfError( errCode );
        }
    else
    	{
    	LoadTestCasesL(_test_case_no, aRunReason, aTestToRun, aTestCases, aResult);
    	}
    
    return _test_case_no;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void CT_ui_context_utility_api::CreateEnvL()
    {
    CActiveScheduler::Install ( NULL );
    ENV_cleanup = CTrapCleanup::New();
    TInt ENV_err = KErrNone;
    ENV_env = new CEikonEnv;
    if ( ENV_env ) 
        { 
        TRAP( ENV_err, ENV_env->ConstructL() ); 
        if ( ENV_err ) 
            { 
            return; 
            }
        ENV_aknAppUI = new CAknAppUi;
        if (ENV_aknAppUI)
            {
            TRAP( ENV_err, ENV_aknAppUI->BaseConstructL( CEikAppUi::ENoAppResourceFile ) ); \
            if ( ENV_err )
                {
                return;
                }
            }
        }
    }
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void CT_ui_context_utility_api::DestroyEnv()
    {
    TInt KWait( 500000 );
    CActiveSchedulerWait*  wait = new (ELeave) CActiveSchedulerWait();
    CPeriodic*  periodic = CPeriodic::NewL( CActive::EPriorityIdle );
    periodic->Start(KWait, KWait, TCallBack( CHgTestBase::CallStop, wait ) );
    wait->Start();
    delete periodic;
    delete wait;
    
    if ( ENV_aknAppUI ) 
        { 
        ENV_aknAppUI->PrepareToExit(); 
        }
    if ( ENV_env ) 
        { 
        ENV_env->DestroyEnvironment(); 
        }
    if( ENV_cleanup )
        {
        delete ENV_cleanup;
        }
    CActiveScheduler::Install ( iActiveScheduler );
    }
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
TInt CT_ui_context_utility_api::LoadTestCasesL(
                    TInt& _test_case_no,
                    CT_ui_context_utility_api::TCallReason aRunReason,
                    TInt aTestToRun,
                    RPointerArray<TTestCaseInfo>& aTestCases,
                    TTestResult& aResult)
    {
#define TEST_CASES
    #include "..\src\T_ui_context_utility_api_cases.cpp"
#undef TEST_CASES
    return KErrNone;
    }
//  End of File
