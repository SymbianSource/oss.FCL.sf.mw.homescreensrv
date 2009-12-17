/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Plugin Interface definistions / plugins uids
 *
*/



#ifndef C_ACTION_HANDLER_PLUGINDEFS_H
#define C_ACTION_HANDLER_PLUGINDEFS_H

#include "ahpluginuids.hrh"

_LIT8( KActionHandlerInterface, "IDataAction" );
_LIT8( KCommand, "cmd" );

// UID of this interface
const TInt32 KAHPluginInterface =
    {
    KAHPluginInterfaceUid
    }; //{0x10089990};

//Supported Commands
_LIT8( KExecute, "Execute" );

// Input arguments
_LIT8( KPluginUid, "plugin_id" );
_LIT8( KMap, "data" );

#endif //C_ACTION_HANDLER_PLUGINDEFS_H
//End of file
