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

#include "menucompletedoperation.h"
#include "mcsmenu.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuCompletedOperation::~CMenuCompletedOperation
// ---------------------------------------------------------
//
EXPORT_C CMenuCompletedOperation::~CMenuCompletedOperation()
    {
    Cancel();
    }

// ---------------------------------------------------------
// CMenuCompletedOperation::NewL
// ---------------------------------------------------------
//
EXPORT_C CMenuCompletedOperation* CMenuCompletedOperation::NewL(
        RMenu &aMenu,
        TInt aPriority,
        TRequestStatus &aObserverStatus,
        TInt aError )
    {
    CMenuCompletedOperation* op = new (ELeave) CMenuCompletedOperation
        ( aMenu, aPriority, aObserverStatus );
    op->ReportCompletion( aError );
    return op;
    }

// ---------------------------------------------------------
// CMenuCompletedOperation::CMenuCompletedOperation
// ---------------------------------------------------------
//
CMenuCompletedOperation::CMenuCompletedOperation
( RMenu &aMenu, TInt aPriority, TRequestStatus &aObserverStatus )
: CMenuOperation( aMenu, aPriority, aObserverStatus )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMenuCompletedOperation::ReportCompletion
// ---------------------------------------------------------
//
void CMenuCompletedOperation::ReportCompletion( TInt aError )
    {
    iObserverStatus = KRequestPending;
    TRequestStatus* ownStatus = &iStatus;
    *ownStatus = KRequestPending;
    SetActive();
    User::RequestComplete( ownStatus, aError );
    }

// ---------------------------------------------------------
// CMenuCompletedOperation::RunL
// ---------------------------------------------------------
//
void CMenuCompletedOperation::RunL()
    {
    // Propagate completion to observer.
    TRequestStatus* status = &iObserverStatus;
    User::RequestComplete( status, iStatus.Int() );
    }

// ---------------------------------------------------------
// CMenuCompletedOperation::DoCancel
// ---------------------------------------------------------
//
void CMenuCompletedOperation::DoCancel()
    {
    TRequestStatus* status = &iObserverStatus;
    User::RequestComplete( status, KErrCancel );
    }

// ---------------------------------------------------------
// CMenuCompletedOperation::RunError
// ---------------------------------------------------------
//
TInt CMenuCompletedOperation::RunError( TInt aError )
    {
    TRequestStatus* status = &iObserverStatus;
    User::RequestComplete( status, aError );

    return KErrNone;
    }
