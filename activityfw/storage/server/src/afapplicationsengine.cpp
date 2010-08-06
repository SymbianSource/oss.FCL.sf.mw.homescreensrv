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
#include "afapplicationsengine.h"
#include "afapplicationsstorage.h"
#include "afapplicationscollection.h"
#include "afapplicationsmonitor.h"

//------------------------------------------------------------------------------
CAfApplicationsEngine* CAfApplicationsEngine::NewL(RApaLsSession& serviceProvider, 
                                                   CAfStorage& storage)
{
    CAfApplicationsEngine *self = new(ELeave)CAfApplicationsEngine();
    CleanupStack::PushL(self);
    self->ConstructL(storage, serviceProvider);
    CleanupStack::Pop(self);
    return self;
}

//------------------------------------------------------------------------------
CAfApplicationsEngine::CAfApplicationsEngine()
{}

//------------------------------------------------------------------------------
void CAfApplicationsEngine::ConstructL(CAfStorage& storage, 
                                       RApaLsSession& serviceProvider)
{
    mCollection = CAfApplicationsCollection::NewL(serviceProvider);
    mMonitor = CAfApplicationsMonitor::NewL(serviceProvider, *mCollection);
    mStorage = CAfApplicationsStorage::NewL(storage, *mCollection);
    
    mCollection->setObserver(*mStorage);
}

//------------------------------------------------------------------------------
CAfApplicationsEngine::~CAfApplicationsEngine()
{
    delete mMonitor;
    delete mStorage;
    delete mCollection;
}
