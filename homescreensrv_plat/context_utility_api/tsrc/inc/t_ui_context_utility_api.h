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
* Description:  T_ui_context_utility_api test module.
*
*/



#ifndef T_ui_context_utility_api_H
#define T_ui_context_utility_api_H

// INCLUDES
#include "hgtestbase.h"
#include "hgctxcontactmatcher.h"

// Logging path
_LIT( KT_ui_context_utility_apiLogPath, "\\logs\\testframework\\T_ui_context_utility_api\\" );
// Log file
_LIT( KT_ui_context_utility_apiLogFile, "T_ui_context_utility_api.txt" );

// CLASS DECLARATION

/**
*  This a T_ui_context_utility_api class.
*/
NONSHARABLE_CLASS(CT_ui_context_utility_api) : public CHgTestBase
    {
    public:

        CT_ui_context_utility_api();

        void ConstructL();

        static CT_ui_context_utility_api* NewL();
        
        void CreateEnvL();
        void DestroyEnv();
        TInt LoadTestCasesL(
                    TInt& _test_case_no,
                    CT_ui_context_utility_api::TCallReason aRunReason,
                    TInt aTestToRun,
                    RPointerArray<TTestCaseInfo>& aTestCases,
                    TTestResult& aResult);
        
        ~CT_ui_context_utility_api();
        
        TInt RunTestL(
            CT_ui_context_utility_api::TCallReason aRunReason,
            TInt aTestToRun,
            RPointerArray<TTestCaseInfo>& aTestCases,
            TTestResult& aResult);
        
        RPtrHashMap<TDesC, TDesC>* GetImplHashTablePtr()
            {
            class CTestUtility : public CBase
                {
            public:
                CHgContextUtilityImpl* iImpl;
                };
            
            class CTestImplementation : CTimer
                {
            public:
                // number of pointers before iMusicContextInfo, calculate M-classes as pointers too
                TInt unneeded[12]; 
                RPtrHashMap<TDesC, TDesC> iMusicContextInfo;
                };
            
            // Fetch the pointer to hash table for testing purposes
            return &((CTestImplementation*)((CTestUtility*)iContextUtility)->iImpl)->iMusicContextInfo;
            }
		#define TEST_VAR_DECLARATIONS
        /**
         * all testmodule-global variables declarations are inserted here
         */
        #include "../src/T_ui_context_utility_api_cases.cpp"
		#undef TEST_VAR_DECLARATIONS
        
        // for creating test env
        CTrapCleanup* ENV_cleanup;
        TInt ENV_err;
        CEikonEnv* ENV_env;
        CAknAppUi* ENV_aknAppUI;
    };

#endif      // T_ui_context_utility_api_H

// End of File
