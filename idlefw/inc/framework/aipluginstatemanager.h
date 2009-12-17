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
* Description:  Plugin state manager
*
*/


#ifndef C_AIPLUGINSTATEMANAGER_H
#define C_AIPLUGINSTATEMANAGER_H

#include <e32base.h>
#include "aistatemanager.h"
#include "aipluginstatemachine.h"
#include "aipluginlifecycleobserver.h"

#include "aistatealive.h"
#include "aistatesuspended.h"
#include "aistateidle.h"
#include "aifwdefs.h"

class MAiPSPropertyObserver;
class MAiDeviceStatusObserver;
class CAiPluginStateManager;
class CAiContentPluginManager;
class CAiPluginStateMachine;
class CAiUiFrameworkObserverImpl;
class MAiUiFrameworkObserver;

/**
 *  Plugin state manager implementation.
 *
 *  @lib aifw
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAiPluginStateManager ) : public CBase,
                                             public MAiStateManager,
                                             public MAiPluginStateResources,
                                             public MAiPluginLifecycleObserver
    {
    
public: // Constructor and destructor

    static CAiPluginStateManager* NewL();

    virtual ~CAiPluginStateManager();

public: // From MAiStateManager

    void ReportStateChange( TAiStateChanges aState );

public: // From MAiPluginStateMachine

    TBool StateVariable( TAiStateVariable aStateVariable );

    TAiTransitionReason TranslateReason( TAiStateChanges aStateChange );

    void RestartSuspendTimer();

public: // From MAiPluginLifecycleObserver

    void PluginCreatedL( CAiContentPublisher& aPlugin );

    void PluginDestroyed( CAiContentPublisher& aPlugin );
    
    void AllPluginsCreated();

    void AllPluginsDestroyed();
              
public: // new methods

    /**
     * Create system status observers (lights/backup/restore etc.).
     */
    void CreateSystemStateObserversL();

    /**
     * Destroy system status observers.
     */
    void DestroySystemStateObservers();

    /**
     * Provide accessor for fw observer.
     * @return MAiUiFrameworkObserver pointer to fw observer.
     */
    MAiUiFrameworkObserver* UiFwObserver() const;
    
    /**
     * Process online state change for a plugin
     * @param aPlugin plugin     
     */
    void ProcessOnlineState( CAiContentPublisher& aPlugin ); 

    /**
     * Process offline state change for a plugin
     * @param aPlugin plugin     
     */    
    void ProcessOfflineState( CAiContentPublisher& aPlugin );
    
private: // Constructors

    CAiPluginStateManager();

    void ConstructL();

private: // New functions
    
    /**
     * Handles Statemachine event event and error array update.
     * @param aState new state.
     * @param aMachine reference to single state machine.
     */
    void ProcessStateChange( TAiStateChanges aState,
                             CAiPluginStateMachine& aMachine );

    /**
     * Handles Statemachine event event and error array update
     * for all state machines.
     * @param aState new state.
     */
    void ProcessStateChangeForAll( TAiStateChanges aState );

    /**
     * Helper to check idle focus status.
     * @return ETrue if idle is focused / foreground app.
     */
    TBool IdleFocused() const;

    /**
     * Helper to check backup (or restore) status.
     * @return ETrue if backup (or restore) is ongoing.
     */
    TBool BackupOngoing() const;

    /**
     * Helper to check phone lights status.
     * @return ETrue if lights are on.
     */
    TBool LightsOn() const;

    /**
     * Helper to check phone call status.
     * @return ETrue if call is ongoing.
     */
    TBool CallOngoing() const;

    /**
     * Standard callback for CPeriodic ie. T1 timer.
     */
    static TInt T1TimerCallback( TAny* aPtr );
        
private:     // Data

    /**
     * Backup operation state observer.
     * Own.
     */
    MAiDeviceStatusObserver* iBackupOperationObserver;

    /**
     * Call state observer.
     * Own.
     */
    MAiDeviceStatusObserver* iCallStateObserver;

    /**
     * Light state observer.
     * Own.
     */
    MAiDeviceStatusObserver* iLightStateObserver;

    /**
     * Enviroment change observer.
     * Own.
     */
    MAiDeviceStatusObserver* iEnvironmentObserver;

    /**
     * Focus change observer. Using telephony idle visiblity PS.
     * Own.
     */
    MAiDeviceStatusObserver* iFocusObserver;

    /**
     * Keylock observer.
     * Own.
     */
    MAiDeviceStatusObserver* iKeylockObserver;

    /**
     * Enviroment change observer. Full class type is used because
     * we need to provide accessor for implemented type MAiUiFrameworkObserver.
     * Own.
     */
    CAiUiFrameworkObserverImpl* iFrameworkObserver;

    /**
     * Timer for suspend, screensaver timout + light fadeout.
     * Own.
     */
    CPeriodic* iT1Timer;

    /**
     * Timer for suspend, screensaver timout + light fadeout.
     */
    TInt iT1Delay;

    /**
     * New state to be timed.
     */
    TAiState iTimedState;

    /**
     * Reason for timers activity.
     */
    TAiTransitionReason iTimedReason;

    /**
     * Indicates whether the device has been properly started.
     */
    TBool iIsDeviceStarted;

    /**
     * State machines for plugins.
     * Own.
     */
    RPointerArray<CAiPluginStateMachine> iStateMachines;
    };

#endif // C_AIPLUGINSTATEMANAGER_H

// End of File.
