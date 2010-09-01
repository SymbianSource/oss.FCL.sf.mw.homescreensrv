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
* Description: 
*     Definitions of domain keys related to Active Idle in CenRep
*
*/

#ifndef AICONTAINERDOMAINCRKEYS_H
#define AICONTAINERDOMAINCRKEYS_H

#include <e32std.h>

/**
 * Active Idle Settings
 *
 */
const TUid KCRUidActiveIdle = {0x10207467};

/**
 * KAINewsTickerStatus
 * Define the status of the News_ticker Plugin
 * - Off = 0
 * - On = 1
 * 
 */
const TUint32 KAINewsTickerStatus = 0x00000001;

/**
 * KAINumOfOperatorPlugins
 * Defines number of operator plugins
 * Keys that are defined after this key, are considered to be plugin uids
 * e.g. if KAINumOfOperatorPlugins = 1, then next key (3) would be read and used as an UID to load the plugin
 */
const TUint32 KAINumOfOperatorPlugins = 0x00000002;

#endif
