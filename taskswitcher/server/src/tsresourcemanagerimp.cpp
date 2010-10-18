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
 * Description :
 *
 */
#include "tsresourcemanagerimp.h"
#include "tswindowgroupsmonitorimp.h"
#include "tsiconproviderimp.h"
#include "tsidlist.h"

const TInt KSkippedApp [] = {0x00000000 /* <-- null/splashscreen*/,
                             0x20022F35 /* <-- hsapplication */,
                             0x100058F3 /* <-- sysapp*/};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CTsResourceManager* CTsResourceManager::NewL()
{
    CTsResourceManager* self =new(ELeave) CTsResourceManager();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CTsResourceManager::ConstructL()
{
    User::LeaveIfError(mWsSession.Connect());
    User::LeaveIfError(mApaSeesion.Connect());
    iMonitor = CTsWindowGroupsMonitor::NewL(*this);
    iIconProvider = CTsIconProvider::NewL(*this);
    
    const TInt count(sizeof( KSkippedApp ) / sizeof(TInt));
    iBlackList = CTsIdList::NewL();
    iBlackList->AppendL(KSkippedApp, count);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CTsResourceManager::~CTsResourceManager()
{
    delete iBlackList;
    delete iIconProvider;
    delete iMonitor;
    mApaSeesion.Close();
    mWsSession.Close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
RWsSession& CTsResourceManager::WsSession()
    {
    return mWsSession;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
RApaLsSession& CTsResourceManager::ApaSession()
    {
    return mApaSeesion;
    }

// -----------------------------------------------------------------------------
MTsWindowGroupsMonitor& CTsResourceManager::WsMonitor()
    {
    return *iMonitor;
    }

// -----------------------------------------------------------------------------
/**
 * @see MTsResourceManager::IconProvider
 */
MTsIconProvider& CTsResourceManager::IconProvider()
    {
    return *iIconProvider;
    }

// -----------------------------------------------------------------------------
/**
 * @see MTsResourceManager::ApplicationsBlackList
 */
const CTsIdList& CTsResourceManager::ApplicationsBlackList() const
    {
    return *iBlackList;
    }

// -----------------------------------------------------------------------------
TBool CTsResourceManager::IsSupported(TInt aFunction) const
    {
    return iMonitor->IsSupported(aFunction);
    }

// -----------------------------------------------------------------------------
void CTsResourceManager::HandleDataL(TInt aFunction, RReadStream& aDataStream)
    {
    iMonitor->HandleDataL(aFunction, aDataStream);
    }
