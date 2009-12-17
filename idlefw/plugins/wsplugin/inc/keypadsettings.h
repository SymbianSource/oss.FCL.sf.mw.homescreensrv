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
* Description:  Keypad settings class for Active Idle WS Plug-in.
*
*/


#ifndef T_AIWSPLUGIN_KEYPADSETTINGS_H
#define T_AIWSPLUGIN_KEYPADSETTINGS_H

#include <e32std.h>
class TRawEvent;

namespace AiWsPlugin {

/**
 *  @ingroup group_wsplugin
 *
 * Internal class for keypad settings.
 */
class TKeypadSettings
    {
public:
    /**
     * Initializes this object from settings repository.
     */
    void ReadFromRepositoryL();
    
    /**
     * Returns true if aScanCode matches the first lock key.
     */
    TBool IsFirstLockKey( TInt aScanCode ) const;

    /**
     * Returns true if aScanCode matches the second lock key.
     */
    TBool IsSecondLockKey( TInt aScanCode ) const;

    /**
     * Returns the key lock timeout in microseconds.
     */
    TTimeIntervalMicroSeconds32 KeylockTimeout() const;
    
    /**
     * Returns true if aScanCode matches the SIND key.
     */
    TBool IsSINDKey( TInt aScanCode ) const;

    /**
     * Returns the key lock timeout in microseconds.
     */
    TTimeIntervalMicroSeconds32 KeySINDTimeout() const;
    
    TInt16 MapNkpScanCodeToChar( TInt aScanCode );    

private:  // data
    /**
     * Scan code of first keypad lock key.
     */
    TInt16 iFirstLockKeyScanCode;
    
    /**
     * Character code (if any) that matches iFirstLockKeyScanCode.
     */
    TInt16 iFirstLockKeyChar;
    
    /**
     * Scan code of second keypad lock key.
     */
    TInt16 iSecondLockKeyScanCode;
    
    /**
     * Character code (if any) that matches iSecondLockKeyChar.
     */
    TInt16 iSecondLockKeyChar;
    
    /**
     * Scan code of optional second keypad lock key.
     */
    TInt16 iSecondLockKeyScanCode2;
    
    /**
     * Character code (if any) that matches iSecondLockKeyChar2.
     */
    TInt16 iSecondLockKeyChar2;
    
    /** 
     * Keypad lock timeout in microseconds.
     */
    TTimeIntervalMicroSeconds32 iKeylockTimeout;

    /**
     * Scan code of SIND keypad key.
     */
    TInt16 iSINDKeyScanCode;
    
    /**
     * Character code (if any) that matches iSINDKeyScanCode.
     */
    TInt16 iSINDKeyScanChar;    
    
    /** 
     * Keypad SIND timeout in microseconds.
     */
    TTimeIntervalMicroSeconds32 iKeySINDTimeout;    
    
    };

} // namespace AiWsPlugin

#endif // T_AIWSPLUGIN_KEYPADETTINGS_H
