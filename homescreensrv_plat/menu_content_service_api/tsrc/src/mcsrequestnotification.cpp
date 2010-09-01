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

#include "mcsrequestnotification.h"

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
    iNotifier.Close();
    }

// ---------------------------------------------------------------------------
// default constructor
// ---------------------------------------------------------------------------
CMCSReqNotification::CMCSReqNotification() : CActive( EPriorityStandard )
    {
    CActiveScheduler::Add ( this );
    }

// ---------------------------------------------------------------------------
// second phase constructor
// ---------------------------------------------------------------------------
void CMCSReqNotification::ConstructL( RMenu& aMCS)
    {
    iStat = -2;
    TInt err = KErrNone;
    err = iNotifier.Open(aMCS);
    iMCS = aMCS;
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
    iNotifier.Cancel();
    }

// ---------------------------------------------------------------------------
// Inherited from CActive class 
// ---------------------------------------------------------------------------
//
void CMCSReqNotification::RunL()
    {
    iStat = iStatus.Int();
    }

// ---------------------------------------------------------------------------
// Start event's notifications 
// ---------------------------------------------------------------------------
//
void CMCSReqNotification::StartNotificationL(TInt aFolderId, 
        TInt aEvent )
    {
    if(IsActive() )
      {
      User::Leave( KErrInUse );
      }
    iNotifier.Notify(aFolderId, aEvent, iStatus );
    SetActive();
    iStat = KErrNone;
    }

// ---------------------------------------------------------------------------
// Stop event's notifications 
// ---------------------------------------------------------------------------
//
TInt CMCSReqNotification::GetStatus()
	{
	return iStat;
	}
