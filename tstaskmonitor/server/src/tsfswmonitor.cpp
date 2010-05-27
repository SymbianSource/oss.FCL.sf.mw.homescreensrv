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
* Description:  ws monitor implementation
 *
*/


#include "tsfswmonitor.h"
#include "tsfswobservers.h"

// --------------------------------------------------------------------------
// CTsFswMonitor::~CTsFswMonitor
// --------------------------------------------------------------------------
//
CTsFswMonitor::~CTsFswMonitor()
    {
    Cancel();
    iWg.Close();
    iWsSession.Close();
    }

// --------------------------------------------------------------------------
// CTsFswMonitor::CTsFswMonitor
// --------------------------------------------------------------------------
//
CTsFswMonitor::CTsFswMonitor(MTsFswTaskListObserver& aTaskListObserver) : CActive( EPriorityStandard ),
    iTaskListObserver(&aTaskListObserver)
    {
    }

// --------------------------------------------------------------------------
// CTsFswMonitor::NewL
// --------------------------------------------------------------------------
//        
CTsFswMonitor* CTsFswMonitor::NewL(MTsFswTaskListObserver& aTaskListObserver)
    {
    CTsFswMonitor* self = NewLC(aTaskListObserver);
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CTsFswMonitor::NewLC
// --------------------------------------------------------------------------
//
CTsFswMonitor* CTsFswMonitor::NewLC(MTsFswTaskListObserver& aTaskListObserver)
    {
    CTsFswMonitor* self = new ( ELeave ) CTsFswMonitor(aTaskListObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// --------------------------------------------------------------------------
// CTsFswMonitor::ConstructL
// --------------------------------------------------------------------------
//
void CTsFswMonitor::ConstructL()
    {
    CActiveScheduler::Add( this );
    //Create window server observer
    ConstractObserverL();
    Subscribe();
    }


// -----------------------------------------------------------------------------
// Subscribe
// -----------------------------------------------------------------------------
//
void CTsFswMonitor::Subscribe()
    {
    if( !IsActive() )
        {
        iWsSession.EventReady( &iStatus );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// ConstractObserverL
// -----------------------------------------------------------------------------
//
void CTsFswMonitor::ConstractObserverL()
{
    User::LeaveIfError( iWsSession.Connect() );
    iWg = RWindowGroup( iWsSession );
    User::LeaveIfError( iWg.Construct( 
                        reinterpret_cast<TUint32>( &iWg ) ) );
    iWg.DisableFocusChangeEvents();
    iWg.DisableModifierChangedEvents();
    iWg.DisableOnEvents();
    iWg.DisableScreenChangeEvents();

    //enable notifications about group lists
    User::LeaveIfError( iWg.EnableGroupListChangeEvents() );
}

void CTsFswMonitor::RunL()
{
    User::LeaveIfError( iStatus.Int() );
    TWsEvent event;
    iWsSession.GetEvent( event );
    Subscribe();//new subscribtion has to be called after picking event
    if( EEventWindowGroupListChanged == event.Type() /*|| EEventFocusGroupChanged == event.Type()*/ )
        {
        if ( iTaskListObserver )
            {
            iTaskListObserver->UpdateTaskList(); 
            }
        }
}

TInt CTsFswMonitor::RunError( TInt /*aError*/ )
{
    Subscribe();
    return KErrNone;
}

void CTsFswMonitor::DoCancel()
{
    if( IsActive() )
        {
        iWsSession.EventReadyCancel();
        }
}

// end of file
