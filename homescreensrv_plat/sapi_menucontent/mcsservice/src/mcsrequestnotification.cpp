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

#include "mcsrequestnotification.h"
#include "mcsservice.h"
#include "mcsconstants.h"


// ---------------------------------------------------------------------------
// two-phased constructor
// ---------------------------------------------------------------------------
CMCSReqNotification* CMCSReqNotification::NewL( RMenu& aMCS )
    {
    CMCSReqNotification* self = new (ELeave) CMCSReqNotification();
    CleanupStack::PushL( self );
    self->ConstructL(aMCS);
    CleanupStack::Pop( self );
    return self;
    }
    
// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
CMCSReqNotification::~CMCSReqNotification()
    {
    Cancel();

  //  delete iRunningApps;
    }

// ---------------------------------------------------------------------------
// default constructor
// ---------------------------------------------------------------------------
CMCSReqNotification::CMCSReqNotification() : CActive( EPriorityStandard )
    {
  
    }

// ---------------------------------------------------------------------------
// second phase constructor
// ---------------------------------------------------------------------------
void CMCSReqNotification::ConstructL( RMenu& aMCS)
    {
    TInt err = KErrNone;
    err = iNotifier.Open(aMCS);
    iMCS = aMCS;
    iIsNotification = EFalse;
    if(err != KErrNone)
        {
        User::Leave( err );
        }
    }

// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
//
void CMCSReqNotification::DoCancel()
    {
    iNotifier.Close();
    }

// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
//
void CMCSReqNotification::RunL()
    {
    TInt event = iStatus.Int();
    if( event != KErrCancel )
        {
        if(iIsNotification)
            {
            if(event)
                {
                iCallback->NotifyResultL( event, (TAny*) &iFolderId );
                iStatus = KRequestPending; 
                iNotifier.Notify(iFolderId, iEvents, iStatus );
                SetActive();
                }
            else
                { 
                if(event != KErrCancel)// error in notification
                    {
                    iCallback->NotifyResultL( event, (TAny*) &iFolderId );
                    }
                }
             }
        else
            {
            iIsNotification = ETrue;
            iStatus = KRequestPending; 
            iNotifier.Notify(iFolderId, iEvents, iStatus );
            SetActive();
            }    
        }
    }
    
// ---------------------------------------------------------------------------
// Activates the asynchronous request
// ---------------------------------------------------------------------------
//
void CMCSReqNotification::ActivateRequest( TInt aReason )
    {
    iStatus = KRequestPending; 
    SetActive();
    TRequestStatus* temp = &iStatus;
    User::RequestComplete( temp, aReason );
    }


// ---------------------------------------------------------------------------
// Start event's notifications 
// ---------------------------------------------------------------------------
//
void CMCSReqNotification::StartNotificationL(TInt aFolderId, 
        TInt aEvent, MMCSCallback* aCallback )
    {
    if(IsActive() )
      {
      User::Leave( KErrInUse );
      }
    iCallback = aCallback;
    iFolderId = aFolderId;
    iEvents = aEvent;
    CActiveScheduler::Add ( this );
    ActivateRequest( KErrNone );
    }

// ---------------------------------------------------------------------------
// Stop event's notifications 
// ---------------------------------------------------------------------------
//
void CMCSReqNotification::StopNotificationL()
    {
    if ( !IsActive() )
        {
        User::Leave(KErrNotFound);  
        }
    else
        {
        TRAP_IGNORE( iCallback->NotifyResultL( KErrCancel, NULL ));
        }
    }


