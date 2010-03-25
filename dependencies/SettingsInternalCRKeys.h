/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Settings UIs domain Central Repository keys.
*
*/



#ifndef SETTINGSINTERNALCRKEYS_H
#define SETTINGSINTERNALCRKEYS_H

// INCLUDES
#include <e32def.h>

// CONSTANTS

/**
* Network settings CR UID.
*/
const TUid KCRUidNetworkSettings = {0x101F8751};


/**
* Determines whether the user see visual notifications
* when entering or leaving different cellular systems.
* Possible interger values:
* 0 = Off
* 1 = On
*/
const TUint32 KSettingsServiceAlertNotes = 0x00000004;

/**
* Determines whether the user will hear a service tone
* when entering or leaving different cellular systems.
* Possible interger values:
* 0 = Off
* 1 = On
*/
const TUint32 KSettingsServiceAlertTones = 0x00000005;

/**
* Micro cellular network (Mcn) Display
*
* Possible values:
* 0 = Off
* 1 = On
*/
const TUint32 KSettingsMcnDisplay = 0x00000006;


/**
* Security settings CR UID
*/
const TUid KCRUidSecuritySettings = {0x101F877A};

/**
* Autolock time
* Possible values are:
* 0-999 minutes
*/
const TUint32 KSettingsAutoLockTime = 0x00000001;

/**
* Autolock status
* Possible values are:
* 0 = phone is not locked
* 1 = phone is locked
*/
const TUint32 KSettingsAutolockStatus = 0x00000002;

/**
* A CDMA Operator specific requirement, to be able to set the phone to
* "Lock On Power Up" in General Settings. When this key is set to 1
* the device security will lock the system (using Autolock) after each
* phone reboot.
*
* Possible values are:
* 0 = The phone doesn't get locked on Power Up
* 1 = The phone gets locked on Power Up
*/
const TUint32 KSettingsLockOnPowerUp = 0x00000003;

/**
* Transaction Tracking status
* Possible values are:
* 0 = disabled
* 1 = enabled
*/
const TUint32 KSettingsTransactionTracking = 0x00000004;

/**
* Automatic keyguard time in seconds
* Possible values are from 0 to 3600
*/
const TUint32 KSettingsAutomaticKeyguardTime = 0x00000005;

/**
* Automatic keyguard maximum time
* Value in seconds
*/
const TUint32 KSettingsMaxAutomaticKeyguardTime = 0x00000006;


/**
* Communication (aka networking) related settings CR UID
*/
const TUid KCRUidCommunicationSettings = {0x101F877B};

/**
* Name of polymorphic DLL to be loaded for VPN management UI.
*/
const TUint32 KSettingsVPNImplementation = 0x00000001;

/**
* VPN tab in Connection settings UI to be displayed or
* hidden from the user.
*
* Possible values are:
* 0 = Tab is hidden
* 1 = Tab is shown
*/
const TUint32 KSettingsVPNSupported = 0x00000002;

/**
* Access Points item in Connection settings folder to be displayed or
* hidden from the user.
*
* Possible values are:
* 0 = item is hidden
* 1 = item is shown
*/
const TUint32 KSettingsAPSettingsSupported = 0x00000003;

/**
* Terminal personalization settings CR UID
*/
const TUid KCRUidPersonalizationSettings = {0x101F877C};



/**
* Is operator logo is shown to the user.
* 0 = Logo not shown
* 1 = Logo shown
*/
const TUint32 KSettingsDisplayOperatorLogo = 0x00000003;

/**
* Defines if the "Operator Logo" list item is to be made visible or
* hidden in General Settings.
*/
const TUint32 KSettingsShowOperatorLogoSetting = 0x00000004;

/**
* Defines if Active Idle functionality is enabled.
* 0 = Disabled
* 1 = Enabled
*/
const TUint32 KSettingsActiveIdleState = 0x00000006;

/**
* Hide/Unhide Confirm Sat operations item in UI
* 0 = Hide
* 1 = Show
*/
const TUint32 KSettingsConfirmSatOperationsSupported = 0x00000007;

