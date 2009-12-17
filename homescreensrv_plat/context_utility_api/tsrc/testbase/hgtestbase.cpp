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
* Description:  hgtestbase class member functions
*
*/



// INCLUDE FILES
#include <Stiftestinterface.h>
#include "hgtestbase.h"
#include <e32math.h>
#include <e32math.h>
#include <aknappui.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHgTestBase::CHgTestBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHgTestBase::CHgTestBase()
    {
    }

// -----------------------------------------------------------------------------
// CHgTestBase::ConstructL
// Symbian 2nd phase constructor can leave.
//
// Note: If OOM test case uses STIF Logger, then STIF Logger must be created
// with static buffer size parameter (aStaticBufferSize). Otherwise Logger 
// allocates memory from heap and therefore causes error situations with OOM 
// testing. For more information about STIF Logger construction, see STIF Users 
// Guide.
// -----------------------------------------------------------------------------
//
void CHgTestBase::BaseConstructL( const TDesC& aTestPath,
                                            const TDesC& aTestFile )
    {
    iLog = CStifLogger::NewL( aTestPath, 
                          aTestFile);

    // Sample how to use logging
    _LIT( KLogStart, "Logging starts!" );
    iLog->Log( KLogStart );

    iVersionLogged = EFalse;
    iNumberOfTestCases = 0;

    // Construct active scheduler
    iActiveScheduler = new ( ELeave ) CActiveScheduler;

    // Install active scheduler
    // We don't need to check whether an active scheduler is already installed
    // as this is a new thread, so there won't be one
    CActiveScheduler::Install ( iActiveScheduler );
    }

// Destructor
CHgTestBase::~CHgTestBase()
    {
    
    delete iTestDllName;
    delete iLog;
    //delete ENV_cleanup;
    delete iActiveScheduler;
    }

// -----------------------------------------------------------------------------
// CHgTestBase::InitL
// InitL is used to initialize the Test Module.
// -----------------------------------------------------------------------------
//
TInt CHgTestBase::InitL( 
    TFileName& /*aIniFile*/, 
    TBool /*aFirstTime*/ )
    {
    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CHgTestBase::GetTestCasesL
// GetTestCases is used to inquire test cases from the Test Module. Test
// cases are stored to array of test cases. The Test Framework will be 
// the owner of the data in the RPointerArray after GetTestCases return
// and it does the memory deallocation. 
// -----------------------------------------------------------------------------
//
TInt CHgTestBase::GetTestCasesL( 
    const TFileName& /*aConfig*/, 
    RPointerArray<TTestCaseInfo>& aTestCases )
    {
    TTestResult dummyResult;
    return MainTestL(CHgTestBase::EEnumerateTestCases, -1, aTestCases, dummyResult); 
    }

// -----------------------------------------------------------------------------
// CHgTestBase::OOMTestQueryL
// Used to check if a particular test case should be run in OOM conditions and 
// which memory allocations should fail.    
// -----------------------------------------------------------------------------
//
TBool CHgTestBase::OOMTestQueryL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt /*aCaseNumber*/, 
                                TOOMFailureType& /* aFailureType */, 
                                TInt& /*aFirstMemFailure*/, 
                                TInt& /*aLastMemFailure*/ ) 
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CHgTestBase::OOMTestInitializeL
// Used to perform the test environment setup for a particular OOM test case. 
// Test Modules may use the initialization file to read parameters for Test 
// Module initialization but they can also have their own configure file or 
// some other routine to initialize themselves.  
//
// NOTE: User may add implementation for OOM test environment initialization.
// Usually no implementation is required.
// -----------------------------------------------------------------------------
//
void CHgTestBase::OOMTestInitializeL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt /* aCaseNumber */ )
    {
    }

// -----------------------------------------------------------------------------
// CHgTestBase::OOMHandleWarningL
// In some cases the heap memory allocation should be skipped, either due to
// problems in the OS code or components used by the code being tested, or even 
// inside the tested components which are implemented this way on purpose (by 
// design), so it is important to give the tester a way to bypass allocation 
// failures.
//
// NOTE: User may add implementation for OOM test warning handling. Usually no
// implementation is required.
// -----------------------------------------------------------------------------
//
void CHgTestBase::OOMHandleWarningL( 
                                const TFileName& /* aTestCaseFile */,
                                const TInt /* aCaseNumber */, 
                                TInt& /* aFailNextValue */ )
    {
    }

