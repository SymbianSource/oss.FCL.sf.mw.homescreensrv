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
* Description: castoragetask.cpp
*
*/
#include <apgwgnam.h>
#include <s32strm.h>

#include "castoragetask.h"
#include "cadef.h"
#include "cainnerquery.h"
#include "cainnerentry.h"
#include "castorageproxy.h"
#include "carunningtaskhandler.h"
#include "caarraycleanup.inl"
#include "cadef.h"

const TInt KDelayTaskRetry(10000);
const TInt KMaxTaskRetry(1);

// -----------------------------------------------------------------------------
// ExecuteL
// -----------------------------------------------------------------------------
//
void CCaStorageTask::ExecuteL( MCaRunningTaskHandler& aTaskHandler,
                               CCaStorageProxy& aStorage,
                               TUid aUid,
                               TInt aWgi )
    {
    CCaStorageTask* self = 
        new (ELeave)CCaStorageTask( aTaskHandler, aStorage, aUid, aWgi );
    CleanupStack::PushL( self );
    self->ConstructL();
    //task instance is registered on task handler. shouldn't be deleted here
    CleanupStack::Pop( self );
    }

// -----------------------------------------------------------------------------
// ~CCaStorageTask
// -----------------------------------------------------------------------------
//
CCaStorageTask::~CCaStorageTask()
    {
    Cancel();
    iTimer.Close();
    }

// -----------------------------------------------------------------------------
// CCaStorageTask
// -----------------------------------------------------------------------------
//
CCaStorageTask::CCaStorageTask( MCaRunningTaskHandler& aTaskHandler,
                                CCaStorageProxy& aStorage,
                                TUid aUid,
                                TInt aWgId )
:
CActive( EPriorityLow ),
iTaskHandler( aTaskHandler ),
iStorage( aStorage ),
iUid( aUid ),
iWgId( aWgId )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CCaStorageTask::ConstructL()
    {
    User::LeaveIfError( iTimer.CreateLocal() );
    iStatus = KRequestPending;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    iTaskHandler.RegisterTaskL( this );
    }

// -----------------------------------------------------------------------------
// UnregisterD
// -----------------------------------------------------------------------------
//
void CCaStorageTask::UnregisterD()
    {
    iTaskHandler.UnregisterTask( this );
    delete this;
    }

// -----------------------------------------------------------------------------
// GetEntriesL
// -----------------------------------------------------------------------------
//
void CCaStorageTask::GetEntriesL( RPointerArray<CCaInnerEntry>& aArray )
    {
    CCaInnerQuery* allAppQuery = CCaInnerQuery::NewLC();
    allAppQuery->SetUid( static_cast<TUint>( iUid.iUid ) );
    iStorage.GetEntriesL( allAppQuery, aArray );
    CleanupStack::PopAndDestroy( allAppQuery );
    }

// -----------------------------------------------------------------------------
// UpdateEntryL
// -----------------------------------------------------------------------------
//
TInt CCaStorageTask::UpdateEntryL()
    {
    TInt retVal( KErrNone );
    RPointerArray<CCaInnerEntry> entries;
    CleanupResetAndDestroyPushL( entries );
    GetEntriesL( entries );
    //update only first found entry. othere are just ignored
    retVal = (0 < entries.Count()) ? UpdateEntryL( *entries[0] ) : KErrNotFound;
    CleanupStack::PopAndDestroy( &entries );
    return retVal;
    }

// -----------------------------------------------------------------------------
// UpdateEntryL
// -----------------------------------------------------------------------------
//
TInt CCaStorageTask::UpdateEntryL( CCaInnerEntry& aEntry )
    {
    if(KErrNotFound == iWgId)
        {
        aEntry.SetFlags( aEntry.GetFlags() & ~ERunning );
        RBuf currValue;//temporary variable required to verify if attr exists
        CleanupClosePushL( currValue );
        currValue.CreateL( KCaMaxAttrValueLen );
        if( aEntry.FindAttribute( KCaAttrWindowGroupId, currValue ) )
            {
            aEntry.RemoveAttributeL( KCaAttrWindowGroupId );
            }
        CleanupStack::PopAndDestroy( &currValue );
        }
    else
        {
        aEntry.SetFlags(aEntry.GetFlags() | ERunning);
        RBuf newValue;
        CleanupClosePushL(newValue);
        newValue.CreateL(KCaMaxAttrValueLen);
        newValue.Num(iWgId);
        aEntry.AddAttributeL( KCaAttrWindowGroupId, newValue );
        CleanupStack::PopAndDestroy( &newValue );
        }
    iStorage.AddL( &aEntry );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RunL
// -----------------------------------------------------------------------------
//
void CCaStorageTask::RunL()
    {
    User::LeaveIfError( iStatus.Int() );
    if( KErrNone == UpdateEntryL() ||  KMaxTaskRetry <= iCount++ )
        {
        UnregisterD();
        }
    else
        {
        iTimer.After( iStatus, TTimeIntervalMicroSeconds32( KDelayTaskRetry ) );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// RunError
// -----------------------------------------------------------------------------
//
TInt CCaStorageTask::RunError( TInt /*aError*/ )
    {
    UnregisterD();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// DoCancel
// -----------------------------------------------------------------------------
//
void CCaStorageTask::DoCancel()
    {
    iTimer.Cancel();
    }
