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

#include <apgwgnam.h>

#include "menutasklist.h"

CMenuTaskList* CMenuTaskList::NewL(RWsSession& aWsSession)
    {
    CMenuTaskList* self = NewLC(aWsSession);
    CleanupStack::Pop(self);
    return self;
    }

CMenuTaskList* CMenuTaskList::NewLC(RWsSession& aWsSession)
    {
    CMenuTaskList* self = new(ELeave) CMenuTaskList(aWsSession);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

CMenuTaskList::CMenuTaskList(RWsSession& aWsSession)
: iWs(aWsSession)
    {
    }

void CMenuTaskList::ConstructL()
    {
    UpdateListL();
    }

CMenuTaskList::~CMenuTaskList()
    {
    iWgs.Close();
    }

void CMenuTaskList::UpdateListL()
    {
    User::LeaveIfError(iWs.WindowGroupList(&iWgs));
    }


TApaTask CMenuTaskList::FindRootApp(TUid aAppUid) const
    {
    TApaTask task(iWs);
    task.SetWgId(0);        // initialise task to non-existant task
    // wgId = 0 tells FindAppByUid to start looking for apps
    TInt wgId=0;
    FOREVER
        {
        CApaWindowGroupName::FindByAppUid(aAppUid, iWs, wgId);
        // KErrNotFound means that no more apps can be found
        if (wgId == KErrNotFound)
            break;
        if (IsRootWindowGroup(wgId))
            {
            // Found a root wg with the right app UID, return it.
            task.SetWgId(wgId);
            break;
            }
        }
    return task;
    }

TBool CMenuTaskList::IsRootWindowGroup(TInt aWgId) const
    {
    TInt count = iWgs.Count();
    for (TInt ii=0; ii<count; ii++)
        {
        const RWsSession::TWindowGroupChainInfo& info = iWgs[ii];
        // find the window group id and check that it has no parent
        if (info.iId == aWgId)
            return (info.iParentId <= 0);
        }
    return EFalse;
    }