// -----------------------------------------------------------------------------
// CHgTestBase::OOMTestFinalizeL
// Used to perform the test environment cleanup for a particular OOM test case.
//
// NOTE: User may add implementation for OOM test environment finalization.
// Usually no implementation is required.
// -----------------------------------------------------------------------------
//
void CHgTestBase::OOMTestFinalizeL( 
                                const TFileName& /* aTestCaseFile */, 
                                const TInt /* aCaseNumber */ )
    {
    }

void CHgTestBase::SendTestModuleVersion( const TDesC& aModuleName )
    {
    TVersion moduleVersion;
    moduleVersion.iMajor = TEST_MODULE_VERSION_MAJOR;
    moduleVersion.iMinor = TEST_MODULE_VERSION_MINOR;
    moduleVersion.iBuild = TEST_MODULE_VERSION_BUILD;
    
    TBool newVersionOfMethod = ETrue;
    TestModuleIf().SendTestModuleVersion(moduleVersion, aModuleName, newVersionOfMethod);
    }
    
// -----------------------------------------------------------------------------
// CHgTestBase::RunTestCaseL
// RunTestCaseL is used to run an individual test case specified
// by aTestCase. Test cases that can be run may be requested from
// Test Module by GetTestCases method before calling RunTestCase.
// -----------------------------------------------------------------------------
//
TInt CHgTestBase::RunTestCaseL(
    const TInt aCaseNumber,
    const TFileName& /*aConfig*/,
    TTestResult& aResult )
    {
    if(!iVersionLogged)
        {
        CHgTestBase::SendTestModuleVersion( *iTestDllName );
        iVersionLogged = ETrue;
        }

    RPointerArray<TTestCaseInfo> aTestCases; //temporary

    /**
     * SetupL is responsible for inicialization of all fields (etc.) common for all testcases
     * MainTestL starts required testcase
     * TeardownL destroys all data that was created by SetupL
     */
    TInt errSetup = KErrNone;
    TInt errTestcase = KErrNone;

    if(aCaseNumber > iNumberOfTestCases)
        return KErrNotFound;

    __UHEAP_MARK;

    //TRAP(errSetup, MainTestL(CHgTestBase::ERunTestCase, STIF_RUN_SETUP, aTestCases, aResult);
    TRAP(errTestcase, MainTestL(CHgTestBase::ERunTestCase, aCaseNumber, aTestCases, aResult));
      //      MainTestL(CHgTestBase::ERunTestCase, STIF_RUN_TEARDOWN, aTestCases, aResult));

    __UHEAP_MARKEND;

    if(errTestcase != KErrNone)
        {
        aResult.SetResult(errTestcase, _L("Testcase failed"));
        }
    if(errSetup != KErrNone)
        {
        aResult.SetResult(errSetup, _L("Setup or teardown failed"));
        }

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CHgTestBase::MainTestL
// Method responsible for enumerating and running test cases (and also setup and teardown activities before
// and after each test case).
// -----------------------------------------------------------------------------
//
TInt CHgTestBase::MainTestL(CHgTestBase::TCallReason aRunReason,
                             TInt aTestToRun,
                             RPointerArray<TTestCaseInfo>& aTestCases,
                             TTestResult& aResult )
    {
    if(aRunReason == CHgTestBase::ERunTestCase)
        {
        if(aTestToRun < 0)
            {
            iLog->Log(_L("Running setup or teardown"));
            }
        else
            {
            iLog->Log(_L("Running test case #%d"), aTestToRun);
            }
        }
    else
        {
        iLog->Log(_L("Enumerating test cases."));
        }

    TInt result = -1;

    // this method must be implemented in the test case
    result = RunTestL( aRunReason, aTestToRun, aTestCases, aResult );

    if(aRunReason != CHgTestBase::ERunTestCase)
        {
        iNumberOfTestCases = result;
        iLog->Log(_L("Enumeration completed."));
        }

    // Test case was executed
    return KErrNone;
    }    
    
//  End of File
