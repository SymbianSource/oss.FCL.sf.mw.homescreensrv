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


#ifndef __MT_AiPSPropertyObserver_H__
#define __MT_AiPSPropertyObserver_H__

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
NONSHARABLE_CLASS( MT_AiPSPropertyObserver )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static MT_AiPSPropertyObserver* NewL();
        static MT_AiPSPropertyObserver* NewLC();
        /**
         * Destructor
         */
        ~MT_AiPSPropertyObserver();

    private:    // Constructors and destructors

        MT_AiPSPropertyObserver();
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

#endif      //  __MT_AiPSPropertyObserver_H__

// End of file