/**
* Screen saver period.
* The value is an integer between 5 and 90.
* Default value is 15
**/
const TUint32 KSettingsScreenSaverPeriod =  0x00000008;

/**
* Confirm Sat operations setting
* 0 = No
* 1 = Yes
*/
const TUint32 KSettingsConfirmSatOperations = 0x00000009;

/**
* Custom banner text for idle screen
* String value.
*/
const TUint32 KSettingsCustomBannerText = 0x0000000A;

/**
* Power Save Led setting value
* Integer value.
*/
const TUint32 KSettingsDisplayTurnoffTimeout = 0x0000000B;

/**
* Plugin UID of the operator specific full screen ActiveIdle.
* Integer value.
* Default value: 0
*/
const TUint32 KSettingsOperatorActiveIdle = 0x0000000C;

/**
* Pen writing speed for HWR
* Possible values are: 1-5 (very fast - very slow)
* Default value: 3
*/
const TUint32 KSettingsWritingSpeed = 0x0000000D;

/**
* Pen trail width for HWR.
* Possible values are: 1-5 (very thin - very thick)
* Default value: 3
*/
const TUint32 KSettingsPenWidth = 0x0000000E;

/**
* Pen trail color for HWR.
* 24-bit RGB colour value with 8 bits each for red, green and blue.
* The integer is of form 0xaabbggrr
* Default value: 0
*/
const TUint32 KSettingsPenColor = 0x0000000F;

/**
* Default input method of the Chinese PRC VKB.
* Possible values are: 1 - PinYin; 4 - Stroke
* Default value: 1
*/
const TUint32 KSettingsDefaultVkbPrc = 0x00000010;

/**
* Default input method of the Chinese TaiWan VKB.
* Possible values are: 2 - ZhuYin; 4 - Stroke
* Default value: 2
*/
const TUint32 KSettingsDefaultVkbTw = 0x00000011;

/**
* Default input method of the Chinese HongKong VKB.
* Possible values are: 4 - Stroke; 8 - CangJie
* Default value: 4
*/
const TUint32 KSettingsDefaultVkbHk = 0x00000012;

/**
* Pen Input Language.
* Possible values defined in enum TLanguage
* Default value: 0
*/
const TUint32 KSettingsPenInputLang = 0x00000013;

/**
* Indicates whether T-coil hearing aid mode is enabled or not.
* Integer value.
* Default value: 0
* 0 = disabled
* 1 = enabled
*/
const TUint32 KSettingsHearingAidMode = 0x00000014;

/**
* Guide line used in hand writing recognition box.
* If enabled, horizontal line will appear in the box.
* Default value: 1
* 0 = Guide Line Off
* 1 = Guide Line On
*/
const TUint32 KSettingsPenInputGuideLine = 0x00000015;

/**
* To Variate Operator Specific ActiveIdle Text.
* String value.
*/
const TUint32 KSettingsOperatorVariantActiveIdleText = 0x00000016;

/**
* To variate screensaver timeout settings item visibility
* 0 = item is hidden (default)
* 1 = item is shown
*/
const TUint32 KSettingsScreensaverTimeoutItemVisibility = 0x00000017;

/**
* Indicates whether the function (recognition with dictionary) is enabled or not.
* Boolean value.
* Default value: 0
* 0 = disabled
* 1 = enabled
*/
const TUint32 KSettingRecognitionwithdictionary = 0x00000018;

/**
* To save the setting for enable / disable optical joystick
* Integer value.
* Default value: 1
* 0 = Guide Line Off
* 1 = Guide Line On
*/
const TUint32 KSettingsOpticalJoystickStatus = 0x00000019;

/**
* Telephony settings CR UID.
*/
const TUid KCRUidTelephonySettings = {0x101F877E};

/**
* Any key answer
*
* Possible values:
* 0 = Off
* 1 = On
*/
const TUint32 KSettingsAnyKeyAnswer = 0x00000001;

/**
* Automatic redial
*
* 0 = Off
* 1 = On
*/
const TUint32 KSettingsAutomaticRedial = 0x00000002;

/**
* One key dialling
*
* 0 = Off
* 1 = On
*/
const TUint32 KSettingsOneKeyDialling = 0x00000003;

