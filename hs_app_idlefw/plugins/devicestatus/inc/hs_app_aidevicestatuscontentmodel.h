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
* Description:  Device Status plug-in content model
*
*/


#ifndef AIDEVICESTATUSCONTENTMODEL_H
#define AIDEVICESTATUSCONTENTMODEL_H

#include <aicontentmodel.h>

/**
 * Content model for Device Status plug-in.
 * This header defines which information Device Status publishes
 */

// ================================= CONTENT ===================================

//content item id's
enum TAiDeviceStatusContentIds
    {
    EAiDeviceStatusContentNetworkIdentity,
    EAiDeviceStatusContentProfileName,
    EAiDeviceStatusContentGeneralProfileName,
    EAiDeviceStatusContentDate,
    EAiDeviceStatusContentSilentIndicator,
    EAiDeviceStatusContentTimedProfileIndicator,
    EAiDeviceStatusContentSIMRegStatus,
    EAiDeviceStatusContentNWStatus,
    EAiDeviceStatusContentMCNIndicator,
    EAiDeviceStatusContentVHZIndicator,
    EAiDeviceStatusContentCUGIndicator,
    EAiDeviceStatusContentGeneralIndicator,
    EAiDeviceStatusContentVHZText,
    EAiDeviceStatusContentCUGMCNIndicator
    };


//content item textual id's
const wchar_t KAiDeviceStatusContentNetworkIdentity_Cid[]	  	= L"NetworkIdentity";
const wchar_t KAiDeviceStatusContentProfileName_Cid[]	  		= L"ProfileName";
const wchar_t KAiDeviceStatusContentGeneralProfileName_Cid[]	= L"GeneralProfileName";
const wchar_t KAiDeviceStatusContentDate_Cid[]			  		= L"Date";
const wchar_t KAiDeviceStatusContentSilentIndicator_Cid[] 		= L"SilentIndicator";
const wchar_t KAiDeviceStatusContentTimedProfileIndicator_Cid[] = L"TimedProfileIndicator";
const wchar_t KAiDeviceStatusContentSIMRegStatus_Cid[]	  		= L"SIMRegStatus";
const wchar_t KAiDeviceStatusContentNWStatus_Cid[]	  		    = L"NWStatus";
const wchar_t KAiDeviceStatusContentMCNIndicator_Cid[]	  		= L"MCNIndicator";
const wchar_t KAiDeviceStatusContentVHZIndicator_Cid[]	  		= L"VHZIndicator";
const wchar_t KAiDeviceStatusContentCUGIndicator_Cid[]	  		= L"CUGIndicator";
const wchar_t KAiDeviceStatusContentGeneralIndicator_Cid[]	  	= L"GeneralIndicator";
const wchar_t KAiDeviceStatusContentVHZText_Cid[]       	  	= L"VHZText";
const wchar_t KAiDeviceStatusContentCUGMCNIndicator_Cid[]	  	= L"CUGMCNIndicator";


const char KAiDeviceStatusMimeTypeTextPlain[]			= "text/plain";

/**
*  Content what device status plugin publishes
*/

const TAiContentItem KAiDeviceStatusContent[] =
    {
    //Published data can be service provider name, offline profile,
    //operator logo or anything related to network status
    { EAiDeviceStatusContentNetworkIdentity, KAiDeviceStatusContentNetworkIdentity_Cid,
    	 KAiDeviceStatusMimeTypeTextPlain },

    //Published data is name of the profile from profiles engine
    { EAiDeviceStatusContentProfileName, KAiDeviceStatusContentProfileName_Cid,
    	KAiDeviceStatusMimeTypeTextPlain },

    //Published data is name of the general profile from profiles engine
    { EAiDeviceStatusContentGeneralProfileName, KAiDeviceStatusContentGeneralProfileName_Cid,
    	 KAiDeviceStatusMimeTypeTextPlain },

    //Published data is current date as a text. Formatted according to current locale
    { EAiDeviceStatusContentDate, KAiDeviceStatusContentDate_Cid,
    	KAiDeviceStatusMimeTypeTextPlain },

    //Published data silent indicator as a text
    { EAiDeviceStatusContentSilentIndicator, KAiDeviceStatusContentSilentIndicator_Cid,
    	 KAiDeviceStatusMimeTypeTextPlain },

    //Published data timed profile indicator as a text
    { EAiDeviceStatusContentTimedProfileIndicator, KAiDeviceStatusContentTimedProfileIndicator_Cid,
    	 KAiDeviceStatusMimeTypeTextPlain },

    //Published data is resource id
    { EAiDeviceStatusContentSIMRegStatus, KAiDeviceStatusContentSIMRegStatus_Cid,
    	KAiDeviceStatusMimeTypeTextPlain },

    //Published data is resource id
    { EAiDeviceStatusContentNWStatus, KAiDeviceStatusContentNWStatus_Cid,
    	KAiDeviceStatusMimeTypeTextPlain },

    //Published data is MCN message
    { EAiDeviceStatusContentMCNIndicator, KAiDeviceStatusContentMCNIndicator_Cid,
    	KAiDeviceStatusMimeTypeTextPlain },

    //Published data is VHZ name
    { EAiDeviceStatusContentVHZIndicator, KAiDeviceStatusContentVHZIndicator_Cid,
    	KAiDeviceStatusMimeTypeTextPlain },

    //Published data is localized text, for example "Group 1"
    { EAiDeviceStatusContentCUGIndicator, KAiDeviceStatusContentCUGIndicator_Cid,
    	KAiDeviceStatusMimeTypeTextPlain },

    //Published data general indicator as a text
    { EAiDeviceStatusContentGeneralIndicator, KAiDeviceStatusContentGeneralIndicator_Cid,
    	KAiDeviceStatusMimeTypeTextPlain },

    //Published data VHZ text
    { EAiDeviceStatusContentVHZText, KAiDeviceStatusContentVHZText_Cid,
    	KAiDeviceStatusMimeTypeTextPlain },
    	
    //Published data is localized text, for example "Group 1" or MCN message
    { EAiDeviceStatusContentCUGMCNIndicator, KAiDeviceStatusContentCUGMCNIndicator_Cid,
        KAiDeviceStatusMimeTypeTextPlain }
    };

const TInt KAiDeviceStatusContentCount = sizeof( KAiDeviceStatusContent ) /
                                            sizeof( KAiDeviceStatusContent[0] );



//content item id's
enum TAiDeviceStatusResourceIds
    {
    EAiDeviceStatusResourceSIMRegFail,
    EAiDeviceStatusResourceNWOk,
    EAiDeviceStatusResourceNWLost
    };

const wchar_t KAiDeviceStatusResourceSIMRegFail_Cid[] = L"SIMRegFail";
const wchar_t KAiDeviceStatusResourceShowNWLost_Cid[] = L"NWLost";


const TAiContentItem KAiDeviceStatusResources[] =
{
    //Published data is resource id
    { EAiDeviceStatusResourceSIMRegFail, KAiDeviceStatusResourceSIMRegFail_Cid,
    	KAiDeviceStatusMimeTypeTextPlain },
    { EAiDeviceStatusResourceNWLost, KAiDeviceStatusResourceShowNWLost_Cid,
    	KAiDeviceStatusMimeTypeTextPlain },
};

const TInt KAiDeviceStatusResourceCount = sizeof( KAiDeviceStatusResources ) /
                                            sizeof( KAiDeviceStatusResources[0] );


#endif // AIDEVICESTATUSCONTENTMODEL_H
