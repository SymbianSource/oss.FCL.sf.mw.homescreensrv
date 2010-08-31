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
* Description: cadrmscanner.mmp
*
*/

#include <apgwgnam.h>

#include "carunningappmonitor.h"
#include "cadef.h"
#include "castoragetask.h"
#include "cainnerquery.h"
#include "cainnerentry.h"
#include "caarraycleanup.inl"
#include "castorageproxy.h"

const TInt KRunningAppGranularity(10);
// -----------------------------------------------------------------------------
// ExecuteL
// -----------------------------------------------------------------------------
//
void CCaRunningAppMonitor::ExecuteL( MCaRunningTaskHandler& aTaskHandler,
                                     CCaStorageProxy& aStorage )
    {
    CCaRunningAppMonitor* self = 
        new (ELeave)CCaRunningAppMonitor( aTaskHandler, aStorage );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    }

// -----------------------------------------------------------------------------
// ~CCaRunningAppMonitor
// -----------------------------------------------------------------------------
//
CCaRunningAppMonitor::~CCaRunningAppMonitor()
    {
    Cancel();
    iWg.Close();
    iSession.Close();
    iRunningTasks.Close();
    }

// -----------------------------------------------------------------------------
// CCaRunningAppMonitor
// -----------------------------------------------------------------------------
//
CCaRunningAppMonitor::CCaRunningAppMonitor( MCaRunningTaskHandler& aTaskHandler, 
                                            CCaStorageProxy& aStorage )
:
CActive( EPriorityStandard ),
iTaskHandler( aTaskHandler ),
iStorage( aStorage )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CCaRunningAppMonitor::ConstructL()
    {
    //Create window server observer
    ConstractObserverL();
    
    //start listen
    Subscribe();
    
    //At plugin start mark all entry as "not running"
    ResetStorageL();
    
    iTaskHandler.RegisterTaskL(this);//this operation has to be as a last one
    }

// -----------------------------------------------------------------------------
// UpdateRunningAppStorageL
// -----------------------------------------------------------------------------
//
void CCaRunningAppMonitor::UpdateRunningAppStorageL()
    {
    RArray<RWsSession::TWindowGroupChainInfo> 
        windowGroupIds(KRunningAppGranularity, 
                       _FOFF( RWsSession::TWindowGroupChainInfo, iId ));
    CleanupClosePushL( windowGroupIds );
    User::LeaveIfError( iSession.WindowGroupList( 0, &windowGroupIds ) );
    TInt itemIndex( KErrNotFound );
    
    //Verify what apps has stopped
    THashMapIter<TInt, TUid> iterator( iRunningTasks );
    RWsSession::TWindowGroupChainInfo item = { KErrNotFound, KErrNotFound };
    while( 0 != iterator.NextKey() )
        {
        item.iId = *iterator.CurrentKey();
        itemIndex = windowGroupIds.Find(item);
        if(KErrNotFound == itemIndex)
            {
            //push new task to register application update in storage
            CCaStorageTask::ExecuteL( iTaskHandler,
                                      iStorage,
                                      *iterator.CurrentValue(),
                                      KErrNotFound );
            iterator.RemoveCurrent();
            }
        else
            {
            //application is already registered.
            //remove entry to skip its processing
            windowGroupIds.Remove( itemIndex );
            }
        }
    
    //Verify what apps has started
    const TInt count(windowGroupIds.Count());
    for(TInt iter(0); iter < count; ++iter)
        {
        if( 0 >= windowGroupIds[iter].iParentId )
            {
            //push new task to register application update in storage and
            //register it running task list 
            CApaWindowGroupName* name = 
                CApaWindowGroupName::NewLC( iSession, windowGroupIds[iter].iId);
            if( 0 != name->AppUid().iUid )
                {
                CCaStorageTask::ExecuteL( iTaskHandler,
                                          iStorage,
                                          name->AppUid(),
                                          windowGroupIds[iter].iId );
                iRunningTasks.Insert( windowGroupIds[iter].iId,
                                      name->AppUid() );
                }
            CleanupStack::PopAndDestroy( name );
            }
        }
    CleanupStack::PopAndDestroy( &windowGroupIds );
    }

// -----------------------------------------------------------------------------
// Subscribe
// -----------------------------------------------------------------------------
//
void CCaRunningAppMonitor::Subscribe()
    {
    if( !IsActive() )
        {
        iSession.EventReady( &iStatus );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// ConstractObserverL
// -----------------------------------------------------------------------------
//
void CCaRunningAppMonitor::ConstractObserverL()
{
    User::LeaveIfError( iSession.Connect() );
    iWg = RWindowGroup( iSession );
    User::LeaveIfError( iWg.Construct( 
                        reinterpret_cast<TUint32>( &iWg ) ) );
    iWg.DisableFocusChangeEvents();
    iWg.DisableModifierChangedEvents();
    iWg.DisableOnEvents();
    iWg.DisableScreenChangeEvents();

    //enable notifications about group lists
    User::LeaveIfError( iWg.EnableGroupListChangeEvents() );
}

// -----------------------------------------------------------------------------
// ResetStorageL
// -----------------------------------------------------------------------------
//
void CCaRunningAppMonitor::ResetStorageL()
{
    CCaInnerQuery* allAppQuery = CCaInnerQuery::NewLC();
    allAppQuery->SetFlagsOn( ERunning );
    RPointerArray<CCaInnerEntry> entries;
    CleanupResetAndDestroyPushL( entries );
    iStorage.GetEntriesL( allAppQuery, entries );
    for( TInt i(0); i < entries.Count(); ++i )
        {
        CCaStorageTask::ExecuteL( iTaskHandler,
                                  iStorage,
                                  TUid::Uid(entries[i]->GetUid()),
                                  KErrNotFound );
        }
    CleanupStack::PopAndDestroy(&entries);
    CleanupStack::PopAndDestroy(allAppQuery);
}

// -----------------------------------------------------------------------------
// RunL
// -----------------------------------------------------------------------------
//
void CCaRunningAppMonitor::RunL()
    {
    User::LeaveIfError( iStatus.Int() );
    TWsEvent event;
    iSession.GetEvent( event );
    Subscribe();//new subscribtion has to be called after picking event
    if( EEventWindowGroupListChanged == event.Type() )
        {
        UpdateRunningAppStorageL();
        }
    }

// -----------------------------------------------------------------------------
// RunError
// -----------------------------------------------------------------------------
//
TInt CCaRunningAppMonitor::RunError( TInt /*aError*/ )
    {
    //ignore error
    Subscribe();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// DoCancel
// -----------------------------------------------------------------------------
//
void CCaRunningAppMonitor::DoCancel()
    {
    if( IsActive() )
        {
        iSession.EventReadyCancel();
        }
    }