/**
* This key will include information whether the prefix change
* is active.
*
* 0 = Off
* 1 = On
*/
const TUint32 KSettingsDialPrefixChangeMode = 0x00000004;

/**
* This key will include the prefix set by user.
* Text string maximum 10 characters.
* Only dial related characters allowed.
*/
const TUint32 KSettingsDialPrefixText = 0x00000005;

/**
* Soft Reject default text used.
*
* 0 = Default in use
* 1 = User defined text in use
*/
const TUint32 KSettingsSoftRejectDefaultInUse = 0x00000006;

/**
* Soft Reject On/Off. Has the user enabled Soft Reject.
* 0 = Off
* 1 = On
*/
const TUint32 KSettingsSoftReject = 0x00000007;

/**
* User defined Soft Reject text. Any text up to 160 characters.
* No default value in CentRep (there is a default value, but it is
* localized).
*/
const TUint32 KSettingsSoftRejectText = 0x00000008;

/**
* Defines if still image for video calls (when camera is disabled) is in use.
* 0 = Off
* 1 = On
*/
const TUint32 KSettingsVTStillImage = 0x00000009;

/**
* This keys is required for the Video Telephony still image
* feature in General Settings to store the still image path
*/
const TUint32 KSettingsVTStillImagePath = 0x0000000A;

/**
* This key is required for VoIP Last Call Timer
*/
const TUint32 KLogsLastCallTimerLineVoIP = 0x0000000B;

/**
* This key is required for VoIP Received Calls Timer
*/
const TUint32 KLogsReceivedCallsTimerLineVoIP = 0x0000000C;

/**
* This key is required for VoIP Dialled Calls Timer
*/
const TUint32 KLogsDialledCallsTimerLineVoIP = 0x0000000D;

/**
* This key is required for SCCP stack installation indication
*/
const TUint32 KSCCPinstallUpgrade = 0x0000000E;

/**
* This key is required for dynamic VoIP
*/
const TUint32 KDynamicVoIP = 0x0000000F;

/**
* This key is required for Long Press of Call Key setting item
* Values: 0,1 --> Inactive
*		  2 --> Video call
* (Default value is 1 and when Restore Fact. Settings is done value is 0)
*/
const TUint32 KSettingsUiLongCallKeyPress = 0x00000010;

/**
* This key is required for VT call answer mute status
* 0 = Ask first (default value)
* 1 = No
* 2 = Yes
*/
const TUint32 KSettingsVTVideoSending = 0x00000011;


/**
* key to define what to do when opening slide
* 0: Not answer call when opening slide
* 1: Answer call when opening slide (default)
*/
const TUint32 KSettingsOpeningSlideAnswer = 0x00000012;

/**
* key to define what to do when closing slide
* 0: Not end call when closing slide
* 1: End call when closing slide (default)
*/
const TUint32 KSettingsClosingSlideEnd = 0x00000013;


/**
* key to define whether to show or not show side settings 
* 0: Not show slide settings (default)
* 1: Show slide settings
*/

const TUint32 KSettingsShowSlideSettings = 0x00000014;

/**
* CDMA Telephony settings CR UID.
*/
const TUid KCRUidCdmaTelephonySettings = {0x101F877F};

/**
* Locale settings CR UID.
*/
const TUid KCRUidLocaleSettings = {0x101F8780};

/**
* Arabic & Hebrew default number mode - integer value.
*/
const TUint32 KSettingsDefaultNumberMode = 0x00000001;

/**
* Default Indic Number Mode for Hindi Digits - integer value.
*/
const TUint32 KSettingsIndicDefaultNumberMode = 0x00000002;

/**
* Peripheral settings CR UID.
*/
const TUid KCRUidPeripheralSettings = {0x101F8781};

/**
* Timeout for setting off the display lights.
* Value is in seconds.
*/
const TUint32 KSettingsDisplayLightsTimeout = 0x00000001;

/**
* Call forwarding settings CR UID.
*/
const TUid KCRUidCallForwarding = {0x101F8782};

/**
* call forwarding "forwarded-to" number 1.
* String value
*/
const TUint32 KSettingsCFDefaultNumber1 = 0x00000001;

