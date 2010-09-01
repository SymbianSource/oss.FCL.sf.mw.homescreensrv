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
* Description:  Implementation of CHSPSBRObserver class
*
*/


#include <sbdefs.h>
#include "hspsbrobserver.h"
#include "hspsthemeserver.h"


// ======== LOCAL FUNCTIONS ====================================================


// ======== MEMBER FUNCTIONS ===================================================


// -----------------------------------------------------------------------------
// CHSPSBRObserver::CHSPSBRObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHSPSBRObserver::CHSPSBRObserver( ChspsThemeServer& aServer ):
    CActive(CActive::EPriorityStandard),
    iServer( aServer )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CHSPSBRObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHSPSBRObserver::ConstructL()
    {
    User::LeaveIfError( iProperty.Attach( 
        KUidSystemCategory, 
        conn::KUidBackupRestoreKey ) );
    }
                    
// -----------------------------------------------------------------------------
// CHSPSBRObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHSPSBRObserver* CHSPSBRObserver::NewL( ChspsThemeServer& aServer )
    {
    CHSPSBRObserver* self = new( ELeave ) CHSPSBRObserver( aServer );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
CHSPSBRObserver::~CHSPSBRObserver()
    {
    Cancel();
    iProperty.Close();
    }


// -----------------------------------------------------------------------------
// CHSPSBRObserver::Start
// Start the system agent to listen to the event
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHSPSBRObserver::Start()
    {
    Cancel();
    iProperty.Subscribe( iStatus );
    SetActive();
    }


// -----------------------------------------------------------------------------                        
// CHSPSBRObserver::RunError
// -----------------------------------------------------------------------------
//  
TInt CHSPSBRObserver::RunError(TInt aError)
    {
    if( aError != KErrCancel )
        {
        Start();           
        }
    else 
        {
        Cancel();
        iProperty.Close();    
        }
    return KErrNone;
    };


// -----------------------------------------------------------------------------
// CHSPSBRObserver::RunL
// from CActive
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHSPSBRObserver::RunL()
    {
    // Resubscribe before processing new value to prevent missing updates
    TInt backupStatus = 0;
    TInt err( iStatus.Int() );
    if ( err == KErrNone )
        {
        Start();
        User::LeaveIfError( iProperty.Get( 
                KUidSystemCategory, 
                conn::KUidBackupRestoreKey, backupStatus ) );
        // Forward backup status to listening server
        iServer.HandleBREventL( backupStatus );
        }
    else if ( err != KErrCancel ) // Ignore all errors except cancel
        {
        Start();
        }
    else // When cancel occurs, stop everything
        {
        User::Leave( KErrCancel );    
        }
    }

// -----------------------------------------------------------------------------
// CHSPSBRObserver::DoCancel
// From CActive
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHSPSBRObserver::DoCancel()
    {
    iProperty.Cancel();
    }

// End of file
