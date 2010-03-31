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
* Description:  State idle
*
*/


#ifndef T_AISTATEIDLE_H
#define T_AISTATEIDLE_H

#include "hs_app_aipluginstate.h"
#include "hs_app_aipluginstatemachine.h"

/**
 * @ingroup group_aifw
 * 
 * Idle state.
 * 
 *  @lib aifw
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( TAiStateIdle ) : public MAiPluginState
    {
public:
    // Construction
    
    TAiStateIdle();
    
    // from MAiPluginState
    
    void Enter( MAiPluginStateMachine& aStateMachine,
                TAiStateChanges aStateChange );
    
    TBool HandleEvent( MAiPluginStateMachine& aStateMachine,
                      TAiStateChanges aStateChange );
    
    void Exit( MAiPluginStateMachine& aStateMachine,
               TAiStateChanges aStateChange );
    
    };

#endif // T_AISTATEIDLE_H
