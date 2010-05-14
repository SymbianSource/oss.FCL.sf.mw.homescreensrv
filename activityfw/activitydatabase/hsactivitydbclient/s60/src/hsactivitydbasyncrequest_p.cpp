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
#include "hsactivitydbclient.h"
#include <fbs.h>
#include <XQConversions>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivityDbAsyncRequestPrivate*
HsActivityDbAsyncRequestPrivate::NewLC(HsActivityDbAsyncRequestObserver &observer,
                                      HsActivityDbClientPrivate &session, 
                                      TAsyncRequest requestType,
                                      void* userData)
{
    HsActivityDbAsyncRequestPrivate *self = 
    new(ELeave)HsActivityDbAsyncRequestPrivate(observer, session, requestType, userData);
    CleanupStack::PushL(self);
    self->mDataBuf.CreateL(64);
    return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbAsyncRequestPrivate::waitActivityLD(
                                HsActivityDbAsyncRequestObserver & observer, 
                                HsActivityDbClientPrivate & session, 
                                const QVariantHash &activity)
{
    HsActivityDbAsyncRequestPrivate *self =
        HsActivityDbAsyncRequestPrivate::NewLC(observer, 
                                               session, 
                                               EWaitActivity);
    self->mSession.PushL(self);
    CleanupStack::Pop(self);
    self->waitActivity(activity);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbAsyncRequestPrivate::notifyDataChangeLD(
                                   HsActivityDbAsyncRequestObserver &observer, 
                                   HsActivityDbClientPrivate &session)
{
    HsActivityDbAsyncRequestPrivate *self = 
        HsActivityDbAsyncRequestPrivate::NewLC(observer, 
                                              session,
                                              ENotifyDataChange);
    self->mSession.PushL(self);
    CleanupStack::Pop(self);
    self->notifyDataChange();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbAsyncRequestPrivate::getThumbnailLD(HsActivityDbAsyncRequestObserver &observer,
                                      HsActivityDbClientPrivate &session, 
                                      QSize size, 
                                      const QString &imagePath, 
                                      const QString &mimeType, 
                                      void *userDdata)
{
    HsActivityDbAsyncRequestPrivate *instance = 
        HsActivityDbAsyncRequestPrivate::NewLC(observer, session, EWaitGetThumbnail, userDdata);
    session.PushL(instance);
    CleanupStack::Pop(instance);
    instance->getThumbnail( size, imagePath, mimeType);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivityDbAsyncRequestPrivate::HsActivityDbAsyncRequestPrivate(HsActivityDbAsyncRequestObserver &observer,
                                                                 HsActivityDbClientPrivate &session,
                                                                 TAsyncRequest requestType, 
                                                                 void* userData)
    :
    CActive(EPriorityStandard),
    mObserver(observer),
    mSession(session),
    mRequestType(requestType),
    mUserData(userData)
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
    delete mBitmapPath;
    delete mBitmapMimeType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbAsyncRequestPrivate::DoCancel()
{
    if (IsActive()) {
        switch (mRequestType) {
        case EWaitActivity: mSession.cancelWaitActivity(); break;
        case ENotifyDataChange: mSession.cancelNotifyDataChange(); break;
        };
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbAsyncRequestPrivate::RunL()
{
    switch (mRequestType) {
    case WaitActivity: {
        QString data;
        if (KErrNone == iStatus.Int()) {
            RBuf8 buff;
            CleanupClosePushL(buff);
            if (0 < mDataSize()) {
                buff.CreateL(mDataSize());
            }
            mSession.getData(mTaskId(), buff);
            data = QString::fromAscii(reinterpret_cast<const char *>(buff.Ptr()),
                                      buff.Length());
            CleanupStack::PopAndDestroy(&buff);
        }
        mObserver.asyncRequestCompleated(iStatus.Int(), mRequestType, data);
        mSession.Pop(this);
        delete this;
        break;
        }
    case EWaitGetThumbnail: {
        QPixmap pixmap;
        if (KErrNone == iStatus.Int()) {
            CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
            CleanupStack::PushL(bitmap);
            User::LeaveIfError(bitmap->Duplicate(mBitmapId()));
            mSession.getData(mTaskId(), mDataBuf);//ACK Bitmap copy
            pixmap = QPixmap::fromSymbianCFbsBitmap(bitmap);
            CleanupStack::PopAndDestroy(bitmap);
        }
        mObserver.asyncRequestCompleated(iStatus.Int(), mRequestType, pixmap, mUserData);
        mSession.Pop(this);
        delete this;
        break;
        }
    case ENotifyDataChange: {
        mObserver.asyncRequestCompleated(iStatus.Int(),mRequestType);
        mSession.Pop(this);
        delete this;
        }
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbAsyncRequestPrivate::waitActivity(const QVariantHash &condition)
{
    iStatus = KRequestPending;
    SetActive();
    mDataSize = condition.find(ActivityApplicationKeyword).value().toInt();
    TPtrC8 actId(KNullDesC8);
    TPtrC8 desc(KNullDesC8);
    mSession.sendDataAsync(mRequestType, TIpcArgs(&mDataSize, &actId, &desc, &mTaskId), iStatus);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbAsyncRequestPrivate::getThumbnail(QSize size, QString imagePath, QString  mimeType)
{
    iStatus = KRequestPending;
    SetActive();
    mBitmapId = size.width();
    mTaskId = size.height();
    mBitmapPath = XQConversions::qStringToS60Desc(imagePath);
    mBitmapMimeType = XQConversions::qStringToS60Desc8(mimeType);
    mSession.sendDataAsync(mRequestType, TIpcArgs(&mBitmapId, &mTaskId, mBitmapPath, mBitmapMimeType), iStatus);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbAsyncRequestPrivate::notifyDataChange()
{
    iStatus = KRequestPending;
    SetActive();
    mSession.sendDataAsync(ENotifyDataChange, TIpcArgs(), iStatus);
}
