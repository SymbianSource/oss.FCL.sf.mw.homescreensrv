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

#ifndef MENU2INTERNALCRKEYS_H
#define MENU2INTERNALCRKEYS_H

// Menu API

const TUid KCRUidMenu = {0x101f8847};
// TARM KEYS
const TUint32 KMenuHideApplication = 0x00000008;
//#ifdef _CP_HIDDEN_APPS_AND_FOLDER
const TUint32 KMenuHideCPApplication = 0x0000000a;
const TUint32 KMenuHideCPFolder = 0x0000000b;
//#endif
// IN 3.1 this key is used only for setting status pane size
// 0 = normal status pane, 1 = flat status pane
const TUint32 KMenuGridMode = 0x00000007;

const TUint32 KMenuForceShowAnimation = 0x0000000d;

#endif      // MENU2INTERNALCRKEYS_H
