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

#ifndef __MT_Map_H__
#define __MT_Map_H__

//  External includes
#include <digia/eunit/ceunittestsuiteclass.h>
#include <e32def.h>

//  Internal includes
#include <hspluginsettings.h>

//  Forward declarations
class HSPluginSettingsIf::CItemMap;
class HSPluginSettingsIf::CPropertyMap;

#ifndef NONSHARABLE_CLASS
    #define NONSHARABLE_CLASS(x) class x
#endif

//  Class definition
/**
 *
 * Homescreen settings API test class. 
 *
 */
NONSHARABLE_CLASS( MT_Map ) : public CEUnitTestSuiteClass
    {
public:     
    // Constructors and destructors

    /**
     * Two phase construction
     */
    static MT_Map* NewL();

    /**
     * Two phase construction
     */
    static MT_Map* NewLC();
    
    /**
     * Destructor
     */
    ~MT_Map();

private:    

    /**
     * Private constructor
     */
    MT_Map();
    
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
    void TeardownPropertyMap();
    
    /**
     * Test set and get the item ID
     */
    void TestItemIdL();

    /**
     * Test set and get the item name
     */
    void TestItemNameL();

    /**
     * Test set and get the property name
     */
    void TestPropertyNameL();

    /**
     * Test set and get the property value
     */
    void TestPropertyValueL();
         
    /**
     * Test set and get the property map
     */
    void TestPropertyMapL();

private:
    /** Test set  */
    EUNIT_DECLARE_TEST_TABLE;

    HSPluginSettingsIf::CItemMap* iItemMap;
    HSPluginSettingsIf::CPropertyMap* iPropertyMap;
    RPointerArray< HSPluginSettingsIf::CPropertyMap > iProperties;
    };

#endif      //  __MT_Map_H__
