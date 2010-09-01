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
* Description: EUnit test for homescreen settings API, main suite
*
*/

#ifndef __MT_HsPluginSettings_H__
#define __MT_HsPluginSettings_H__

//  External includes
#include <digia/eunit/ceunittestsuiteclass.h>
#include <e32def.h>

//  Internal includes
#include "chomescreensettingsobserver.h"

//  Forward declarations
class CItemMap;

#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  Class definition

/**
 *
 * Homescreen settings API test class. 
 *
 */
NONSHARABLE_CLASS( MT_HsPluginSettings ) : public CEUnitTestSuiteClass
    {
public:
    // Constructors and destructors

    /**
     * Two phase construction
     */
    static MT_HsPluginSettings* NewL();

    /**
     * Two phase construction
     */
    static MT_HsPluginSettings* NewLC();
        
    /**
     * Destructor
     */
     ~MT_HsPluginSettings();

private:    

    /**
     * Private constructor
     */
    MT_HsPluginSettings();

    /**
     * Leaving constructor
     */
    void ConstructL();

public:

private:

    /**
     * Setup the test
     */
    void SetupL();
        
    /**
     * Clean after the test  
     */
    void Teardown();
        
    /**
     * Test the get of settings  
     */
    void TestGetSettingsL();

    /**
     * Test the set of settings  
     */
    void TestSetSettingsL();

private:
    /** Test set */
    EUNIT_DECLARE_TEST_TABLE;

    HSPluginSettingsIf::CHomescreenSettings* iHomescreenSettings; // Not owned.
    CHomescreenSettingsObserver* iObserver;
    };

#endif      //  __MT_HsPluginSettings_H__
