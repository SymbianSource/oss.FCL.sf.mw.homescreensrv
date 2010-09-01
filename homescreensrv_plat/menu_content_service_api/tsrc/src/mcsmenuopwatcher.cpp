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
* Description:  
*
*/

#include <mcsmenuoperation.h>
#include "mcsmenuopwatcher.h"

// ---------------------------------------------------------------------------
// two-phased constructor
// ---------------------------------------------------------------------------
CMCSMenuOpWatcher* CMCSMenuOpWatcher::NewL( TBool aStopScheduler )
	{
	CMCSMenuOpWatcher* self = new (ELeave) CMCSMenuOpWatcher(aStopScheduler);
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// default constructor
// ---------------------------------------------------------------------------
CMCSMenuOpWatcher::CMCSMenuOpWatcher( TBool aStopScheduler )
	: CActive( CActive::EPriorityStandard ),
	iStopScheduler(aStopScheduler)
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
CMCSMenuOpWatcher::~CMCSMenuOpWatcher()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// second phase constructor
// ---------------------------------------------------------------------------
void CMCSMenuOpWatcher::ConstructL()
	{
	
	}

// ---------------------------------------------------------------------------
// Watch  Async
// ---------------------------------------------------------------------------
void CMCSMenuOpWatcher::Watch( CMenuOperation& aOperation)
    {
    __ASSERT_DEBUG( KRequestPending == iStatus.Int(), User::Invariant() );
    __ASSERT_DEBUG( !iOperation, User::Invariant() ); 
    iOperation = &aOperation;
    SetActive();
    }

// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
void CMCSMenuOpWatcher::RunL()
    {
    delete iOperation;
    iOperation = NULL;    
    //TRAP_IGNORE( iObserver.MenuOpCompletedL(iStatus.Int() ) );
    if (iStopScheduler)
        {
        CActiveScheduler::Stop();
        }
    }

// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
void CMCSMenuOpWatcher::DoCancel()
    {
    //iOperation->Cancel();
    delete iOperation;
    iOperation = NULL;
    }

TInt CMCSMenuOpWatcher::GetStatus()
    {
    return iStatus.Int();
    }