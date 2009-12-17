/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  State class for alive states
*
*/


#include "aistatealive.h"
#include "aipluginstatemachine.h"
#include "debug.h"


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// TAiStateAliveActive::TAiStateAliveActive()
// ----------------------------------------------------------------------------
//
TAiStateAliveActive::TAiStateAliveActive()
    {
    }

// ----------------------------------------------------------------------------
// TAiStateAliveActive::Enter()
// ----------------------------------------------------------------------------
//
void TAiStateAliveActive::Enter( MAiPluginStateMachine& aStateMachine,
    TAiStateChanges aStateChange )
    {
    __PRINTS( "XAI: Enter Alive active" );
    
    const TAiTransitionReason reason( 
            aStateMachine.TranslateReason( aStateChange ) );
    
    aStateMachine.ChangePluginState( reason, CAiContentPublisher::Resume );                                    
    }
    
// ----------------------------------------------------------------------------
// TAiStateAliveActive::HandleEvent()
// ----------------------------------------------------------------------------
//
TBool TAiStateAliveActive::HandleEvent( MAiPluginStateMachine& aStateMachine,
    TAiStateChanges aStateChange )
    {
    __PRINTS( "XAI: Alive active handles event" );
    switch( aStateChange )
        {
        case ESMAIBacklightOff:
            {
            aStateMachine.SwitchToState( EAiAliveInactive, aStateChange );
            return ETrue;
            }
        default:
            {
            return EFalse;
            }
        }
    }
    
// ----------------------------------------------------------------------------
// TAiStateAliveActive::Exit()
// ----------------------------------------------------------------------------
//
void TAiStateAliveActive::Exit( MAiPluginStateMachine& /*aStateMachine*/,
    TAiStateChanges /*aStateChange*/ )
    {
    __PRINTS( "XAI: Exit alive active" );
    }

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// TAiStateAliveInactive::TAiStateAliveInactive()
// ----------------------------------------------------------------------------
//
TAiStateAliveInactive::TAiStateAliveInactive()
    {
    }

// ----------------------------------------------------------------------------
// TAiStateAliveInactive::Enter()
// ----------------------------------------------------------------------------
//
void TAiStateAliveInactive::Enter( MAiPluginStateMachine& aStateMachine,
    TAiStateChanges aStateChange )
    {
    __PRINTS( "XAI: Enter alive inactive" );
    const TAiTransitionReason reason( 
            aStateMachine.TranslateReason( aStateChange ) );
    
    aStateMachine.ChangePluginState( reason, CAiContentPublisher::Resume );
                                         
    aStateMachine.RestartSuspendTimer();
    }

// ----------------------------------------------------------------------------
// TAiStateAliveInactive::HandleEvent()
// ----------------------------------------------------------------------------
//
TBool TAiStateAliveInactive::HandleEvent( MAiPluginStateMachine& aStateMachine,
    TAiStateChanges aStateChange )
    {
    __PRINTS( "XAI: alive inactive handles event" );
    switch( aStateChange )
        {
        case ESMAIIdleForeground:
        case ESMAIBacklightOn:
            {
            aStateMachine.SwitchToState( EAiAliveActive, aStateChange );
            return ETrue;
            }
        case ESMAIT1Timeout:
            {
            aStateMachine.SwitchToState( EAiSuspended, aStateChange );
            return ETrue;
            }
        default:
            {
            return EFalse;
            }
        }
    }

// ----------------------------------------------------------------------------
// TAiStateAliveInactive::Exit()
// ----------------------------------------------------------------------------
//
void TAiStateAliveInactive::Exit( MAiPluginStateMachine& /*aStateMachine*/,
    TAiStateChanges /*aStateChange*/ )
    {
    __PRINTS( "XAI: Exit alive inactive" );
    }


// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// TAiStateAliveIncall::TAiStateAliveIncall()
// ----------------------------------------------------------------------------
//
TAiStateAliveIncall::TAiStateAliveIncall()
    {
    }

// ----------------------------------------------------------------------------
// TAiStateAliveIncall::Enter()
// ----------------------------------------------------------------------------
//
void TAiStateAliveIncall::Enter( MAiPluginStateMachine& aStateMachine,
    TAiStateChanges aStateChange )
    {
    __PRINTS( "XAI: Enter alive incall" );
    
    const TAiTransitionReason reason( 
            aStateMachine.TranslateReason( aStateChange ) );
    
    aStateMachine.ChangePluginState( reason, CAiContentPublisher::Resume );                                    
    }

// ----------------------------------------------------------------------------
// TAiStateAliveIncall::HandleEvent()
// ----------------------------------------------------------------------------
//
TBool TAiStateAliveIncall::HandleEvent( MAiPluginStateMachine& aStateMachine,
    TAiStateChanges aStateChange )
    {
    __PRINTS( "XAI: alive incall handles event" );
    switch( aStateChange )
        {
        case ESMAINoCall:
            {
            // To alive switch
            aStateMachine.SwitchToState( EAiAlive, aStateChange );
            return ETrue;
            }
        case ESMAIInCall:
            {
            // prevent master state from handling this
            return ETrue;
            }
        default:
            {
            return EFalse;
            }
        }
    }

// ----------------------------------------------------------------------------
// TAiStateAliveIncall::Exit()
// ----------------------------------------------------------------------------
//
void TAiStateAliveIncall::Exit( MAiPluginStateMachine& /*aStateMachine*/,
    TAiStateChanges /*aStateChange*/ )
    {
    __PRINTS( "XAI: Exit alive incall" );
    }
   
// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// TAiStateAlive::TAiStateAlive()
// ----------------------------------------------------------------------------
//
TAiStateAlive::TAiStateAlive( MAiPluginStateMachine& aParentStateMachine )
  : iCurrentState( &iStateAliveInactive ),
    iParentStateMachine( &aParentStateMachine )
    {
    }

// ----------------------------------------------------------------------------
// TAiStateAlive::SwitchToState()
// ----------------------------------------------------------------------------
//
void TAiStateAlive::SwitchToState( TAiState aState,
    TAiStateChanges aStateChange )
    {
    switch( aState )
        {
        case EAiAliveInactive:
            {
            iCurrentState = &iStateAliveInactive;
            break;
            }
        case EAiAliveActive:
            {
            iCurrentState = &iStateAliveActive;
            break;
            }
        case EAiAliveIncall:
            {
            iCurrentState = &iStateAliveIncall;
            break;
            }
        default:
            {
            iParentStateMachine->SwitchToState( aState, aStateChange );
            return;
            }
        }   
    iCurrentState->Enter( *this, aStateChange );
    }

// ----------------------------------------------------------------------------
// TAiStateAlive::StateVariable()
// ----------------------------------------------------------------------------
//
TBool TAiStateAlive::StateVariable( TAiStateVariable aStateVariable )
    {
    return iParentStateMachine->StateVariable( aStateVariable );
    }

// ----------------------------------------------------------------------------
// TAiStateAlive::Plugin()
// ----------------------------------------------------------------------------
//
CAiContentPublisher& TAiStateAlive::Plugin() const
    {
    return iParentStateMachine->Plugin();
    }

// ----------------------------------------------------------------------------
// TAiStateAlive::TranslateReason()
// ----------------------------------------------------------------------------
//
TAiTransitionReason TAiStateAlive::TranslateReason( 
    TAiStateChanges aStateChange )
    {
    return iParentStateMachine->TranslateReason( aStateChange );
    }

// ----------------------------------------------------------------------------
// TAiStateAlive::RestartSuspendTimer()
// ----------------------------------------------------------------------------
//
void TAiStateAlive::RestartSuspendTimer()
    {
    iParentStateMachine->RestartSuspendTimer();
    }

// ----------------------------------------------------------------------------
// TAiStateAlive::Enter()
// ----------------------------------------------------------------------------
//
void TAiStateAlive::Enter( MAiPluginStateMachine& aStateMachine,
    TAiStateChanges aStateChange )
    {
    __PRINTS( "XAI: Alive switch - enter" );
    if( aStateMachine.StateVariable( ESMAICallStatus ) )
        {
        // If call is ongoing enter alive in call
        iCurrentState = &iStateAliveIncall;
        }
    else if( !aStateMachine.StateVariable( ESMAILightStatus ) )
        {
        // If light is off enter alive inactive
        iCurrentState = &iStateAliveInactive;
        }
    else
        {
        // Otherwise alive active
        iCurrentState = &iStateAliveActive;
        }
    // finally call the Enter() method
    iCurrentState->Enter( *this, aStateChange );
    }
 
// ----------------------------------------------------------------------------
// TAiStateAlive::HandleEvent()
// ----------------------------------------------------------------------------
//
TBool TAiStateAlive::HandleEvent( MAiPluginStateMachine& /*aStateMachine*/,
    TAiStateChanges aStateChange )
    {
    __PRINTS( "XAI: Alive switch handle event" );

    if( !iCurrentState->HandleEvent( *this, aStateChange ) )
        {
        // Only master state machine handles the "backup/restore on" and
        // screen layout changed cases.
        // Other event are forwarded to the currently active sub state.
        // Other common events may be added here in the future also.
        switch( aStateChange )
            {
            case ESMAIOffLine:
            case ESMAIOnLine:
            case ESMAIKeylockEnabled:
            case ESMAIKeylockDisabled:
            case ESMAIRelayoutScreen:
            case ESMAIIdleForeground:
            case ESMAIIdleBackground:
            case ESMAIPageSwitch:
            case ESMAIGeneralThemeChanged: // fallthrough
                {
                iCurrentState->Enter( *this, aStateChange );
                // Handled the event ok
                break;
                }
            case ESMAIInCall:
                {
                SwitchToState( EAiAliveIncall, aStateChange );
                // Handled the event ok
                break;
                }
            default:
                {
                // Neither current state or master handled this event
                return EFalse;
                }
            }
        }
    // Current state handled the event
    return ETrue;
    }

// ----------------------------------------------------------------------------
// TAiStateAlive::Exit()
// ----------------------------------------------------------------------------
//
void TAiStateAlive::Exit( MAiPluginStateMachine& /*aStateMachine*/,
    TAiStateChanges aStateChange )
    {
    __PRINTS( "XAI: Alive switch exit" );
    iCurrentState->Exit( *this, aStateChange );
    }

// ----------------------------------------------------------------------------
// TAiStateAlive::ChangePluginState()
// ----------------------------------------------------------------------------
//
void TAiStateAlive::ChangePluginState( TAiTransitionReason aReason,       
    void (CAiContentPublisher::*aStateChangeMethod)(TAiTransitionReason) )
    {
    iParentStateMachine->ChangePluginState( aReason, aStateChangeMethod );
    }
   
// End of file
