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
* Description:  hgtestbase module.
*
*/


#include <e32svr.h>
#include "StifTestModule.h"
#include <StifLogger.h>

#include "hgtestbasemacros.h"

// MACROS
#define TEST_MODULE_VERSION_MAJOR 0
#define TEST_MODULE_VERSION_MINOR 0
#define TEST_MODULE_VERSION_BUILD 0

// CLASS DECLARATION

// FORWARD DECLARATIONS
class CHgTestClass;
class CEikonEnv;
class CAknAppUi;

// DATA TYPES
typedef TInt (CHgTestClass::* TestFunction)(TTestResult&);

/**
*  An internal structure containing a test case name and
*  the pointer to function doing the test
*/
class TCaseInfoInternal
{
public:
    const TText*    iCaseName;
    TestFunction    iMethod;
    TBool           iIsOOMTest;
    TInt            iFirstMemoryAllocation;
    TInt            iLastMemoryAllocation;
};

// CLASS DECLARATION

/**
*  A structure containing a test case name and
*  the pointer to function doing the test
*/
class TCaseInfo
{
public:
    TPtrC           iCaseName;
    TestFunction    iMethod;
    TBool           iIsOOMTest;
    TInt            iFirstMemoryAllocation;
    TInt            iLastMemoryAllocation;

TCaseInfo( const TText* a ) : iCaseName( (TText*) a )
    {
    };

};
    
class CHgTestBase : public CTestModuleBase
{

public:  //Enums
        // Reason for running test method
		enum TCallReason
		    {
		    EEnumerateTestCases,
		    ERunTestCase,
		    };
        
public:  // Constructors and destructor
    /**
    * Destructor.
    */
    virtual ~CHgTestBase();
    
public: // New functions
    static TInt CallStop( TAny* aWait )
        {
        (static_cast<CActiveSchedulerWait*>(aWait))->AsyncStop();
        }
public: // Functions from base classes

    /**
    * From CTestModuleBase InitL is used to initialize the 
    *       test class object. It is called once for every instance of 
    *       TestModule test class object after its creation.
    * @param aIniFile Initialization file for the test module (optional)
    * @param aFirstTime Flag is true when InitL is executed for first 
    *               created instance of test class object
    * @return Symbian OS error code
    */
    TInt InitL( TFileName& aIniFile, TBool aFirstTime );

    /**
    * From CTestModuleBase GetTestCasesL is used to inquiry test cases 
    *   from test class object. 
    * @param aTestCaseFile Test case file (optional)
    * @param aTestCases  Array of TestCases returned to test framework
    * @return Symbian OS error code
    */
    TInt GetTestCasesL( const TFileName& aTestCaseFile, 
                        RPointerArray<TTestCaseInfo>& aTestCases );


    /**
    * From CTestModuleBase; OOMTestQueryL is used to specify is particular
    * test case going to be executed using OOM conditions
    * @param aTestCaseFile Test case file (optional)
    * @param aCaseNumber Test case number (optional)
    * @param aFailureType OOM failure type (optional)
    * @param aFirstMemFailure The first heap memory allocation failure value (optional)
    * @param aLastMemFailure The last heap memory allocation failure value (optional)
    * @return TBool
    */
    virtual TBool OOMTestQueryL( const TFileName& /* aTestCaseFile */, 
                                 const TInt /* aCaseNumber */, 
                                 TOOMFailureType& aFailureType, 
                                 TInt& /* aFirstMemFailure */, 
                                 TInt& /* aLastMemFailure */ );

    /**
    * From CTestModuleBase; OOMTestInitializeL may be used to initialize OOM
    * test environment
    * @param aTestCaseFile Test case file (optional)
    * @param aCaseNumber Test case number (optional)
    * @return None
    */
    virtual void OOMTestInitializeL( const TFileName& /* aTestCaseFile */, 
                                const TInt /* aCaseNumber */ ); 

    /**
    * From CTestModuleBase; OOMHandleWarningL
    * @param aTestCaseFile Test case file (optional)
    * @param aCaseNumber Test case number (optional)
    * @param aFailNextValue FailNextValue for OOM test execution (optional)
    * @return None
    *
    * User may add implementation for OOM test warning handling. Usually no
    * implementation is required.           
    */
    virtual void OOMHandleWarningL( const TFileName& /* aTestCaseFile */,
                                    const TInt /* aCaseNumber */, 
                                    TInt& /* aFailNextValue */); 

    /**
    * From CTestModuleBase; OOMTestFinalizeL may be used to finalize OOM
    * test environment
    * @param aTestCaseFile Test case file (optional)
    * @param aCaseNumber Test case number (optional)
    * @return None
    *
    */
    virtual void OOMTestFinalizeL( const TFileName& /* aTestCaseFile */, 
                                   const TInt /* aCaseNumber */ );

    /**
     * Method used to log version of test module
     */
    void SendTestModuleVersion( const TDesC& aTestPath );

private:
    /**
    * From CTestModuleBase RunTestCaseL is used to run an individual 
    *   test case. 
    * @param aCaseNumber Test case number
    * @param aTestCaseFile Test case file (optional)
    * @param aResult Test case result returned to test framework (PASS/FAIL)
    * @return Symbian OS error code (test case execution error, which is 
    *           not reported in aResult parameter as test case failure).
    */   
    TInt RunTestCaseL( const TInt aCaseNumber, 
                       const TFileName& aTestCaseFile,
                       TTestResult& aResult );

    /**
    * Method containing all test cases, setup and teardown sections.
    */
    TInt MainTestL(CHgTestBase::TCallReason aRunReason, TInt aTestToRun, RPointerArray<TTestCaseInfo>& aTestCases,
            TTestResult& aResult);

protected:

    /**
    * C++ default constructor.
    */
    CHgTestBase();

    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void BaseConstructL( const TDesC& aTestPath,
                     const TDesC& aTestFile );

    /**
     * Put here the #include of the test cases cpp file
     */
    virtual TInt RunTestL(
            CHgTestBase::TCallReason aRunReason, 
            TInt aTestToRun, 
            RPointerArray<TTestCaseInfo>& aTestCases,
            TTestResult& aResult) = 0;

protected:    // Data
    // Pointer to test (function) to be executed
    TestFunction iMethod;

    // Pointer to logger
    CStifLogger * iLog; 

    // Flag saying if version of test module was already sent
    TBool iVersionLogged;
    // Total number of test cases
    TInt iNumberOfTestCases;
      HBufC16* iTestDllName;
    // activescheduler for connecting
    CActiveScheduler* iActiveScheduler;
};
