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


#ifndef _AIFWDEFS_H
#define _AIFWDEFS_H

// System inclides

// User includes

// Type definitions

/**
 * AiFw State definitions.
 *
 * @since S60 5.2
 */
enum TAiFwState
    {
    EAiFwBacklightOn = 1,
    EAiFwBacklightOff,
    EAiFwForeground,
    EAiFwBackground,
    EAiFwBackupRestoreStart,
    EAiFwBackupRestoreEnd,
    EAiFwGeneralThemeChange,    
    EAiFwUiStartup,
    EAiFwUiShutdown,
    EAiFwOnline,
    EAiFwOffline
    };

/**
 * AiFw data plugin load reasons.
 *
 * @since S60 5.2
 */    
enum TAiFwLoadReason
    {
    EAiFwSystemStartup = 1,
    EAiFwPageStartup,
    EAiFwPluginStartup
    };    

/**
 * AiFw data plugin destroy reasons.
 *
 * @since S60 5.2
 */        
enum TAiFwDestroyReason
    {
    EAiFwSystemShutdown = 1,
    EAiFwPageShutdown,
    EAiFwPluginShutdown
    };

#endif // _AIFWDEFS_H

// End of file
