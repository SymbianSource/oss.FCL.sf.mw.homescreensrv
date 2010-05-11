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
* Description:  Content model for data plug-in.
*
*/


#ifndef SAPIDATAPLUGINCONST_H
#define SAPIDATAPLUGINCONST_H

#include <aicontentmodel.h>
#include "sapidatapluginuids.hrh"

// AI Data Plug-in ECOM implementation UID.
const TInt KImplUidDataPlugin = SAPIDP_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_DATAPLUGIN;
const TUid KUidDataPlugin = { KImplUidDataPlugin };

// ================================= CONTENT ===================================

/**
 * Content Model type identifier
 */
_LIT(KText ,"text");
_LIT(KImage, "image");
_LIT(KPlugin, "plugin");

_LIT(KNewsTicker ,"newsticker");
_LIT(KTextEditor, "texteditor");


// CPS Constants
_LIT(KService,         "service");
_LIT(KInterface,       "interface");
_LIT(KCommand,         "command");
_LIT8( KType,          "type");
_LIT( KCpData,         "cp_data");
_LIT( KPubData,        "publisher" );
_LIT( KCpData_PubData, "cp_data:publisher");

_LIT8( KFilter,        "filter" );
_LIT8( KDataMap,       "data_map");
_LIT8( KActionTrigger, "action_trigger" );
_LIT8( KGetList ,      "GetList" );
_LIT8( KExecuteAction, "ExecuteAction" );
_LIT8( KRequestNotification, "RequestNotification" );
_LIT8( KChangeInfo, "change_info" );
_LIT8( KListMap, "list_map" );
_LIT8( KPublisherId, "publisher" );
_LIT ( KPublisher16,  "publisher" );
_LIT8( KContentType,   "content_type" );
_LIT ( KContentType16, "content_type" );
_LIT8( KContentId,   "content_id" );
_LIT8( KResults,      "results");

_LIT( KDataKey,      "data_key");
_LIT8( KMenuItems,     "menuitems");
_LIT( KMenuItem16,     "menuitem");
_LIT( KAll, "all");
_LIT8( KOperation, "operation" );
_LIT8( KFLAG,      "flag");

_LIT( KOperationAdd, "add" );
_LIT( KOperationUpdate, "update" );
_LIT( KOperationDelete, "delete" );
_LIT( KOperationExecute, "execute" );
_LIT( KAddUpdateDelete, "add:update:delete" );
_LIT( KAddUpdate, "add:update" );

_LIT(KWidget, "hswidget");
_LIT8( KDeActive, "deactive");
_LIT8( KActive, "active");
_LIT8( KSystemStartup, "systemstartup");
_LIT8( KPageStartup, "pagestartup");
_LIT8( KPluginStartup, "pluginstartup");
_LIT8( KSuspend , "suspend");
_LIT8( KResume, "resume");
_LIT8( KSystemShutdown, "systemshutdown");
_LIT8( KPageShutdown, "pageshutdown");
_LIT8( KPluginShutdown, "pluginshutdown");
_LIT8( KOnLine, "online");
_LIT8( KOffLine, "offline");


// reserved extension for retrieving mask handle 
_LIT8( KMask, "_mask");

_LIT( KSkin, 		 "skin" );
_LIT( KMif, 		 "mif" );
_LIT( KMIFExtension, ".mif" );
_LIT( KColon,		 ":");

const TUint KLeftParenthesis = '(';

// Option for registering in CPS
const TUint KExtendedNotifications = 0x4000;

// Sapi Data Plugin Constants
const TUint KPluginNameSeprator =  '/';

/**
 * Maximum length for the content name
 */
const TInt KMaxTypeLength = 8;

/**
 * Maximum length for the content name
 */
const TInt KSAPIContentNameMaxLength = 255;

/**
 * Maximum length for the content value
 */
const TInt KSAPIContentValueMaxLength = 255;

/**
 * Maximum length for the plugin name
 */
const TInt KAiPluginNameMaxLength = 255;


#endif /* SAPIDATAPLUGINCONST_H */