/**
* call forwarding "forwarded-to" number 2.
* String value
*/
const TUint32 KSettingsCFDefaultNumber2 = 0x00000002;

/**
* call forwarding "forwarded-to" number 3.
* String value
*/
const TUint32 KSettingsCFDefaultNumber3 = 0x00000003;

/**
* call forwarding "forwarded-to" number 1. Alternate line
* String value
*/
const TUint32 KSettingsCFDefaultNumberAls1 = 0x00000004;

/**
* call forwarding "forwarded-to" number 2. Alternate line
* String value
*/
const TUint32 KSettingsCFDefaultNumberAls2 = 0x00000005;

/**
* call forwarding "forwarded-to" number 3. Alternate line
* String value
*/
const TUint32 KSettingsCFDefaultNumberAls3 = 0x00000006;

/**
* "call forwarding when no reply"-timer value.
* Integer value.
*/
const TUint32 KSettingsCFTimerValue = 0x00000007;

/**
* "call forwarding when no reply"-timer value for alternate line.
* Integer value.
*/
const TUint32 KSettingsCFTimerValueAls = 0x00000008;

/**
* Last used number diverted data calls
* String value
*/
const TUint32 KSettingsCFUsedDataNumber = 0x00000009;

/**
* Last used number diverted fax calls
* String value
*/
const TUint32 KSettingsCFUsedFaxNumber = 0x0000000A;

/**
* Common Telephony Settings CR UID.
*/
const TUid KCRUidCommonTelephonySettings = {0x101F8831};

/**
* Summary after call
*
* 0 = Off
* 1 = On
*/
const TUint32 KSettingsSummaryAfterCall = 0x00000001;

/**
* Settings for Rich Call features
*/
const TUid KCRUidRichCallSettings = {0x101F883B};

/**
* Calling Line Identification Restriction for PS calls
*
* 0 = KRCSEClirOff
* 1 = KRCSEClirOn
* 2 = KRCSEClirDefault
*/
const TUint32 KRCSEClir = 0x00000000;

/**
* Determines whether warning is given to user, when WLAN signal
* strength drops too low.
*
* 0 = KRCSEEarlyWarningOff
* 1 = KRCSEEarlyWarningOn
*/
const TUint32 KRCSEWlanEarlyWarning = 0x00000001;

/**
* Circuit Switched call waiting.
*
* 0 = KRCSECSCallWaitingOff
* 1 = KRCSECSCallWaitingOn
*/
const TUint32 KRCSECSCallWaiting = 0x00000002;

/**
* Determines whether the registeration of VoIP is automatic or manual.
*
* 0 = KRCSEAutomatic
* 1 = KRCSEManual
*/
const TUint32 KRCSEPSLogin = 0x00000003;

/**
* Determines whether the user wants to receive anonymous calls.
* Possible values are:
* 0 = KRCSEAnonymousCallsAllowed
* 1 = KRCSEAnonymousCallsBlocked
*/
const TUint32 KRCSEAnonymousCallBlockRule = 0x00000004;

/**
* Determines call type preference.
* Possible values are:
* 0 = KRCSECSCallPreferred
* 1 = KRCSEInternetCallPreferred
*/
const TUint32 KRCSEPreferredTelephony = 0x00000005;

/**
* Determines status of Do Not Disturb setting.
* Possible values are:
* 0 = KRCSEDoNoDisturbOff
* 1 = KRCSEDoNoDisturbOn
*/
const TUint32 KRCSEDoNotDisturb = 0x00000006;

/**
* Determines status of PS (VoIP) call waiting setting.
* Possible values are:
* 0 = KRCSEPSCallWaitingOff
* 1 = KRCSEPSCallWaitingOn
*/
const TUint32 KRCSPSCallWaiting = 0x00000007;

/**
* Determines id of preferred VOIP service.
* 0 = Not set
* 1 = CS / Video service
* 2 = voip stub
* 3 = voip service 1
* 4 = voip service 2
* n = voip service n
*/
const TUint32 KRCSPSPreferredService = 0x00000008;


