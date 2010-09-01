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

#include "menuclientoperation.h"
#include "menumsg.h"
#include "mcsmenu.h"
#include "menubuf.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RMenuOperation::Close
// ---------------------------------------------------------
//
void RMenuOperation::Close()
    {
    if ( SubSessionHandle() )
        {
        CloseSubSession( EMenuOperationClose );
        }
    }

// ---------------------------------------------------------
// RMenuOperation::CreateRemove
// ---------------------------------------------------------
//
TInt RMenuOperation::CreateRemove( RMenu& aMenu, TInt aId )
    {
    __ASSERT_DEBUG( !SubSessionHandle(), User::Invariant() );
    return CreateSubSession(
        aMenu,
        EMenuOperationCreateRemove,
        TIpcArgs( aId ) );
    }

// ---------------------------------------------------------
// RMenuOperation::CreateMoveToFolder
// ---------------------------------------------------------
//
TInt RMenuOperation::CreateMoveToFolder
( RMenu& aMenu, const RMenuBuf& aItems, TInt aFolder, TInt aMoveBefore )
    {
    __ASSERT_DEBUG( !SubSessionHandle(), User::Invariant() );
    return CreateSubSession(
        aMenu,
        EMenuOperationCreateMoveToFolder,
        TIpcArgs( aItems.SubSessionHandle(), aFolder, aMoveBefore ) );
    }

// ---------------------------------------------------------
// RMenuOperation::CreateReorder
// ---------------------------------------------------------
//
TInt RMenuOperation::CreateReorder
( RMenu& aMenu, TInt aId, TInt aMoveBefore )
    {
    __ASSERT_DEBUG( !SubSessionHandle(), User::Invariant() );
    return CreateSubSession(
        aMenu,
        EMenuOperationCreateReorder,
        TIpcArgs( aId, aMoveBefore ) );
    }

// ---------------------------------------------------------
// RMenuOperation::CreateAdd
// ---------------------------------------------------------
//
TInt RMenuOperation::CreateAdd
( RMenu& aMenu, const TDesC& aType, const RMenuBuf& aChanges, TInt& aId )
    {
    __ASSERT_DEBUG( !SubSessionHandle(), User::Invariant() );
    TPckg<TInt> id( aId );
    id() = 0;
    return CreateSubSession(
        aMenu,
        EMenuOperationCreateAdd,
        TIpcArgs( &aType, aChanges.SubSessionHandle(), &id ) );
    }

// ---------------------------------------------------------
// RMenuOperation::CreateUpdate
// ---------------------------------------------------------
//
TInt RMenuOperation::CreateUpdate
( RMenu& aMenu, TInt aId, const RMenuBuf& aChanges )
    {
    __ASSERT_DEBUG( !SubSessionHandle(), User::Invariant() );
    return CreateSubSession(
        aMenu,
        EMenuOperationCreateUpdate,
        TIpcArgs( aId, aChanges.SubSessionHandle() ) );
    }

// ---------------------------------------------------------
// RMenuOperation::Cancel
// ---------------------------------------------------------
//
void RMenuOperation::Cancel()
    {
    SendReceive( EMenuOperationCancel );
    }

// ---------------------------------------------------------
// RMenuOperation::Start
// ---------------------------------------------------------
//
void RMenuOperation::Start( TRequestStatus &aStatus )
    {
    aStatus = KRequestPending;
    SendReceive( EMenuOperationStart, aStatus );
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuClientOperation::~CMenuClientOperation
// ---------------------------------------------------------
//
CMenuClientOperation::~CMenuClientOperation()
    {
    Cancel();
    iOperation.Close();
    }

// ---------------------------------------------------------
// CMenuClientOperation::CMenuClientOperation
// ---------------------------------------------------------
//
CMenuClientOperation::CMenuClientOperation
( RMenu &aMenu, TInt aPriority, TRequestStatus &aObserverStatus )
: CMenuOperation( aMenu, aPriority, aObserverStatus )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMenuClientOperation::RemoveL
// ---------------------------------------------------------
//
void CMenuClientOperation::RemoveL( TInt aId )
    {
    User::LeaveIfError( iOperation.CreateRemove( iMenu, aId ) );
    Start();
    }

// ---------------------------------------------------------
// CMenuClientOperation::MoveToFolderL
// ---------------------------------------------------------
//
void CMenuClientOperation::MoveToFolderL
( const RMenuBuf& aItems, TInt aFolder, TInt aMoveBefore )
    {
    User::LeaveIfError( iOperation.CreateMoveToFolder
        ( iMenu, aItems, aFolder, aMoveBefore ) );
    Start();
    }

// ---------------------------------------------------------
// CMenuClientOperation::ReorderL
// ---------------------------------------------------------
//
void CMenuClientOperation::ReorderL( TInt aId, TInt aMoveBefore )
    {
    User::LeaveIfError( iOperation.CreateReorder( iMenu, aId, aMoveBefore ) );
    Start();
    }

// ---------------------------------------------------------
// CMenuClientOperation::AddL
// ---------------------------------------------------------
//
TInt CMenuClientOperation::AddL
( const TDesC& aType, const RMenuBuf& aChanges )
    {
    TInt id = 0;
    User::LeaveIfError( iOperation.CreateAdd( iMenu, aType, aChanges, id ) );
    Start();
    return id;
    }

// ---------------------------------------------------------
// CMenuClientOperation::UpdateL
// ---------------------------------------------------------
//
void CMenuClientOperation::UpdateL
( TInt aId, const RMenuBuf& aChanges )
    {
    User::LeaveIfError( iOperation.CreateUpdate( iMenu, aId, aChanges ) );
    Start();
    }

// ---------------------------------------------------------
// CMenuClientOperation::Start
// ---------------------------------------------------------
//
void CMenuClientOperation::Start()
    {
    iObserverStatus = KRequestPending;
    iOperation.Start( iStatus );
    SetActive();
    }

// ---------------------------------------------------------
// CMenuClientOperation::RunL
// ---------------------------------------------------------
//
void CMenuClientOperation::RunL()
    {
    // Propagate completion to observer.
    TRequestStatus* status = &iObserverStatus;
    User::RequestComplete( status, iStatus.Int() );
    }

// ---------------------------------------------------------
// CMenuClientOperation::DoCancel
// ---------------------------------------------------------
//
void CMenuClientOperation::DoCancel()
    {
    iOperation.Cancel();
    // Propagate completion to observer.
    TRequestStatus* status = &iObserverStatus;
    User::RequestComplete( status, KErrCancel );
    }
