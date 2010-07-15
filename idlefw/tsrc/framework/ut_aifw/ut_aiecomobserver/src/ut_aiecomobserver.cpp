/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: EUnit unit test class for CAiStateProvider
*
*/

// System includes
#include <e32base.h>
#include <e32cmn.h> 
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/teunitassertioninfo.h>

// User includes
#include "ut_aiecomobserver.h"

#include "aiecomobserver.h"

// Constants


// ======== LOCAL FUNCTIONS ========
    
// ======== MEMBER FUNCTIONS =======
// ----------------------------------------------------------------------------
// UT_AiEcomObserver::NewL()
//
// ----------------------------------------------------------------------------
//
UT_AiEcomObserver* UT_AiEcomObserver::NewL()
    {
    UT_AiEcomObserver* self = UT_AiEcomObserver::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// UT_AiEcomObserver::NewLC()
//
// ----------------------------------------------------------------------------
//
UT_AiEcomObserver* UT_AiEcomObserver::NewLC()
    {
    UT_AiEcomObserver* self = new ( ELeave ) UT_AiEcomObserver();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// UT_AiEcomObserver::~UT_AiEcomObserver()
//
// ----------------------------------------------------------------------------
//
UT_AiEcomObserver::~UT_AiEcomObserver()
    {    
    }

// ----------------------------------------------------------------------------
// UT_AiEcomObserver::UT_AiEcomObserver()
//
// ----------------------------------------------------------------------------
//
UT_AiEcomObserver::UT_AiEcomObserver()
    {
    }

// ----------------------------------------------------------------------------
// UT_AiEcomObserver::ConstructL()
//
// ----------------------------------------------------------------------------
//
void UT_AiEcomObserver::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// ----------------------------------------------------------------------------
// UT_AiEcomObserver::SetupL()
//
// ----------------------------------------------------------------------------
//
void UT_AiEcomObserver::SetupL()
    {
    }

// ----------------------------------------------------------------------------
// UT_AiEcomObserver::Teardown()
//
// ----------------------------------------------------------------------------
//
void UT_AiEcomObserver::Teardown()
    {
    }

// ----------------------------------------------------------------------------
// UT_AiEcomObserver::NotifyEcomRegistryChanged()
//
// ----------------------------------------------------------------------------
//
void UT_AiEcomObserver::NotifyEcomRegistryChanged()
    {
    iNotificationReceived = ETrue;
    
    iWait->AsyncStop();
    }

// ----------------------------------------------------------------------------
// UT_AiEcomObserver::TestStartObserveL()
//
// ----------------------------------------------------------------------------
//
void UT_AiEcomObserver::TestStartObserveL()
    {
    CAiEcomObserver* observer = CAiEcomObserver::NewLC();
    
    EUNIT_ASSERT_EQUALS( observer->iEComSession.Handle() != 0, ETrue );
    EUNIT_ASSERT_EQUALS( observer->IsAdded(), ETrue );
    EUNIT_ASSERT_EQUALS( observer->IsActive(), ETrue );
    EUNIT_ASSERT_EQUALS( observer->iObservers.Count(), 0 );
    
    CleanupStack::PopAndDestroy( observer );
    }

// ----------------------------------------------------------------------------
// UT_AiEcomObserver::TestAddObserverL()
//
// ----------------------------------------------------------------------------
//
void UT_AiEcomObserver::TestAddObserverL()
    {
    CAiEcomObserver* observer = CAiEcomObserver::NewLC();

    observer->AddObserverL( NULL );
    
    EUNIT_ASSERT_EQUALS( observer->iObservers.Count(), 0 );
    
    observer->AddObserverL( this );       
    
    EUNIT_ASSERT_EQUALS( observer->iObservers.Count(), 1 );
    EUNIT_ASSERT_EQUALS( observer->iObservers[0] == this, ETrue );
    
    CleanupStack::PopAndDestroy( observer );    
    }

// ----------------------------------------------------------------------------
// UT_AiEcomObserver::TestNotifyObserverL()
//
// ----------------------------------------------------------------------------
//
void UT_AiEcomObserver::TestNotifyObserverL()
    {
    iNotificationReceived = EFalse;

    iWait = new ( ELeave ) CActiveSchedulerWait;
           
    CAiEcomObserver* observer = CAiEcomObserver::NewLC();

    observer->AddObserverL( this );       

    EUNIT_ASSERT_EQUALS( observer->IsAdded(), ETrue );
    EUNIT_ASSERT_EQUALS( observer->IsActive(), ETrue );
    
    TRequestStatus* stat( &observer->iStatus );
    
    User::RequestComplete( stat, KErrNone );
                   
    iWait->Start();

    delete iWait;
    iWait = NULL;
        
    EUNIT_ASSERT_EQUALS( iNotificationReceived, ETrue );

    // Ensure observer is active again
    EUNIT_ASSERT_EQUALS( observer->IsAdded(), ETrue );
    EUNIT_ASSERT_EQUALS( observer->IsActive(), ETrue );        
        
    CleanupStack::PopAndDestroy( observer );        
    }

//------------------------------------------------------------------------------
// Test case table
//
//------------------------------------------------------------------------------
//
EUNIT_BEGIN_TEST_TABLE(
    UT_AiEcomObserver,
    "Unit test suite for CAiEcomObserver",
    "UNIT" )

    EUNIT_TEST(
        "Start observe",
        "CAiEcomObserver",
        "",
        "FUNCTIONALITY",
        SetupL, TestStartObserveL, Teardown )
        
    EUNIT_TEST(
        "Add observer",
        "CAiEcomObserver",
        "",
        "FUNCTIONALITY",
        SetupL, TestAddObserverL, Teardown )

    EUNIT_TEST(
        "notify Observer",
        "CAiEcomObserver",
        "",
        "FUNCTIONALITY",
        SetupL, TestNotifyObserverL, Teardown )

EUNIT_END_TEST_TABLE

// End of file
