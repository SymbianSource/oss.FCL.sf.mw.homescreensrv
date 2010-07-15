/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __MT_AiContentItemArrayIterator_H__
#define __MT_AiContentItemArrayIterator_H__

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
NONSHARABLE_CLASS( MT_AiContentItemArrayIterator )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static MT_AiContentItemArrayIterator* NewL();
        static MT_AiContentItemArrayIterator* NewLC();
        /**
         * Destructor
         */
        ~MT_AiContentItemArrayIterator();

    private:    // Constructors and destructors

        MT_AiContentItemArrayIterator();
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

#endif      //  __MT_AiContentItemArrayIterator_H__

// End of file
