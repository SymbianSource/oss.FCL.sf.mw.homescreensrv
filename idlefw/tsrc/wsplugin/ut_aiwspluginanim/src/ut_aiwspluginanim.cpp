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
#include "aiwspluginanim.h"

#include "animgeneralfunctions_stub.h"
#include "numerickeyhandler_stub.h"
#include "wsanim_stub.h"
#include "testeventhandler_stub.h"

#include "ut_aiwspluginanimtls.h"
#include "ut_aiwspluginanim.h"

// Constants
namespace 
    {
    const TInt KAiWgId = 42;
    const TInt KTargetWgId = KAiWgId + 1;
    const TAiWsPluginAnimInitData KWsPluginInitData = 
        { KAiWgId, KTargetWgId };
    const TBool KNoFocus = EFalse;
    }


// ======== LOCAL FUNCTIONS ========
    
// ----------------------------------------------------------------------------
// ResetEvent()
//
// ----------------------------------------------------------------------------
//
void ResetEvent( TRawEvent& aEvent )
    {
    Mem::FillZ( &aEvent, sizeof( TRawEvent ) );
    }

// ----------------------------------------------------------------------------
// operator==()
//
// ----------------------------------------------------------------------------
//
TBool operator==( const TRawEvent& aLhs, const TRawEvent& aRhs )
    {
    const TUint8* lhs = reinterpret_cast<const TUint8*>( &aLhs );
    const TUint8* rhs = reinterpret_cast<const TUint8*>( &aRhs );
    return ( Mem::Compare( lhs, sizeof(TRawEvent), rhs, sizeof(TRawEvent) ) == 0 );
    }

