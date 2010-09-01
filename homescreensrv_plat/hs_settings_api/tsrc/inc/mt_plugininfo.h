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
* Description: EUnit test for homescreen settings API, object map tests
*
*/

#ifndef __MT_PLUGININFO_H__
#define __MT_PLUGININFO_H__

//  External includes
#include <digia/eunit/ceunittestsuiteclass.h>
#include <e32def.h>

//  Internal includes

//  Forward declarations
namespace HSPluginSettingsIf
    {
    class CPluginInfo;
    }

#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  Class definition
/**
 *
 * Homescreen settings API test class for object map tests. 
 *
 */
NONSHARABLE_CLASS( MT_PluginInfo ) : public CEUnitTestSuiteClass
    {
public:
    // Constructors and destructors

    /**
     * Two phase construction
     */
    static MT_PluginInfo* NewL();

    /**
     * Two phase construction
     */
    static MT_PluginInfo* NewLC();
    
    /**
     * Destructor
     */
    ~MT_PluginInfo();

private:

    /**
     * Private constructor
     */
    MT_PluginInfo();

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
     * Test set and get ConfigurationState
     */
    void TestConfigurationStateL();

    /**
     * Test set and get Name
     */
    void TestNameL();

    /**
     * Test set and get Interface
     */
    void TestInterfaceL();

    /**
     * Test set and get Type
     */
    void TestTypeL();

    /**
     * Test set and get Uid
     */
    void TestUidL();
  
private:
    /** Test set  */
    EUNIT_DECLARE_TEST_TABLE;

    HSPluginSettingsIf::CPluginInfo* iPluginInfo;
    };

#endif      //  __MT_PLUGININFO_H__
