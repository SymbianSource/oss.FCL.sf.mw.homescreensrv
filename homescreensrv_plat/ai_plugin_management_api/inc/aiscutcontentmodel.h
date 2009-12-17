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
* Description:  Content model for shortcut plug-in.
*
*/


#ifndef AISCUTCONTENTMODEL_H
#define AISCUTCONTENTMODEL_H

#include <aicontentmodel.h>

#include <platform/mw/aiscutuids.hrh>

// AI Shortcut Plug-in ECOM implementation UID.
const TInt KImplUidScutPlugin = AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_SCUTPLUGIN;
const TUid KUidScutPlugin = { KImplUidScutPlugin };

// ================================= CONTENT ===================================

/**
 * Content Ids.
 */
enum TAiScutContentIds
{
    EAiScutContentShortcutIcon,
    EAiScutContentShortcutCaption,
    EAiScutContentShortcutShortCaption,
    EAiScutContentShortcutMskCaption,
    EAiScutContentShortcutSkCaption,
    EAiScutContentShortcutSkIcon,
    EAiScutContentShortcutToolbarCaption,
    EAiScutContentShortcutToolbarIcon,
    EAiScutContentPopupTextCaptionLine,
    EAiScutContentPopupTextFirstLine,
    EAiScutContentPopupTextSecondLine,
    EAiScutContentPopupTextThirdLine
};

/**
 * Content that the plug-in will publish.
 */
const TAiContentItem KAiScutContent[] =
{
    // Shortcut icon as CFbsBitmap.
    { EAiScutContentShortcutIcon,         L"ShortcutIcon",         KAiContentTypeBitmap }
    ,
    // Shortcut caption as plain text (used for long application titles).
    { EAiScutContentShortcutCaption,      L"ShortcutCaption",      "text/plain" }
    ,
    // Shortcut short caption as plain text (used for short application titles in softkeys).
    { EAiScutContentShortcutShortCaption, L"ShortcutShortCaption", "text/plain" }
    ,
    // Shortcut msk caption as plain text (used for short application titles in MSK).
    { EAiScutContentShortcutMskCaption,   L"ShortcutMskCaption",   "text/plain" }
       ,
    // Shortcut sk caption as plain text (used for short application titles in SK).
    { EAiScutContentShortcutSkCaption,   L"ShortcutSkCaption",   "text/plain" }
   ,
    // Shortcut sk icon as bitmap (used for application icons in SK).
    { EAiScutContentShortcutSkIcon,   L"ShortcutSkIcon",   KAiContentTypeBitmap }
   ,
    // Shortcut sk caption as plain text (used for short application titles in SK).
    { EAiScutContentShortcutToolbarCaption,   L"ShortcutToolbarCaption",   "text/plain" }
   ,
     // Shortcut toolbar icon as bitmap (used for application icons in toolbar).
    { EAiScutContentShortcutToolbarIcon,   L"ShortcutToolbarIcon",   KAiContentTypeBitmap }
    ,
    // Caption line of Popup as plain text.
    { EAiScutContentPopupTextCaptionLine,  L"ShortcutPopupCaptionLine", "text/plain" }
    ,
    // 1st actual line of Popup as plain text.
    { EAiScutContentPopupTextFirstLine,  L"ShortcutPopup1stLine", "text/plain" }
    ,
    // 2nd actual line of Popupn as plain text.
    { EAiScutContentPopupTextSecondLine, L"ShortcutPopup2ndLine", "text/plain" }
    ,
    // 3rd actual line of Popup as plain text.
    { EAiScutContentPopupTextThirdLine,  L"ShortcutPopup3rdLine", "text/plain" }
};

const TInt KAiScutContentCount = sizeof( KAiScutContent ) / 
                                            sizeof( KAiScutContent[0] );


// ================================ RESOURCES ==================================

/**
 * Resource Ids.
 */
enum TAiScutPluginResourceIds
{
    EAiScutResourceDefaultIcon,
    EAiScutResourceEmptyIcon,
    EAiScutResourceBackCaption,
    EAiScutResourceEmptyCaption,
    EAiScutResourceNewMsgCaption,
    EAiScutResourceNewEmailCaption,
    EAiScutResourceNewSyncMLMailCaption,
    EAiScutResourceNewPostcardCaption,
    EAiScutResourceNewAudioMsgCaption,
    EAiScutResourceSelectMsgTypeCaption,
    EAiScutResourceChangeThemeCaption,
    EAiScutResourceNewMsgShortCaption,
    EAiScutResourceNewEmailShortCaption,
    EAiScutResourceNewSyncMLMailShortCaption,
    EAiScutResourceNewPostcardShortCaption,
    EAiScutResourceNewAudioMsgShortCaption,
    EAiScutResourceSelectMsgTypeShortCaption,
    EAiScutResourceChangeThemeShortCaption
};

/**
 * Resources that the plug-in will publish.
 */
