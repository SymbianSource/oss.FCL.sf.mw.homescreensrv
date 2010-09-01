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
* Description: EUnit test for homescreen settings API, hsps configuration tests
*
*/

#ifndef __MT_HSPSCONFIGURATION_H__
#define __MT_HSPSCONFIGURATION_H__

//  External includes
#include <digia/eunit/ceunittestsuiteclass.h>
#include <e32def.h>

//  Internal includes
#include "hspsconfiguration.h"

#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  Class definition
/**
 *
 * Homescreen settings API test class for hspsconfiguration tests. 
 *
 */
NONSHARABLE_CLASS( MT_HspsConfiguration ) : public CEUnitTestSuiteClass
    {
public:     
    // Constructors and destructors

    /**
     * Two phase construction
     */
    static MT_HspsConfiguration* NewL();

    /**
     * Two phase construction
     */
    static MT_HspsConfiguration* NewLC();

    /**
     * Destructor
     */
    ~MT_HspsConfiguration();

private:    

    /**
     * Private constructor
     */
    MT_HspsConfiguration();

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
     * Test confId
     */
    void ConfigIdTestL();

    /**
     * Test PluginInfo handling functions
     */
    void PluginInfoTestL();

    /**
     * Test Plugin maps
     */
    void PluginsTestL();

    /**
     * Test Items
     */
    void ItemsTestL();

    /**
     * Test Objects
     */
    void ObjectsTestL();

private:
    /** Test set  */
    EUNIT_DECLARE_TEST_TABLE;

    HSPluginSettingsIf::CHspsConfiguration* iConfiguration;
    };

#endif      //  __MT_HSPSCONFIGURATION_H__
