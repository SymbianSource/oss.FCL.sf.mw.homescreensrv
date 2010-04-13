/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   View UIDs for GS FW
* These UIDs are used to identify the parent of the view/plugin. The same UID
* is used for both plugin and view identification.
*
*/


#ifndef GSFWVIEWUIDS_H
#define GSFWVIEWUIDS_H


// Use this UID if plugin belongs to Main view:
const TUid KGSMainViewUid = { 0x1020723B };


// No item is selected in the container's listbox
const TUid KGSNoneSelected = { 0x00000000 };

// UIDs used by the sub-view plugins:

// Use this UID if plugin belongs to General view:
const TUid KGSGenPluginUid  = { 0x10207237 };

// Use this UID if plugin belongs to Telephony view:
const TUid KGSTelPluginUid  = { 0x1020723D };

// Use this UID if plugin belongs to Connection view:
const TUid KGSConPluginUid  = { 0x10207250 };

// Use this UID if plugin belongs to Applications view:
const TUid KGSAppsPluginUid = { 0x10207239 };

// Use this UID if plugin belongs to Personalisation view:
const TUid KGSPrslnPluginUid = { 0x10207252 };

// Use this UID if plugin belongs to Security view:
const TUid KGSSecurityPluginUid = { 0x1020743A };

// Use this UID if plugin belongs to Standby view:
const TUid KGSStandbyPluginUid = { 0x1020743F };

// UID of General Settings application.
// This UID is used for accessing the help IDs controlled by GS help files
const TUid KUidGS = { 0x100058EC };

// Use this UID if plugin belongs to Packet Data:
const TUid KGSPDataPluginUid = { 0x1020724E };

// Use this UID if plugin belongs to Interaction:
const TUid KGSInteractionPluginUid = { 0x10283343 };

// Use this UID if plugin belongs to Administration:
const TUid KGSAdminPluginUid = { 0x10283341 };

// Use this UID if plugin belongs to Device Management:
const TUid KGSDeviceManagementPluginUid = { 0x10283317 };

// Use this UID if plugin belongs to Device Updates:
const TUid KGSDeviceUpdatesPluginUid = { 0x1028331B };

// Use this UID if plugin belongs to Synchronisation view:
const TUid KGSSynchronisationPluginUid = { 0x10283319 };

// Use this UID if plugin belongs to Installations view:
const TUid KGSInstallationsPluginUid = { 0x10283321 };

#endif // GSFWVIEWUIDS_H
// End of File
