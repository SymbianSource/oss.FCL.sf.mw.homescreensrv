/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

// INCLUDE FILES

#include "hs_app_mcsplugincompletedoperation.h"
#include <mcsmenu.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMCSPluginCompletedOperation::~CMCSPluginCompletedOperation
// ---------------------------------------------------------
//
CMCSPluginCompletedOperation::~CMCSPluginCompletedOperation()
    {
    Cancel();
    }

// ---------------------------------------------------------
// CMCSPluginCompletedOperation::NewL
// ---------------------------------------------------------
//
CMCSPluginCompletedOperation* CMCSPluginCompletedOperation::NewL(
        RMenu &aMenu,
        TInt aPriority,
        TRequestStatus &aObserverStatus,
        TInt aError )
    {
    CMCSPluginCompletedOperation* op = new (ELeave) CMCSPluginCompletedOperation
        ( aMenu, aPriority, aObserverStatus );
    op->ReportCompletion( aError );
    return op;
    }

// ---------------------------------------------------------
// CMCSPluginCompletedOperation::CMCSPluginCompletedOperation
// ---------------------------------------------------------
//
CMCSPluginCompletedOperation::CMCSPluginCompletedOperation
( RMenu &aMenu, TInt aPriority, TRequestStatus &aObserverStatus )
: CMenuOperation( aMenu, aPriority, aObserverStatus )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMCSPluginCompletedOperation::ReportCompletion
// ---------------------------------------------------------
//
void CMCSPluginCompletedOperation::ReportCompletion( TInt aError )
    {
    iObserverStatus = KRequestPending;
    TRequestStatus* ownStatus = &iStatus;
    *ownStatus = KRequestPending;
    SetActive();
    User::RequestComplete( ownStatus, aError );
    }

// ---------------------------------------------------------
// CMCSPluginCompletedOperation::RunL
// ---------------------------------------------------------
//
void CMCSPluginCompletedOperation::RunL()
    {
    // Propagate completion to observer.
    TRequestStatus* status = &iObserverStatus;
    User::RequestComplete( status, iStatus.Int() );
    }

// ---------------------------------------------------------
// CMCSPluginCompletedOperation::DoCancel
// ---------------------------------------------------------
//
void CMCSPluginCompletedOperation::DoCancel()
    {
    TRequestStatus* status = &iObserverStatus;
    User::RequestComplete( status, KErrCancel );
    }

// ---------------------------------------------------------
// CMCSPluginCompletedOperation::RunError
// ---------------------------------------------------------
//
TInt CMCSPluginCompletedOperation::RunError( TInt aError )
    {
    TRequestStatus* status = &iObserverStatus;
    User::RequestComplete( status, aError );

    return KErrNone;
    }
