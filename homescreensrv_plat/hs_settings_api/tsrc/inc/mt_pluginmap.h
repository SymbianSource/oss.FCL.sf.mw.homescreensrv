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
* Description: EUnit test for homescreen settings API, plugin map tests
*
*/

#ifndef __MT_PLUGINMAP_H__
#define __MT_PLUGINMAP_H__

//  External includes
#include <digia/eunit/ceunittestsuiteclass.h>
#include <e32def.h>

//  Internal includes
#include "pluginmap.h"

#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  Class definition
/**
 *
 * Homescreen settings API test class for plugin map tests. 
 *
 */
NONSHARABLE_CLASS( MT_PluginMap ) : public CEUnitTestSuiteClass
    {
public:     
    // Constructors and destructors

    /**
     * Two phase construction
     */
    static MT_PluginMap* NewL();

    /**
     * Two phase construction
     */
    static MT_PluginMap* NewLC();
    
    /**
     * Destructor
     */
    ~MT_PluginMap();

private:    

    /**
     * Private constructor
     */
    MT_PluginMap();

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
     * Test Set/Get plugin Id
     */
    void PluginIdTestL();
    
    /**
     * Test Set/Get plugin Uid
     */
    void PluginUidTestL();
    
private:
    /** Test set  */
    EUNIT_DECLARE_TEST_TABLE;

    HSPluginSettingsIf::CPluginMap* iPluginMap;
    };

#endif      //  __MT_PLUGINMAP_H__
