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


#ifndef __UT_SAPIDATAPLUGIN_H__
#define __UT_SAPIDATAPLUGIN_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <msvapi.h>

//  INTERNAL INCLUDES

//  FORWARD DECLARATIONS
class CSapiData;
class CSapiDataPlugin;
class Observer;

#include <e32def.h>
#ifndef NONSHARABLE_CLASS
#define NONSHARABLE_CLASS(x) class x
#endif

_LIT( KPublisher,  "publisher" );
_LIT( KMtSapiDataPlugin, "mt_sapi_plugin" );
_LIT8( KId, "4" );

//  CLASS DEFINITION
class MTest
	{
	public:
		virtual void GetMenuItemsL();
	};
/**
 *
 * Unit test for the mcspluginhandler
 *
 */
NONSHARABLE_CLASS( UT_SapiData ) : 
    public CEUnitTestSuiteClass
    {
public:     // Constructors and destructors

    /**
     * Two phase construction
     */
    static UT_SapiData* NewL();
    static UT_SapiData* NewLC();
    /**
     * Destructor
     */
    ~UT_SapiData();

private:    // Constructors and destructors

    UT_SapiData();
    void ConstructL();

private:    // New methods

    void SetupL();

    void Teardown();

    void TestConfigureL();
    void TestSetContentIdL();
    void TestSetStartupReasonL();
    void TestCreateFilterLC();
    void TestRefreshL();
    void TestIsPluginActive();
    void TestChangePublisherStatusL();
    void TestTriggerActiveL();
    void TestUpdatePublisherStatusL();    
    void TestGetMenuItemsL();
    void TestCanUpdate();
    void TestHasMenuItem();
    void TestRemoveL();
    
private:    // Data

    CSapiData* iData;
    
    CSapiDataPlugin* iPlugin;
    Observer* iContentObserver;
    
    EUNIT_DECLARE_TEST_TABLE;
    };

#endif      //  __UT_SAPIDATAPLUGIN_H__

// End of file
