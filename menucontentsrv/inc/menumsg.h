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
* Description:  Definition of menu client-server command IDs
*  Version     : %version: sa1spcx1#8 % << Don't touch! Updated by Synergy at check-out.
*
*/


#ifndef __MENUMSG_H__
#define __MENUMSG_H__

#include <e32std.h>

/// Menu functions.
enum TMenuFunction
    {
    EMenuNullFunction,
    
    // Functions with test capability
    EMenuTestCapabilityStart,
    
    EMenuResourceMarkStart,
    EMenuResourceMarkEnd,
    EMenuResourceCount,
    EMenuSetHeapFailure,
    
    EMenuTestCapabilityEnd,
    
    // Functions with read device data capability
    EMenuReadCapabilityStart,

    EMenuConstructSession,
    EMenuRootFolder,
    EMenuStreamClose,
    EMenuStreamOpen,
    EMenuStreamRead,
    EMenuStreamWrite,
    EMenuGetHdr,
    EMenuGetItems,
    EMenuGetRunningApps,
    EMenuGetItemsFiltered,
    EMenuItemGetAttribute,
    EMenuItemGetAttributeList,
    EMenuNotifierClose,
    EMenuNotifierOpen,
    EMenuNotifierNotify,
    EMenuNotifierNotifyCancel,
    EMenuItemGetIcon,
    EMenuGetListSize,
    EMenuGetListData,
    EMenuReadCapabilityEnd,
    
    // Functions with write device data capability
    EMenuWriteCapabilityStart,

    EMenuOperationClose,
    EMenuOperationCreateRemove,
    EMenuOperationCreateMoveToFolder,
    EMenuOperationCreateReorder,
    EMenuOperationCreateAdd,
    EMenuOperationCreateUpdate,
    EMenuOperationCancel,
    EMenuOperationStart,

    EMenuWriteCapabilityEnd,
    
    EMenuMaxFunction                  // End marker for sanity check.
    };

#endif // __MENUMSG_H__


