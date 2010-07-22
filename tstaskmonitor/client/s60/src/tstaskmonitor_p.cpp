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
#include "tstaskmonitor_p.h"
#include "tstaskmonitor.h"

#include <eikenv.h>

#include "tstaskmonitorclient.h"
#include "tsapplicationtask.h"

#include "tsutils.h"
using TaskSwitcher::CleanupResetAndDestroyPushL;

TsTaskMonitorPrivate::TsTaskMonitorPrivate(TsTaskMonitor *q) : q_ptr(q), mClient(0), mWsSession(CEikonEnv::Static()->WsSession())
{
    QT_TRAP_THROWING(mClient = CTsTaskMonitorClient::NewL());
    mClient->Subscribe(*this);
}

TsTaskMonitorPrivate::~TsTaskMonitorPrivate()
{
    mClient->CancelSubscribe();
    delete mClient;
}

QList< QSharedPointer<TsTask> > TsTaskMonitorPrivate::taskList()
{
    QList< QSharedPointer<TsTask> > tasks;

    QT_TRAP_THROWING (    
        RPointerArray<CTsEntry> entries;   
        CleanupResetAndDestroyPushL<CTsEntry>(entries);
        mClient->TaskListL(entries);
        
        // TsTask claims ownership of CTsFswEntry, so we have to remove entries from 
        // entries array after adding task to tasks array to prevent double delete of
        // CTsFswEntry objects in case of leave (for example due to OOM) in the middle 
        // of the loop below.
        while (entries.Count()) {
            CTsEntry *firstEntry = entries[0];
            CleanupStack::PushL(firstEntry);
            entries.Remove(0);
            
            // @todo switch by entry type
            QT_TRYCATCH_LEAVING(tasks.append(QSharedPointer<TsTask>(new TsApplicationTask(mWsSession, firstEntry))));
            
            CleanupStack::Pop(firstEntry);
        }
        
        CleanupStack::PopAndDestroy(&entries);    
    );
    
    return tasks;
}

void TsTaskMonitorPrivate::HandleRunningAppChange()
{
    emit q_ptr->taskListChanged();
}
