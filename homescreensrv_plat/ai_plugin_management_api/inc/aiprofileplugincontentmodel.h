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
* Description:  Content model for profile plug-in.
*
*/


#ifndef AIPROFILEPLUGINCONTENTMODEL_H
#define AIPROFILEPLUGINCONTENTMODEL_H

#include <aicontentmodel.h>
#include <aiprofilepluginuids.hrh>

// AI Profile Plug-in ECOM implementation UID.
const TInt KImplUidProfilePlugin = AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_PROFILEPLUGIN;
const TUid KUidProfilePlugin = { KImplUidProfilePlugin };


/**
* Default profile identifiers. Default profiles are those pre-installed in the phone:
*
* Id of General Profile: 0
* Id of Silent  Profile: 1
* Id of Meeting Profile: 2
* Id of Outdoor Profile: 3
* Id of Pager 	Profile: 4
* Id of OffLine Profile: 5 //Use this only if Off-Line Profile is supported
* Id of Drive	Profile: 6 //Use this only if Drive Profile is supported
*/


// ================================= CONTENT ===================================

/**
 * Content Ids.
 */
enum TAiProfileContentIds
{
    EAiProfileContentActiveProfileName = 1,
    EAiProfileContentProfileName,
    EAiProfileContentSwapProfileName,
    EAiProfileActiveProfileSilentChar,
    EAiProfileActiveProfileIcon,
    EAiProfileActiveProfileNameAndIconChar
};

/**
 * Content that the plug-in will publish.
 */
const TAiContentItem KAiProfileContent[] =
{
    // Active Profile name as plain text.
    { EAiProfileContentActiveProfileName,	L"ActiveProfileName",	"text/plain" }
    ,
    // Profile names as plain text.
    { EAiProfileContentProfileName,      	L"ProfileName",      	"text/plain" }   
    ,
    // Swap profile name as plain text.
    { EAiProfileContentSwapProfileName,     L"SwapProfileName",		"text/plain" }
    ,
    // Active Profile Silent indicator as plain text.
    { EAiProfileActiveProfileSilentChar,	L"ActiveProfileSilentChar",	"text/plain" }
    ,
    // Active Profile Silent indicator as image.
    { EAiProfileActiveProfileIcon,	L"ActiveProfileIcon",	KAiContentTypeBitmap }
    ,
    // Active Profile name and PUA char
    { EAiProfileActiveProfileNameAndIconChar,  L"ActiveProfileNameAndIconChar",   "text/plain" }
};
const TInt KAiProfileContentCount = sizeof( KAiProfileContent ) / 
                                            sizeof( KAiProfileContent[0] );
// ================================ RESOURCES ==================================

/**
 * Resource Ids.
 */
enum TAiProfileResourceIds
{
    EAiProfileActiveProfileSilentIconResource = 1,
    EAiProfileActiveProfileGeneralIconResource,
    EAiProfileActiveProfileTimedIconResource
};

/**
 * Resources that the plug-in will publish.
 */
const TAiContentItem KAiProfileResources[] =
{
    // Active Profile Silent indicator as image.
    { EAiProfileActiveProfileSilentIconResource,	L"ActiveProfileSilentIconResource",	"image/*" },
    { EAiProfileActiveProfileGeneralIconResource,	L"ActiveProfileGeneralIconResource",	"image/*" },
    { EAiProfileActiveProfileTimedIconResource,		L"ActiveProfileTimedIconResource",	"image/*" }
    
};

// ============================ SERVICES (Events) ==============================

/**
 * Event Ids.
 */
enum TAiProfileEventIds
{
    EAiProfileEventSwitchByIndex,
    EAiProfileEventSwitchByName,
    EAiProfileEventSwap,
    EAiProfileEditActive
};

/**
 * Services that the plug-in can perform.
 */
const TAiContentItem KAiProfileEvents[] =
{
    // Switches a profile by its index that is delivered as an integer.
    // Usage: Profile/SwitchProfileByIndex(1)
    { EAiProfileEventSwitchByIndex, L"SwitchProfileByIndex", "int" }
    ,
    // Switches a profile by its value that is delivered in a descriptor.
    // Usage: Profile/SwitchProfileByName(Silent)
    { EAiProfileEventSwitchByName, L"SwitchProfileByName", "str" }
    ,
    // Swap a profile from active profile to profile X and from profile X
    // to General profile.
    // Usage: Profile/SwapProfile(1)
    { EAiProfileEventSwap,  L"SwapProfile",  "str" },

    // Open the active profile directly to edit mode
    // in Profiles application
    { EAiProfileEditActive,  L"EditActiveProfile",  "str" } 
   

};

#endif // AIPROFILEPLUGINCONTENTMODEL_H

// End of File.