/**
* Miscelaneous Settings
*/
const TUid KCRUidMiscSettings = {0x1000A82B};

/**
Phone settings CR UID
*/
const TUid KCRUidPhoneSettings = {0x1028239D};

/**
Call waiting feature
0 = disabled
1 = Enabled
*/
const TUint32 KPSetCallWaiting = 0x00000001;

/**
*  Device Management Settings CR UID.
*/
const TUid KCRUidDeviceManagementSettings = {0x2002120B};

/**
Determines whether to show power saving query before entering power saving mode. 
0: Off
1: On (Default)
*/
const TUint32 KSettingsPowerSavingQuery = 0x00000001;

/**
*  Slide Settings CR UID.
*/
const TUid KCRUidSlideSettings = {0x20015159};

enum TSlideSetting
    {
    ESlideSettingOff = 0,
    ESlideSettingOn  
    };

/**********************************************************
 * User settings for slide handling
 *
 */

/**
 * Used to control which behavior is chosen when closing the cover. 
 * Possible values are 0=Off, 1=On
 */
const TUint32 KSlideCloseCover                               = 0x00000001;

const TUint32 KSlideOpenCoverUri                             = 0x00000002;

const TUint32 KSlideOpenCoverCaption                         = 0x00000003;

/**
 * This setting defines whether keyguard is activated by closing the cover 
 *
 * Possible values:
 * 0: Closing cover does not activate keyguard.
 * 1: Closing the cover activates keyguard.
 * 2: User is asked "Do you want to activate keyguard?".
 * 3: Automatic - keyguard returns to the state before opening the cover.
 */
const TUint32 KSlideKeyguard                                 = 0x00000004;
enum TSlideSettingKeyguard
    {
    ESlideSettingsKeyguardActivatingOff = 0,
    ESlideSettingsKeyguardActivatingOn,
    ESlideSettingsKeyguardActivatingAskMe,
    ESlideSettingsKeyguardActivatingAutomatic
    }; 

/**
 * Used to control if audio is routed when opening the slide. 
 * Possible values are 0=routing not changed, 1=routing changed
 */
const TUint32 KSlideAudioRouting                             = 0x00000005;

/**
 * User setting for Intelligent Slide
 * Possible values are listed in TSlideSetting
 */
const TUint32 KSlideAutomaticAction                          = 0x00000010;

/**********************************************************
 * Configurability settings 
 * This group of settings is used to specify which slide behavior is enabled
 * Different slide types need different items 
 * For example ITU-T slide need call handling with slide
 * And full-Qwerty slide does not use call handling but requires audio routing with slide
 */

/**
 * Defines which localisation to use for slide settings
 * Possible values
 * 0: Slide
 * 1: Cover
 * 2: Flip
 * 3: Fold
 * 4: Swivel
 * 5: reserved 
 */
const TUint32 KSlideTerminology                             = 0x00000101;

/**
 * Slide Events to react to
 * Depending on this value, different P/S keys in KPSUidHWRM are used for monitoring the slide state 
 * Possible values are 
 * 0: Grip
 * 1: Flip
 * 2: Twist  
 */
const TUint32 KSlideShowEventType                               = 0x00000102;

/**
 * Perform call handling with slide 
 * Possible values
 * 0: Disabled
 * 1: Enabled 
 */
const TUint32 KSlideShowCallHandling                            = 0x00000110;

/**
 * Perform audio routing with slide 
 * Possible values
 * 0: Disabled
 * 1: Enabled 
 */
const TUint32 KSlideShowAudioRouting                            = 0x00000111;

/**
 * Perform keypad lock handling with slide 
 * Possible values
 * 0: Disabled
 * 1: Enabled 
 */
const TUint32 KSlideShowKeypadlock                              = 0x00000112;

/**
 * Application launch when slide open in idle
 * Possible values
 * 0: Disabled
 * 1: Enabled 
 */
const TUint32 KSlideShowAppLaunch                               = 0x00000113;

/**
 * Go to idle with slide close
 * Possible values
 * 0: Disabled
 * 1: Enabled 
 */
const TUint32 KSlideShowGoToIdle                                = 0x00000114;

