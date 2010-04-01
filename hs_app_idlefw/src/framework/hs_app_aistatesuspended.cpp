/*
* Copyright (c) 2005-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  State class for suspended state
*
*/


#include "hs_app_aistatesuspended.h"
#include "hs_app_aipluginstatemachine.h"
#include "debug.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// TAiStateSuspended::TAiStateSuspended()
// ----------------------------------------------------------------------------
//
TAiStateSuspended::TAiStateSuspended()
    {
    }

// ----------------------------------------------------------------------------
// TAiStateSuspended::Enter()
// ----------------------------------------------------------------------------
//
void TAiStateSuspended::Enter( MAiPluginStateMachine& aStateMachine,
    TAiStateChanges aStateChange )
    {
    __PRINTS( "XAI: Enter Suspended" );
    const TAiTransitionReason reason( 
            aStateMachine.TranslateReason( aStateChange ) );

    aStateMachine.ChangePluginState( reason, &CAiContentPublisher::Suspend );                                    
    }
    
// ----------------------------------------------------------------------------
// TAiStateSuspended::HandleEvent()
// ----------------------------------------------------------------------------
//
TBool TAiStateSuspended::HandleEvent( MAiPluginStateMachine& aStateMachine,
    TAiStateChanges aStateChange )
    {
    __PRINTS( "XAI: Suspended handle event" );
    switch( aStateChange )
        {
        case ESMAIIdleForeground:
        case ESMAIBacklightOn:
        case ESMAILocaleChanged:
        case ESMAITimeChanged:
        case ESMAIMidnightCrossover:
        case ESMAIRelayoutScreen:
        case ESMAIGeneralThemeChanged:
        case ESMAIInCall: // fallthrough
            {
            aStateMachine.SwitchToState( EAiAlive, aStateChange );
            return ETrue;
            }
        case ESMAIKeylockEnabled:
        case ESMAIKeylockDisabled:
            {
            Enter( aStateMachine, aStateChange );
            return ETrue;
            }
        default:
            {
            return EFalse;
            }
        }
    }

// ----------------------------------------------------------------------------
// TAiStateSuspended::Exit()
// ----------------------------------------------------------------------------
//
void TAiStateSuspended::Exit( MAiPluginStateMachine& /*aStateMachine*/,
    TAiStateChanges /*aStateChange*/ )
    {
    __PRINTS( "XAI: Exit Suspended" );
    }
   
