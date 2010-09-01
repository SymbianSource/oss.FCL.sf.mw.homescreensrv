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
* Description:  CMCSOrganize implementation
*
*/


#include <liwvariant.h>
#include "mcsmenu.h"
#include "mcsmenuitem.h"
#include "mcsorganizeimpl.h"
#include "mcsservice.h"
#include "mcsmenuopwatcher.h"
#include "mcsoperationmanager.h"

// ---------------------------------------------------------------------------
// two-phased constructor
// ---------------------------------------------------------------------------
CMCSOrganizeImpl* CMCSOrganizeImpl::NewL( RMenu& aMCS, MMCSCallback* aCallback,
                                    RMcsOperationManager& aOperationManager )
    {
    CMCSOrganizeImpl* self = new (ELeave) CMCSOrganizeImpl( aMCS, aCallback, 
                                                          aOperationManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
CMCSOrganizeImpl::~CMCSOrganizeImpl()
    {
    Cancel();
    iList.Close();
    }

// ---------------------------------------------------------------------------
// default constructor
// ---------------------------------------------------------------------------
CMCSOrganizeImpl::CMCSOrganizeImpl( RMenu& aMCS, MMCSCallback* aCallback,
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
void CMCSOrganizeImpl::ConstructL()
    {
    iIsSingle = ETrue;
    }

// ---------------------------------------------------------------------------
// Organize
// ---------------------------------------------------------------------------
void CMCSOrganizeImpl::OrganizeL( 
		TInt aItemId, TInt aBeforeItemId )
    {
    if( !iCallback )
        {
        User::Leave( KErrArgument );
        }
    	
    	iItemId = aItemId;
    	iBeforeItemId = aBeforeItemId;
    	iIsSingle = ETrue;
    	
        CActiveScheduler::Add ( this );
        ActivateRequest( KErrNone );
    }

// ---------------------------------------------------------------------------
// Organize
// ---------------------------------------------------------------------------
void CMCSOrganizeImpl::OrganizeL( const RArray<TInt>& aList, 
		TInt aFolderId,	TInt aBeforeItemId )
    {
    if( !iCallback )
        {
        User::Leave( KErrArgument );
        }
    
    	iList.Reset();
    	//iList = aList;
    	for( TInt idx = 0; idx < aList.Count(); idx++ )
    		{
    		TInt value = aList[idx];
        	iList.AppendL( value );
    		}
    	iFolderId = aFolderId;
    	iBeforeItemId = aBeforeItemId;
    	iIsSingle = EFalse;
    	
        CActiveScheduler::Add ( this );
        ActivateRequest( KErrNone );
    }

// ---------------------------------------------------------------------------
// Organize
// ---------------------------------------------------------------------------
void CMCSOrganizeImpl::OrganizeL()
    {
    CMCSMenuOpWatcher* watcherAO =  CMCSMenuOpWatcher::NewL( *this, 
	                                                    iOperationManager );
	CMenuOperation* operation = NULL;
	                                                    
	CleanupStack::PushL( watcherAO );
	iOperationManager.AddL( watcherAO );
    CleanupStack::Pop( watcherAO );// we pop item from cleanupstack because
                                  //ownership was transfered to operation manager
    
	if( iIsSingle )
    	{
		TInt id = ( TInt )iItemId;
		operation = iMCS.ReorderL( id, iBeforeItemId, watcherAO->iStatus );
    	}
    else
    	{
        TInt folder = ( iFolderId == 0 ) ? iMCS.RootFolderL() : iFolderId;  
		operation = iMCS.MoveToFolderL( iList, folder, iBeforeItemId, 
				watcherAO->iStatus );
    	}
	watcherAO->Watch( *operation);
    }

// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
//
void CMCSOrganizeImpl::DoCancel()
    {
//    TRequestStatus* temp = &iStatus;
//    User::RequestComplete( temp, KErrCancel );
    TRAP_IGNORE( iCallback->NotifyResultL( KErrCancel, NULL ));
//    NotifyRequestResult( err );
    }

// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
//
void CMCSOrganizeImpl::RunL()
    {
    TInt err = iStatus.Int();

    if ( err == KErrNone )
        {
        TRAP( err, OrganizeL() );
        }       
    if(err != KErrNone)
    	{
        NotifyRequestResult( err );
    	}
    }

// ---------------------------------------------------------------------------
// CMCSOrganizeImpl::RunError
// ---------------------------------------------------------------------------
//
TInt CMCSOrganizeImpl::RunError(TInt aError)
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
void CMCSOrganizeImpl::ActivateRequest( TInt aReason )
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
void CMCSOrganizeImpl::NotifyRequestResult( TInt aReason )
    {
    iOperationManager.Remove( this );//remove it 
                                     //from operation manager  
    if ( iCallback )
        {
        TRAP_IGNORE( iCallback->NotifyResultL( aReason, NULL ) );
        // iCallback deletes itself.
        }
    delete this;

    }

// --------------------------------------------------------------------------------------------------------
// Notifies callback the result for asynchronous request.
// --------------------------------------------------------------------------------------------------------
//
void CMCSOrganizeImpl::MenuOpCompletedL(TInt aStatus )
    {
    NotifyRequestResult( aStatus );
    }
