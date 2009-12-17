/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Constants definitions used across SAPI
*
*/


#ifndef _MCSCONSTANTS_H_
#define _MCSCONSTANTS_H_

const TInt KArrayGran            = 4;
const TInt KArrayZeroIndex       = 0;
const TInt KMaxActionLen         = 16;

_LIT8(KGenericParamID,           "cmd");
_LIT8(KMCSDataSourceInterface,   "IDataSource");
_LIT8(KMCSMenuContentInterface,  "IMenuContent");
_LIT8(KMCSService,               "Service.MenuContent");

_LIT8(KErrorCode,                "ErrorCode");
//_LIT8(KReturnValue,              "ReturnValue");
_LIT8(KTransactionID,            "TransactionID");

// Command Names
_LIT8(KCmdAdd,                   "Add");
_LIT8(KCmdDelete,                "Delete");
_LIT8(KCmdGetList,               "GetList");
_LIT8(KCmdReqNotification,       "RequestNotification");
_LIT8(KCmdExecuteAction,         "ExecuteAction");
_LIT8(KcmdRunningApps,           "GetRunningApplication");
_LIT8(KCmdOrganize,              "Organize");

// Execute actions
_LIT8(KAction,                   "action");
_LIT8(KActionOpen,               "open");
_LIT8(KActionRemove,             "remove");
_LIT8(KContentName,              "ContentName");

// GetList Parameters
//_LIT8(KInData,                   "InData");
//_LIT8(KFilter,                   "Filter");
//_LIT8(KType,                     "type");
//_LIT8(KAddRemove,                "add_remove");
//_LIT8(KReorder,                  "reorder");
//_LIT8(KAttributeChange,          "attribute_change");
//_LIT8(KParentId,                 "parent_id");
//_LIT8(KBeforeId,                 "before_item");

// Organize Parameters
_LIT8(KItemIds,                  "item_ids");
_LIT8(KFolderId,                 "folder_id");
_LIT8(KBeforeItemId,             "before_item_id");

// Flags
//_LIT8(KNameLocked,               "name_locked");
//_LIT8(KIconLocked,               "icon_locked");
//_LIT8(KDeleteLocked,             "delete_locked");
//_LIT8(KMoveLocked,               "move_locked");
//_LIT8(KAddingLocked,             "adding_locked");
//_LIT8(KHidden,                   "hidden");
//_LIT8(KRunning,                  "running");
//_LIT8(KMissing,                  "missing");

// Search and Result Types
//_LIT8(KRecursiveSearch,          "recursive_search");
//_LIT8(KChildren,                 "children");
//_LIT8(KFlatResult,               "flat_result");
//_LIT8(KParentOnly,               "parent_only");
//
//_LIT8(KIgnoreAttributes,         "ignore_attributes");
//_LIT8(KRequiredAttributes,       "required_attributes");

// Prefix hex format
//_LIT(KHexFormat,                 "0x%X");
//_LIT(KHexPrefix,                 "0x");

// Built-in types and attributes.
//_LIT8( KBitmapBuffer, "bitmap_buffer");
//_LIT8( KMaskBuffer, "mask_buffer");
//_LIT8( KChildrenCount, "children_count");   

#endif _MCSCONSTANTS_H_

// End of file
