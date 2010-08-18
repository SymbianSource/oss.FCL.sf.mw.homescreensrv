/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description :
 *
 */
#include "tswindowgroupsobserver.h"
#include <apgwgnam.h>

const int KOrdinalPositionNoZOrder(-1);
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C CTsWindowGroupsMonitor* CTsWindowGroupsMonitor::NewL(MTsResourceManager &resources)
{
    CTsWindowGroupsMonitor* self = new (ELeave) CTsWindowGroupsMonitor(resources);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CTsWindowGroupsMonitor::CTsWindowGroupsMonitor(MTsResourceManager &resources)
:
    CActive(EPriorityStandard),
    mResources(resources)
{
    CActiveScheduler::Add(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CTsWindowGroupsMonitor::ConstructL()
{
    
    // Initial window group
    mWg = RWindowGroup (mResources.WsSession());
    User::LeaveIfError (mWg.Construct ((TUint32)&mWg, EFalse));
    mWg.SetOrdinalPosition (KOrdinalPositionNoZOrder);
    mWg.EnableReceiptOfFocus (EFalse);
    
    // Hide window
    CApaWindowGroupName* wn = CApaWindowGroupName::NewLC(mResources.WsSession());
    wn->SetHidden (ETrue);
    wn->SetWindowGroupName (mWg);
    CleanupStack::PopAndDestroy (wn);
    
    // Window group change event
    User::LeaveIfError (mWg.EnableGroupListChangeEvents());
    Subscribe();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C CTsWindowGroupsMonitor::~CTsWindowGroupsMonitor()
{
    CActive::Cancel();
    mWg.Close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CTsWindowGroupsMonitor::SubscribeL(MTsWindowGroupsObserver &observer)
{
    const TInt offset(mObservers.Find(&observer));
    KErrNotFound == offset ? mObservers.InsertL(&observer, 0) : 
                             User::Leave(KErrAlreadyExists);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CTsWindowGroupsMonitor::Cancel(MTsWindowGroupsObserver & observer)
{
    const TInt offset(mObservers.Find(&observer));
    if (KErrNotFound != offset) {
        mObservers.Remove(offset);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CTsWindowGroupsMonitor::RunL()
{
    User::LeaveIfError(iStatus.Int());
    ProvideEventL();
    Subscribe();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CTsWindowGroupsMonitor::DoCancel()
{
    if (IsActive()) {
        mResources.WsSession().EventReadyCancel();
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt CTsWindowGroupsMonitor::RunError(TInt error)
{
    if (!IsActive() && KErrCancel != error) {
        Subscribe();
    }
    return KErrNone;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CTsWindowGroupsMonitor::Subscribe()
{
    mResources.WsSession().EventReady( &iStatus );
    SetActive();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CTsWindowGroupsMonitor::ProvideEventL()
{
    TWsEvent wsEvent;
    mResources.WsSession().GetEvent(wsEvent);
    for (TInt iter(0); iter < mObservers.Count(); ++iter) {
        ProvideEventL(wsEvent, *mObservers[iter]);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CTsWindowGroupsMonitor::ProvideEventL(TWsEvent event, 
                                          MTsWindowGroupsObserver &observer)
{
    RArray<RWsSession::TWindowGroupChainInfo> wgInfo;
    CleanupClosePushL(wgInfo);
    switch(event.Type()) {
    case EEventWindowGroupListChanged:
        User::LeaveIfError(mResources.WsSession().WindowGroupList(0, &wgInfo));
        observer.HandleWindowGroupChanged(mResources, wgInfo.Array());
        break;
    }
    CleanupStack::PopAndDestroy(&wgInfo);
}
