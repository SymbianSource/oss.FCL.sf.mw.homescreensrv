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
#include "activityobservertask.h"
#include "activitycmd.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CActivityObserverTask::CActivityObserverTask(MActivityTaskStorage& globalStorage, 
                                             MActivityTaskStorage& localStorage, 
                                             const RMessage2& msg)
:
    mGlobalStorage(globalStorage),
    mLocalStorage(localStorage),
    mMsg(msg)
{
    // No implementation required
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CActivityObserverTask::~CActivityObserverTask()
{
    mData.Close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityObserverTask::ExecuteLD(MActivityTaskStorage& globalStorage, 
                                      MActivityTaskStorage& localStorage, 
                                      const RMessage2& msg)
{
    CActivityObserverTask *self = new(ELeave)CActivityObserverTask(globalStorage, 
                                                                   localStorage, 
                                                                   msg);
    CleanupStack::PushL(self);
    globalStorage.PushL(self);
    CleanupStack::Pop(self);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const TDesC8& CActivityObserverTask::Data() const
{
    return mData;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityObserverTask::BroadcastReceivedL(const RMessage2& msg)
{
    if (WaitActivity == mMsg.Function() &&
        LaunchActivity == msg.Function()) {
        TPckgBuf<TInt> observerdId;
        mMsg.ReadL(KRequestAppIdOffset, observerdId);

        TPckgBuf<TInt> requestedId;
        msg.ReadL(KRequestAppIdOffset, requestedId);
        if (observerdId() == requestedId()) {
            if (msg.GetDesLength(KRequestActOffset) > mData.MaxLength()) {
                mData.ReAllocL(msg.GetDesLength(1));
            }
            msg.ReadL(KRequestActOffset, mData, 0);
            WriteResponseL();
            mLocalStorage.PushL(this);
            mGlobalStorage.Pop(this);
            mMsg.Complete(KErrNone);
        }
    } else if (WaitActivity == mMsg.Function() &&
               CancelWait == msg.Function() &&
               mMsg.Session() == msg.Session()) {
        mGlobalStorage.Pop(this);
        mMsg.Complete(KErrCancel);
        delete this;
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityObserverTask::WriteResponseL()
{
    mMsg.WriteL(KResponseDataSizeOffset, TPckgBuf<TInt>(mData.Length()));//write data size
    mMsg.WriteL(KResponseIdOffset, TPckgBuf<CBase*>(this));//task identyfier
}
