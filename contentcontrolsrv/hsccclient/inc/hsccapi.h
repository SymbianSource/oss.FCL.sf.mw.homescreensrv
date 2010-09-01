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
* Description:  Homescreen content control server API definitions
*
*/

#ifndef HSCCAPI_H
#define HSCCAPI_H

// System includes
#include <e32base.h>
#include <ccsrvapi.h>
// Forward declarations

// Constants

// ----------------------------------------------------------------------------
// Home screen cotent control server messages
// ----------------------------------------------------------------------------
enum THsCcSrvMessages
    {
// ----------------------------------------------------------------------------
// WidgetListChangedNtf
// ----------------------------------------------------------------------------
// Purpose:
// This notification is used to inform api observer of changed widget list
//
// Message parameters:
// - Message id:     EHsCcWidgetListChangedNtf
// - Transaction id: None
// - Message data:   None
    EHsCcWidgetListChangedNtf = ECcProviderMessageBase,
// ----------------------------------------------------------------------------
// ViewListChangedNtf
// ----------------------------------------------------------------------------
// Purpose:
// This notification is used to inform api observer of changed view list
//
// Message parameters:
// - Message id:     EHsCcViewListChangedNtf
// - Transaction id: None
// - Message data:   None
    EHsCcViewListChangedNtf,
// ----------------------------------------------------------------------------
// AppListChangedNtf
// ----------------------------------------------------------------------------
// Purpose:
// This notification is used to inform api observer of changed application list
//
// Message parameters:
// - Message id:     EHsCcAppListChangedNtf
// - Transaction id: None
// - Message data:   None
    EHsCcAppListChangedNtf,
// ----------------------------------------------------------------------------
// WidgetListReq
// ----------------------------------------------------------------------------
// Purpose:
// This request is used to get a list of installed Home screen widgets
//
// Message parameters:
// - Message id:     EHsCcWidgetListReq
// - Transaction id: Unique transaction id
// - Message data:   None
    EHsCcWidgetListReq,
// ----------------------------------------------------------------------------
// WidgetListResp
// ----------------------------------------------------------------------------
// Purpose:
// This response contains the list of installed widgets
//
// Message parameters:
// - Message id:     EHsCcWidgetListResp
// - Transaction id: Transaction id from corresponding WidgetListReq
// - Status:         Operation status (Symbian error code)
// - Message data:   Successful operation: CHsContentInfoArray instance 
//                   containing installed widgets
//                   Failed operation: None
    EHsCcWidgetListResp,
// ----------------------------------------------------------------------------
// AddWidgetReq
// ----------------------------------------------------------------------------
// Purpose:
// This request is used to add a widget to Home screen
//
// Message parameters:
// - Message id:     EHsCcAddWidgetReq
// - Transaction id: Unique transaction id
// - Message data:   CHsContentInfo instance defining the added widget
    EHsCcAddWidgetReq,
// ----------------------------------------------------------------------------
// AddWidgetResp
// ----------------------------------------------------------------------------
// Purpose:
// This response contains the status of adding widget
//
// Message parameters:
// - Message id:     EHsCcAddWidgetResp
// - Transaction id: Transaction id from corresponding AddWidgetReq
// - Status:         Operation status (Symbian error code)
// - Message data:   None
    EHsCcAddWidgetResp,
// ----------------------------------------------------------------------------
// RemoveWidgetReq
// ----------------------------------------------------------------------------
// Purpose:
// This request is used to remove a widget from Home screen
//
// Message parameters:
// - Message id:     EHsCcRemoveWidgetReq
// - Transaction id: Unique transaction id
// - Message data:   CHsContentInfo instance defining the removed widget
    EHsCcRemoveWidgetReq,
// ----------------------------------------------------------------------------
// RemoveWidgetResp
// ----------------------------------------------------------------------------
// Purpose:
// This response contains the status of removing widget
//
// Message parameters:
// - Message id:     EHsCcRemoveWidgetResp
// - Transaction id: Transaction id from corresponding RemoveWidgetReq
// - Status:         Operation status (Symbian error code)
// - Message data:   None
    EHsCcRemoveWidgetResp,
// ----------------------------------------------------------------------------
// ViewListReq
// ----------------------------------------------------------------------------
// Purpose:
// This request is used to get a list of installed Home screen views
//
// Message parameters:
// - Message id:     EHsCcViewListReq
// - Transaction id: Unique transaction id
// - Message data:   None
    EHsCcViewListReq,
// ----------------------------------------------------------------------------
// ViewListResp
// ----------------------------------------------------------------------------
// Purpose:
// This response contains the list of installed views
//
// Message parameters:
// - Message id:     EHsCcViewListResp
// - Transaction id: Transaction id from corresponding ViewListReq
// - Status:         Operation status (Symbian error code)
// - Message data:   Successful operation: CHsContentInfoArray instance 
//                   containing installed views
//                   Failed operation: None
    EHsCcViewListResp,
// ----------------------------------------------------------------------------
// AddViewReq
// ----------------------------------------------------------------------------
// Purpose:
// This request is used to add a view to Home screen
//
// Message parameters:
// - Message id:     EHsCcAddViewReq
// - Transaction id: Unique transaction id
// - Message data:   CHsContentInfo instance defining the added view
    EHsCcAddViewReq,
// ----------------------------------------------------------------------------
// AddViewResp
// ----------------------------------------------------------------------------
// Purpose:
// This response contains the status of adding view
//
// Message parameters:
// - Message id:     EHsCcAddViewResp
// - Transaction id: Transaction id from corresponding AddViewReq
// - Status:         Operation status (Symbian error code)
// - Message data:   None
    EHsCcAddViewResp,
// ----------------------------------------------------------------------------
// RemoveViewReq
// ----------------------------------------------------------------------------
// Purpose:
// This request is used to remove a view from Home screen
//
// Message parameters:
// - Message id:     EHsCcRemoveViewReq
// - Transaction id: Unique transaction id
// - Message data:   CHsContentInfo instance defining the removed view
    EHsCcRemoveViewReq,
// ----------------------------------------------------------------------------
// RemoveViewResp
// ----------------------------------------------------------------------------
// Purpose:
// This response contains the status of removing view
//
// Message parameters:
// - Message id:     EHsCcRemoveViewResp
// - Transaction id: Transaction id from corresponding RemoveViewReq
// - Status:         Operation status (Symbian error code)
// - Message data:   None
    EHsCcRemoveViewResp,
// ----------------------------------------------------------------------------
// ActivateViewReq
// ----------------------------------------------------------------------------
// Purpose:
// This request is used to activate a Home screen view
//
// Message parameters:
// - Message id:     EHsCcActivateViewReq
// - Transaction id: Unique transaction id
// - Message data:   CHsContentInfo instance defining the activated view
    EHsCcActivateViewReq,
// ----------------------------------------------------------------------------
// ActivateViewResp
// ----------------------------------------------------------------------------
// Purpose:
// This response contains the status of activating view
//
// Message parameters:
// - Message id:     EHsCcActivateViewResp
// - Transaction id: Transaction id from corresponding ActivateViewReq
// - Status:         Operation status (Symbian error code)
// - Message data:   None
    EHsCcActivateViewResp,
// ----------------------------------------------------------------------------
// ActiveViewReq
// ----------------------------------------------------------------------------
// Purpose:
// This request is used to get active view configuration
//
// Message parameters:
// - Message id:     EHsCcActiveViewReq
// - Transaction id: Unique transaction id
// - Message data:   None
    EHsCcActiveViewReq,
// ----------------------------------------------------------------------------
// ActiveViewResp
// ----------------------------------------------------------------------------
// Purpose:
// This response contains the active view configuration
//
// Message parameters:
// - Message id:     EHsCcActiveViewResp
// - Transaction id: Transaction id from corresponding ActiveViewReq
// - Status:         Operation status (Symbian error code)
// - Message data:   Successful operation: CHsContentInfo instance defining the 
//                   active view configuration
//                   Failed operation: None
    EHsCcActiveViewResp,
// ----------------------------------------------------------------------------
// AppListReq
// ----------------------------------------------------------------------------
// Purpose:
// This request is used to get a list of installed Home screen application
// configurations
//
// Message parameters:
// - Message id:     EHsCcAppListReq
// - Transaction id: Unique transaction id
// - Message data:   None
    EHsCcAppListReq,
// ----------------------------------------------------------------------------
// AppListResp
// ----------------------------------------------------------------------------
// Purpose:
// This response contains the list of installed application configurations
//
// Message parameters:
// - Message id:     EHsCcAppListResp
// - Transaction id: Transaction id from corresponding AppListReq
// - Status:         Operation status (Symbian error code)
// - Message data:   Successful operation: CHsContentInfoArray instance 
//                   containing installed application configurations
//                   Failed operation: None
    EHsCcAppListResp,
// ----------------------------------------------------------------------------
// ActivateAppReq
// ----------------------------------------------------------------------------
// Purpose:
// This request is used to activate a Home screen application configuration
//
// Message parameters:
// - Message id:     EHsCcActivateAppReq
// - Transaction id: Unique transaction id
// - Message data:   CHsContentInfo instance defining the activated application
//                   configuration
    EHsCcActivateAppReq,
// ----------------------------------------------------------------------------
// ActivateAppResp
// ----------------------------------------------------------------------------
// Purpose:
// This response contains the status of activating application configuration
//
// Message parameters:
// - Message id:     EHsCcActivateAppResp
// - Transaction id: Transaction id from corresponding ActivateAppReq
// - Status:         Operation status (Symbian error code)
// - Message data:   None
    EHsCcActivateAppResp,
// ----------------------------------------------------------------------------
// ActiveAppReq
// ----------------------------------------------------------------------------
// Purpose:
// This request is used to get active application configuration
//
// Message parameters:
// - Message id:     EHsCcActivateAppReq
// - Transaction id: Unique transaction id
// - Message data:   None
    EHsCcActiveAppReq,
// ----------------------------------------------------------------------------
// ActiveAppResp
// ----------------------------------------------------------------------------
// Purpose:
// This response contains the active application configuration
//
// Message parameters:
// - Message id:     EHsCcActiveAppResp
// - Transaction id: Transaction id from corresponding ActiveAppReq
// - Status:         Operation status (Symbian error code)
// - Message data:   Successful operation: CHsContentInfo instance defining the 
//                   active application configuration
//                   Failed operation: None
    EHsCcActiveAppResp,
// ----------------------------------------------------------------------------
// NotSupportedResp
// ----------------------------------------------------------------------------
// Purpose:
// This request is used to get a list of installed Home screen widgets
//
// Message parameters:
// - Message id:     EHsCcNotSupportedResp
// - Transaction id: Transaction id from corresponding not supported request
// - Status:         Operation status KErrNotSupported
// - Message data:   None
    EHsCcNotSupportedResp
    };

#endif // HSCCAPI_H

// End of file