/**
 * Device lock with slide 
 * Possible values
 * 0: Disabled
 * 1: Enabled 
 */
const TUint32 KSlideShowDeviceLock                              = 0x00000115;

/**********************************************************
 * Intelligent Slide configurability settings
 * The following items describe in which applications/views 
 * Intelligent slide feature is enabled
 *
 */

/**
 * Messaging Main View
 * Possible values are listed in TSlideSetting
 */
const TUint32 KMessagingMainView                            = 0x00001001;

/**
 * Messaging Message List View
 * Possible values are listed in TSlideSetting
 */
const TUint32 KMessagingMessageListView                     = 0x00001002;

/**
 * Messaging One Row Message List View
 * Possible values are listed in TSlideSetting
 */
const TUint32 KMessagingOneRowMessageListView               = 0x00001003;

/**
 * Messaging Text Message Viewer
 * Possible values are listed in TSlideSetting
 */
const TUint32 KMessagingTextMessageViewer                   = 0x00001004;

/**
 * Messaging Multimedia Message Viewer
 * Possible values are listed in TSlideSetting
 */
const TUint32 KMessagingMultimediaMessageViewer             = 0x00001005;

/**
 * Messaging Uni Editor
 * Possible values are listed in TSlideSetting
 */
const TUint32 KMessagingUniEditor                           = 0x00001006;

/**
 * Email Viewer
 * Possible values are listed in TSlideSetting
 */
const TUint32 KEmailViewer                                  = 0x00001010;

/**
 * Email Editor
 * Possible values are listed in TSlideSetting
 */
const TUint32 KEmailEditor                                  = 0x00001011;

/**
 * Browser Content view
 * Possible values are listed in TSlideSetting
 */
const TUint32 KBrowserContentView                           = 0x00001020;

/**
 * Browser Bookmarks view
 * Possible values are listed in TSlideSetting
 */
const TUint32 KBrowserBookmarksView                         = 0x00001021;

/**
 * Clock Main view 
 * Possible values are listed in TSlideSetting
 */
const TUint32 KClockMainView                                = 0x00001030;

/**
 * Clock Alarm editor 
 * Possible values are listed in TSlideSetting
 */
const TUint32 KClockAlarmEditor                             = 0x00001031;

/**
 * Clock MultiAlarm view 
 * Possible values are listed in TSlideSetting
 */
const TUint32 KClockMultiAlarmView                          = 0x00001032;

/**
 * Clock World clock view
 * Possible values are listed in TSlideSetting
 */
const TUint32 KClockWorldClockView                          = 0x00001033;

/**
 * Clock World clock view
 * Possible values are listed in TSlideSetting
 */
const TUint32 KClockCountDownView                           = 0x00001034;


/**
 * Notepad Editor
 * Possible values are listed in TSlideSetting
 */
const TUint32 KNotepadEditor                                = 0x00001040;

/**
 * Notepad List view
 * Possible values are listed in TSlideSetting
 */
const TUint32 KNotepadListView                              = 0x00001041;

/**
 * Phone Incoming call 
 * Possible values are listed in TSlideSetting
 */
const TUint32 KPhoneIncomingCall                            = 0x00001050;

/**
 * File Manager Folder view 
 * Possible values are listed in TSlideSetting
 */
const TUint32 KFileManagerFolderView                        = 0x00001060;

/**
 * Calendar Main(Month/Week/Day/To-Do) view 
 * Possible values are listed in TSlideSetting
 */
const TUint32 KCalendarMainView                             = 0x00001070;

/**
 * Calendar Event(Meeting/Memo/Anniversary/To-Do note) view 
 * Possible values are listed in TSlideSetting
 */
const TUint32 KCalendarEventView                            = 0x00001071;

/**
 * Calendar Event(Meeting/Memo/Anniversary/To-Do note) editor 
 * Possible values are listed in TSlideSetting
 */
const TUint32 KCalendarEventEditor                          = 0x00001072;

/**
 * Contacts Group view 
 * Possible values are listed in TSlideSetting
 */
const TUint32 KContactsGroupView                            = 0x00001080;

