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
* Description:  Shortcut Plug-in CenRep keys.
*
*/


#ifndef AISCUTPLUGINDOMAINCRKEYS_H
#define AISCUTPLUGINDOMAINCRKEYS_H

/**
 * Uid for central repository file that holds shortcut plug-in settings.
 */
const TUid KCRUidShortcutItems = { 0x10275104 };

/**                                           
 * Flag bit 31                                  
 * Theme-default shortcuts.                     1000 0000 0000 0000 0000 0000 0000 0000
 */
const TUint32 KScutFlagBitThemeDefault        = 0x80000000;

/**                                             
 * Mask to unset the theme-default flag.        0111 1111 1111 1111 1111 1111 1111 1111
 */
const TUint32 KScutBitMaskThemeDefault        = 0x7FFFFFFF;

/**
 * Flag bit 30
 * Locked shortcuts.                            0100 0000 0000 0000 0000 0000 0000 0000
 */
const TUint32 KScutFlagBitLocked              = 0x40000000;

/**
 * Mask to unset the locked flag.               1011 1111 1111 1111 1111 1111 1111 1111
 */
const TUint32 KScutBitMaskLocked              = 0xBFFFFFFF;
                                                
/**
 * Flag bit 24
 * Optionally visible shortcut (softkeys).      0000 0001 0000 0000 0000 0000 0000 0000
 */
const TUint32 KScutFlagBitOptionallyVisible   = 0x01000000;

/**
 * Flag bit 25
 * Non-visible shortcut (key press).            0000 0010 0000 0000 0000 0000 0000 0000
 */
const TUint32 KScutFlagBitNonVisible          = 0x02000000;

/**
* Flag bits 26-31                               //0000 0001 0000 0000 0000 0001 0000 0000
* Icon ovverides                                0010 0000 0000 0000 0000 0000 0000 0000
*/
const TUint32 KScutFlagBitIconOverride        = 0x20000000;

/**
 * Flag bit for toolbar shortcuts
 */
const TUint32 KScutFlagBitToolbarShortcut     = 0x10000000;

/**
 * Mask for additional settings. 
 */
const TUint32 KScutMaskAdditionalSetting      = 0xFFFFFFFF ^ (KScutFlagBitIconOverride + KScutFlagBitToolbarShortcut);

/**
 * Partial key for theme-default shortcut ids.
 */
const TUint32 KScutCenRepKeyThemeDefault      = KScutFlagBitThemeDefault;

/**
 * Partial key for user defined shortcut ids.
 */
const TUint32 KScutCenRepKeyUserDefined       = 0x0;

/**
 * Mask used to separate theme-default ids from user defined ids.
 */
const TUint32 KScutCenRepKeyMask              = KScutFlagBitThemeDefault;

/**
 * Shortcut count for the active theme
 */
const TUint32 KScutCenRepShorcutCount         = 0xFFFFFFF0;

/**
 * A key which contains a space-separated list of application UIDs
 * identifying the applications that should not show the xSP tip message.
 */
const TUint32 KAIxSPNoTipAppsList             = 0x0001FFFF;

/**
 * Shortcut plug-in keycodes for optionally visible shortcuts.
 */
enum TAiScutKeyCodes
{
    // Navigation keys.
    EAiScutScrollKeyLeft  = 0x0000,
    EAiScutScrollKeyRight = 0x0001,
    EAiScutScrollKeyUp    = 0x0002,
    EAiScutScrollKeyDown  = 0x0003,
    EAiScutSelectionKey   = 0x0004,

    // Soft keys.
    EAiScutSoftKeyLeft    = 0x0100,
    EAiScutSoftKeyRight   = 0x0101,
    
    // Touch toolbar
    EAiScutToolbarFirst   = 0x1000,
    EAiScutToolbarSecond  = 0x1001,
    EAiScutToolbarThird   = 0x1002
};

#endif // AISCUTPLUGINDOMAINCRKEYS_H

// End of File.
