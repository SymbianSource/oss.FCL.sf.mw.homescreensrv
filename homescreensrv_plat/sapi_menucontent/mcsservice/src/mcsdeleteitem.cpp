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

#include "mcsmenu.h"
#include "mcsmenuitem.h"

#include "mcsdeleteitem.h"
#include "mcscallback.h"
#include "mcsservice.h"
#include "mcsmenuopwatcher.h"
#include "mcsoperationmanager.h"


// ---------------------------------------------------------------------------
// two-phased constructor
// ---------------------------------------------------------------------------
CMCSDeleteItem* CMCSDeleteItem::NewL( RMenu& aMenu,
        MMCSCallback* aCallback,  RMcsOperationManager& aOperationManager )
    {
    CMCSDeleteItem* self = new (ELeave) CMCSDeleteItem( aMenu, aCallback, 
                                                            aOperationManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
CMCSDeleteItem::~CMCSDeleteItem()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// default constructor
// ---------------------------------------------------------------------------
CMCSDeleteItem::CMCSDeleteItem( RMenu& aMenu, MMCSCallback* aCallback,  
                RMcsOperationManager& aOperationManager ) :
                        CActive( EPriorityStandard ),
                        iCallback( aCallback ),
                        iMCS( aMenu ),
    					iOperationManager(aOperationManager)						
    {
    }

// ---------------------------------------------------------------------------
// second phase constructor
// ---------------------------------------------------------------------------
void CMCSDeleteItem::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// Async delete item
// ---------------------------------------------------------------------------
void CMCSDeleteItem::DeleteItemL(TInt aItemId)
    {
    iItemId = aItemId;
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
void CMCSDeleteItem::DoCancel()
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
void CMCSDeleteItem::RunL()
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

        CMenuOperation* operation = iMCS.RemoveL( iItemId , watcherAO->iStatus );

	    watcherAO->Watch( *operation );        
        }
    else
        {
        NotifyRequestResult( err );
        }
    }
 
// ---------------------------------------------------------------------------
// CMCSDeleteItem::RunError
// ---------------------------------------------------------------------------
//
TInt CMCSDeleteItem::RunError(TInt aError)
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
void CMCSDeleteItem::ActivateRequest( TInt aReason )
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
void CMCSDeleteItem::NotifyRequestResult( TInt aReason )
    {
    iOperationManager.Remove( this );//remove it 
                                     //from operation manager   

    if ( iCallback )
        {
        TRAP_IGNORE(iCallback->NotifyResultL( aReason ));//, NULL);
        }
    
    delete this;
    }

// --------------------------------------------------------------------------------------------------------
// Notifies callback the result for asynchronous request.
// --------------------------------------------------------------------------------------------------------
//
void CMCSDeleteItem::MenuOpCompletedL(TInt aStatus )
    {
    NotifyRequestResult( aStatus );
    }

// End of file
