/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Keylock handler for Active Idle WS Plug-in.
*
*/


#ifndef C_AIWSPLUGIN_KEYLOCKHANDLER_H
#define C_AIWSPLUGIN_KEYLOCKHANDLER_H

#include "eventhandler.h"
#include "keylockcontrol.h"
#include "keypadsettings.h"
#include "keyhandlertimer.h"

class CKeyLockPolicyApi;
namespace AiWsPlugin {

class TKeylockState;

/**
 *  @ingroup group_wsplugin
 *
 *  Keylock handler for Active Idle WS Plug-in. 
 *
 *  Monitors key presses that activate keyboard lock on the device.
 *
 *  @since S60 3.2
 */
class CKeyLockHandler : 
        public CEventHandler, 
        public MKeyLockHandlerControl,
        public MHandlerTimer
    {
public:

    static CKeyLockHandler* NewLC();

    ~CKeyLockHandler();

// from MHandlerTimer

    void TimerDone();    

private:

// from base class CEventHandler

    void SetUiStateQuery( MUiState& aUiState );
    
    void FocusChanged( TBool aState );
    
    TBool OfferRawEvent(const TRawEvent& aRawEvent);

// from base class MKeyLockHandlerControl

    const TKeypadSettings& Settings() const;
    
    void StartTimeoutTimer( TInt aTimeout );
    
    void StopTimeoutTimer();
    
    void ActivateKeypadLock();
    
    void CancelKeypadLock();
    
    void KeypadLockTimeout();
    
    void SetNextState( const TKeylockState& aState );
    
    void ActivateNextState();
    
    void SetLastLockKeyScanCode( TInt aScanCode );
    
    TInt LastLockKeyScanCode() const;
    
    TBool HasFocus();

    TBool TimeoutTimerActive();
    
// construction

    CKeyLockHandler();
    
    void ConstructL();
    
    static TInt TimerElapsed( TAny* aSelf );
    
    TBool IsFirstLockKey( TInt aScanCode );
    
    TBool IsSecondLockKey( TInt aScanCode );

private: // data
    /**
     * Current state of this object.
     * Not owned.
     */
    const TKeylockState* iCurrentState;

    /**
     * Next state of this object.
     * Not owned.
     */
    const TKeylockState* iNextState;
    
    /**
     * Timeout timer for keypad lock.
     * Own.
     */
    CPeriodic* iKeypadLockTimer;

    /**
     * Key lock settings.
     */
    TKeypadSettings iSettings;
    
    /**
     * Last value of SetLastLockKeyScanCode( TInt aScanCode).
     */
    TInt iLastLockKeyScanCode;
    
    /**
     * Pointer to ui state.
     * Not owned.
     */
    MUiState* iUiState;
    
    /**
     * For checking keylock buttons
     * Own
     */
    CKeyLockPolicyApi *iKeylockApi;
    };

} // namespace AiWsPlugin


#endif // C_AIWSPLUGIN_KEYLOCKHANDLER_H
