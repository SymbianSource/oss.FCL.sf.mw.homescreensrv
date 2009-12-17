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
* Description:  Definition of different constants
*
*/


#ifndef __MCSDEF_H__
#define __MCSDEF_H__

#include <e32base.h>
#include "mcsinternaltypes.h"


// CONSTANTS
const TUid KMmUid3 = { 0x101f4cd2 };
const TUid KCRUidMenu = { 0x101F8847 };
const TUint32 KMenuHideApplication = 0x00000008;
const TUint32 KMenuHideCPApplication = 0x0000000a;
const TUint32 KMenuHideCPFolder = 0x0000000b;
const TInt KCenRepBufferSize = 255;

//
// Internal types, cannot be accessed (KErrArgument).
//

_LIT( KMenuTypeData, "menu:data" ); ///< Internal type, cannot be accessed or created.


//
// Internal attributes, cannot be accessed (KErrArgument).
//

_LIT( KMenuAttrFlags, "flags" ); ///< Internal attribute, cannot be accessed. Use Flags().
_LIT( KMenuAttrId, "id" ); ///< Internal attribute, cannot be accessed. Use Id().

//
// Attributes commonly used for all items.
// These are the attributes that are normally used when displaying a menu of items
// in a menu UI. To get a consistent look of various items, it is recommended that
// some of these attributes are properly set (at least one of the xxx_name, and
// some kind of icon information for non-applications).
// Note, however, that none of these is guaranteed to be set, or be used by a menu UI.
// Menu clients always need to check that the necessary attributes are present.
//

_LIT( KMenuAttrTitleName, "title_name" ); ///< Title name (displayed in Title Pane).
_LIT( KMenuAttrShortName, "short_name" ); ///< Short name.
_LIT( KMenuAttrLongName, "long_name" ); ///< Long name.
_LIT( KMenuAttrIconFile, "icon_file" ); ///< Custom icon file (MIF or MBM).
_LIT( KMenuAttrIconId, "icon_id" ); ///< Icon index into "icon_file".
_LIT( KMenuAttrMaskId, "mask_id" ); ///< Mask index into "icon_file".
_LIT( KMenuAttrM3gIconFile, "m3g_icon_file" ); ///< Custom M3G icon file.
_LIT( KMenuAttrIconSkinMajorId, "icon_skin_major_id" ); ///< Icon major skin id.
_LIT( KMenuAttrIconSkinMinorId, "icon_skin_minor_id" ); ///< Icon minor skin id.

_LIT( KMenuAttrDrmProtection, "drm_protection" ); ///< DRM Protection status
_LIT( KMenuDrmUnknown, "unknown" ); ///< Value of the DRM Protection attribute
_LIT( KMenuDrmNotProtected, "not_protected" ); ///< Value of the DRM Protection attribute
_LIT( KMenuDrmProtected, "protected" ); ///< Value of the DRM Protection attribute
_LIT( KMenuDrmRightsExpired, "expired" ); ///< Value of the DRM Protection attribute

_LIT( KMenuAttrNative, "native" );
_LIT( KMenuTrue, "1" );
_LIT( KMenuFalse, "0" );

// Built-in types and attributes.
//

// Folder.
_LIT( KMenuTypeFolder, "menu:folder" ); ///< Menu folder type.
_LIT( KMenuAttrAppGroupName, "applicationgroup_name" ); ///< Application Group name.
_LIT( KMenuAttrDefault, "default" ); ///< Default folder.
_LIT( KChildrenCount, "children_count");  
_LIT( KRunningStatus, "RunningStatus" );

// Application.
_LIT( KMenuTypeApp, "menu:application" ); ///< Menu application type.
_LIT( KMenuTypeSuite, "menu:suite" );
_LIT( KMenuAttSuiteName, "suite_name" );
_LIT( KMenuAttSuiteAdded, "suite_added" );
_LIT( KMenuAttrUid, "uid" ); ///< UID.
_LIT( KMenuAttrView, "view" ); ///< View.
_LIT( KMenuAttrHideIfInstalledUid, "hide_if_installed_uid" ); ///< Hide app if installed other app with uid.

// URL.
_LIT( KMenuTypeUrl, "menu:url" ); ///< Menu url type.
_LIT( KMenuAttrUrl, "url" ); ///< URL.

// Link.
_LIT( KMenuTypeLink, "menu:link" ); ///< Menu link type.
_LIT( KMenuAttrRefId, "ref_id" ); ///< Referred id. Refers to Id() of some other item.

//
// Command strings.
//

_LIT8( KMenuCmdOpen, "open" ); ///< Open item.
_LIT8( KMenuCmdRemove, "remove" ); ///< Remove item.


_LIT( KMenuSatUiUid, "0x101F4CE0" ); 

const TInt KDefaultGranularity = 5;
const TUint KBackSlashSuite = '\\';
const TUint KDot = '.';
_LIT(KXmlExt, "xml");
_LIT(KImportDir, "import");


_LIT( KColen, ":" );
const TUid KSatUid = { 0x101F4CE0 };
const TUint32 KMenuSatUIIconId = 0x00000004;

const TUint32 KMenuShowSatUI = 0x00000002;
const TUint32 KMenuSatUIName = 0x00000003;

_LIT(KComma, ",");
_LIT(KHex, "0x");

//Constants
_LIT8(KMenuAttrId8,              "id");
_LIT8(KInData,                   "InData");
_LIT8(KFilter,                   "Filter");
_LIT8(KType,                     "type");
_LIT8(KMenuParentId,			 "parent_id");
_LIT8(KAddRemove,                "add_remove");
_LIT8(KReorder,                  "reorder");
_LIT8(KAttributeChange,          "attribute_change");
_LIT8(KParentId,                 "parent_id");
_LIT8(KBeforeId,                 "before_item");
_LIT8(KRecursiveSearch,          "recursive_search");
_LIT8(KChildren,                 "children");
_LIT8(KFlatResult,               "flat_result");
_LIT8(KParentOnly,               "parent_only");
_LIT8(KIncludeParent,            "include_parent");
_LIT8(KIgnoreAttributes,         "ignore_attributes");
_LIT8(KRequiredAttributes,       "required_attributes");
// Flags
_LIT8(KNameLocked,               "name_locked");
_LIT8(KIconLocked,               "icon_locked");
_LIT8(KDeleteLocked,             "delete_locked");
_LIT8(KMoveLocked,               "move_locked");
_LIT8(KAddingLocked,             "adding_locked");
_LIT8(KHidden,                   "hidden");
_LIT8(KMissing,                  "missing");

_LIT8( KBitmapBuffer, "bitmap_buffer");
_LIT8( KMaskBuffer, "mask_buffer");
_LIT8( KRunning, "running");
_LIT8( KIconAttributes, "icon_attributes");
_LIT8(KReturnValue,              "ReturnValue");

// Prefix hex format
_LIT(KHexFormat,                 "0x%X");
_LIT(KHexPrefix,                 "0x");



#endif // __MCSDEF_H__
