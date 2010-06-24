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

#include "tsapplicationtask.h"

#include <apgtask.h>
#include <eikenv.h>
#include <AknDef.h>

#include "tsentry.h"

TsApplicationTask::TsApplicationTask(RWsSession &wsSession, CTsEntry* entry) : TsTask(entry), mWsSession(wsSession)
{
}

void TsApplicationTask::open()
{
    TApaTaskList taskList(mWsSession);
    TApaTask task = taskList.FindApp(mEntry->AppUid());
    task.BringToForeground();
}

void TsApplicationTask::close()
{
    TWsEvent event;
    event.SetTimeNow();
    event.SetType(KAknShutOrHideApp);
    mWsSession.SendEventToWindowGroup(mEntry->Key().WindowGroupId(), event);
}
