/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CenRepListener listens to given Central Repository setting id and notifies its
*                observer if there is any change.
*
*
*/


#include "hspscenreplistener.h"

#include "hsps_builds_cfg.hrh"
#include <centralrepository.h>
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChspsCenRepListener::ChspsCenRepListener
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsCenRepListener::ChspsCenRepListener( MhspsCenRepObserver& aObserver,
                                    const TUid& aRepositoryUid ) :
    CActive( EPriorityStandard ), 
    iObserver( aObserver ),
    iRepositoryUid( aRepositoryUid )
    {
    }

// -----------------------------------------------------------------------------
// ChspsCenRepListener::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsCenRepListener::ConstructL()
    {
    iRepository = CRepository::NewL( iRepositoryUid );
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// ChspsCenRepListener::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChspsCenRepListener* ChspsCenRepListener::NewL( MhspsCenRepObserver& aObserver,
                                               const TUid& aRepositoryUid )
    {
    ChspsCenRepListener* self = new(ELeave) ChspsCenRepListener( aObserver, 
                                                         aRepositoryUid );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// ------------------------------------------------------------------------------
// ChspsCenRepListener::~ChspsCenRepListener
//
// ------------------------------------------------------------------------------
//
ChspsCenRepListener::~ChspsCenRepListener()
    {
    Cancel();
    delete iRepository;
    }

// -----------------------------------------------------------------------------
// ChspsCenRepListener::RunL
// From CActive. Called when asynchronous request is completed. 
// Notifies the observer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsCenRepListener::RunL()
    {
    TUint32 status = static_cast<TUint32>( iStatus.Int() );
    iObserver.HandleCenRepChangeL( status );
    }

// -----------------------------------------------------------------------------
// ChspsCenRepListener::DoCancel
// From CActive. Called when asynchronous request is canceled
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void ChspsCenRepListener::DoCancel()
    {
    if( IsActive() )
        {
        iRepository->NotifyCancelAll();
        }
    }

// ------------------------------------------------------------------------------
// ChspsCenRepListener::Setup
// Sets up the listener to listen to changes in Central Repository setting aId
// (other items were commented in a header).
// ------------------------------------------------------------------------------
//
void ChspsCenRepListener::Setup( TUint32 aId )
    {
    if( !IsActive() )
        {
        iRepository->NotifyRequest( aId, iStatus );
        iRepositoryUid.iUid = aId;
        SetActive();
        }
    }

// ------------------------------------------------------------------------------
// ChspsCenRepListener::SetupAll
// Sets up the listener to listen to changes in Central Repository for all settings 
// (other items were commented in a header).
// ------------------------------------------------------------------------------
//
void ChspsCenRepListener::SetupAll()
    {
     if( !IsActive() )
        {
        TInt err = iRepository->NotifyRequest( 0x00000000,0x00000000, iStatus );
        SetActive();
        }
    }


// End of File
