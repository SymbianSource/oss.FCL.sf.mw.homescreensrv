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
* Description:  Shortcut plug-in definitions.
*
*/


#ifndef AISCUTDEFS_H
#define AISCUTDEFS_H

#include <e32std.h>
#include <platform/mw/aiscutuids.hrh>
#include "aiscutappuids.hrh"

// =============================================================================
// =============== UIDs and view ids to external applications ==================
// =============================================================================

// Active Idle UID.
const TUid KScutActiveIdleUid       = { 0x102750F0 };

// Shortcut Settings DLL UID.
const TUid KScutSettingsDllUid      = { AI_UID_ECOM_DLL_SETTINGS_SCUTPLUGIN };

// Keypad lock UID
const TUid KScutKeyLockUid          = { 0x10000000 };

// Web Browser application UID.
const TUid KScutBrowserUid          = { KScutBrowserUidValue };

// General Settings application UID.
const TUid KScutGeneralSettingsUid  = { KScutGeneralSettingsUidValue };

// Message Centre application UID.
const TUid KScutMessagingUid        = { KScutMessagingCenterUidValue };

// Remote mailbox view id. Message list view id (0x2) defined in MceIds.hrh.
const TUid KScutRemoteMailboxViewId = { KScutRemoteMailboxViewIdValue };

// Personalisation application UID.
const TUid KScutPersonalisationUid  = { KScutPersonalisationUidValue };

// Change-Theme view id in the Personalisation application.
const TUid KScutChangeThemeViewId   = { KScutChangeThemeViewIdValue };

// Installation view id in the Control Panel.
const TUid KScutInstallationViewId  = { KScutInstallationViewIdValue };

// Connectivity view id in the Control Panel.
const TUid KScutConnectivityViewId  = { KScutConnectivityStatusViewIdValue };
// Voice dialer UID
const TUid KScutVoiceDialUid        = { KScutVoiceDialUidValue };

// Logs UID
const TUid KScutLogsUid             = { KScutLogsUidValue };

// AppShell UID
const TUid KScutAppShellUid         = { KScutApplicationShellUidValue };

// Telephony UID
const TUid KScutDiallerUid          = { KScutDiallerUidValue };

// Postcard editor UID
const TUid KScutPostcardEditorUid   = { KScutPostcardEditorUidValue };

// On-Screen Dialler view id in the Telephony application.
const TUid KScutDiallerViewId       = { KScutDiallerViewIdValue };

// On-Screen Dialler view command
const TUid KScutDiallerViewCommand  = { 0x1 };
// AppMngr
const TUid KScutAppMngrUid			= { 0x101F8512 };

// EasyVoip Application UID
const TUid KScutEasyVoIPApplicationUid = { 0x1020E566 };

// VoIP launcher UID
const TUid KScutVoIPLauncherUid        = { 0x10275424 };

// EasyVoip Central Repository UID
const TUid KUidEasyVoIPRepository      = { 0x1020E593 };

// EasyVoIP shortcut startup flag
const TUint32 KEasyVoIPShortcutStartup = 0x00000004;

// Logs views
_LIT8( KLogsMissedCallsView         ,  "missed"   );
_LIT8( KLogsDialledCallsView        ,  "dialled"  );
_LIT8( KLogsReceivedCallsView       ,  "received" );
_LIT8( KLogsMainView                ,  "counters" );

// Softkeys
_LIT( KLeftSoftkey                  ,  "0x01000100" );
_LIT( KRightSoftkey                 ,  "0x01000101" );

const TUint32 KLeftSoftkeyId        = { 0x01000100 };
const TUint32 KRightSoftkeyId       = { 0x01000101 };

/**
 * Bit fields for content items that the observers support.
 */
enum TSupportedContentItems
{
    ESupportIcon         = 0x1,
    ESupportCaption      = 0x2,
    ESupportShortCaption = 0x4
};

class CAiScutShortcut;
class CAiScutShortcutInfo;
typedef RPointerArray<CAiScutShortcut> RAiShortcutArray;
typedef RPointerArray<CAiScutShortcutInfo> RAiShortcutInfoArray;

/**
 * Definitions for application titles.
 */
enum TAiScutAppTitleType
{
    EAiScutLongTitle,
    EAiScutShortTitle,
    EAiScutSkeyTitle,
    EAiScutMskTitle
};

class TAiScutAppTitleEntry
{
public:
    TUid    iAppUid;
    TUid    iViewId;
    HBufC*  iLongTitle;
    HBufC*  iShortTitle;
    HBufC*  iSkeyTitle;
    HBufC*  iMskTitle;
};

/**
 * Localized send ui resource for error note.
 */
_LIT( KSendNormResource, "sendnorm.rsc" );

/**
 * Shortcut menu strings
 */
_LIT( KSettings, "widgetsettings" );
#endif // AISCUTDEFS_H

// End of File.
