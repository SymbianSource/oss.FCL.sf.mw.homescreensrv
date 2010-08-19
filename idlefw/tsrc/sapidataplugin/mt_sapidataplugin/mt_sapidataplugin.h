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

#ifndef __MT_SAPIDATAPLUGIN_H__
#define __MT_SAPIDATAPLUGIN_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  FORWARD DECLARATIONS
class CAiCpsCommandBuffer;
class CSapiDataPlugin;
class Observer;

#ifndef NONSHARABLE_CLASS
#define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 *
 * EUnitWizard generated test class.
 *
 */
NONSHARABLE_CLASS( MT_SapiDataPlugin )
    : public CEUnitTestSuiteClass
{
public:     // Constructors and destructors

    /**
     * Two phase construction
     */
    static MT_SapiDataPlugin* NewL();

    /**
     * Two phase constructor
     */
    static MT_SapiDataPlugin* NewLC();

    /**
     * Destructor
     */
    ~MT_SapiDataPlugin();

private:    // Constructors and destructors

    MT_SapiDataPlugin();

    void ConstructL();

private:    // New methods

    /**
     * Called everytime at first
     * to prepare plugin for test
     */
    void SetupL();

    /**
     * Called everytime to deconstruct the plugin
     */
    void Teardown();

    /**
     * Tests Stop and Resume
     */
    void TestActivity();

    /**
     * Tests if SuspendL pass
     */
    void TestSuspendL();

    /**
     * Tests property information from/to plugin
     */
    void TestPropertyL();

    /**
     * Test publishing 
     */
    void TestPublishL();
    
    /**
     * Test command buffer
     */
    void TestCommandBuffer();

private:    // Data

    EUNIT_DECLARE_TEST_TABLE;

    CSapiDataPlugin*        iPlugin;
    Observer*               iContentObserver;
    CAiCpsCommandBuffer*    iCommandBuffer;
};

#endif      //  __MT_SAPIDATAPLUGIN_H__

// End of file
