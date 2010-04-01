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
* Description:  Keypad settings implmentation for Active Idle WS Plug-in.
*
*/


#include "hs_app_keypadsettings.h"

#include <e32keys.h>
#include <e32event.h>
#include <centralrepository.h>

#include <activeidle2domaincrkeys.h>

#include "debug.h"

namespace AiWsPlugin {


/**
 * Maps selected numeric keypad scan codes to character.
 *
 * @param aScanCode scan code for which to find a character mapping.
 * @return character mapping for aScanCode or -1 if no mapping is found.
 */
TInt16 TKeypadSettings::MapNkpScanCodeToChar( TInt aScanCode )
    {
    TInt16 result;
    switch( aScanCode )
        {
        case EStdKeyHash:        result = '#'; break;
        case EStdKeyMinus:       result = '-'; break;
        case EStdKeyNkpAsterisk: result = '*'; break;
        case EStdKeyNkpMinus:    result = '-'; break;
        case EStdKeyNkpPlus:     result = '+'; break;
        case EStdKeyNkp1:        result = '1'; break;
        case EStdKeyNkp2:        result = '2'; break;
        case EStdKeyNkp3:        result = '3'; break;
        case EStdKeyNkp4:        result = '4'; break;
        case EStdKeyNkp5:        result = '5'; break;
        case EStdKeyNkp6:        result = '6'; break;
        case EStdKeyNkp7:        result = '7'; break;
        case EStdKeyNkp8:        result = '8'; break;
        case EStdKeyNkp9:        result = '9'; break;
        case EStdKeyNkp0:        result = '0'; break;
        default:                 result =  -1; break;
        }
    return result;
    }

void TKeypadSettings::ReadFromRepositoryL()
    {
    CRepository* repository = CRepository::NewLC( TUid::Uid( KCRUidActiveIdleLV ) );
    TInt value;
    
    // KAIFirstKeyLockKey
    User::LeaveIfError( repository->Get( KAIFirstKeyLockKey,  value ) );
    iFirstLockKeyScanCode = TInt16( value );
    iFirstLockKeyChar = MapNkpScanCodeToChar( value );
    __PRINT( 
        __DBG_FORMAT("AiWsPlugin: KAIFirstKeyLockKey = %d, ch=%d"), 
        TInt(iFirstLockKeyScanCode), TInt(iFirstLockKeyChar) );
    
    // KAISecondKeyLockKey
	User::LeaveIfError( repository->Get( KAISecondKeyLockKey, value ) );
	iSecondLockKeyScanCode = TInt16( value );
	iSecondLockKeyChar = MapNkpScanCodeToChar( value );
    __PRINT( 
        __DBG_FORMAT("AiWsPlugin: KAISecondKeyLockKey = %d, ch=%d"), 
        TInt(iSecondLockKeyScanCode), TInt(iSecondLockKeyChar) );
    
    // KAISecondKeyLockKey2
	User::LeaveIfError( repository->Get( KAISecondKeyLockKey2, value ) );
	iSecondLockKeyScanCode2 = TInt16( value );
	iSecondLockKeyChar2 = MapNkpScanCodeToChar( value );
    __PRINT( 
        __DBG_FORMAT("AiWsPlugin: KAISecondKeyLockKey2 = %d, ch=%d"), 
        TInt(iSecondLockKeyScanCode2), TInt(iSecondLockKeyChar2) );

    // KAIKeyLockTimeout    
	User::LeaveIfError( repository->Get( KAIKeyLockTimeout, value ) );
    __PRINT( __DBG_FORMAT("AiWsPlugin: KAIKeyLockTimeout=%d ms"), value );
	// Convert timeout from milliseconds to microseconds
    const TInt KUsInMs = 1000;
    iKeylockTimeout = KUsInMs * value;
    
    // KAISINDKey
	User::LeaveIfError( repository->Get( KAIVoiceDialLaunchKey, value ) );
	iSINDKeyScanCode = TInt16( value );
	iSINDKeyScanChar = MapNkpScanCodeToChar( value );
    __PRINT( 
        __DBG_FORMAT("AiWsPlugin: iSINDKey = %d, ch=%d"), 
        TInt(iSINDKeyScanCode), TInt(iSINDKeyScanChar) );

    // KAISINDKeyTimeout    
	User::LeaveIfError( repository->Get( KAIVoiceDialKeyTimeout, value ) );
    __PRINT( __DBG_FORMAT("AiWsPlugin: KAISINDKeyTimeout=%d ms"), value );
	// Convert timeout from milliseconds to microseconds
    iKeySINDTimeout = KUsInMs * value;
	
	CleanupStack::PopAndDestroy( repository );
    }

TBool TKeypadSettings::IsFirstLockKey( TInt aScanCode ) const
    {
    return ( aScanCode == iFirstLockKeyScanCode || aScanCode == iFirstLockKeyChar );
    }
    
TBool TKeypadSettings::IsSecondLockKey( TInt aScanCode ) const
    {
    return (
        ( aScanCode == iSecondLockKeyScanCode || aScanCode == iSecondLockKeyChar ) ||
        ( aScanCode == iSecondLockKeyScanCode2 || aScanCode == iSecondLockKeyChar2 ) );
    }

TTimeIntervalMicroSeconds32 TKeypadSettings::KeylockTimeout() const
    {
    return iKeylockTimeout;
    }

} // namespace AiWsPlugin
