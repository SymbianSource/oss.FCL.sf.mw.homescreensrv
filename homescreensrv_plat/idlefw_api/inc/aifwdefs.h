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
* Description:  Internal array type definition
*
*/


#ifndef AIFWDEFS_H
#define AIFWDEFS_H


#include <e32std.h>
#include <aipropertyextension.h>

/**
 * Array of publisher Content publisher info records.
 */
typedef RArray<TAiPublisherInfo> RAiPublisherInfoArray;

_LIT(KOnline_Offline, "online_offline");

enum TAifwStates
	{
	EAifwOffline,
	EAifwOnline,
	EAifwPageSwitch
	};

#endif // AIFWDEFS_H
