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

#ifndef __MT_OBJECTMAP_H__
#define __MT_OBJECTMAP_H__

//  External includes
#include <digia/eunit/ceunittestsuiteclass.h>
#include <e32def.h>

//  Internal includes

//  Forward declarations
namespace HSPluginSettingsIf
    {
    class CObjectMap;
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
NONSHARABLE_CLASS( MT_ObjectMap ) : public CEUnitTestSuiteClass
    {
public:
    // Constructors and destructors

    /**
     * Two phase construction
     */
    static MT_ObjectMap* NewL();

    /**
     * Two phase construction
     */
    static MT_ObjectMap* NewLC();
    
    /**
     * Destructor
     */
    ~MT_ObjectMap();

private:

    /**
     * Private constructor
     */
    MT_ObjectMap();

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
     * Test Set and Get object Media Type
     */
    void TestMediaTypeL();
    
    /**
     * Test Set and Get object Name
     */
    void TestNameL();
    
    /**
     * Test Set and Get object Path
     */
    void TestPathL();
    
private:
    /** Test set  */
    EUNIT_DECLARE_TEST_TABLE;

    HSPluginSettingsIf::CObjectMap* iObjectMap;
    };

#endif      //  __MT_OBJECTMAP_H__