const TAiContentItem KAiScutResources[] =
{
    // Default shortcut icon.
    { EAiScutResourceDefaultIcon,            L"DefaultIcon",            "image/*" }
    ,
    // Empty shortcut icon, used when shortcut target is unknown.
    { EAiScutResourceEmptyIcon,              L"EmptyIcon",              "image/*" }
    ,
    // Localizable caption for the back shortcut
    { EAiScutResourceBackCaption,            L"BackCaption",            "text/plain" }
    ,
    // Localizable caption for the empty shortcut.
    { EAiScutResourceEmptyCaption,           L"EmptyCaption",           "text/plain" }
    ,
    // Localizable caption for the "new message" shortcut.
    { EAiScutResourceNewMsgCaption,          L"NewMessageCaption",      "text/plain" }
    ,
    // Localizable caption for the "new email" shortcut.
    { EAiScutResourceNewEmailCaption,        L"NewEmailCaption",        "text/plain" }
    ,
    // Localizable caption for the "new syncml mail" shortcut.
    { EAiScutResourceNewSyncMLMailCaption,   L"NewSyncMLMailCaption",   "text/plain" }
    ,
    // Localizable caption for the "new postcard" shortcut.
    { EAiScutResourceNewPostcardCaption,     L"NewPostcardCaption",     "text/plain" }
    ,
    // Localizable caption for the "new audio message" shortcut.
    { EAiScutResourceNewAudioMsgCaption,     L"NewAudioMsgCaption",     "text/plain" }
    ,
    // Localizable caption for the "select message type" shortcut.
    { EAiScutResourceSelectMsgTypeCaption,   L"SelectMsgTypeCaption",   "text/plain" }
    ,
    // Localizable caption for the "change theme" shortcut.
    { EAiScutResourceChangeThemeCaption,     L"ChangeThemeCaption",     "text/plain" }
    ,
    // Localizable short caption for the "new message" shortcut.
    { EAiScutResourceNewMsgShortCaption,          L"NewMessageShortCaption",      "text/plain" }
    ,
    // Localizable short caption for the "new email" shortcut.
    { EAiScutResourceNewEmailShortCaption,        L"NewEmailShortCaption",        "text/plain" }
    ,
    // Localizable short caption for the "new syncml mail" shortcut.
    { EAiScutResourceNewSyncMLMailShortCaption,   L"NewSyncMLMailShortCaption",   "text/plain" }
    ,
    // Localizable short caption for the "new postcard" shortcut.
    { EAiScutResourceNewPostcardShortCaption,     L"NewPostcardShortCaption",     "text/plain" }
    ,
    // Localizable short caption for the "new audio message" shortcut.
    { EAiScutResourceNewAudioMsgShortCaption,     L"NewAudioMsgShortCaption",     "text/plain" }
    ,
    // Localizable short caption for the "new message" shortcut.
    { EAiScutResourceSelectMsgTypeShortCaption,   L"SelectMsgTypeShortCaption",   "text/plain" }
    ,
    // Localizable short caption for the "change theme" shortcut.
    { EAiScutResourceChangeThemeShortCaption,     L"ChangeThemeShortCaption",     "text/plain" }

};

const TInt KAiScutResourceCount = sizeof( KAiScutResources ) / 
                                            sizeof( KAiScutResources[0] );


// ============================ SERVICES (Events) ==============================

/**
 * Event Ids.
 */
enum TAiScutEventIds
{
    EAiScutEventLaunchByIndex,
    EAiScutEventLaunchByValue,
    EAiScutEventShowSettings,
    EAiScutEventShowSetting,
    EAiScutEventLaunchFastswap,
    EAiScutEventLaunchByIndexAlternate,
    EAiScutEventLoseFocus,
    EAiScutEventGainFocus
};

/**
 * Services that the plug-in can perform.
 */
const TAiContentItem KAiScutEvents[] =
{
    // Launches a shortcut by its index that is delivered as an integer.
    { EAiScutEventLaunchByIndex, L"LaunchByIndex", "int" }
    ,
    // Launches a shortcut by its value that is delivered in a descriptor.
    { EAiScutEventLaunchByValue, L"LaunchByValue", "str" }
    ,
    // Shows the shortcut plug-in settings dialog.
    { EAiScutEventShowSettings,  L"ShowSettings",  "" }
    ,
    // Shows the shortcut plug-in setting.
    { EAiScutEventShowSetting,  L"ShowSetting",  "" }    
    ,
    // Opens the fast swap window    
    { EAiScutEventLaunchFastswap, L"LaunchFastSwap", "" }
    ,
    // Special launching. If the index points to appshell
    // fastswap is opened. Other special cases can be added to the engine
    { EAiScutEventLaunchByIndexAlternate, L"LaunchByIndexAlternate", "int" }
	,
    // Used when we move out of a SC button (Needed for Popup-functionality).
    // index of shortcut is delivered as an integer
    { EAiScutEventLoseFocus, L"LoseFocus", "int" }
    ,
    // Used when we move into a SC button (Needed for Popup-functionality).
    // index of shortcut is delivered as an integer.
    { EAiScutEventGainFocus, L"GainFocus", "int" }

};

#endif // AISCUTCONTENTMODEL_H

// End of File.
