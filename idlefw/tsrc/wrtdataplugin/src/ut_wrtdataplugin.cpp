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

#include "ut_wrtdataplugin.h"
#include "wrtdatapluginobserver.h"

#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/eunitdecorators.h>
#include <hscontentpublisher.h>
#include <wrtdatapluginuids.hrh>
#include <wrtdataplugin.h>

_LIT( KWRTPublisherName,"WRTWidget" );
_LIT8( KNameSpace, "5" );


// - Construction -----------------------------------------------------------

UT_wrtdataplugin* UT_wrtdataplugin::NewL()
    {
    UT_wrtdataplugin* self = UT_wrtdataplugin::NewLC();
    CleanupStack::Pop();
    return self;
    }

UT_wrtdataplugin* UT_wrtdataplugin::NewLC()
    {
    UT_wrtdataplugin* self = new( ELeave ) UT_wrtdataplugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

UT_wrtdataplugin::~UT_wrtdataplugin()
    {
       if ( iPlugin )
        {
        delete iPlugin;
        }
    if ( iContentObserver )
        {
        delete iContentObserver;
        }

    }

UT_wrtdataplugin::UT_wrtdataplugin()
    {
    }

void UT_wrtdataplugin::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    }

// - Test methods -----------------------------------------------------------


void UT_wrtdataplugin::TestGetIdL()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }
    
void UT_wrtdataplugin::TestGetTypeL()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }
    
void UT_wrtdataplugin::TestRefreshL()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }
    
void UT_wrtdataplugin::TestIsActive()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }
    
void UT_wrtdataplugin::TestPublishTextL()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }

void UT_wrtdataplugin::TestPublishImageHandleL()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }

void UT_wrtdataplugin::TestPublishImagePathL()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }
    
void UT_wrtdataplugin::TestClean()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }

void UT_wrtdataplugin::TestShowLoadingIcon()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }

void UT_wrtdataplugin::TestHideLoadingIcon()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }
    
void UT_wrtdataplugin::TestData()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }

void UT_wrtdataplugin::TestNetworkStatus()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }
    
void UT_wrtdataplugin::TestStartTimer()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }


void UT_wrtdataplugin::TestConfigureL()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }

void UT_wrtdataplugin::TestRegisterL()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }
    
void UT_wrtdataplugin::TestUpdatePublisherStatusL()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }

void UT_wrtdataplugin::TestPublishInitialDataL()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }

void UT_wrtdataplugin::TestPublishDefaultImageL()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }
            
void UT_wrtdataplugin::TestPublishL()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }
    
void UT_wrtdataplugin::TestDataRefreshL()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }
            
void UT_wrtdataplugin::TestExecuteActionL()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }

void UT_wrtdataplugin::TestIsPluginActive()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }

void UT_wrtdataplugin::TestNotifyPublisherL()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }
    
void UT_wrtdataplugin::TestSetCommandBuffer()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }

void UT_wrtdataplugin::TestHandleNotifyL()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }


// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    UT_wrtdataplugin,
    "Add test suite description here.",
    "MODULE" )

/*EUNIT_TEST(
    "StartL",
    "WRTDataPlugin",
    "StartL",
    "FUNCTIONALITY",
    SetupL, TestStartL, Teardown)
*/
EUNIT_END_TEST_TABLE
