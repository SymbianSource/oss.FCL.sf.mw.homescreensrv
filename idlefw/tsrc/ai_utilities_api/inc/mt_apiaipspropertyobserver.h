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

#ifndef __MT_ApiAiPSPropertyObserver_H__
#define __MT_ApiAiPSPropertyObserver_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class MAiPSPropertyObserver;

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
NONSHARABLE_CLASS( MT_ApiAiPSPropertyObserver )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static MT_ApiAiPSPropertyObserver* NewL();
        static MT_ApiAiPSPropertyObserver* NewLC();
        /**
         * Destructor
         */
        ~MT_ApiAiPSPropertyObserver();

    private:    // Constructors and destructors

        MT_ApiAiPSPropertyObserver();
        void ConstructL();

    public:     // From observer interface

        

    private:    // New methods

         void SetupL();
        
         void Teardown();

         void TestBasicFunctionalityL();

         void TestGettersL();

         void TestDoCancelL();
         
         static TInt HandleSystemStateChangedL(TAny* aPtr);
        
    private:    // Data

        EUNIT_DECLARE_TEST_TABLE; 
        
        MAiPSPropertyObserver* iPSObserver;
    
        TBool iStateOk;


    };

#endif      //  __MT_ApiAiPSPropertyObserver_H__

// End of file
