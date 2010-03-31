/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Defines constants shared between this subsystem and 
*                clients of this subsystem.
*
*/


#ifndef AIWSPLUGINANIMDEF_H
#define AIWSPLUGINANIMDEF_H

// INCLUDES
#include <e32std.h>
#include <e32keys.h>

// CONSTANTS

_LIT( KAiWsPluginAnimDllName, "hs_app_aiwsplugin.dll" );

// DATA TYPES

/** 
* Animation services
*
* EAnimKeyForward - Key forwarding to call handling application
*/
enum TAiWsPluginAnimType
    {
    EAnimKeyForward
    };

/**
* Active Idle anim plugin initialisation data.
*/
struct TAiWsPluginAnimInitData
    {
    // Idle window group id
    TInt iAiWgId;
    // Window group to activate 
    TInt iTargetWgId;
    };

#endif // AIWSPLUGINANIMDEF_H

