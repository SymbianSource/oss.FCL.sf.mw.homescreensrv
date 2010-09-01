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

#ifndef __MT_ApiContentPriorityMap_H__
#define __MT_ApiContentPriorityMap_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
namespace AiUtility
    {
class CContentPriorityMap;
    }
    
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
NONSHARABLE_CLASS( MT_ApiContentPriorityMap )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static MT_ApiContentPriorityMap* NewL();
        static MT_ApiContentPriorityMap* NewLC();
        /**
         * Destructor
         */
        ~MT_ApiContentPriorityMap();

    private:    // Constructors and destructors

        MT_ApiContentPriorityMap();
        void ConstructL();

    public:     // From observer interface

        

    private:    // New methods

         void SetupL();
        
         void Teardown();

         void TestEmptyMapL();

         void TestClearL();

         void TestUpdateL();
         
         void TestOverrideL();
         
         void TestResetL();
           
         
    private:    // Data

        EUNIT_DECLARE_TEST_TABLE; 
        
        AiUtility::CContentPriorityMap* iContentPriorityMap;
    
    };

#endif      //  __MT_ApiContentPriorityMap_H__

// End of file
