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
#include "activitystorageasynctask.h"
#include "activitycmd.h"

_LIT(KUnsupportedStorageAsyncTask, "Unsupported async storage task");

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CActivityStorageAsyncTask::CActivityStorageAsyncTask()
{
    // No implementation required
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CActivityStorageAsyncTask::~CActivityStorageAsyncTask()
{
    mData.Close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityStorageAsyncTask::ExecuteLD(MActivityTaskStorage& taskStorage, 
                                          CActivityStorage& dataStorage, 
                                          const RMessage2& msg)
{
    CActivityStorageAsyncTask* self = new (ELeave) CActivityStorageAsyncTask();
    CleanupStack::PushL(self);
    self->ExecuteL(dataStorage, msg);
    taskStorage.PushL(self);
    CleanupStack::Pop(self);
    msg.Complete(KErrNone);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const TDesC8& CActivityStorageAsyncTask::CActivityStorageAsyncTask::Data() const
{
    return mData;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityStorageAsyncTask::BroadcastReceivedL(const RMessage2& )
{
    // No implementation required
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityStorageAsyncTask::ExecuteL(CActivityStorage& dataStorage, 
                                         const RMessage2& msg)
{
    switch (msg.Function()) {
    case Activities:
        AllActivitiesL(dataStorage, msg);
        break;
    case ApplicationActivities:
        ApplicationActivitiesL(dataStorage, msg);
        break;
    default:
        //this code shouldn't be called. fatal error: means wrong session implementation 
        User::Panic(KUnsupportedStorageAsyncTask, KErrGeneral);
    };
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityStorageAsyncTask::AllActivitiesL(CActivityStorage& dataStorage, 
                                               const RMessage2& msg)
{
    dataStorage.ActivitiesL(mData);
    WriteResponseL(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityStorageAsyncTask::ApplicationActivitiesL(CActivityStorage& dataStorage, 
                                                       const RMessage2& msg)
{
    TPckgBuf<TInt> appId;
    msg.ReadL(KRequestAppIdOffset, appId);
    dataStorage.ActivitiesL(mData, appId());
    WriteResponseL(msg);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityStorageAsyncTask::WriteResponseL(const RMessage2& msg)
{
    if (EFalse == msg.IsNull()) {
        msg.WriteL(KResponseDataSizeOffset, 
                   TPckgBuf<TInt>(mData.Length()));//write data size
        msg.WriteL(KResponseIdOffset, 
                   TPckgBuf<CBase*>(this));//task identyfier
    }
}
