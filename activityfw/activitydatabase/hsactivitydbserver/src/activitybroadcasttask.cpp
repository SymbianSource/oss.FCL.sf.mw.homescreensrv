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
#include "activitybroadcasttask.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void ActivityBroadcastTask::ExecuteL(MActivityTaskStorage& storage, 
                                     const RMessage2& msg)
{
    const RPointerArray<CActivityTask> &tasks(storage.StorageData());
    for (TInt iter(tasks.Count() - 1); iter >= 0 ; --iter ) {
        (tasks[iter])->BroadcastReceivedL(msg);
    }
    msg.Complete(KErrNone);
}