// ======== MEMBER FUNCTIONS =======
// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::NewL()
//
// ----------------------------------------------------------------------------
//
UT_AiWsPluginAnim* UT_AiWsPluginAnim::NewL()
    {
    UT_AiWsPluginAnim* self = UT_AiWsPluginAnim::NewLC();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::NewLC()
//
// ----------------------------------------------------------------------------
//
UT_AiWsPluginAnim* UT_AiWsPluginAnim::NewLC()
    {
    UT_AiWsPluginAnim* self = new( ELeave ) UT_AiWsPluginAnim();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::~UT_AiWsPluginAnim()
//
// ----------------------------------------------------------------------------
//
UT_AiWsPluginAnim::~UT_AiWsPluginAnim()
    {
    delete iCAiWsPluginAnim;
    delete iAnimGeneralFunctions;
    UT_AiWsPluginAnimTls::Destroy();
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::UT_AiWsPluginAnim()
//
// ----------------------------------------------------------------------------
//
UT_AiWsPluginAnim::UT_AiWsPluginAnim()
    {
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::ConstructL()
//
// ----------------------------------------------------------------------------
//
void UT_AiWsPluginAnim::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::EmptySetup()
//
// ----------------------------------------------------------------------------
//
void UT_AiWsPluginAnim::EmptySetup()
    {
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::SetupTlsL()
//
// ----------------------------------------------------------------------------
//
void UT_AiWsPluginAnim::SetupTlsL()
    {
    if ( !iTls )
        {
        UT_AiWsPluginAnimTls::CreateL();
        iTls = UT_AiWsPluginAnimTls::Instance();
        }
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::SetupDependenciesL()
//
// ----------------------------------------------------------------------------
//
void UT_AiWsPluginAnim::SetupDependenciesL()
    {
    if ( !iAnimGeneralFunctions )
        {
        iAnimGeneralFunctions = CAnimGeneralFunctionsStub::NewL();
        }
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::SetupTestObjL()
//
// ----------------------------------------------------------------------------
//
void UT_AiWsPluginAnim::SetupTestObjL( TBool aHasFocus )
    {
    CAiWsPluginAnim* obj = new (ELeave) CAiWsPluginAnim;
    CleanupStack::PushL( obj );
    
    CWsAnim::SetFunctions( *obj, iAnimGeneralFunctions );
    obj->ConstructL( const_cast<TAiWsPluginAnimInitData*>
        (&KWsPluginInitData), aHasFocus );
    
    CleanupStack::Pop( obj );
    iCAiWsPluginAnim = obj;
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::SetupL()
//
// ----------------------------------------------------------------------------
//
void UT_AiWsPluginAnim::UT_AiWsPluginAnim::SetupL( TBool aHasFocus )
    {
    SetupTlsL();
    SetupDependenciesL();
    SetupTestObjL( aHasFocus );
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::SetupL()
//
// ----------------------------------------------------------------------------
//
void UT_AiWsPluginAnim::SetupL()
    {
    SetupL( KNoFocus );
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::SetupNoSliderL()
//
// ----------------------------------------------------------------------------
//
void UT_AiWsPluginAnim::SetupNoSliderL( TBool aHasFocus )
    {
    SetupTlsL();
    iTls->SetFeatureSupportedL( KFeatureIdKeypadNoSlider, ETrue );
    SetupDependenciesL();
    SetupTestObjL( aHasFocus );
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::SetupNoSliderL()
//
// ----------------------------------------------------------------------------
//
void UT_AiWsPluginAnim::SetupNoSliderL()
    {
    SetupNoSliderL( KNoFocus );
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::Teardown()
//
// ----------------------------------------------------------------------------
//
void UT_AiWsPluginAnim::Teardown()
    {
    delete iCAiWsPluginAnim; 
    iCAiWsPluginAnim = NULL;
    
    delete iAnimGeneralFunctions;
    iAnimGeneralFunctions = NULL;
    
    if ( iTls )
        {
        // Store feature manager ref count for testing in T_ConstructL
        iFeatMgrCount = iTls->FeatMgrCount();
        }
    
    UT_AiWsPluginAnimTls::Destroy();
    iTls = NULL;
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::T_ConstructL()
//
// ----------------------------------------------------------------------------
//
void UT_AiWsPluginAnim::T_ConstructL( TBool aHasFocus )
    {
    // Test that iCAiWsPluginAnim is in the correct state
    EUNIT_ASSERT_EQUALS( iCAiWsPluginAnim->HasFocus(), aHasFocus );
    EUNIT_ASSERT_EQUALS( iCAiWsPluginAnim->Modifiers(), 0 );
    
    // Test that event handlers are initialized to correct state
    TArray<T_AiWsPlugin::CTestEventHandler*> 
        eventHandlers( iTls->EventHandlers() );
        
    for( TInt i = 0; i < eventHandlers.Count(); ++i )
        {
        T_AiWsPlugin::CTestEventHandler* handler = eventHandlers[i];
        EUNIT_ASSERT_EQUALS( handler->Focus(), aHasFocus );
        EUNIT_ASSERT( handler->UiState() );
        EUNIT_ASSERT_EQUALS( handler->UiState()->HasFocus(), aHasFocus );
        EUNIT_ASSERT_EQUALS( handler->UiState()->Modifiers(), 0 );
        }
        
    // Test that numeric key handler is correctly set up
    AiWsPlugin::CNumericKeyHandler* numKeyHandler = 
        iTls->FindEventHandler< AiWsPlugin::CNumericKeyHandler >();
    
    EUNIT_ASSERT( numKeyHandler );
    EUNIT_ASSERT_EQUALS( numKeyHandler->iTargetWgId, KTargetWgId );
    
    MAnimGeneralFunctionsWindowExtension* ext = 
        static_cast<MAnimGeneralFunctionsWindowExtension*>( 
            iAnimGeneralFunctions->ExtendedInterface
                ( MAnimGeneralFunctions::EWindowExtensionInterface ) );
    
    EUNIT_ASSERT_EQUALS( numKeyHandler->iWindowExt, ext );
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::T_ConstructL()
//
// ----------------------------------------------------------------------------
//
void UT_AiWsPluginAnim::T_ConstructL()
    {
    // Test construction with no focus
    SetupL( EFalse );
    EUNIT_ASSERT_EQUALS( iFeatMgrCount, 0 );
    T_ConstructL( EFalse );
    Teardown();
    SetupNoSliderL( EFalse );
    T_ConstructL( EFalse );
    Teardown();
    EUNIT_ASSERT_EQUALS( iFeatMgrCount, 0 );

    // Test construction with focus
    SetupL( ETrue );
    T_ConstructL( ETrue );
    Teardown();
    EUNIT_ASSERT_EQUALS( iFeatMgrCount, 0 );
    SetupNoSliderL( ETrue );
    T_ConstructL( ETrue );
    Teardown();
    EUNIT_ASSERT_EQUALS( iFeatMgrCount, 0 );
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::T_CAiWsPluginAnim_EmptyFunctionsL()
//
// ----------------------------------------------------------------------------
//
void UT_AiWsPluginAnim::T_CAiWsPluginAnim_EmptyFunctionsL()
    {
    // Call empty functions to ensure coverage targets are reached
    TInt result = KErrNotFound;
    EUNIT_ASSERT_NO_LEAVE( result = iCAiWsPluginAnim->CommandReplyL( 0, NULL ) );
    EUNIT_ASSERT_EQUALS( result, KErrNone );
    
    EUNIT_ASSERT_NO_LEAVE( iCAiWsPluginAnim->Command( 0, NULL ) );
    
    EUNIT_ASSERT_NO_LEAVE( iCAiWsPluginAnim->Animate( NULL ) );
    
    EUNIT_ASSERT_NO_LEAVE( iCAiWsPluginAnim->Redraw() );
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::T_CAiWsPluginAnim_FocusChangedL()
//
// ----------------------------------------------------------------------------
//
void UT_AiWsPluginAnim::T_CAiWsPluginAnim_FocusChangedL( TBool aFocus )
    {
    iCAiWsPluginAnim->FocusChanged( aFocus );
    
    EUNIT_ASSERT_EQUALS( iCAiWsPluginAnim->HasFocus(), aFocus );
    
    TArray<T_AiWsPlugin::CTestEventHandler*> 
        eventHandlers( iTls->EventHandlers() );
    
    for ( TInt i = 0; i < eventHandlers.Count(); ++i )
        {
        T_AiWsPlugin::CTestEventHandler* handler = eventHandlers[i];
        EUNIT_ASSERT_EQUALS( handler->Focus(), aFocus );
        EUNIT_ASSERT( handler->UiState() );
        EUNIT_ASSERT_EQUALS( handler->UiState()->HasFocus(), aFocus );
        }
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::T_CAiWsPluginAnim_FocusChangedL()
//
// ----------------------------------------------------------------------------
//
void UT_AiWsPluginAnim::T_CAiWsPluginAnim_FocusChangedL()
    {
    T_CAiWsPluginAnim_FocusChangedL( ETrue );
    T_CAiWsPluginAnim_FocusChangedL( EFalse );
    T_CAiWsPluginAnim_FocusChangedL( !iCAiWsPluginAnim->HasFocus() );
    T_CAiWsPluginAnim_FocusChangedL( !iCAiWsPluginAnim->HasFocus() );
    }

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::TestLastEventsL()
//
// ----------------------------------------------------------------------------
//
void TestLastEventsL(  
    const TArray<T_AiWsPlugin::CTestEventHandler*>& aEventHandlers,
    const TRawEvent& aRefEvent )
    {
    for( TInt i = 0; i < aEventHandlers.Count(); ++i )
        {
        T_AiWsPlugin::CTestEventHandler* handler = aEventHandlers[i];
        EUNIT_ASSERT_EQUALS( handler->LastEvent(), aRefEvent );
        handler->ResetLastEvent();
        }
    }        

// ----------------------------------------------------------------------------
// UT_AiWsPluginAnim::T_CAiWsPluginAnim_OfferRawEventL()
//
// ----------------------------------------------------------------------------
//
void UT_AiWsPluginAnim::T_CAiWsPluginAnim_OfferRawEventL()
    {
    TRawEvent nullEvent;
    ResetEvent( nullEvent );
    TRawEvent testEvent( nullEvent );
    TArray<T_AiWsPlugin::CTestEventHandler*> eventHandlers( iTls->EventHandlers() );
    
    // Check initial state
    TestLastEventsL( eventHandlers, nullEvent );

    // Test modifier key event
    testEvent.Set( TRawEvent::EKeyDown, EStdKeyLeftShift );
    EUNIT_ASSERT( !iCAiWsPluginAnim->OfferRawEvent( testEvent ) );
    // Modifier key events should not be forwarded to event handlers
    TestLastEventsL( eventHandlers, nullEvent );
    testEvent.Set( TRawEvent::EKeyUp, EStdKeyLeftShift );
    EUNIT_ASSERT( !iCAiWsPluginAnim->OfferRawEvent( testEvent ) );
    // Modifier key events should not be forwarded to event handlers
    TestLastEventsL( eventHandlers, nullEvent );
    
    // Test non-modifier key event
    testEvent.Set( TRawEvent::EKeyDown, EStdKeyDevice0 );
    EUNIT_ASSERT( !iCAiWsPluginAnim->OfferRawEvent( testEvent ) );
    // Non-modifier key events should be forwarded to event handlers
    TestLastEventsL( eventHandlers, nullEvent );
    testEvent.Set( TRawEvent::EKeyUp, EStdKeyDevice0 );
    EUNIT_ASSERT( !iCAiWsPluginAnim->OfferRawEvent( testEvent ) );
    // Non-modifier key events should be forwarded to event handlers
    TestLastEventsL( eventHandlers, nullEvent );
    
    // Test event consuming
    testEvent.Set( TRawEvent::EKeyDown, EStdKeyDevice1 );
    // Make the first event handler consume the event
    eventHandlers[0]->RespondToEvent( testEvent );
    // OfferRawEvent should return true because the event is consumed
    EUNIT_ASSERT( iCAiWsPluginAnim->OfferRawEvent( testEvent ) );    
    }
    
//------------------------------------------------------------------------------
// Test case table
//
//------------------------------------------------------------------------------
//
EUNIT_BEGIN_TEST_TABLE(
    UT_AiWsPluginAnim,
    "Test suite for class CAiWsPluginAnim",
    "UNIT" )

    EUNIT_TEST(
        "Construction",
        "CAiWsPluginAnim",
        "ConstructL",
        "FUNCTIONALITY",
        EmptySetup, T_ConstructL, Teardown)
        
    EUNIT_TEST(
        "Test empty pure virtual overloads",
        "CAiWsPluginAnim",
        "CommandReplyL - test0",
        "FUNCTIONALITY",
        SetupL, T_CAiWsPluginAnim_EmptyFunctionsL, Teardown)
        
    EUNIT_TEST(
        "FocusChanged",
        "CAiWsPluginAnim",
        "FocusChanged",
        "FUNCTIONALITY",
        SetupL, T_CAiWsPluginAnim_FocusChangedL, Teardown)
        
    EUNIT_TEST(
        "OfferRawEvent",
        "CAiWsPluginAnim",
        "OfferRawEvent",
        "FUNCTIONALITY",
        SetupL, T_CAiWsPluginAnim_OfferRawEventL, Teardown)
   
EUNIT_END_TEST_TABLE

