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
#include "hsrunningappsession.h"
#include "tstaskmonitorglobals.h"

#include "hsdataobservertask.h"
#include "hsdataprovidertask.h"
#include "tsdatatask.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CRunningAppSession::CRunningAppSession(MHsDataObserverStorage& observerStorage,
                                       const MHsDataProvider& dataProvider,
                                       MTsDataStorage& dataStorage)
:
    mObserverStorage(observerStorage),
    mDataProvider(dataProvider),
    mDataStorage(dataStorage)
{
    // No implementation required
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CRunningAppSession::~CRunningAppSession()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CRunningAppSession* CRunningAppSession::NewL(MHsDataObserverStorage& observerStorage, 
                                             const MHsDataProvider& dataProvider,
                                             MTsDataStorage& dataStorage)
{
    CRunningAppSession* self = new (ELeave) CRunningAppSession(observerStorage, 
                                                               dataProvider,
                                                               dataStorage);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CRunningAppSession::ConstructL()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CRunningAppSession::ServiceL(const RMessage2& message)
{
    switch (message.Function()) {
    case SubscribeRunningAppChanges:
        CHsDataObserverTask::ExecuteLD(mObserverStorage, message);
        break;
    
    case CancelSubscribeRunningAppChanges:
        mObserverStorage.Cancel(message);
        break;
    
    case GetRunningAppInfo:
    case FlushData:
        HsDataProviderTask::ExecuteL(mDataProvider, message);
        break;
        
    case RegisterScreenshotMessage:
    case UnregisterScreenshotMessage:
    case VisibilityChange:
        TsDataTask::ExecuteL(mDataStorage, message);
        break;
    
    default:
        message.Complete(CServer2::EBadMessageNumber);
    }
}
