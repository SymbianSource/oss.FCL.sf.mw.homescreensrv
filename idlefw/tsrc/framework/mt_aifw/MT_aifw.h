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
* Description:
*
*/


#ifndef __MT_AIFW_H__
#define __MT_AIFW_H__

//  EXTERNAL INCLUDES
#include <CEUnitTestSuiteClass.h>
#include "AiFwTestControllerPlugin.h"
#include "aiuiframeworkobserver.h"

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS

class CAiFw;
class CAiFwTestContentPlugin;

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
NONSHARABLE_CLASS( MT_aifw )
     : public CEUnitTestSuiteClass,
       public MAiUiFrameworkObserver
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static MT_aifw* NewL();
        static MT_aifw* NewLC();
        /**
         * Destructor
         */
        ~MT_aifw();

    private:    // Constructors and destructors

        MT_aifw();
        void ConstructL();

    public:     // From observer interface

         void HandleResourceChange( TInt /*aType*/ )
            {
            };
    
         void HandleForegroundEvent( TBool /*aForeground*/ )
            {
            };


    private:    // New methods

         void SetupL();

         void Teardown();
        
         void TestStartupL();
         
         void TestSubscriptionL();

         void TestStartupResumeL();

         void TestUiActivationL();

         void TestUiDefinitionLoadL();

         void TestSettingsPassingL();

         void TestEventForwardingL();

    private:    // Data

        EUNIT_DECLARE_TEST_TABLE;
        
        CAiFw* iFramework;
        
        AiTestUiController::CAiFwTestControllerPlugin* iControllerPlugin;
        
        CAiFwTestContentPlugin* iContentPlugin;

    };

#endif      //  __MT_AIFW_H__

// End of file
