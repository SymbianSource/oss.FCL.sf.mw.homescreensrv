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
 *  Version     : %version: ou1s60ui#10.1.1 % << Don't touch! Updated by Synergy at check-out.
 *
 */

#ifndef CADEF_H
#define CADEF_H

#include <e32base.h>
#include "cainternaltypes.h"

// ---------------------------------------------------------
//Ca constants

enum TChangeType
    {
    EAddChangeType,
    ERemoveChangeType,
    EUpdateChangeType
    };

enum TEntryRole
    {
    ENoneEntryRole = 0,
    EItemEntryRole = 1,
    EGroupEntryRole = 2
    };

//Entry flags
enum TEntryFlag
    {
    EUsed = 1,
    ESystem = 2,
    ERemovable = 4,
    EVisible = 8,
    ERunning = 16,
    ENative = 32,
    EMissing = 64
    };
// Ca - Application
_LIT( KCaTypeFolder, "folder" );
_LIT( KCaTypeCollection, "collection" );
_LIT( KCaTypeApp, "application" );
_LIT( KCaTypeWidget, "widget" );
_LIT( KCaTypeUrl, "url" );
_LIT( KCaAttrView, "view" ); ///< View.
_LIT( KCaAttrUrl, "url" );
_LIT( KCaAttrWindowGroupId, "window_group_id" );
_LIT( KCaTypeCollectionDownload, "collection::downloaded" );
_LIT( KCaPackageUid,"packageuid");

const TInt KGranularityOne = 1;
const TInt KUidChars = 10;
const TInt KDefaultGranularity = 5;
const TInt KDefaultExpandSize = 512;
const TInt KCaMaxAttrLenght = 255;
// MMC related constants
_LIT( KCaAttrMmcId, "mmc_id" );
_LIT( KCaMassStorage, "mass_storage" );
_LIT( KCaMmcHistoryFname, "mmchistory" );

//Ca constants
// ---------------------------------------------------------


// CONSTANTS
const TUid KMmUid3 =
    {
    0x101f4cd2
    };
    
const TUid KSatUid =
    {
    0x101F4CE0
    };

const TInt KCenRepBufferSize = 255;
//
// Attributes commonly used for all items.
// These are the attributes that are normally used when displaying a menu of items
// in a menu UI. To get a consistent look of various items, it is recommended that
// some of these attributes are properly set (at least one of the xxx_name, and
// some kind of icon information for non-applications).
// Note, however, that none of these is guaranteed to be set, or be used by a menu UI.
// Menu clients always need to check that the necessary attributes are present.
//
_LIT( KCaAttrLongName, "long_name" ); ///< Long name.

// Built-in types and attributes.
//
//
// Command strings.
//

_LIT8( KCaCmdOpen, "open" ); ///< Open item.
_LIT8( KCaCmdRemove, "remove" ); ///< Remove item.
_LIT8( KCaCmdClose, "close" ); ///< Close item.


//Properties in database

_LIT( KCaDbPropVersion, "Version" ); /// db version
_LIT( KCaDbPropLanguage, "Language" ); // language of ca strings in db
_LIT( KCaDbPropQMfile, "QMfile" ); // prefix of qm file used in translation 



_LIT(KComma, ",");
// Prefix hex format
_LIT(KHexFormat, "0x%X");

#endif // CADEF_H
