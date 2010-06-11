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
#include "tswindowgroupsobserver.h"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C CTsResourceManager* CTsResourceManager::NewL()
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
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C CTsResourceManager::~CTsResourceManager()
{
    mApaSeesion.Close();
    mWsSession.Close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C RWsSession& CTsResourceManager::WsSession()
{
    return mWsSession;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
EXPORT_C RApaLsSession& CTsResourceManager::ApaSession()
{
    return mApaSeesion;
}
