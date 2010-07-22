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
* Description:
*
*/

#include "hsdataobservertask.h"
#include "tstaskmonitorglobals.h"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CHsDataObserverTask::CHsDataObserverTask(MHsDataObserverStorage& storage, 
                                         const RMessage2& msg)
:
    mStorage(storage),
    mMsg(msg)
{
    // No implementation required
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CHsDataObserverTask::~CHsDataObserverTask()
{
    if (EFalse == mMsg.IsNull()) {
        mMsg.Complete(mDataChanged ? KErrNone : KErrCancel);
    }
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CHsDataObserverTask::ExecuteLD(MHsDataObserverStorage& storage,const RMessage2& msg)
{
    CHsDataObserver *self = new (ELeave)CHsDataObserverTask(storage, msg);
    CleanupStack::PushL(self);
    storage.PushL(self);
    CleanupStack::Pop(self);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CHsDataObserverTask::DataChanged()
{
    mDataChanged = ETrue;
    mStorage.Pop(this);
    delete this; 
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CHsDataObserverTask::Cancel(const RMessage2& reason)
{
    if (CancelSubscribeRunningAppChanges == reason.Function() &&
        mMsg.Session() == reason.Session()) {
        mStorage.Pop(this);
        delete this;
   }
}
