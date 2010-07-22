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
#include <tswindowgroupsobserver.h>
#include "tstaskmonitorglobals.h"
#include "hsrunningappserver.h"
#include "hsrunningappsession.h"
#include "tsrunningappstorage.h"
#include "tsscreenshotprovider.h"
#include "tsbackstepping.h"

_LIT(KErrObserverExists, "Observer already exists");
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CRunningAppServer::CRunningAppServer()
:
CServer2(EPriorityStandard)
{
    // No implementation required
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CRunningAppServer::~CRunningAppServer()
{
    delete mBacksteppingEngine;
    delete mScreenshotProviderStarter;
    delete mStorage;
    delete mMonitor;
    mObservers.ResetAndDestroy();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CRunningAppServer* CRunningAppServer::NewLC()
{
    CRunningAppServer* self = new (ELeave) CRunningAppServer();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CRunningAppServer::ConstructL()
{
    StartL(KRunningAppServerName);
    User::LeaveIfError(mWsSession.Connect());
    mResources = CTsResourceManager::NewL();
    mMonitor = CTsWindowGroupsMonitor::NewL(*mResources);
    mStorage = CRunningAppStorage::NewL(*this, *mResources, *mMonitor);
    TRAP_IGNORE(mScreenshotProviderStarter = CTsScreenshotProvider::NewL(*mStorage);
    mBacksteppingEngine = CTsBackstepping::NewL(*mMonitor);)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSession2* CRunningAppServer::NewSessionL(const TVersion &, const RMessage2&) const
{
    return CRunningAppSession::NewL(*const_cast<CRunningAppServer *>(this),
                                    *const_cast<CRunningAppServer *>(this)->mStorage,
                                    *const_cast<CRunningAppServer *>(this)->mStorage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CRunningAppServer::DataChanged()
{
    while (0 < mObservers.Count()) {
        mObservers[0]->DataChanged();
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CRunningAppServer::PushL(CHsDataObserver* observer)
{
    ( 0 <= mObservers.Find(observer)) ?
    User::Panic(KErrObserverExists, KErrAlreadyExists) :
    mObservers.AppendL(observer);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CRunningAppServer::Pop(CHsDataObserver* observer)
{
    const TInt offset(mObservers.Find(observer));
    if(0 <= offset) {
        mObservers.Remove(offset);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CRunningAppServer::Cancel(const RMessage2& reason)
{
    for (TInt iter(mObservers.Count() - 1); 0 <= iter; --iter) {
        mObservers[iter]->Cancel(reason);
    }
    reason.Complete(KErrNone);
}
