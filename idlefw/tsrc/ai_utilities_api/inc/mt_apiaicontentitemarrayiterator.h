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

#ifndef __MT_ApiAiContentItemArrayIterator_H__
#define __MT_ApiAiContentItemArrayIterator_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS


#include <e32def.h>
#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class. 
 *
 */
NONSHARABLE_CLASS( MT_ApiAiContentItemArrayIterator )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static MT_ApiAiContentItemArrayIterator* NewL();
        static MT_ApiAiContentItemArrayIterator* NewLC();
        /**
         * Destructor
         */
        ~MT_ApiAiContentItemArrayIterator();

    private:    // Constructors and destructors

        MT_ApiAiContentItemArrayIterator();
        void ConstructL();

    public:     // From observer interface

        

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void TestEmptyL();
         void TestOneL();
         void TestManyL();

    private:    // Data

        EUNIT_DECLARE_TEST_TABLE; 

    };

#endif      //  __MT_ApiAiContentItemArrayIterator_H__

// End of file
