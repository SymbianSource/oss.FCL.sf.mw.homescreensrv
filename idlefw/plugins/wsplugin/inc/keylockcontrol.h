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
* Description:  Keylock handler settings class and control interface for 
*                Active Idle WS Plug-in.
*
*/


#ifndef M_AIWSPLUGIN_KEYLOCKCONTROL_H
#define M_AIWSPLUGIN_KEYLOCKCONTROL_H

#include <e32def.h>
class TRawEvent;

namespace AiWsPlugin {

class TKeypadSettings;
class TKeylockState;

/**
 *  @ingroup group_wsplugin
 *
 * Callback interface for CKeyLockHandler internal states.
 */
class MKeyLockHandlerControl
    {
public:

    virtual const TKeypadSettings& Settings() const = 0;
    
    virtual void StartTimeoutTimer( TInt aTimeout = -1 ) = 0;
    
    virtual void StopTimeoutTimer() = 0;
    
    virtual void ActivateKeypadLock() = 0;
    
    virtual void CancelKeypadLock() = 0;

    virtual void KeypadLockTimeout() = 0;
    
    virtual void SetNextState( const TKeylockState& aState ) = 0;
    
    virtual void ActivateNextState() = 0;
    
    virtual void SetLastLockKeyScanCode( TInt aScanCode ) = 0;
    
    virtual TInt LastLockKeyScanCode() const = 0;
    
    virtual TBool HasFocus() = 0;
    
    virtual TBool TimeoutTimerActive()  = 0;
    
    virtual TBool IsFirstLockKey( TInt aScanCode ) = 0;
    
    virtual TBool IsSecondLockKey( TInt aScanCode ) = 0;

    
protected:
    /**
     * Protected dtor prevents deletion through this interface.
     */
    ~MKeyLockHandlerControl() { }
    };

} // namespace AiWsPlugin

#endif // M_AIWSPLUGIN_KEYLOCKCONTROL_H
