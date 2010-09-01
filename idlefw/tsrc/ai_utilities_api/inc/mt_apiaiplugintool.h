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

#ifndef __MT_ApiAiplugintool_H__
#define __MT_ApiAiplugintool_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES


//  FORWARD DECLARATIONS
class MAiPluginTool;
class CHsContentPublisher;

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
NONSHARABLE_CLASS( MT_ApiAiplugintool )
     : public CEUnitTestSuiteClass
    {
    public:     // Constructors and destructors

        /**
         * Two phase construction
         */
        static MT_ApiAiplugintool* NewL();
        static MT_ApiAiplugintool* NewLC();
        /**
         * Destructor
         */
        ~MT_ApiAiplugintool();

    private:    // Constructors and destructors

        MT_ApiAiplugintool();
        void ConstructL();

    public:     // From observer interface

        

    private:    // New methods

         void SetupL();
        
         void Teardown();
        
         void TestContentItemIteratorLL();

    private:    // Data

        EUNIT_DECLARE_TEST_TABLE; 
        MAiPluginTool* iAiPluginTool;
        CHsContentPublisher* iPublisher;

    };

#endif      //  __MT_ApiAiplugintool_H__

// End of file
