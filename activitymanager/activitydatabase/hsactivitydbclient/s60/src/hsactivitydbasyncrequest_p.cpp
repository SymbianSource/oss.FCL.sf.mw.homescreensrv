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

#include "hsactivitydbasyncrequest_p.h"
#include "hsactivitydbclient_p.h"
#include "hsserializer.h"
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivityDbAsyncRequestPrivate* 
    HsActivityDbAsyncRequestPrivate::NewL(HsActivityDbAsyncRequestObserver& observer,
                                          HsActivityDbClientPrivate& session)
{
    HsActivityDbAsyncRequestPrivate* self = 
        HsActivityDbAsyncRequestPrivate::NewLC(observer, session);
    CleanupStack::Pop(self);
    return self;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivityDbAsyncRequestPrivate* 
    HsActivityDbAsyncRequestPrivate::NewLC(HsActivityDbAsyncRequestObserver& observer,
                                           HsActivityDbClientPrivate& session)
{
    HsActivityDbAsyncRequestPrivate* self = 
        new (ELeave)HsActivityDbAsyncRequestPrivate(observer, session);
    CleanupStack::PushL(self);
    self->mDataBuf.CreateL(64);
    return self;
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivityDbAsyncRequestPrivate::HsActivityDbAsyncRequestPrivate(HsActivityDbAsyncRequestObserver &observer,
                                                                 HsActivityDbClientPrivate &session)
:
    CActive(EPriorityStandard),
    mObserver(observer),
    mSession(session),
    mRequestType(-1),
    mDataSize()
{
    CActiveScheduler::Add(this);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivityDbAsyncRequestPrivate::~HsActivityDbAsyncRequestPrivate()
{
    mDataBuf.Close();
    Cancel();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbAsyncRequestPrivate::DoCancel()
{
    if(IsActive()) {
    mSession.cancelWaitActivity();
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbAsyncRequestPrivate::RunL()
{
    RBuf8 buff;
    CleanupClosePushL(buff);
    QString data;
    int requestResult(iStatus.Int());
    if(KErrNone == requestResult) {
        if(0 < mDataSize()) {
            buff.CreateL(mDataSize());
        }
        mSession.getData(buff);
        data = QString::fromAscii(reinterpret_cast<const char*>(buff.Ptr()), 
                                  buff.Length());
        buff.Close();
    }
    mObserver.asyncRequestCompleated(requestResult, mRequestType, data);
    CleanupStack::PopAndDestroy(&buff);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbAsyncRequestPrivate::waitActivity(const QVariantHash& condition)
{
    mRequestType = WaitActivity;
    iStatus = KRequestPending;
    SetActive();
    mDataBuf << condition;
    mSession.sendDataAsync(mRequestType, TIpcArgs(&mDataBuf, &mDataSize), iStatus);
}
