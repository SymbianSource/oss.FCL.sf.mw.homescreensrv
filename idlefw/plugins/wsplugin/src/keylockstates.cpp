/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Keylock states implementation for Active Idle WS Plug-in
*
*/


#include "keylockstates.h"
#include "keylockcontrol.h"
#include "keypadsettings.h"
#include "panic.h"
#include "activeidle2domainpskeys.h"

#include <e32event.h>
#include <e32property.h>

#include "debug.h"


namespace AiWsPlugin {

const TInt KTenMilliSecondsInu = 10 * 1000;

/**
 * Helper macro for declaring internal state classes for CKeyLockHandler.
 */
#define AI_DECLARE_KEYLOCK_STATE(StateCls) \
    class StateCls { \
    public: \
        static const TKeylockState KState; \
        static const TKeylockState& DefaultNextState(); \
        static void FocusChanged( MKeyLockHandlerControl& aControl, TBool aState ); \
        static TBool OfferRawEvent( MKeyLockHandlerControl& aControl, const TRawEvent& aRawEvent ); \
        static void TimerElapsed( MKeyLockHandlerControl& aControl ); \
        }

/**
 * Inactive state class for CKeyLockHandler.
 */
AI_DECLARE_KEYLOCK_STATE(TKeylockStateInactive);

/**
 * Active state class for CKeyLockHandler.
 */
AI_DECLARE_KEYLOCK_STATE(TKeylockStateWaitForFirstKeyDown);

/**
 * Keylock sequence in progress state class for CKeyLockHandler.
 */
AI_DECLARE_KEYLOCK_STATE(TKeylockStateWaitForSecondKeyDown);

/**
 * Keylock sequence finalizer state class for CKeyLockHandler.
 */
AI_DECLARE_KEYLOCK_STATE(TKeylockStateWaitForSecondKeyUp);

/**
 * Helper macro for defining key lock state function tables.
 */
#define AI_DEFINE_KEYLOCK_STATE_FT(StateCls) \
    const TKeylockState StateCls::KState = {     \
        &StateCls::DefaultNextState,             \
        &StateCls::FocusChanged,                 \
        &StateCls::OfferRawEvent,                \
        &StateCls::TimerElapsed }

/// Defines function table for key lock inactive state
AI_DEFINE_KEYLOCK_STATE_FT(TKeylockStateInactive);

/// Defines function table for key lock active state
AI_DEFINE_KEYLOCK_STATE_FT(TKeylockStateWaitForFirstKeyDown);

/// Defines function table for key lock in progress state
AI_DEFINE_KEYLOCK_STATE_FT(TKeylockStateWaitForSecondKeyDown);

/// Defines function table for key lock finalizing state
AI_DEFINE_KEYLOCK_STATE_FT(TKeylockStateWaitForSecondKeyUp);

// TKeyLockState
const TKeylockState& TKeylockState::StartupState()
    {
    return TKeylockStateInactive::KState;
    }

// TKeylockStateInactive
const TKeylockState& TKeylockStateInactive::DefaultNextState()
    {
    return TKeylockStateWaitForFirstKeyDown::KState;
    }
    
void TKeylockStateInactive::FocusChanged
        ( MKeyLockHandlerControl& aControl, TBool aState )
    {
    __PRINT( __DBG_FORMAT("AiWsPlugin: TKeylockStateInactive::FocusChanged(%d)"), aState );
    if ( aState )
        {
        // Gained focus, switch to active state
        aControl.ActivateNextState();    
        }
    }
    
TBool TKeylockStateInactive::OfferRawEvent
        ( MKeyLockHandlerControl& /*aControl*/, const TRawEvent& /*aRawEvent*/ )
    {
    return EFalse;
    }
    
void TKeylockStateInactive::TimerElapsed
        ( MKeyLockHandlerControl& aControl )
    {
    // Illegal event in this state
    aControl.KeypadLockTimeout();
#ifndef NDEBUG
    //Panic( EPanicInvalidKeylockEvent );
#endif    
    }

// TKeylockStateWaitForFirstLockKeyDown
const TKeylockState& TKeylockStateWaitForFirstKeyDown::DefaultNextState()
    {
    return TKeylockStateWaitForSecondKeyDown::KState;
    }

void TKeylockStateWaitForFirstKeyDown::FocusChanged
        ( MKeyLockHandlerControl& aControl, TBool aState )
    {
    __PRINT( __DBG_FORMAT("AiWsPlugin: TKeylockStateWaitForFirstKeyDown::FocusChanged(%d)"), aState );
    if ( !aState )
        {
        if( !aControl.TimeoutTimerActive() )
            {
            // Lost focus, switch to inactive state
            aControl.SetNextState( TKeylockStateInactive::KState );
            aControl.ActivateNextState();
            }
        }
    }
    
TBool TKeylockStateWaitForFirstKeyDown::OfferRawEvent
        ( MKeyLockHandlerControl& aControl, const TRawEvent& aRawEvent )
    {
    switch ( aRawEvent.Type() )
        {
        case TRawEvent::EKeyDown:
            {
            const TInt scanCode = aRawEvent.ScanCode();
            __PRINT( __DBG_FORMAT("AiWsPlugin: TKeylockStateWaitForFirstKeyDown::OfferRawEvent(EKeyDown,ScanCode=%d)" ), scanCode );
            if ( aControl.IsFirstLockKey( scanCode ) && aControl.HasFocus() )
                {
                // First lock key was pressed down
                aControl.SetLastLockKeyScanCode( scanCode );
                // (Re)activate the keylock timeout timer
                aControl.StartTimeoutTimer(KTenMilliSecondsInu);
                }
            }
        }

    // Never consume the event to enable its processing if keylock
    // is not activated within the timeout
    return EFalse;
    }
    
void TKeylockStateWaitForFirstKeyDown::TimerElapsed
        ( MKeyLockHandlerControl& aControl )
    {
                TInt value = EPSAiNotDisplayingMenuOrDialog;          
                TInt err = RProperty::Get(
                   KPSUidAiInformation, 
                   KActiveIdlePopupState, 
                   value );
                
                if( value == EPSAiNotDisplayingMenuOrDialog && err == KErrNone )
                    {
                    // (Re)activate the keylock timeout timer
                    aControl.StartTimeoutTimer();
                    // Switch to wait for second lock key down (see constructor of this state)
                    aControl.ActivateNextState();
                    }
                else
                    {
                    if( aControl.HasFocus() )
                        {
                        aControl.StopTimeoutTimer();
                        }
                    else
                        {
                        // Lost focus, switch to inactive state
                        aControl.SetNextState( TKeylockStateInactive::KState );
                        aControl.ActivateNextState();
                        }
                    }
    // Illegal event in this state
#ifndef NDEBUG
    //Panic( EPanicInvalidKeylockEvent );
#endif    
    }

// TKeylockStateWaitForSecondKeyDown
const TKeylockState&  TKeylockStateWaitForSecondKeyDown::DefaultNextState()
    {
    // Assume the keylock sequence is cancelled
    return TKeylockStateWaitForFirstKeyDown::KState;
    }

void TKeylockStateWaitForSecondKeyDown::FocusChanged
        ( MKeyLockHandlerControl& aControl, TBool aState )
    {
    __PRINT( __DBG_FORMAT("AiWsPlugin: TKeylockStateWaitForSecondKeyDown::FocusChanged(%d)"), aState );
    if ( aState )
        {
        // Gained focus: return to active state if keylock sequence is cancelled
        aControl.SetNextState( TKeylockStateWaitForFirstKeyDown::KState );
        }
    else
        {
        // Lost focus: return to inactive state if keylock sequence is cancelled
        aControl.SetNextState( TKeylockStateInactive::KState );
        }
    }
    
TBool TKeylockStateWaitForSecondKeyDown::OfferRawEvent
        ( MKeyLockHandlerControl& aControl, const TRawEvent& aRawEvent )
    {
    TBool consumedEvent = EFalse;
    
    switch ( aRawEvent.Type() )
        {
        case TRawEvent::EKeyDown:
            {
            const TInt scanCode = aRawEvent.ScanCode();
            __PRINT( __DBG_FORMAT("AiWsPlugin: TKeylockStateWaitForSecondKeyDown::OfferRawEvent(EKeyDown,ScanCode=%d)" ), scanCode );
            if ( aControl.IsSecondLockKey( scanCode ) )
                {
                // Second lock key was pressed down. Cancel keylock timer
                // and switch to wait for key up event.
                aControl.StopTimeoutTimer();
                aControl.SetLastLockKeyScanCode( scanCode );
                aControl.SetNextState( TKeylockStateWaitForSecondKeyUp::KState );
                aControl.ActivateNextState();
                consumedEvent = ETrue;
                }
            else
                {
                // Some other key than second lock key was pressed while
                // keylock timer was running. Cancel the keylock sequence and
                // switch to previous state. (Depending on focus status,
                // see TKeylockStateWaitForFirstKeyDown::FocusChanged.)
                aControl.CancelKeypadLock();
                aControl.ActivateNextState();
                consumedEvent = EFalse;
                }
            break;
            }
        }
        
    return consumedEvent;
    }
    
void TKeylockStateWaitForSecondKeyDown::TimerElapsed
        ( MKeyLockHandlerControl& aControl )
    {
    __PRINTS( "AiWsPlugin: TKeylockStateWaitForSecondKeyDown::TimerElapsed()" ); 
    aControl.KeypadLockTimeout();
    aControl.ActivateNextState();
    }

// TKeylockStateWaitForSecondKeyUp
const TKeylockState& TKeylockStateWaitForSecondKeyUp::DefaultNextState()
    {
    // Idle must currently have focus so return to active state from this state
    return TKeylockStateWaitForFirstKeyDown::KState;
    }

void TKeylockStateWaitForSecondKeyUp::FocusChanged
        ( MKeyLockHandlerControl& aControl, TBool aState )
    {
    __PRINT( __DBG_FORMAT("AiWsPlugin: TKeylockStateWaitForSecondKeyUp::FocusChanged(%d)"), aState );
    if ( aState )
        {
        // Gained focus: return to active state if keylock sequence is cancelled
        aControl.SetNextState(
            TKeylockStateWaitForFirstKeyDown::KState );
        }
    else
        {
        // Lost focus: return to inactive state if keylock sequence is cancelled
        aControl.SetNextState( TKeylockStateInactive::KState );
        }
    }
    
TBool TKeylockStateWaitForSecondKeyUp::OfferRawEvent
        ( MKeyLockHandlerControl& aControl, const TRawEvent& aRawEvent )
    {
    TBool consumedEvent = EFalse;
    
    switch ( aRawEvent.Type() )
        {
        case TRawEvent::EKeyUp:
            {
            const TInt scanCode = aRawEvent.ScanCode();
            __PRINT( __DBG_FORMAT("AiWsPlugin: TKeylockStateWaitForSecondKeyUp::OfferRawEvent(EKeyUp,ScanCode=%d)" ), scanCode );
            if ( scanCode == aControl.LastLockKeyScanCode() )
                {
                // Second lock key was released. Activate the keypad lock and
                // switch back to initial state. (Depending on focus status,
                // see TKeylockStateWaitForSecondKeyUp::FocusChanged.)
                aControl.ActivateKeypadLock();
                aControl.ActivateNextState();
                }
            // Consume all key events until the keylock sequence is finalized
            consumedEvent = ETrue;
            break;
            }

        case TRawEvent::EKeyDown:
            {
            // Consume all key events until the keylock sequence is finalized
            consumedEvent = ETrue;
            break;
            }
        }
        
    return consumedEvent;
    }
    
void TKeylockStateWaitForSecondKeyUp::TimerElapsed
        ( MKeyLockHandlerControl& /*aControl*/ )
    {
    // Illegal event in this state
#ifndef NDEBUG
    //Panic( EPanicInvalidKeylockEvent );
#endif    
    }

} // namespace AiWsPlugin
