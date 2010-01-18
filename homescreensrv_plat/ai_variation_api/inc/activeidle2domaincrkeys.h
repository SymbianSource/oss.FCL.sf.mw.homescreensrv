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
* Description:  Internal Central Repository keys for Active Idle 2
*
*/


#ifndef __AI2INTERNALCRKEYS_H__
#define __AI2INTERNALCRKEYS_H__

// Active Idle 2 Central Repository UIDs
const TUint32 KCRUidActiveIdleLV        = 0x10275102;

// Active Idle 2 Central Repository keys

const TUint32 KAiMainUIController        = 0x00000100;

// UI controller key range 0x00000101 to 0x000001ff
const TUint32 KAiFirstUIController       = 0x00000101;
const TUint32 KAiLastUIController   = 0x000001ff;

// Key for status pane layout
const TUint32 KAiStatusPaneLayout           = 0x00000200;

enum KAiSPLayoutSelector
  {
  EAiStatusPaneLayoutIdleNormal,
  EAiStatusPaneLayoutIdleFlat,
  EAiStatusPaneLayoutIdleHidden
  };

// Keypad lock keys
/**
 * Key id for scan code for the 1st lock key in the key lock sequence.
 */
const TUint32 KAIFirstKeyLockKey   = 0x00000300;

/**
 * Key id for scan code for the 2nd lock key in the key lock sequence.
 */
const TUint32 KAISecondKeyLockKey  = 0x00000301;

/**
 * Key id for scan code for the optional extra 2nd lock key.
 */
const TUint32 KAISecondKeyLockKey2 = 0x00000302;

/**
 * Key id for key lock sequence timeout in milliseconds.
 */
const TUint32 KAIKeyLockTimeout    = 0x00000303;

/**
 * Key id for scan code for voice dial launch key. 
 */
const TUint32 KAIVoiceDialLaunchKey = 0x00000304;

/**
 * Key id for Voice dial key timeout in milliseconds. 
 * This should be the same as long key press timeout.
 */
const TUint32 KAIVoiceDialKeyTimeout = 0x00000305;

/**
 * Key id for Display PLMN and SPN indicators simultaneously. 
 */
const TUint32 KAISPNAndEONS = 0x00000400;

/**
 * Key id for publish VHZ in navipane. 
 */
const TUint32 KAIVHZInMainpane = 0x00000500;

/**
 * Key id to show/hide external StandbyScreen/default Idle Themes
 */
const TUint32 KAIExternalStatusScreen = 0x00000600;

/**
 * Key id for touch toolbar visibility configuration
 */
const TUint32 KAITouchToolbarVisible = 0x00000700;

/**
 * Range start for plugin settings. Three keys per setting
 */
const TUint32 KAIPluginSettingsKeyRangeStart = 0x00001000;

/**
 * Range start for extra shortcuts such as URLs. These are placed
 * to shortcut settings.
 */
const TUint32 KAIExtraShortcutsKeyRangeStart = 0x00002000;

/**
 * Range start for backup application definitions.
 */
const TUint32 KAIBackupShortcutDefinitionStart = 0x00003000;

/**
 * Key id to store homescreen status online/offline 
 */
const TUint32 KAIWebStatus = 0x0000300F;

/**
 * Key id to store wallpaper change type
 */
const TUint32 KAIWallpaperChangeType = 0x00000850;

/**
 * Key id to store wallpaper path that is common for all Homescreen pages.
 */
const TUint32 KAIWallpaperPath = 0x00000851;

#endif // __AI2INTERNALCRKEYS_H__
