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
* Description:  State alive
*
*/


#ifndef T_AISTATEALIVE_H
#define T_AISTATEALIVE_H

#include "hs_app_aipluginstate.h"
#include "hs_app_aipluginstatemachine.h"

/**
 * @ingroup group_aifw
 * 
 * Alive active state
 * 
 *  @lib aifw
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( TAiStateAliveActive ) : public MAiPluginState
    {
public: // Constructor
    
    TAiStateAliveActive();
    
public: // from MAiPluginState
    
    void Enter( MAiPluginStateMachine& aStateMachine,
                TAiStateChanges aStateChange );
    
    TBool HandleEvent( MAiPluginStateMachine& aStateMachine,
                      TAiStateChanges aStateChange );
    
    void Exit( MAiPluginStateMachine& aStateMachine,
               TAiStateChanges aStateChange );
    
    };

/**
 * Alive inactive state
 * 
 *  @lib aifw
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( TAiStateAliveInactive ) : public MAiPluginState
    {
public: // Constructor    
    
    TAiStateAliveInactive();
    
public: // from MAiPluginState
    
    void Enter( MAiPluginStateMachine& aStateMachine,
                TAiStateChanges aStateChange );
    
    TBool HandleEvent( MAiPluginStateMachine& aStateMachine,
                      TAiStateChanges aStateChange );
    
    void Exit( MAiPluginStateMachine& aStateMachine,
               TAiStateChanges aStateChange );    
    };

/**
 * Alive incall state
 * 
 *  @lib aifw
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( TAiStateAliveIncall ) : public MAiPluginState
    {
public: // Constructor
    
    TAiStateAliveIncall();
    
public: // from MAiPluginState
    
    void Enter( MAiPluginStateMachine& aStateMachine,
                TAiStateChanges aStateChange );
    
    TBool HandleEvent( MAiPluginStateMachine& aStateMachine,
                      TAiStateChanges aStateChange );
    
    void Exit( MAiPluginStateMachine& aStateMachine,
               TAiStateChanges aStateChange );    
    };

/**
 * Alive switch state
 * 
 *  @lib aifw
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( TAiStateAlive ) : public MAiPluginState,
                                     public MAiPluginStateMachine
    {
public: // Constructor
        
    TAiStateAlive( MAiPluginStateMachine& aParentStateMachine );
    
public: // from MAiPluginStateMachine
    
    TBool StateVariable( TAiStateVariable aStateVariable );
    
    TAiTransitionReason TranslateReason( TAiStateChanges aStateChange );
    
    void RestartSuspendTimer();
    
    void SwitchToState( TAiState aState, TAiStateChanges aStateChange );
                                  
    CAiContentPublisher& Plugin() const;
                 
    void ChangePluginState( TAiTransitionReason aReason,                
                            void (CAiContentPublisher::*aStateChangeMethod)(TAiTransitionReason) );

public: // from MAiPluginState
    
    void Enter( MAiPluginStateMachine& aStateMachine,
                TAiStateChanges aStateChange );
    
    TBool HandleEvent( MAiPluginStateMachine& aStateMachine,
                       TAiStateChanges aStateChange );
    
    void Exit( MAiPluginStateMachine& aStateMachine,
               TAiStateChanges aStateChange );
               
private: // data

    /**
     * Pointer to current state.
     * Not owned.
     */
    MAiPluginState* iCurrentState;

    /**
     * Pointer to parent state machine.
     * Not owned.
     */
    MAiPluginStateMachine* iParentStateMachine;

    /**
     * Alive active state.
     */
    TAiStateAliveActive iStateAliveActive;

    /**
     * Alive incall state.
     */
    TAiStateAliveIncall iStateAliveIncall;

    /**
     * Alive inactive state.
     */
    TAiStateAliveInactive iStateAliveInactive;    
    };

#endif // T_AISTATEALIVE_H
