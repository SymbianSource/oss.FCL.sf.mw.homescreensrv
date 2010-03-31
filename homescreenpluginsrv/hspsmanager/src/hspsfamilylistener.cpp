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
* Description:  FamilyListener listens to resolution&orientation
*               changes in emulator environment.
*
*/

#include "hspsfamilylistener.h"
#include "hsps_builds_cfg.hrh"
#include "hspsmanifest.h"


_LIT8(KTch, "_tch");
const TInt KMaxFamilyLength( 20 );


// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// ChspsFamilyListener::NewL
// -----------------------------------------------------------------------------
//
ChspsFamilyListener* ChspsFamilyListener::NewL( MhspsFamilyObserver& aObserver )
    {
    ChspsFamilyListener* self = new(ELeave) ChspsFamilyListener( );
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// ChspsFamilyListener::ChspsFamilyListener
// -----------------------------------------------------------------------------
//
ChspsFamilyListener::ChspsFamilyListener() 
    {
    }

// -----------------------------------------------------------------------------
// ChspsFamilyListener::ConstructL
// -----------------------------------------------------------------------------
//
void ChspsFamilyListener::ConstructL( MhspsFamilyObserver& aObserver )
    {    

    // ChspsFamily::ConstructL ChspsFamily's second phase constructor call 
    ChspsFamily::ConstructL();

    iFamilyListenerActive = ChspsFamilyListenerActive::NewL( *this, aObserver );

    // A group needs to be instansiated so that we're able to receive events 
    iWindowGroup = RWindowGroup( WsSession() );
    User::LeaveIfError( iWindowGroup.Construct(2,ETrue) ); // '2' is a meaningless handle
    
    // Enables for EEventScreenDeviceChanged events
    iWindowGroup.EnableScreenChangeEvents(); 
    
    // Start the listener
    iFamilyListenerActive->Queue();    
    }





ChspsFamilyListenerActive* ChspsFamilyListenerActive::NewL( 
    ChspsFamilyListener& aListener,
    MhspsFamilyObserver& aObserver )
    {
    ChspsFamilyListenerActive* self = new( ELeave ) ChspsFamilyListenerActive( 
            aListener,
            aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
        
ChspsFamilyListenerActive::~ChspsFamilyListenerActive()
	{
    Cancel();
	}

void ChspsFamilyListenerActive::ConstructL()
	{
	}

ChspsFamilyListenerActive::ChspsFamilyListenerActive( 
		ChspsFamilyListener& aListener,
		MhspsFamilyObserver& aObserver )
    :CActive( EPriorityStandard ), 
    iListener( aListener),
    iObserver( aObserver )
	{
    CActiveScheduler::Add(this);
	}


// ------------------------------------------------------------------------------
// ChspsFamilyListenerActive::Queue
// ------------------------------------------------------------------------------
void ChspsFamilyListenerActive::Queue()
    {
    ASSERT ( !IsActive() );
    iListener.WsSession().EventReady( &iStatus );
    SetActive();
    }

// ------------------------------------------------------------------------------
// ChspsFamilyListener::~ChspsFamilyListener
// ------------------------------------------------------------------------------
ChspsFamilyListener::~ChspsFamilyListener()
    {
    if ( iFamilyListenerActive )
        {
        delete iFamilyListenerActive;
        }
    iWindowGroup.Close();
    }




// -----------------------------------------------------------------------------
// ChspsFamilyListener::RunL 
// -----------------------------------------------------------------------------
void ChspsFamilyListenerActive::RunL()
    {
    TWsEvent wsEvent;
    iListener.WsSession().GetEvent(wsEvent);
    switch( wsEvent.Type() )
        {
        case EEventScreenDeviceChanged:
            {                    
            ThspsFamily newFamily = iListener.GetFamilyType();
            if ( newFamily > EhspsFamilyUnknown )
                {                
                iObserver.HandleFamilyChangeL( newFamily );
                }
            break;
            }
        default:
            break;
    }
       
    Queue();    
    }
 
// -----------------------------------------------------------------------------
// ChspsFileChangeListener::DoCancel
// -----------------------------------------------------------------------------
//
void ChspsFamilyListenerActive::DoCancel()
    {
    iListener.WsSession().EventReadyCancel();
    }
 
// -----------------------------------------------------------------------------
// ChspsFileChangeListener::RunError
// -----------------------------------------------------------------------------
TInt ChspsFamilyListenerActive::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

// End of File

