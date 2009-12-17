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

#include "mcsmenuitem.h"
#include "mcsmenuoperation.h"

#include "mcsupdateitem.h"
#include "mcscallback.h"
#include "mcsservice.h"
#include "mcsmenuopwatcher.h"
#include "mcsoperationmanager.h"

// ---------------------------------------------------------------------------
// two-phased constructor
// ---------------------------------------------------------------------------
CMCSUpdateItem* CMCSUpdateItem::NewL( CMenuItem& aMenu,
        MMCSCallback* aCallback, RMcsOperationManager& aOperationManager )
	{
	CMCSUpdateItem* self = new (ELeave) CMCSUpdateItem( aMenu, aCallback, 
	                                                    aOperationManager );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}
	
// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
CMCSUpdateItem::~CMCSUpdateItem()
	{
	Cancel();
	delete iMenu; 
	}

// ---------------------------------------------------------------------------
// default constructor
// ---------------------------------------------------------------------------
CMCSUpdateItem::CMCSUpdateItem( CMenuItem& aMenu, MMCSCallback* aCallback,
                                    RMcsOperationManager& aOperationManager) :
						CActive( EPriorityStandard ),
						iCallback( aCallback ),
						iMenu( &aMenu ),
    					iOperationManager(aOperationManager)						
	{
	}

// ---------------------------------------------------------------------------
// second phase constructor
// ---------------------------------------------------------------------------
void CMCSUpdateItem::ConstructL()
	{
	}

// ---------------------------------------------------------------------------
// Async update (add/change) item
// ---------------------------------------------------------------------------
void CMCSUpdateItem::UpdateItemL()
    {
    if( !iCallback )
        {
        User::Leave( KErrArgument );
        }
        CActiveScheduler::Add ( this );
        ActivateRequest( KErrNone );
    }

// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
//
void CMCSUpdateItem::DoCancel()
	{
	//TRequestStatus* temp = &iStatus;
	//User::RequestComplete( temp, KErrCancel );
	TRAP_IGNORE( iCallback->NotifyResultL( KErrCancel, NULL ));
	//NotifyRequestResult( err );
	}

// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
//
void CMCSUpdateItem::RunL()
	{
	TInt err = iStatus.Int();
    if ( err == KErrNone )
        {
    	CMCSMenuOpWatcher* watcherAO =  CMCSMenuOpWatcher::NewL( *this, 
	                                                    iOperationManager );
	    CleanupStack::PushL( watcherAO );
	    iOperationManager.AddL( watcherAO );
        CleanupStack::Pop( watcherAO );// we pop item from cleanupstack because
                                  //ownership was transfered to operation manager

        CMenuOperation* operation = iMenu->SaveL( watcherAO->iStatus );

	    watcherAO->Watch( *operation );
        }
    else
        {
        NotifyRequestResult( err );
        }
	}

// ---------------------------------------------------------------------------
// CMCSUpdateItem::RunError
// ---------------------------------------------------------------------------
//
TInt CMCSUpdateItem::RunError(TInt aError)
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
void CMCSUpdateItem::ActivateRequest( TInt aReason )
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
void CMCSUpdateItem::NotifyRequestResult( TInt aReason )
    {
    iOperationManager.Remove( this );//remove it 
                                     //from operation manager   
    if ( iCallback )
        {
        TInt id = iMenu->Id(); 
        TRAP_IGNORE(iCallback->NotifyResultL( aReason, (TAny*)&id));
        }
    delete this;
    }

// --------------------------------------------------------------------------------------------------------
// Notifies callback the result for asynchronous request.
// --------------------------------------------------------------------------------------------------------
//
void CMCSUpdateItem::MenuOpCompletedL(TInt aStatus )
    {
    NotifyRequestResult( aStatus );
    }

// End of file
