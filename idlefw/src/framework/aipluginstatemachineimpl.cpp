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
* Description:  Plugin state machine impl
*
*/


#include "aipluginstatemachineimpl.h"
#include "aipluginstatemachine.h"
#include "aipluginlifecycleobserver.h"
#include "aipluginstate.h"
#include "aifwpanic.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CAiPluginStateMachine::CAiPluginStateMachine()
// ----------------------------------------------------------------------------
//
CAiPluginStateMachine::CAiPluginStateMachine(
    MAiPluginStateResources& aPluginStateResource,    
    CAiContentPublisher& aPlugin )
      : iAlive( *this ),
        iCurrentState( NULL ),
        iPluginStateResource( aPluginStateResource ),        
        iPlugin( aPlugin )
    {
    }

// ---------------------------------------------------------------------------
// CAiPluginStateMachine::SwitchToState()
// ----------------------------------------------------------------------------
//
void CAiPluginStateMachine::SwitchToState( TAiState aState,
    TAiStateChanges aStateChange )
    {
    // Store previous state
    MAiPluginState* previousState( iCurrentState );
    
    // Determine new current state
    switch( aState )
        {
        case EAiAlive:
            {
            iCurrentState = &iAlive;
            break;
            }
        case EAiSuspended:
            {
            iCurrentState = &iSuspended;
            break;
            }
        case EAiIdle:
            {
            iCurrentState = &iIdle;
            break;
            }
        default:
            {
#ifdef _DEBUG
            AiFwPanic::Panic( AiFwPanic::EAiFwPanic_IllegalPluginStateChange );
#endif
            break;
            }
        }
    
    if( previousState != iCurrentState )
        {
        if( previousState )
            {
            // Exit the previous state
            previousState->Exit( *this, aStateChange );            
            }
        
        if( iCurrentState )
            {
            // Enter the new state
            iCurrentState->Enter( *this, aStateChange );                    
            }
        }
    }

// ---------------------------------------------------------------------------
// CAiPluginStateMachine::StateVariable()
// ----------------------------------------------------------------------------
//
TBool CAiPluginStateMachine::StateVariable( TAiStateVariable aStateVariable )
    {
    return iPluginStateResource.StateVariable( aStateVariable );
    }

// ---------------------------------------------------------------------------
// CAiPluginStateMachine::Plugin()
// ----------------------------------------------------------------------------
//
CAiContentPublisher& CAiPluginStateMachine::Plugin() const
    {
    return iPlugin;
    }

// ---------------------------------------------------------------------------
// CAiPluginStateMachine::HandleEvent()
// ----------------------------------------------------------------------------
//
TBool CAiPluginStateMachine::HandleEvent( TAiStateChanges aStateChange )
    {
    // State machine handles some state changes directly.
    switch( aStateChange )
        {
        case ESMAISystemBoot:
            {
            // State machine handles startup event(boot/theme change).
            // Check if backup is ongoing
            if( iPluginStateResource.StateVariable( ESMAIBackupRestoreStatus ) )
                {
                SwitchToState( EAiIdle, aStateChange );
                }
            else
                {
                SwitchToState( EAiAlive, aStateChange );
                }
            break;
            }
        case ESMAIBackupOn:
            {
            // Backup/restore directs straight to idle state.
            SwitchToState( EAiIdle, aStateChange );
            break;
            }
        case ESMAIReportThemeChangeStarted:
        case ESMAISystemShutdown:
            {
            // Shutdown drives directly to idle state.
            ChangePluginState( iPluginStateResource.TranslateReason( aStateChange ),
                               CAiContentPublisher::Stop );
            break;
            }
        case ESMAIOnLine:
            {
            if( !iOnline && iCurrentState )
                {
                iOnline = ETrue;
                
                return iCurrentState->HandleEvent( *this, aStateChange );
                }                
            break;
            }
        case ESMAIOffLine:
            {
            if( iCurrentState )
                {
                iOnline = EFalse;
                
                return iCurrentState->HandleEvent( *this, aStateChange );                
                }            
            break;
            }
        default:
            {
            if( ( aStateChange == ESMAIBacklightOn ) &&
                !iPluginStateResource.StateVariable( ESMAIIdleFocusStatus ) )
                {
                // Ignore lights on when on background
                return ETrue;
                }
            
            if( iCurrentState )
                {
                // Other events are handled by the current set state.
                // Current state determines return value.
                return iCurrentState->HandleEvent( *this, aStateChange );                
                }
            }
        }
    
    // Return event handled.
    return ETrue;
    }

// ---------------------------------------------------------------------------
// CAiPluginStateMachine::TranslateReason()
// ----------------------------------------------------------------------------
//    
TAiTransitionReason CAiPluginStateMachine::TranslateReason( 
    TAiStateChanges aStateChange )
    {
    return iPluginStateResource.TranslateReason( aStateChange );
    }

// ---------------------------------------------------------------------------
// CAiPluginStateMachine::RestartSuspendTimer()
// ----------------------------------------------------------------------------
//
void CAiPluginStateMachine::RestartSuspendTimer()
    {
    iPluginStateResource.RestartSuspendTimer();
    }

// ---------------------------------------------------------------------------
// CAiPluginStateMachine::ChangePluginState()
// ----------------------------------------------------------------------------
//
void CAiPluginStateMachine::ChangePluginState( TAiTransitionReason aReason,         
    void (CAiContentPublisher::*aStateChangeMethod)( TAiTransitionReason ) )
    {       
	TRAP_IGNORE( ( iPlugin.*aStateChangeMethod)( aReason ) );	
    }

// End of file.
