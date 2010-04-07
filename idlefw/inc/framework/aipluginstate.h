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
* Description:  Plugin state main class
*
*/


#ifndef M_AIPLUGINSTATE_H
#define M_AIPLUGINSTATE_H

#include "aipluginstatemachine.h"

/**
 * Plugin state interface.
 * 
 *  @lib aifw_hs
 *  @since S60 3.2
 */
class MAiPluginState
    {
public:
    
    /**
     * This method is called when state is entered.
     * 
     * @param aStateMachine reference to the owning state machine.
     * @param aStateChange the system state variable change that caused this 
     *        state change.
     */
    virtual void Enter( MAiPluginStateMachine& aStateMachine,
                        TAiStateChanges aStateChange ) = 0;
    
    /**
     * This method handles system state variable state changes
     * handles events in the current state.
     * 
     * @param aStateMachine reference to the owning state machine.
     * @param aStateChange the system state variable change that is the cause
     *        of this event.
     * @return TBool did the state handle the event.
     */
    virtual TBool HandleEvent( MAiPluginStateMachine& aStateMachine,
                              TAiStateChanges aStateChange ) = 0;
    
    /**
     * This method is called when state is exited.
     * 
     * @param aStateMachine reference to the owning state machine.
     * @param aStateChange the system state variable change that caused this 
     *        state change.
     */
    virtual void Exit( MAiPluginStateMachine& aStateMachine,
                       TAiStateChanges aStateChange ) = 0;

protected:

    ~MAiPluginState()
        {
        };
    
    };

#endif // M_AIPLUGINSTATE_H
