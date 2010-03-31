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
* Description:
* Content model for data plug-in.
*
*/


#ifndef WRTDATAPLUGINCONST_H
#define WRTDATAPLUGINCONST_H

#include <aicontentmodel.h>
#include "hs_app_wrtdatapluginuids.hrh"

// AI Data Plug-in ECOM implementation UID.
const TInt KImplUidDataPlugin = WRTDP_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_DATAPLUGIN;
const TUid KUidDataPlugin = { KImplUidDataPlugin };

// ================================= CONTENT ===================================

/**
 * Content Model type identifier
 */
_LIT(KText ,"text");
_LIT(KImage, "image");
_LIT(KAnimation, "animation");
_LIT(KPlugin, "plugin");

_LIT(KNewsTicker ,"newsticker");


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
_LIT8( KContentType, "content_type" );
_LIT8( KContentId,   "content_id" );
_LIT8( KResults,      "results");
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

_LIT( KDeActive, "deactive");
_LIT( KActive, "active");
_LIT( KSuspend , "suspend");
_LIT( KResume, "resume");
_LIT( KOnLine, "online");
_LIT( KOffLine, "offline");
_LIT( KInActive, "inactive");

// reserved extension for retrieving mask handle 
_LIT8( KImageMask, "image1_mask");

_LIT( KSkin, 		 "skin" );
_LIT( KMif, 		 "mif" );
_LIT( KMIFExtension, ".mif" );
_LIT( KColon,		 ":");

_LIT( KTemplateWidget,"ai3templatedwidget");
_LIT( KWRTPublisher, "wrt_publisher");
_LIT8(KImage1, "image1");
_LIT8( KWidgetInfo,   "widget_info");
_LIT8( KWidgetName, "widget_name");
_LIT8( KWidgetIcon, "widget_icon");

_LIT( KHexPrefix,     "0x" );
_LIT( KUid,           "uid" );
_LIT8(KElement, "loading");
_LIT8(KDisplay, "display");
_LIT8(KShow, "block");
_LIT8(KHide, "none");

#ifdef __WINS__
_LIT( KDrive, "C:" );
#else
_LIT( KDrive, "Z:" );
#endif

#ifdef WRT_PREDEFINED_IMAGE  
_LIT(KImgFolder, "200286dd\\");
_LIT(KJPEG, ".jpg");
_LIT(KPNG, ".png");
#endif

const TUint KLeftParenthesis = '(';


// WRT Data Plugin Constants
const TUint KPluginNameSeprator =  '/';

/**
 * Maximum length for the content name
 */
const TInt KMaxTypeLength = 8;

/**
 * Maximum length for the content name
 */
const TInt KWRTContentNameMaxLength = 255;

/**
 * Maximum length for the content value
 */
const TInt KWRTContentValueMaxLength = 255;

/**
 * Maximum length for the plugin name
 */
const TInt KAiPluginNameMaxLength = 255;

/**
 * Maximum length for the application uid string
 * uid(0x123456578)
 * 
 */
const TInt KWRTAppUidLenth = 16; 

#endif /* WRTDATAPLUGINCONST_H */
