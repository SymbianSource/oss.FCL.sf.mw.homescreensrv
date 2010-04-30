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
#include "activitystoragesynctask.h"
#include "activitycmd.h"

_LIT(KUnsupportedStorageSyncTask, "Unsupported sync storage task");

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void ActivityStorageSyncTask::ExecuteL(CActivityStorage& dataStorage, const RMessage2& msg)
{
    switch (msg.Function()) {
    case AddActivity: 
        AddActivityL(dataStorage, msg);
        break;
    case UpdateActivity:
        UpdateActivityL(dataStorage, msg);
        break;
    case RemoveActivity:
        DeleteActivityL(dataStorage, msg);
        break;
    case RemoveApplicationActivities:
        DeleteApplicationActivitiesL(dataStorage, msg);
        break;
    default:
        //this code shouldn't be called. fatal error: means wrong session implementation 
        User::Panic(KUnsupportedStorageSyncTask, KErrGeneral);
    };
    msg.Complete(KErrNone);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void ActivityStorageSyncTask::AddActivityL(CActivityStorage& dataStorage, const RMessage2& msg)
{
    TPckgBuf<TInt> appId;
    msg.ReadL(KRequestAppIdOffset, appId);
    
    RBuf8 actId;
    CleanupClosePushL(actId);
    actId.CreateL(msg.GetDesLengthL(KRequestActOffset));
    msg.ReadL(KRequestActOffset, actId, 0);
    
    RBuf8 data;
    CleanupClosePushL(data);
    data.CreateL(msg.GetDesLengthL(KRequestData));
    msg.ReadL(KRequestData, data, 0);
    
    dataStorage.AddActivityL(appId(), actId, data);
    
    CleanupStack::PopAndDestroy(&data);
    CleanupStack::PopAndDestroy(&actId);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void ActivityStorageSyncTask::UpdateActivityL(CActivityStorage& dataStorage, const RMessage2& msg)
{
    TPckgBuf<TInt> appId;
    msg.ReadL(KRequestAppIdOffset, appId);
    
    RBuf8 actId;
    CleanupClosePushL(actId);
    actId.CreateL(msg.GetDesLengthL(KRequestActOffset));
    msg.ReadL(KRequestActOffset, actId, 0);
    
    RBuf8 data;
    CleanupClosePushL(data);
    data.CreateL(msg.GetDesLengthL(KRequestData));
    msg.ReadL(KRequestData, data, 0);
    
    dataStorage.UpdateActivityL(appId(), actId, data);
    
    CleanupStack::PopAndDestroy(&data);
    CleanupStack::PopAndDestroy(&actId);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void ActivityStorageSyncTask::DeleteActivityL(CActivityStorage& dataStorage, const RMessage2& msg)
{
    TPckgBuf<TInt> appId;
    msg.ReadL(KRequestAppIdOffset, appId);
    
    RBuf8 actId;
    CleanupClosePushL(actId);
    actId.CreateL(msg.GetDesLengthL(KRequestActOffset));
    msg.ReadL(KRequestActOffset, actId, 0);
    
    dataStorage.DeleteActivityL(appId(), actId);
    
    CleanupStack::PopAndDestroy(&actId);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void ActivityStorageSyncTask::DeleteApplicationActivitiesL(CActivityStorage& dataStorage, const RMessage2& msg)
{
    TPckgBuf<TInt> appId;
    msg.ReadL(KRequestAppIdOffset, appId);
    dataStorage.DeleteActivitiesL(appId());
}
