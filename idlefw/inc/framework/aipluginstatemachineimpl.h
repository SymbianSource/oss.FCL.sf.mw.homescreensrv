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
* Description:  Plugin state machine implementation
*
*/


#ifndef C_AIPLUGINSTATEMACHINEIMPL_H
#define C_AIPLUGINSTATEMACHINEIMPL_H

#include <e32base.h>
#include <aicontentpublisher.h>
#include "aipluginstatemachine.h"
#include "aistatealive.h"
#include "aistatesuspended.h"
#include "aistateidle.h"

class MAiPluginStateControl;
class MAiPluginState;

/**
 * Plugin state machine implementation.
 * 
 *  @lib aifw_hs
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAiPluginStateMachine ) : public CBase,
                                             public MAiPluginStateMachine
    {
public: // Constructor 
       
    CAiPluginStateMachine( MAiPluginStateResources& aPluginStateResource,                           
                           CAiContentPublisher& aPlugin );
    
public: // from MAiPluginStateMachine
                                  
    TBool StateVariable( TAiStateVariable aStateVariable );
    
    TAiTransitionReason TranslateReason( TAiStateChanges aStateChange );
    
    void RestartSuspendTimer();
    
    void SwitchToState( TAiState aState, TAiStateChanges aStateChange );
    
    CAiContentPublisher& Plugin() const;
              
    void ChangePluginState( TAiTransitionReason aReason,                
                            void (CAiContentPublisher::*aStateChangeMethod)(TAiTransitionReason) );
                
public: // new methods

    TBool HandleEvent( TAiStateChanges aStateChange );
        
private: // data

    /**
     * Alive master state.
     */
    TAiStateAlive iAlive;

    /**
     * Suspended state.
     */
    TAiStateSuspended iSuspended;

    /**
     * Idle master state.
     */
    TAiStateIdle iIdle;

    /**
     * Pointer to current state.
     * Not owned.
     */
    MAiPluginState* iCurrentState;

    /**
     * Pointer to parent state machine.
     * Not owned.
     */
    MAiPluginStateResources& iPluginStateResource;
    
    /**
     * Pointer to managed plugin.
     * Not owned.
     */
    CAiContentPublisher& iPlugin;    

    /**
     * Online sub-state     
     */    
    TBool iOnline;
    };

#endif // C_AIPLUGINSTATEMACHINEIMPL_H