/**
 * Contacts Details view 
 * Possible values are listed in TSlideSetting
 */
const TUint32 KContactsDetailsView                          = 0x00001081;

/**
 * Contacts Editor 
 * Possible values are listed in TSlideSetting
 */
const TUint32 KContactsEditor                               = 0x00001082;

/**
 * Contacts Communication Launcher view 
 * Possible values are listed in TSlideSetting
 */
const TUint32 KContactsCommunicationLauncherView            = 0x00001083;


/**
 * Logs view 
 * Possible values are listed in TSlideSetting
 */
const TUint32 KLogsView                                     = 0x00001090;


/**
 * Modifier keys used for global shortcuts
 * Possible values of scan codes are listed in e32keys.h
 * For example EStdKeyLeftShift=0x12, EStdKeyLeftCtrl=0x16
 */
const TUint32 KGlobalShortcutModifier1                      = 0x00002001;
const TUint32 KGlobalShortcutModifier2                      = 0x00002002;

/**
 * Action key scancode
 * Possible values of scan codes are listed in e32keys.h
 */
const TUint32 KGlobalShortcutKey00                          = 0x00002100;
const TUint32 KGlobalShortcutKey01                          = 0x00002101;
const TUint32 KGlobalShortcutKey02                          = 0x00002102;
const TUint32 KGlobalShortcutKey03                          = 0x00002103;
const TUint32 KGlobalShortcutKey04                          = 0x00002104;
const TUint32 KGlobalShortcutKey05                          = 0x00002105;
const TUint32 KGlobalShortcutKey06                          = 0x00002106;
const TUint32 KGlobalShortcutKey07                          = 0x00002107;
const TUint32 KGlobalShortcutKey08                          = 0x00002108;
const TUint32 KGlobalShortcutKey09                          = 0x00002109;
const TUint32 KGlobalShortcutKey10                          = 0x00002110;
const TUint32 KGlobalShortcutKey11                          = 0x00002111;
const TUint32 KGlobalShortcutKey12                          = 0x00002112;
const TUint32 KGlobalShortcutKey13                          = 0x00002113;
const TUint32 KGlobalShortcutKey14                          = 0x00002114;
const TUint32 KGlobalShortcutKey15                          = 0x00002115;
const TUint32 KGlobalShortcutKey16                          = 0x00002116;
const TUint32 KGlobalShortcutKey17                          = 0x00002117;
const TUint32 KGlobalShortcutKey18                          = 0x00002118;
const TUint32 KGlobalShortcutKey19                          = 0x00002119;

/**
 * Application UID that should be brought to foreground/background 
 * when corresponding action key is pressed
 */
const TUint32 KGlobalShortcutApp00                          = 0x00002200;
const TUint32 KGlobalShortcutApp01                          = 0x00002201;
const TUint32 KGlobalShortcutApp02                          = 0x00002202;
const TUint32 KGlobalShortcutApp03                          = 0x00002203;
const TUint32 KGlobalShortcutApp04                          = 0x00002204;
const TUint32 KGlobalShortcutApp05                          = 0x00002205;
const TUint32 KGlobalShortcutApp06                          = 0x00002206;
const TUint32 KGlobalShortcutApp07                          = 0x00002207;
const TUint32 KGlobalShortcutApp08                          = 0x00002208;
const TUint32 KGlobalShortcutApp09                          = 0x00002209;
const TUint32 KGlobalShortcutApp10                          = 0x00002210;
const TUint32 KGlobalShortcutApp11                          = 0x00002211;
const TUint32 KGlobalShortcutApp12                          = 0x00002212;
const TUint32 KGlobalShortcutApp13                          = 0x00002213;
const TUint32 KGlobalShortcutApp14                          = 0x00002214;
const TUint32 KGlobalShortcutApp15                          = 0x00002215;
const TUint32 KGlobalShortcutApp16                          = 0x00002216;
const TUint32 KGlobalShortcutApp17                          = 0x00002217;
const TUint32 KGlobalShortcutApp18                          = 0x00002218;
const TUint32 KGlobalShortcutApp19                          = 0x00002219;

#endif      // SETTINGSINTERNALCRKEYS_H

// End of file