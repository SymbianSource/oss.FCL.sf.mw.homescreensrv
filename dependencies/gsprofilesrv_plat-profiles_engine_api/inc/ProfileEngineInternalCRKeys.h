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
* Description:  Profiles engine internal Central Repository keys.
*
*/



#ifndef PROFILEENGINEINTERNALCRKEYS_H
#define PROFILEENGINEINTERNALCRKEYS_H

//  INCLUDES
#include <e32std.h>
#include <ProfileEngineDomainCRKeys.h>

// CONSTANTS

/**
 * Information about whether self recorded tones can be used as ringing tones
 * or not. Integer value.
 *
 * Possible values:
 *
 * 0 (disabled)
 * 1 (enabled)
 */
const TUint32 KProEngRecordedRingingTones = 0x80000100;

/**
 * If this is true, DRM protected MP4 tones are not allowed as ringing tones.
 *
 * Possible values:
 *
 * 0 (allowed, default)
 * 1 (not allowed)
 */
const TUint32 KProEngNoDrmMP4RingingTones = 0x80000101;

/**
 * If this is true, WMA tones are not allowed as ringing tones.
 *
 * Possible values:
 *
 * 0 (allowed, default)
 * 1 (not allowed)
 */
const TUint32 KProEngNoWmaRingingTones = 0x80000102;

/**
 * Tactile feedback setting for the currently active profile.
 * THIS COULD BE MOVED TO THE SDK HEADER LATER?
 *
 * Possible values:
 *
 * 0 (Off)
 * 1 (Level 1)
 * 2 (Level 2)
 * 3 (Level 3)
 */
const TUint32 KProEngActiveTactileFeedback = 0x7E000019;

/**
 * Audio feedback setting for the currently active profile.
 * THIS COULD BE MOVED TO THE SDK HEADER LATER?
 *
 * Possible values:
 *
 * 0 (Off)
 * 1 (Level 1)
 * 2 (Level 2)
 * 3 (Level 3)
 */
const TUint32 KProEngActiveAudioFeedback = 0x7E00001A;


#endif      // PROFILEENGINEINTERNALCRKEYS_H

// End of File
