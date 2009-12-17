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
* Description:  Backup and restore events listener.
 *
*/


#include <connect/sbdefs.h>

#include "cpserverburlistener.h"
#include "cpdebug.h"
// conn - "Declaration of Types and Constants for Secure Backup and Restore" namespace, see sbdefs.h
using namespace conn;

// ============================ MEMBER FUNCTIONS =============================

// -----------------------------------------------------------------------------
// CSpaceDataStorageBURListener::CSpaceDataStorageBURListener
// C++ default constructor
// -----------------------------------------------------------------------------
//
CCPServerBURListener::CCPServerBURListener( MBURListenerCallback* aCallback ) :
    CActive(EPriorityNormal)
    {
    CActiveScheduler::Add( this );
    iCallback = aCallback;

    // Prepare automatically
    iProperty.Attach( KUidSystemCategory, KUidBackupRestoreKey );
    SetActive( );
    iProperty.Subscribe( iStatus );
    }

// -----------------------------------------------------------------------------
// CSpaceDataStorageBURListener::ConstructL
// S2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CCPServerBURListener::ConstructL()
    {

    }

// ---------------------------------------------------------------------------
// CSpaceDataStorageBURListener::NewL
// ---------------------------------------------------------------------------
//
CCPServerBURListener* CCPServerBURListener::NewL(
    MBURListenerCallback* aCallback )
    {
    CCPServerBURListener* self = new (ELeave)
    CCPServerBURListener( aCallback );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CSpaceDataStorageBURListener::~CSpaceDataStorageBURListener
// ---------------------------------------------------------------------------
//
CCPServerBURListener::~CCPServerBURListener()
    {
    Cancel( );
    iProperty.Close( );
    }

// ---------------------------------------------------------------------------
// CSpaceDataStorageBURListener::DoCancel
// ---------------------------------------------------------------------------
//
void CCPServerBURListener::DoCancel()
    {
    iProperty.Cancel( );
    }

// ---------------------------------------------------------------------------
// CSpaceDataStorageBURListener::RunError
// ---------------------------------------------------------------------------
//
TInt CCPServerBURListener::RunError( TInt /*aError*/)
    {
    // No need to do anything      
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCPServerBURListener::CheckStatus
// ---------------------------------------------------------------------------
//	
TBool CCPServerBURListener::CheckBUR()
    {
    TInt pnsStatus;
    TInt err;
    err = iProperty.Get( KUidSystemCategory, KUidBackupRestoreKey, pnsStatus );
    if ( pnsStatus < EBURBackupFull && err >= KErrNone )
        {
        return EFalse;
        }
    else
        {
        return ETrue;
        }
    }

// ---------------------------------------------------------------------------
// CSpaceDataStorageBURListener::RunL
// ---------------------------------------------------------------------------
//
void CCPServerBURListener::RunL()
    {
    CP_DEBUG( _L8("CCPServerBURListener::RunL()") );
    // Re-issue request before notifying
    SetActive( );
    iProperty.Subscribe( iStatus );

    TInt pnsStatus;
    User::LeaveIfError( iProperty.Get( KUidSystemCategory,
        KUidBackupRestoreKey, pnsStatus ) );

    MBURListenerCallback::TBURStatus
            burStatus(MBURListenerCallback::EBURStatusNone);
    if ( pnsStatus & (EBURBackupFull | EBURBackupPartial ) )
        {
        burStatus = MBURListenerCallback::EBURStatusBackup;
        }
    else if ( pnsStatus & (EBURRestoreFull | EBURRestorePartial ) )
        {
        burStatus = MBURListenerCallback::EBURStatusRestore;
        }

    iCallback->HandleBUREventL( burStatus );
    }

// End of File
