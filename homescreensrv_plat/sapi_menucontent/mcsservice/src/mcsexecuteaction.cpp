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

#include "mcsdef.h"
#include "mcsmenu.h"
#include "mcsmenufilter.h"
#include "mcsexecuteaction.h"
#include "mcsservice.h"
#include "mcsconstants.h"
#include "mcsmenuopwatcher.h"
#include "mcsoperationmanager.h"

// ---------------------------------------------------------------------------
// two-phased constructor
// ---------------------------------------------------------------------------
CMCSExecuteAction* CMCSExecuteAction::NewL( RMenu& aMCS, MMCSCallback* aCallback,
                                    RMcsOperationManager& aOperationManager )
	{
	CMCSExecuteAction* self = new (ELeave) CMCSExecuteAction( aMCS, aCallback, 
	                                                        aOperationManager );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}
	
// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
CMCSExecuteAction::~CMCSExecuteAction()
	{
    Cancel();
	}

// ---------------------------------------------------------------------------
// default constructor
// ---------------------------------------------------------------------------
CMCSExecuteAction::CMCSExecuteAction( RMenu& aMCS, MMCSCallback* aCallback,  
                                   RMcsOperationManager& aOperationManager ) :
						CActive( EPriorityStandard ),
						iCallback( aCallback ),
						iMCS( aMCS ),
    					iOperationManager(aOperationManager)						
	{
	}

// ---------------------------------------------------------------------------
// second phase constructor
// ---------------------------------------------------------------------------
void CMCSExecuteAction::ConstructL()
	{

	}

// ---------------------------------------------------------------------------
// ExecuteActionL - Async 
// ---------------------------------------------------------------------------
void CMCSExecuteAction::ExecuteActionL( TInt aItemId, TDesC8& aAction)
    {
    if( !iCallback )
        {
        User::Leave( KErrArgument );
        }
    iItemId = aItemId;
    iAction.Set(aAction);

    CActiveScheduler::Add ( this );
    ActivateRequest( KErrNone );
    }

// ---------------------------------------------------------------------------
// ExecuteL - Sync
// ---------------------------------------------------------------------------
void CMCSExecuteAction::ExecuteL(TInt aItemId, TDesC8& aAction )
    {
    CMenuItem* item = CMenuItem::OpenL( iMCS, aItemId );
	CleanupStack::PushL( item );

	CMCSMenuOpWatcher* watcherAO =  CMCSMenuOpWatcher::NewL( *this, 
	                                                    iOperationManager );
	CleanupStack::PushL( watcherAO );
	iOperationManager.AddL( watcherAO );
    CleanupStack::Pop( watcherAO );// we pop item from cleanupstack because
                                  //ownership was transfered to operation manager

	CMenuOperation* operation = item->HandleCommandL( aAction, 
	                                    KNullDesC8, watcherAO->iStatus);
	watcherAO->Watch( *operation );
	
	CleanupStack::PopAndDestroy( item );
    }


// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
//
void CMCSExecuteAction::DoCancel()
	{
//	TRequestStatus* temp = &iStatus;
//	User::RequestComplete( temp, KErrCancel );
	TRAP_IGNORE( iCallback->NotifyResultL( KErrCancel, NULL ));
//	NotifyRequestResult( err );
	}

// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
//
void CMCSExecuteAction::RunL()
	{
	TInt err = iStatus.Int();

    if ( err == KErrNone )
        {
        TRAP( err, ExecuteL( iItemId, iAction ) );
        }   
    if(err != KErrNone)
    	{
    	NotifyRequestResult( err );
    	}
	}

// ---------------------------------------------------------------------------
// CMCSExecuteAction::RunError
// ---------------------------------------------------------------------------
//
TInt CMCSExecuteAction::RunError(TInt aError)
	{
    NotifyRequestResult( aError );
    if ( (aError != KErrDiskFull) && (aError != KErrNoMemory))
    	{
    	aError = KErrNone;
    	}    
	return aError; 
	}

// ---------------------------------------------------------------------------
// Activates the asynchronous request
// ---------------------------------------------------------------------------
//
void CMCSExecuteAction::ActivateRequest( TInt aReason )
	{
	iStatus = KRequestPending;
	SetActive();
	TRequestStatus* temp = &iStatus;
    User::RequestComplete( temp, aReason );
	}

// --------------------------------------------------------------------------------------------------------
// Notifies callback the result for asynchronous request.
// --------------------------------------------------------------------------------------------------------
//
void CMCSExecuteAction::NotifyRequestResult( TInt aReason )
    {
    iOperationManager.Remove( this );//remove it 
       								//from operation manager  
    if ( iCallback )
        {
        TRAP_IGNORE( iCallback->NotifyResultL( aReason) ); 
        // iCallback deletes itself.
        }
    delete this;
    }

// --------------------------------------------------------------------------------------------------------
// Notifies callback the result for asynchronous request.
// --------------------------------------------------------------------------------------------------------
//
void CMCSExecuteAction::MenuOpCompletedL(TInt aStatus )
	{
	NotifyRequestResult( aStatus );
	}
