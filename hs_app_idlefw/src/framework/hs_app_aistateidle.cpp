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
* Description:  State class for idle state
*
*/


#include "hs_app_aistateidle.h"
#include "hs_app_aipluginstatemachine.h"
#include "debug.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// TAiStateIdle::TAiStateIdle()
// ----------------------------------------------------------------------------
//
TAiStateIdle::TAiStateIdle()
    {
    }

// ----------------------------------------------------------------------------
// TAiStateIdle::Enter()
// ----------------------------------------------------------------------------
//
void TAiStateIdle::Enter( MAiPluginStateMachine& aStateMachine,
    TAiStateChanges aStateChange )
    {
    __PRINTS( "XAI: Enter idle backup restore" );
    const TAiTransitionReason reason( 
            aStateMachine.TranslateReason( aStateChange ) );
    
    aStateMachine.ChangePluginState( reason, CAiContentPublisher::Stop );                                    
    }
    
// ----------------------------------------------------------------------------
// TAiStateIdle::HandleEvent()
// ----------------------------------------------------------------------------
//
TBool TAiStateIdle::HandleEvent( MAiPluginStateMachine& aStateMachine,
    TAiStateChanges aStateChange )
    {
    __PRINTS( "XAI: Idle backup restore handle event" );
    switch( aStateChange )
        {
        case ESMAIBackupOff:
            {
            aStateMachine.SwitchToState( EAiAlive, aStateChange );
            return ETrue;
            }
        case ESMAIKeylockEnabled:
        case ESMAIKeylockDisabled: // fallthrough
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
// TAiStateIdle::Exit()
// ----------------------------------------------------------------------------
//
void TAiStateIdle::Exit( MAiPluginStateMachine& /*aStateMachine*/,
    TAiStateChanges /*aStateChange*/ )
    {
    __PRINTS( "XAI: Exit idle backup restore" );
    }

// End of file
