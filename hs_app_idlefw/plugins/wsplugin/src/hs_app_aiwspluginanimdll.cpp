/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include    "hs_app_aiwspluginanimdef.h"
#include    "hs_app_aiwspluginanimdll.h"
#include    "hs_app_aiwspluginanim.h"

// Create idle key forwarding plug-in.
CAnim* CAiWsPluginAnimDll::CreateInstanceL( TInt aType )
    {
    switch ( aType )
        {
        case EAnimKeyForward:
            return new ( ELeave ) CAiWsPluginAnim();
        default:
            return NULL;
        }
    }

// Create plug-in. Factory function.
EXPORT_C CAnimDll* CreateCAnimDllL()
    {
    return new ( ELeave ) CAiWsPluginAnimDll();
    }

