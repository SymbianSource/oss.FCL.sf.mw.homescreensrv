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
#include "activitysession.h"
#include "activitytask.h"
#include "activitycmd.h"

#include "activitystorageasynctask.h"
#include "activitystoragesynctask.h"
#include "activityobservertask.h"
#include "activitybroadcasttask.h"
#include "activitydataprovidertask.h"
#include "activitythumbnailtask.h"

_LIT(KTaskAlreadyExists, "Activity task exists");

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CActivitySession::CActivitySession(RFs& fileSession, 
                                   MActivityTaskStorage& taskStorage, 
                                   CActivityStorage& storage)
:
mFileSession(fileSession),
mTasksStorage(taskStorage),
mStorage(storage)
{
    // No implementation required
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CActivitySession::~CActivitySession()
{
    mRunningTasks.ResetAndDestroy();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CActivitySession* CActivitySession::NewL(RFs& fileSession,
                                         MActivityTaskStorage& taskStorage,
                                         CActivityStorage& storage)
{
    CActivitySession* self = new (ELeave) CActivitySession(fileSession, 
                                                           taskStorage, 
                                                           storage);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivitySession::ConstructL()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivitySession::ServiceL(const RMessage2& message)
{
    switch (message.Function()) {
    case AddActivity:
    case UpdateActivity:
    case RemoveActivity:
    case RemoveApplicationActivities:
        ActivityStorageSyncTask::ExecuteL(mStorage, message);
        break;
    
    case Activities:
    case ApplicationActivities:
        CActivityStorageAsyncTask::ExecuteLD(*this, mStorage, message);
        break;
    
    case WaitActivity:
        CActivityObserverTask::ExecuteLD(mTasksStorage, *this, message);
        break;
    case GetThumbnail:
        CActivityThumbnailTask::ExecuteLD(mFileSession, *this, message);
        break;
    
    case LaunchActivity:
    case CancelWait:
        ActivityBroadcastTask::ExecuteL(mTasksStorage, message);
        break;
    
    case GetData:
        ActivityDataProviderTask::ExecuteL(*this,message);
        break;
    
    default:
        message.Complete(CServer2::EBadMessageNumber);
        break;
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivitySession::PushL(CActivityTask * task)
{
    (KErrNotFound == mRunningTasks.Find(task)) ?
     mRunningTasks.AppendL(task) :
     User::Panic(KTaskAlreadyExists, KErrAlreadyExists);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivitySession::Pop(CActivityTask *task)
{
    const TInt offset(mRunningTasks.Find(task));
    if (KErrNotFound != offset) {
        mRunningTasks.Remove(offset);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const RPointerArray<CActivityTask>& CActivitySession::StorageData() const
{
    return mRunningTasks;
}
