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
* Description:  Menu item operation watcher. 
*
*/

#include <mcsmenuoperation.h>
#include "hs_app_mcspluginwatcher.h"
#include "../publisher/inc/mcspluginengine.h"

// ---------------------------------------------------------------------------
// two-phased constructor
// ---------------------------------------------------------------------------
CMCSPluginWatcher* CMCSPluginWatcher::NewL( const Type& aType )
	{
	CMCSPluginWatcher* self = new (ELeave) CMCSPluginWatcher( aType );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// default constructor
// ---------------------------------------------------------------------------
CMCSPluginWatcher::CMCSPluginWatcher( const Type& aType )
	: CActive( CActive::EPriorityStandard ),
	iType( aType )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
CMCSPluginWatcher::~CMCSPluginWatcher()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// second phase constructor
// ---------------------------------------------------------------------------
void CMCSPluginWatcher::ConstructL()
	{
	
	}

// ---------------------------------------------------------------------------
// Watch  Async
// ---------------------------------------------------------------------------
void CMCSPluginWatcher::Watch( CMenuOperation* aOperation)
    {
    __ASSERT_DEBUG( KRequestPending == iStatus.Int(), User::Invariant() );
    //__ASSERT_DEBUG( !iOperation, User::Invariant() ); 
    iOperation = aOperation;
    SetActive();
    }

// ---------------------------------------------------------------------------
// Watch  Async
// ---------------------------------------------------------------------------
void CMCSPluginWatcher::WatchNotify( MMCSPluginWatcherObserver* aObserver )
    {
    __ASSERT_DEBUG( KRequestPending == iStatus.Int(), User::Invariant() );
    //__ASSERT_DEBUG( !iOperation, User::Invariant() ); 
    iObserver = aObserver;
    SetActive();
    }

void CMCSPluginWatcher::StopAndWatch( CMenuOperation* aOperation, 
                                      CActiveSchedulerWait* aWaitScheduler )
    {
    __ASSERT_DEBUG( KRequestPending == iStatus.Int(), User::Invariant() );
    iWaitScheduler = aWaitScheduler;
    iOperation = aOperation;
    SetActive();
    }

// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
void CMCSPluginWatcher::RunL()
    {
    delete iOperation;
    iOperation = NULL;  
    
    if( iType == ENotify )
        {
        iObserver->HandleNotifyL();
        }
    if ( iWaitScheduler && iWaitScheduler->IsStarted() )
        {
        Cancel();
        iWaitScheduler->AsyncStop();
        }
    //CActiveScheduler::Stop();
    }

// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
void CMCSPluginWatcher::DoCancel()
    {
    delete iOperation;
    iOperation = NULL;
    }

TInt CMCSPluginWatcher::GetStatus()
    {
    return iStatus.Int();
    }

