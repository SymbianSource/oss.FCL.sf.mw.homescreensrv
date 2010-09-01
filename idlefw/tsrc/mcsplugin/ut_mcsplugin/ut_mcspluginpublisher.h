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


#ifndef __UT_MCSSETTINGS_H__
#define __UT_MCSSETTINGS_H__

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>

//  INTERNAL INCLUDES
#include <e32def.h>
#include <msvapi.h>
#include <itemmap.h>
#include <hspluginsettings.h>

//  FORWARD DECLARATIONS
class CMCSPlugin;

#ifndef NONSHARABLE_CLASS
#define NONSHARABLE_CLASS(x) class x
#endif

//  CLASS DEFINITION
/**
 *
 * Unit test for the mcspluginpublisher
 *
 */
NONSHARABLE_CLASS( UT_MCSPluginPublisher ) : 
    public CEUnitTestSuiteClass,
    public MMsvSessionObserver
    {
public:     // Constructors and destructors

    /**
     * Two phase construction
     */
    static UT_MCSPluginPublisher* NewL();
    static UT_MCSPluginPublisher* NewLC();
    
    /**
     * Destructor
     */
    ~UT_MCSPluginPublisher();
    
    /**
     * From class MMsvSessionObserver,
     * Handles an event from the message server.
     */
    void HandleSessionEventL(
             TMsvSessionEvent /*aEvent*/, 
             TAny* /*aArg1*/, 
             TAny* /*aArg2*/,
             TAny* /*aArg3*/ );
    
    

private:    // Constructors and destructors

    UT_MCSPluginPublisher();
    
    void ConstructL();

private:    // New methods

    void SetupL();

    void Teardown();

    void TestMCSDataL();

    void TestMCSPluginData_RemoveDataL();

    void TestMCSPluginData_GetBkmDataL();

    void TestMCSPluginData_GetFolderDataL();

    void TestMCSPluginData_GetMailboxDataL();

    void TestMCSPluginEngine_CreateBkmItemL();

    void TestMCSPluginEngine_CreateMailboxItemL();

    void TestMCSPluginEngine_ItemIconL();

private:    // Data
    CMCSPlugin* iMCSPlugin;

    RPointerArray<HSPluginSettingsIf::CItemMap> iSettings;

    HSPluginSettingsIf::CHomescreenSettings* iPluginSettings;

    EUNIT_DECLARE_TEST_TABLE;
    };

#endif      //  __UT_McsPluginpublisher_H__

// End of file
