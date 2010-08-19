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

#ifndef __UT_WRTDATAPLUGIN_H__
#define __UT_WRTDATAPLUGIN_H__

// INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <digia/eunit/eunitdecorators.h>



// FORWARD DECLARATIONS
class CWrtDataPlugin;
class CWrtDataPluginObserver;
class CWrtDataObserver;
class TAknsItemID;

#ifndef NONSHARABLE_CLASS
#define NONSHARABLE_CLASS(x) class x
#endif


// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( UT_wrtdataplugin )
    : public CEUnitTestSuiteClass
    {
public:  // Constructors and destructor

    static UT_wrtdataplugin* NewL();
    static UT_wrtdataplugin* NewLC();
    ~UT_wrtdataplugin();

private: // Constructors

    UT_wrtdataplugin();
    void ConstructL();

private: // New methods
    
    void SetupL();
       
    void Teardown();

        // CWrtDataPlugin test cases
    void TestGetIdL();
    
    void TestGetTypeL();
    
    void TestRefreshL();
    
    void TestIsActive();
    
    void TestPublishTextL();

    void TestPublishImageHandleL();

    void TestPublishImagePathL();
    
    void TestClean();

    void TestShowLoadingIcon();

    void TestHideLoadingIcon();    
    
    void TestData();

    void TestNetworkStatus();
    
    void TestStartTimer();

        // CWrtData test cases 
    void TestConfigureL();

    void TestRegisterL();
    
    void TestUpdatePublisherStatusL();     

    void TestPublishInitialDataL();

    void TestPublishDefaultImageL();
            
    void TestPublishL();
    
    void TestDataRefreshL();
            
    void TestExecuteActionL();

    void TestIsPluginActive();

    void TestNotifyPublisherL();
    
    void TestSetCommandBuffer();
    
        // CWrtDataObserver test cases
    void TestHandleNotifyL();
    
private: // Data


    CWrtDataPlugin*            iPlugin;
    CWrtDataPluginObserver*    iContentObserver;
    CWrtDataObserver*          iDataObserver;
    EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __MT_WRTDATAPLUGIN_H__
