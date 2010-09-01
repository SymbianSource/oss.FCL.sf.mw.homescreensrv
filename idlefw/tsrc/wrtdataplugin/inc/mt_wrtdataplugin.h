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

#ifndef __MT_WRTDATAPLUGIN_H__
#define __MT_WRTDATAPLUGIN_H__

// INCLUDES
#include <digia/eunit/ceunittestsuiteclass.h>
#include <digia/eunit/eunitdecorators.h>


// FORWARD DECLARATIONS
class CHsContentPublisher;
class CWrtDataPluginObserver;

#ifndef NONSHARABLE_CLASS
#define NONSHARABLE_CLASS(x) class x
#endif


// CLASS DEFINITION
/**
 * Generated EUnit test suite class.
 */
NONSHARABLE_CLASS( MT_wrtdataplugin )
    : public CEUnitTestSuiteClass
    {
public:  // Constructors and destructor

    static MT_wrtdataplugin* NewL();
    static MT_wrtdataplugin* NewLC();
    ~MT_wrtdataplugin();

private: // Constructors

    MT_wrtdataplugin();
    void ConstructL();

private: // New methods

    void SetupL();
       
    void Teardown();
        
    void TestStartL();

    void TestStopL();
    
    void TestResumeL();

    void TestSuspendL();

    void TestSetOnlineL();

    void TestSetOfflineL();

    void TestSubscribeL();

    void TestConfigureL();

    void TestSetPropertyL();

    void TestGetPropertyL();

    void TestHandleEventL();  
    
    void TestHasMenuItemL();
    
    void TestExtensionL();
    
    void TestPublisherInfoL();
    

private: // Data


    CHsContentPublisher*       iPlugin;
    CWrtDataPluginObserver*    iContentObserver;

    EUNIT_DECLARE_TEST_TABLE;

    };

#endif      //  __MT_WRTDATAPLUGIN_H__
