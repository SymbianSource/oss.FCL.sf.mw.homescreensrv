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
#include "tsdataobserver.h"

// --------------------------------------------------------------------------
// CTsFswMonitor::~CTsFswMonitor
// --------------------------------------------------------------------------
//
CTsFswMonitor::~CTsFswMonitor()
{
    Cancel();
    mWg.Close();
    mWsSession.Close();
}

// --------------------------------------------------------------------------
// CTsFswMonitor::CTsFswMonitor
// --------------------------------------------------------------------------
//
CTsFswMonitor::CTsFswMonitor(MHsDataObserver& taskListObserver) :
    CActive(EPriorityStandard), mTaskListObserver(&taskListObserver)
{
}

// --------------------------------------------------------------------------
// CTsFswMonitor::NewL
// --------------------------------------------------------------------------
//        
CTsFswMonitor* CTsFswMonitor::NewL(MHsDataObserver& taskListObserver)
{
    CTsFswMonitor* self = NewLC(taskListObserver);
    CleanupStack::Pop(self);
    return self;
}

// --------------------------------------------------------------------------
// CTsFswMonitor::NewLC
// --------------------------------------------------------------------------
//
CTsFswMonitor* CTsFswMonitor::NewLC(MHsDataObserver& taskListObserver)
{
    CTsFswMonitor* self = new (ELeave) CTsFswMonitor(taskListObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// --------------------------------------------------------------------------
// CTsFswMonitor::ConstructL
// --------------------------------------------------------------------------
//
void CTsFswMonitor::ConstructL()
{
    CActiveScheduler::Add(this);
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
    if (!IsActive()) {
        mWsSession.EventReady(&iStatus);
        SetActive();
    }
}

// -----------------------------------------------------------------------------
// ConstractObserverL
// -----------------------------------------------------------------------------
//
void CTsFswMonitor::ConstractObserverL()
{
    User::LeaveIfError(mWsSession.Connect());
    mWg = RWindowGroup(mWsSession);
    User::LeaveIfError(mWg.Construct(reinterpret_cast<TUint32> (&mWg)));
    mWg.DisableFocusChangeEvents();
    mWg.DisableModifierChangedEvents();
    mWg.DisableOnEvents();
    mWg.DisableScreenChangeEvents();

    //enable notifications about group lists
    User::LeaveIfError(mWg.EnableGroupListChangeEvents());
}

void CTsFswMonitor::RunL()
{
    User::LeaveIfError(iStatus.Int());
    TWsEvent event;
    mWsSession.GetEvent(event);
    Subscribe();//new subscribtion has to be called after picking event
    if (EEventWindowGroupListChanged == event.Type() /*|| EEventFocusGroupChanged == event.Type()*/) {
        if (mTaskListObserver) {
            mTaskListObserver->DataChanged();
        }
    }
}

TInt CTsFswMonitor::RunError(TInt /*aError*/)
{
    Subscribe();
    return KErrNone;
}

void CTsFswMonitor::DoCancel()
{
    if (IsActive()) {
        mWsSession.EventReadyCancel();
    }
}

// end of file
