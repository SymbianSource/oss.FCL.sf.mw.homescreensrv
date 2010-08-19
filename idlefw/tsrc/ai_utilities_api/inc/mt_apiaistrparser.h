/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

#ifndef MT_ApiAiStrParser_H
#define MT_ApiAiStrParser_H

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES


#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

class MAiStrParser;

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
NONSHARABLE_CLASS( MT_ApiAiStrParser )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static MT_ApiAiStrParser* NewL();
        static MT_ApiAiStrParser* NewLC();
        /**
         * Destructor
         */
        ~MT_ApiAiStrParser();

    private:    // Constructors and destructors

        MT_ApiAiStrParser();
        void ConstructL();

    public:     // From observer interface

        

    private:    // New methods

         void SetupL();
         
         void SetupUtf8BufferL();
         
         void SetupUnicodeBufferL();
         
         void SetupHexIntL();
         
         void SetupIntL();
         
         void SetupNoIntL();
         
         void SetupHexIntUnicodeL();
         
         void SetupIntUnicodeL();
         
         void SetupNoIntUnicodeL();
         
         void Teardown();
        
         void TestCopyUnicodeToUnicodeL();
         
         void TestCopyUnicodeToUtf8L();
            
         void TestCopyUtf8ToUnicodeL();
                 
         void TestCopyUtf8ToUtf8L();
         
         void TestParseInt();
         
         void TestParseIntUnicode();
         
         void TestEnsureBufMaxLengthL();

         
    private:    // Data

        EUNIT_DECLARE_TEST_TABLE;
        
        HBufC* iUnicodeBuffer;
        
        HBufC8* iUtf8Buffer;
        
        TInt iTestResult;
        
        TInt32 iTestValue;
        
        MAiStrParser* iStrParser;
    };

#endif      //  __UT_ActiveTransactionStack_H__

// End of file
