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
* Description:  State suspended
*
*/


#ifndef T_AISTATESUSPENDED_H
#define T_AISTATESUSPENDED_H

#include "hs_app_aipluginstate.h"

/**
 * @ingroup group_aifw
 * 
 * Suspended state
 * 
 *  @lib aifw
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( TAiStateSuspended ) : public MAiPluginState
    {
public:
    // Construction
    
    TAiStateSuspended();
    
    // from MAiPluginState
    
    void Enter( MAiPluginStateMachine& aStateMachine,
                TAiStateChanges aStateChange );
    
    TBool HandleEvent( MAiPluginStateMachine& aStateMachine,
                      TAiStateChanges aStateChange );
    
    void Exit( MAiPluginStateMachine& aStateMachine,
               TAiStateChanges aStateChange );
    
    };

#endif // T_AISTATESUSPENDED_H
