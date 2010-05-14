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
#include "activitydataprovidertask.h"
#include "activitycmd.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void ActivityDataProviderTask::ExecuteL(MActivityTaskStorage& storage, 
                                       const RMessage2& msg)
{
    TPckgBuf<void*> requestId;
    CActivityTask* taskPtr(0);
    msg.Read(KRequestIdOffset, requestId);
    
    for (TInt iter(0);iter <storage.StorageData().Count();++iter) {
        taskPtr = storage.StorageData()[iter];
        if (taskPtr == requestId()) {//compare requested task address with storage objects
            ProvideDataL(msg, *taskPtr);
            storage.Pop(taskPtr);
            delete taskPtr;
            return;
        }
    }
    User::Leave(KErrNotFound);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void ActivityDataProviderTask::ProvideDataL(const RMessage2& msg, 
                                            const CActivityTask& src)
{
    msg.WriteL(KResponseDataOffset, src.Data());
    msg.Complete(KErrNone);
}
