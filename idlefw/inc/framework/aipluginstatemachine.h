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
* Description:  Plugin state mmchine base class
*
*/


#ifndef M_AIPLUGINSTATEMACHINE_H
#define M_AIPLUGINSTATEMACHINE_H

#include <aicontentpublisher.h>

class MAiPluginState;

/**
 * TAiStateChanges state change enumeration for each separate monitored state.
 */
enum TAiStateChanges
	{
	ESMAIInCall,
	ESMAINoCall,
    ESMAIBacklightOn,
    ESMAIBacklightOff,
    ESMAIBackupOn,
    ESMAIBackupOff,
    ESMAIIdleForeground,
    ESMAIIdleBackground,
    ESMAISystemBoot,
    ESMAISystemShutdown,
    ESMAILocaleChanged,
    ESMAITimeChanged,
    ESMAIMidnightCrossover,
    ESMAIReportThemeChangeStarted,
    ESMAIReportThemeChangeReady,
    ESMAIRelayoutScreen,
    ESMAIGeneralThemeChanged,
    ESMAIT1Timeout,
    ESMAIKeylockEnabled,
    ESMAIKeylockDisabled,
    ESMAIUnknownState,
    ESMAIEnterEditMode,
    ESMAIExitEditMode,
    ESMAIOffLine,
    ESMAIOnLine,
    ESMAIPageSwitch
	};
	
/**
 * Sate variable enumeration for querying the status of the monitored item.
 */
enum TAiStateVariable
	{
	ESMAICallStatus,
	ESMAILightStatus,
    ESMAIBackupRestoreStatus,
    ESMAIIdleFocusStatus
	};
	
/**
 * Possible states of the state machine (and plugin).
 */
enum TAiState
	{
	EAiIdle = 1,
	EAiSuspended,
	EAiAlive,
	EAiAliveActive,
	EAiAliveIncall,
	EAiAliveInactive
	};

/**
 * Sub states of idle state.
 */
enum TAiIdleSubState
    {
	EAiIdleCreatingPlugins = EAiAliveInactive + 1,
	EAiIdleBackupRestore
    };
	
/**
 * @ingroup group_aifw
 * 
 * Plugin state resource interface.
 * 
 *  @lib aifw_hs
 *  @since S60 3.2
 */
class MAiPluginStateResources
    {
public:
    
    /*
     * Check whether the specified state variable is active or not.
     *
     * @param aStateVariable system variable state change that is to be checked.
     */
    virtual TBool StateVariable( TAiStateVariable aStateVariable ) = 0;
    
    /**
     * Translates system state change reason to plugin state state reason.
     *
     * @param aStateChange the system state variable change to be translated.
     * @return TAiTransitionReason plugin state transition reason.
     */    
    virtual TAiTransitionReason TranslateReason( TAiStateChanges aStateChange ) = 0;
    
    /**
     * Restart plugin suspend timer.
     */    
    virtual void RestartSuspendTimer() = 0;

protected:
    ~MAiPluginStateResources() {}    
    };
    
/**
 * Plugin state machine interface.
 * 
 *  @lib aifw_hs
 *  @since S60 3.2
 */
class MAiPluginStateMachine : public MAiPluginStateResources
    {
public:
    
    /*
     * Method that changes this state machine to a state.
     *
     * @param aState state to change to.
     * @param aStateChange system variable state change that is the cause for this call.
     */
    virtual void SwitchToState( TAiState aState,
                                TAiStateChanges aStateChange ) = 0;
    
    /*
     * Reference to the plugin that is managed by this state machine.
     *
     * @return CAiContentPublisher reference to the plugin.
     */
    virtual CAiContentPublisher& Plugin() const = 0;

    /**
     * Change plugin states.
     *
     * @param aReason for transition
     * @param aStateChangeMethod state change method to call
     * @param aLogOpCode operation code for logging
     */
    virtual void ChangePluginState(
                TAiTransitionReason aReason, 
                void (CAiContentPublisher::*aStateChangeMethod)(TAiTransitionReason) ) = 0;
    
protected:
    ~MAiPluginStateMachine() {}
    };

#endif // M_AIPLUGINSTATEMACHINE_H
