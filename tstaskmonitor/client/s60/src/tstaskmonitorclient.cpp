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

#include <s32mem.h>

#include "tstaskmonitorclient.h"
#include "tstaskmonitorclientimpl.h"

CTsTaskMonitorClient *CTsTaskMonitorClient::NewL()
{
    CTsTaskMonitorClient* self = NewLC();
    CleanupStack::Pop(self);
    return self;
}

CTsTaskMonitorClient *CTsTaskMonitorClient::NewLC()
{
    CTsTaskMonitorClient* self = new (ELeave) CTsTaskMonitorClient;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

void CTsTaskMonitorClient::ConstructL()
{
    iImpl = CTsTaskMonitorClientImpl::NewL();
}

CTsTaskMonitorClient::CTsTaskMonitorClient()
{
}

CTsTaskMonitorClient::~CTsTaskMonitorClient()
{
    delete iImpl;
}

void CTsTaskMonitorClient::TaskListL(RTsFswArray& tasks)
{
    iImpl->TaskListL(tasks);
}

void CTsTaskMonitorClient::Subscribe(MTsTaskMonitorObserver& aObserver)
{
    iImpl->Subscribe(aObserver);
}

void CTsTaskMonitorClient::CancelSubscribe()
{
    iImpl->CancelSubscribe();
}
