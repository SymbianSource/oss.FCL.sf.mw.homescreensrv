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
* Description:
*
*/


// System includes
#include <e32base.h>
#include <e32cmn.h> 
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/teunitassertioninfo.h>
#include <digia/eunit/eunitdecorators.h>

// User includes
#include "modifierkeytracker.h"

#include "ut_modifierkeytracker.h"


// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// UT_MofifierKeyTracker::NewL()
//
// -----------------------------------------------------------------------------
//
UT_MofifierKeyTracker* UT_MofifierKeyTracker::NewL()
    {
    UT_MofifierKeyTracker* self = UT_MofifierKeyTracker::NewLC();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// UT_MofifierKeyTracker::NewLC()
//
// -----------------------------------------------------------------------------
//
UT_MofifierKeyTracker* UT_MofifierKeyTracker::NewLC()
    {
    UT_MofifierKeyTracker* self = new( ELeave ) UT_MofifierKeyTracker();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// -----------------------------------------------------------------------------
// UT_MofifierKeyTracker::~UT_MofifierKeyTracker()
//
// -----------------------------------------------------------------------------
//
UT_MofifierKeyTracker::~UT_MofifierKeyTracker()
    {
    delete iTModifierKeyTracker;
    }

// -----------------------------------------------------------------------------
// UT_MofifierKeyTracker::UT_MofifierKeyTracker()
//
// -----------------------------------------------------------------------------
//
UT_MofifierKeyTracker::UT_MofifierKeyTracker()
    {
    }

// -----------------------------------------------------------------------------
// UT_MofifierKeyTracker::ConstructL()
//
// -----------------------------------------------------------------------------
//
void UT_MofifierKeyTracker::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// -----------------------------------------------------------------------------
// UT_MofifierKeyTracker::SetupL()
//
// -----------------------------------------------------------------------------
//
void UT_MofifierKeyTracker::SetupL()
    {
    iTModifierKeyTracker = new( ELeave ) AiWsPlugin::TModifierKeyTracker;
    }

// -----------------------------------------------------------------------------
// UT_MofifierKeyTracker::Teardown()
//
// -----------------------------------------------------------------------------
//
void UT_MofifierKeyTracker::Teardown()
    {
    delete iTModifierKeyTracker; 
    iTModifierKeyTracker = NULL; 
    }

// -----------------------------------------------------------------------------
// UT_MofifierKeyTracker::T_TModifierKeyTracker_ConstructL()
//
// -----------------------------------------------------------------------------
//
void UT_MofifierKeyTracker::T_TModifierKeyTracker_ConstructL()
    {
    EUNIT_ASSERT_EQUALS( iTModifierKeyTracker->Status( ), 0 );
    }    

// -----------------------------------------------------------------------------
// UT_MofifierKeyTracker::T_TModifierKeyTracker_NonModiferUpdateL()
//
// -----------------------------------------------------------------------------
//
void UT_MofifierKeyTracker::T_TModifierKeyTracker_NonModiferUpdateL()
    {
    TRawEvent testEvent;
    testEvent.Set( TRawEvent::EKeyDown, EStdKeyDevice0 );
    EUNIT_ASSERT( !iTModifierKeyTracker->Update( testEvent ) );
    EUNIT_ASSERT_EQUALS( iTModifierKeyTracker->Status( ), 0 );
    testEvent.Set( TRawEvent::EKeyUp, EStdKeyDevice0 );
    EUNIT_ASSERT( !iTModifierKeyTracker->Update( testEvent ) );
    }    

// -----------------------------------------------------------------------------
// UT_MofifierKeyTracker::T_TModifierKeyTracker_TestModifierKeyDownL()
//
// -----------------------------------------------------------------------------
//
void UT_MofifierKeyTracker::T_TModifierKeyTracker_TestModifierKeyDownL(
    TInt aScanCode, TUint aExcpectedStatus )
    {
    TRawEvent testEvent;
    testEvent.Set( TRawEvent::EKeyDown, aScanCode );
    EUNIT_ASSERT( iTModifierKeyTracker->Update( testEvent ) );
    EUNIT_ASSERT_EQUALS( iTModifierKeyTracker->Status( ), aExcpectedStatus );
    }

// -----------------------------------------------------------------------------
// UT_MofifierKeyTracker::T_TModifierKeyTracker_TestModifierKeyUpL()
//
// -----------------------------------------------------------------------------
//
void UT_MofifierKeyTracker::T_TModifierKeyTracker_TestModifierKeyUpL(
    TInt aScanCode, TUint aExcpectedStatus )
    {
    TRawEvent testEvent;
    testEvent.Set( TRawEvent::EKeyUp, aScanCode );
    EUNIT_ASSERT( iTModifierKeyTracker->Update( testEvent ) );
    EUNIT_ASSERT_EQUALS( iTModifierKeyTracker->Status( ), aExcpectedStatus );
    }

// -----------------------------------------------------------------------------
// UT_MofifierKeyTracker::T_TModifierKeyTracker_SingleModiferUpdateL()
//
// -----------------------------------------------------------------------------
//
void UT_MofifierKeyTracker::T_TModifierKeyTracker_SingleModiferUpdateL(
    TInt aScanCode, TUint aExcpectedStatus  )
    {
    T_TModifierKeyTracker_TestModifierKeyDownL( aScanCode, aExcpectedStatus );
    T_TModifierKeyTracker_TestModifierKeyUpL( aScanCode, 0 );
    }

// -----------------------------------------------------------------------------
// UT_MofifierKeyTracker::T_TModifierKeyTracker_SingleModiferUpdateL()
//
// -----------------------------------------------------------------------------
//
void UT_MofifierKeyTracker::T_TModifierKeyTracker_SingleModiferUpdateL()
    {
    T_TModifierKeyTracker_SingleModiferUpdateL( 
            EStdKeyLeftShift, EModifierLeftShift | EModifierShift );
    
    T_TModifierKeyTracker_SingleModiferUpdateL( 
            EStdKeyRightShift, EModifierRightShift | EModifierShift );
    
    T_TModifierKeyTracker_SingleModiferUpdateL( 
            EStdKeyLeftCtrl, EModifierLeftCtrl | EModifierCtrl );
    
    T_TModifierKeyTracker_SingleModiferUpdateL( 
            EStdKeyRightCtrl, EModifierRightCtrl | EModifierCtrl );
    
    T_TModifierKeyTracker_SingleModiferUpdateL( 
            EStdKeyLeftAlt, EModifierLeftAlt | EModifierAlt );
    
    T_TModifierKeyTracker_SingleModiferUpdateL( 
            EStdKeyRightAlt, EModifierRightAlt | EModifierAlt );
    
    T_TModifierKeyTracker_SingleModiferUpdateL( 
            EStdKeyLeftFunc, EModifierLeftFunc | EModifierFunc );
    
    T_TModifierKeyTracker_SingleModiferUpdateL( 
            EStdKeyRightFunc, EModifierRightFunc | EModifierFunc );
    }    

// -----------------------------------------------------------------------------
// UT_MofifierKeyTracker::T_TModifierKeyTracker_MultiModiferUpdateL()
//
// -----------------------------------------------------------------------------
//
void UT_MofifierKeyTracker::T_TModifierKeyTracker_MultiModiferUpdateL()
    {
    TUint expectedStatus = EModifierLeftShift | EModifierShift ;
    T_TModifierKeyTracker_TestModifierKeyDownL( EStdKeyLeftShift, expectedStatus );
    expectedStatus |= EModifierRightShift;
    T_TModifierKeyTracker_TestModifierKeyDownL( EStdKeyRightShift, expectedStatus );

    expectedStatus |= EModifierLeftCtrl | EModifierCtrl;
    T_TModifierKeyTracker_TestModifierKeyDownL( EStdKeyLeftCtrl, expectedStatus );
    expectedStatus |= EModifierRightCtrl;
    T_TModifierKeyTracker_TestModifierKeyDownL( EStdKeyRightCtrl, expectedStatus );

    expectedStatus |= EModifierLeftAlt | EModifierAlt;
    T_TModifierKeyTracker_TestModifierKeyDownL( EStdKeyLeftAlt, expectedStatus );
    expectedStatus |= EModifierRightAlt;
    T_TModifierKeyTracker_TestModifierKeyDownL( EStdKeyRightAlt, expectedStatus );

    expectedStatus |= EModifierLeftFunc | EModifierFunc;
    T_TModifierKeyTracker_TestModifierKeyDownL( EStdKeyLeftFunc, expectedStatus );
    expectedStatus |= EModifierRightFunc;
    T_TModifierKeyTracker_TestModifierKeyDownL( EStdKeyRightFunc, expectedStatus );

    expectedStatus &= ~EModifierLeftShift;
    T_TModifierKeyTracker_TestModifierKeyUpL( EStdKeyLeftShift, expectedStatus );
    expectedStatus &= ~(EModifierRightShift | EModifierShift);
    T_TModifierKeyTracker_TestModifierKeyUpL( EStdKeyRightShift, expectedStatus );

    expectedStatus &= ~EModifierLeftCtrl;
    T_TModifierKeyTracker_TestModifierKeyUpL( EStdKeyLeftCtrl, expectedStatus );
    expectedStatus &= ~(EModifierRightCtrl | EModifierCtrl);
    T_TModifierKeyTracker_TestModifierKeyUpL( EStdKeyRightCtrl, expectedStatus );

    expectedStatus &= ~EModifierLeftAlt;
    T_TModifierKeyTracker_TestModifierKeyUpL( EStdKeyLeftAlt, expectedStatus );
    expectedStatus &= ~(EModifierRightAlt | EModifierAlt);
    T_TModifierKeyTracker_TestModifierKeyUpL( EStdKeyRightAlt, expectedStatus );

    expectedStatus &= ~EModifierLeftFunc;
    T_TModifierKeyTracker_TestModifierKeyUpL( EStdKeyLeftFunc, expectedStatus );
    expectedStatus &= ~(EModifierRightFunc | EModifierFunc);
    T_TModifierKeyTracker_TestModifierKeyUpL( EStdKeyRightFunc, expectedStatus );
    
    EUNIT_ASSERT_EQUALS( iTModifierKeyTracker->Status( ), 0 );
    }    

// -----------------------------------------------------------------------------
// UT_MofifierKeyTracker::T_TModifierKeyTracker_MixedModiferNonModiferUpdateL()
//
// -----------------------------------------------------------------------------
//
void UT_MofifierKeyTracker::T_TModifierKeyTracker_MixedModiferNonModiferUpdateL()
    {
    TUint expectedStatus = EModifierLeftShift | EModifierShift ;
    T_TModifierKeyTracker_TestModifierKeyDownL( EStdKeyLeftShift, expectedStatus );
    
    TRawEvent testEvent;
    testEvent.Set( TRawEvent::EKeyDown, EStdKeyDevice0 );
    EUNIT_ASSERT( !iTModifierKeyTracker->Update( testEvent ) );
    EUNIT_ASSERT_EQUALS( iTModifierKeyTracker->Status( ), expectedStatus );
    testEvent.Set( TRawEvent::EKeyUp, EStdKeyDevice0 );
    EUNIT_ASSERT( !iTModifierKeyTracker->Update( testEvent ) );
    EUNIT_ASSERT_EQUALS( iTModifierKeyTracker->Status( ), expectedStatus );
    
    expectedStatus |= EModifierLeftCtrl | EModifierCtrl;
    T_TModifierKeyTracker_TestModifierKeyDownL( EStdKeyLeftCtrl, expectedStatus );
    
    testEvent.Set( TRawEvent::EKeyDown, EStdKeyDevice1 );
    EUNIT_ASSERT( !iTModifierKeyTracker->Update( testEvent ) );
    EUNIT_ASSERT_EQUALS( iTModifierKeyTracker->Status( ), expectedStatus );
    testEvent.Set( TRawEvent::EKeyUp, EStdKeyDevice1 );
    EUNIT_ASSERT( !iTModifierKeyTracker->Update( testEvent ) );
    EUNIT_ASSERT_EQUALS( iTModifierKeyTracker->Status( ), expectedStatus );
    
    expectedStatus &= ~(EModifierLeftShift | EModifierShift);
    T_TModifierKeyTracker_TestModifierKeyUpL( EStdKeyLeftShift, expectedStatus );

    testEvent.Set( TRawEvent::EKeyDown, EStdKeyDevice2 );
    EUNIT_ASSERT( !iTModifierKeyTracker->Update( testEvent ) );
    EUNIT_ASSERT_EQUALS( iTModifierKeyTracker->Status( ), expectedStatus );
    testEvent.Set( TRawEvent::EKeyUp, EStdKeyDevice2 );
    EUNIT_ASSERT( !iTModifierKeyTracker->Update( testEvent ) );
    EUNIT_ASSERT_EQUALS( iTModifierKeyTracker->Status( ), expectedStatus );
    }    

// -----------------------------------------------------------------------------
// Test case table
//
// -----------------------------------------------------------------------------
//
EUNIT_BEGIN_TEST_TABLE(
    UT_MofifierKeyTracker,
    "Add test suite description here.",
    "UNIT" )

    EUNIT_TEST(
        "Constructor",
        "TModifierKeyTracker",
        "Constructor",
        "FUNCTIONALITY",
        SetupL, T_TModifierKeyTracker_ConstructL, Teardown)
        
    EUNIT_TEST(
        "Update - Single active modifier key",
        "TModifierKeyTracker",
        "Update",
        "FUNCTIONALITY",
        SetupL, T_TModifierKeyTracker_SingleModiferUpdateL, Teardown)
    
    EUNIT_TEST(
        "Update - Multiple active modifier keys",
        "TModifierKeyTracker",
        "Update",
        "FUNCTIONALITY",
        SetupL, T_TModifierKeyTracker_MultiModiferUpdateL, Teardown)
        
    EUNIT_TEST(
        "Update - Mixed modfier and other keys",
        "TModifierKeyTracker",
        "Update",
        "FUNCTIONALITY",
        SetupL, T_TModifierKeyTracker_MixedModiferNonModiferUpdateL, Teardown)
           
EUNIT_END_TEST_TABLE

// End of file
