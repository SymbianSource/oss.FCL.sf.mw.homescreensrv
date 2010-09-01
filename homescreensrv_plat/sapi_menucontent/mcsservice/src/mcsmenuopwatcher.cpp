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

#include "mcsmenuoperation.h"
#include "mcsmenuopwatcher.h"
#include "mcsmenuopobserver.h"
#include "mcsoperationmanager.h"

// ---------------------------------------------------------------------------
// two-phased constructor
// ---------------------------------------------------------------------------
CMCSMenuOpWatcher* CMCSMenuOpWatcher::NewL(  MMCSMenuOpObserver& aObserver, 
                                RMcsOperationManager& aOperationManager )
	{
	CMCSMenuOpWatcher* self = new (ELeave) CMCSMenuOpWatcher( aObserver, 
	                                            aOperationManager );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// default constructor
// ---------------------------------------------------------------------------
CMCSMenuOpWatcher::CMCSMenuOpWatcher( MMCSMenuOpObserver& aObserver,   
                                    RMcsOperationManager& aOperationManager )
	: CActive( CActive::EPriorityStandard ), iObserver( aObserver ),
	    					iOperationManager(aOperationManager)						
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
// CMCSMenuOpWatcher::RunError
// ---------------------------------------------------------------------------
//
TInt CMCSMenuOpWatcher::RunError( TInt aError )
    {
    TRAP_IGNORE( iOperationManager.AddL( this ) );
    if ( (aError != KErrDiskFull) && (aError != KErrNoMemory))
    	{
    	aError = KErrNone;
    	}
    return aError;
    }

// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
void CMCSMenuOpWatcher::RunL()
    {
    delete iOperation;
    iOperation = NULL; 
    iOperationManager.Remove( this );//delete this object and remove it 
                                         //from operation manager      
    TRAP_IGNORE( iObserver.MenuOpCompletedL(iStatus.Int() ) );
    User::LeaveIfError( iStatus.Int() );
    
    delete this;
    }

// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
void CMCSMenuOpWatcher::DoCancel()
    {
    delete iOperation;
    iOperation = NULL;
    iOperationManager.Remove( this );//delete this object and remove it 
    }

