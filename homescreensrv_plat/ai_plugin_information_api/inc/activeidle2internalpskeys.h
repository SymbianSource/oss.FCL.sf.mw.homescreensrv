/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Internal Publish & Subscribe keys for Active Idle 2
*
*/


#ifndef __ACTIVEIDLE2INTERNALPSKEYS_H__
#define __ACTIVEIDLE2INTERNALPSKEYS_H__

/**
 * Gives possibility to execute shortcuts defined by localapp URL.
 * This UID is used as a key id under KUidSystemCategory.
 * Clients must supply string in localapp URL format defined by RFC 2396.
 *
 * @type RProperty::EText
 */
const TUint32 KPSUidShortcutCmd = 0x102750FF;

/**
 * Shortcut Plug-in command for enabling keylock.
 */
_LIT( KAiPSEnableKeyLock, "localapp:keylock?on");

/**
 * Shortcut Plug-in command for canceling keylock.
 */
_LIT( KAiPSSkipKeyLock, "localapp:keylock?skip");

/**
 * Shortcut Plug-in command for timeoutting keylock.
 */
_LIT( KAiPSKeyLockTimeout, "localapp:keylock?timeout");

/**
 * Shortcut Plug-in command for launching Logs to dialed calls view.
 */
_LIT( KAiPSLaunchLogs, "localapp:logs?view=dialled");

/**
 * Shortcut Plug-in command for launching Voice Dial UI.
 */
_LIT( KAiPSLaunchNameDialer, "localapp:voicedial");

/**
 * Shortcut Plug-in command for canceling voice dial ui.
 */
_LIT( KAiPSSkipNameDialer, "localapp:voicedial?skip");


// Restart code for category KPSUidAiInformation = 0x101FD657 key KActiveIdleRestartAI2 0xA
const TInt KActiveIdleRestartCode = 0xFA93BAD2;



/**
 * Publish and Subscribe key for data plugin states in KPSUidActiveIdle2 category (AI plug-in registry API)
 */
const TUint KPSAiDataPluginState = 0x0000000F; //  Contains one value from following emuneration

/**
 * States for the KPSAiDataPluginState key
 */
enum EPSAiDataPluginState
{    
    EPSAiDataPluginsNotLoaded,
    EPSAiDataPluginsLoaded
};


#endif // __ACTIVEIDLE2INTERNALPSKEYS_H__
