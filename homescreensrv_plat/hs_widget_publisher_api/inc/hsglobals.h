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
* Description:  Updates applications and icons in Operator Tile.
*
*/


#ifndef GLOBALS_H_
#define GLOBALS_H_

_LIT8( KCPInterface, "IDataSource" );
_LIT8( KCPService, "Service.ContentPublishing" );
_LIT( KStartUpFailed, "Homescreen Publishing Api startup failed.");

_LIT( KCpData, "cp_data" );
_LIT( KPublisher, "publisher" );
_LIT( KHsWidgetPublisher, "hswidgetpublisher" );
_LIT( KHSTemplate, "hstemplate" );
_LIT( KContentAndPublisher_Registry, "cp_data:publisher" );

_LIT8( KPublisherId, "publisher" );
_LIT8( KContentType, "content_type" );
_LIT8( KContentId, "content_id" );
_LIT8( KTemplateType, "template_type" );
_LIT8( KWidgetName, "widget_name" );
_LIT8( KWidgetUid, "widget_uid" );
_LIT8( KWidgetInfo, "widget_info" );
_LIT8( KWidgetMaxInstance, "widget_maxinstance" );
_LIT8( KWidgetDesc, "widget_desc" );
_LIT8( KWidgetIcon, "widget_icon" );

_LIT8( KDataMap, "data_map" );
_LIT8( KActionMap, "action_map" );

_LIT8( KUIDs, "UID" );
_LIT( KContentHarvesterUid, "0x10282E5A" );
_LIT8( KAccessList, "ACL" );
_LIT8( KACLRead, "read" );
_LIT8( KACLWrite, "write" );
_LIT8( KACLDelete, "delete" );
_LIT( KHexPrefix, "0x" );

_LIT( KTriggerMap, "data");
_LIT8( KExecuteAction, "ExecuteAction" );

_LIT8( KFilter, "filter" );
_LIT8( KOperation, "operation" );
_LIT8( KActionTrigger, "action_trigger" );

_LIT8( KCommand, "cmd" );
_LIT8( KType, "type" );

_LIT8( KPluginId,"plugin_id");
_LIT8( KApplicationUid, "application_uid" );
_LIT8( KData, "data" );
_LIT8( KLaunchMethod, "launch_method" );
_LIT8( KApaCommand, "apa_command" );
_LIT( KApaCommandBackground, "apa_command_background" );
_LIT( KLaunchMethodValueCmdLine, "cmd_line" );
_LIT( KActionValueLaunchApplication, "launch_application" );

_LIT8( KPublisherTwoRow, "PublisherTwoRow" );
_LIT8( KItem, "item" );

_LIT8( KAdd, "Add" );
_LIT8( KRequestNotification, "RequestNotification" );
_LIT8( KDelete, "Delete" );
_LIT8( KErrorCode, "ErrorCode" );

_LIT8( KActive, "active" );
_LIT8( KDeActive, "deactive" );
_LIT8( KPluginshutdown, "pluginshutdown");

_LIT8( KSuspend, "suspend" );
_LIT8( KResume, "resume" );
_LIT8( KSelected, "selected" );
_LIT8( KSelectedColon, "selected:" );
_LIT8( KSelectedFormat, "selected:%s" );
_LIT8( KContentIdFormat, "%s:%s" );
_LIT( KColon, ":" );

_LIT8( KChangeInfo, "change_info" );
_LIT8( KOperationExecute, "execute" );

const TInt32 KCASpaAppLauncherPlugin = {0x10282E5F};

_LIT( KAll, "all" );

#endif /*GLOBALS_H_*/
