/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef MENU2PRIVATECRKEYS_H
#define MENU2PRIVATECRKEYS_H

// Menu API

#include <menu2internalcrkeys.h>

// This key is used to set up AppShell in a predefined folder
const TUint32 KMenuShowFolder = 0x0000000c;

//SAT KEYS
const TUint32 KMenuShowSatUI = 0x00000002;
const TUint32 KMenuSatUIName = 0x00000003;
const TUint32 KMenuSatUIIconId = 0x00000004;

//deprecated (no support anymore through MenuInterface.lib)
const TUint32 KMenuUpdateData = 0x00000001;
const TUint32 KMenuLVFlags = 0x00000005;
const TUint32 KMenuIconPositionUpdate = 0x00000006;
const TUint32 KMenuShowApplication = 0x00000009;



#endif      // MENU2PRIVATECRKEYS_H
