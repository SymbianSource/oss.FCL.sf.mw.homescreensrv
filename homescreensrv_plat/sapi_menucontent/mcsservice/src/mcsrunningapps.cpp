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

#include "mcsrunningapps.h"
#include "mcsservice.h"
#include "mcsconstants.h"
#include "mcsmenuopwatcher.h"
#include "mcsoperationmanager.h"

// ---------------------------------------------------------------------------
// two-phased constructor
// ---------------------------------------------------------------------------
CMCSRunningApps* CMCSRunningApps::NewL( RMenu& aMCS, MMCSCallback* aCallback, 
                                    RMcsOperationManager& aOperationManager )
    {
    CMCSRunningApps* self = new (ELeave) CMCSRunningApps( aMCS, aCallback, 
                                                        aOperationManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
CMCSRunningApps::~CMCSRunningApps()
    {
    Cancel();
    iUidArray.Close();
    }

// ---------------------------------------------------------------------------
// default constructor
// ---------------------------------------------------------------------------
CMCSRunningApps::CMCSRunningApps( RMenu& aMCS, MMCSCallback* aCallback, 
                                    RMcsOperationManager& aOperationManager ) :
                        CActive( EPriorityStandard ),
                        iMCS( aMCS ), 
                        iCallback( aCallback ),
    					iOperationManager(aOperationManager)						
    {
    }

// ---------------------------------------------------------------------------
// second phase constructor
// ---------------------------------------------------------------------------
void CMCSRunningApps::ConstructL( )
    {
    }

// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
//
void CMCSRunningApps::DoCancel()
    {
    TRAP_IGNORE( iCallback->NotifyResultL( KErrCancel, NULL ));
    }

// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
//
void CMCSRunningApps::RunL()
    {
    TInt err = iStatus.Int();
  
    if ( err == KErrNone )
        {
        RunningAppsChangedL();//temporary        
        }   
     if(err != KErrNone)
        {
        NotifyRequestResult( err );
        }
    }

// ---------------------------------------------------------------------------
// CMCSRunningApps::RunError
// ---------------------------------------------------------------------------
//
TInt CMCSRunningApps::RunError(TInt aError)
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
void CMCSRunningApps::ActivateRequest( TInt aReason )
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
void CMCSRunningApps::NotifyRequestResult( TInt aReason )
    {
    iOperationManager.Remove( this );//remove it 
                                     //from operation manager   
    if ( iCallback )
        {
        TRAP_IGNORE( iCallback->NotifyResultL( aReason, (TAny*)(&iUidArray)));
        // iCallback deletes itself.
        }
    delete this;
    }

// ---------------------------------------------------------------------------
// ExecuteActionL - Async 
// ---------------------------------------------------------------------------
void CMCSRunningApps::GetRunningApplicationL()
    {
    if( !iCallback )
        {
        User::Leave( KErrArgument );
        }
    CActiveScheduler::Add ( this );
    ActivateRequest( KErrNone );
    }

// --------------------------------------------------------------------------------------------------------
// Notifies callback the result for asynchronous request.
// --------------------------------------------------------------------------------------------------------
//
void CMCSRunningApps::RunningAppsChangedL()
    {
     TRAPD(err,
      {
      RArray<TUid> uids;
      CleanupClosePushL( uids );
      iMCS.GetRunningAppsL( uids );    		  
      for(TInt i =0; i<uids.Count(); i++)
          {
          iUidArray.AppendL( uids[i] );
          }
      CleanupStack::PopAndDestroy( &uids );    
      });
    NotifyRequestResult( err );
    }
