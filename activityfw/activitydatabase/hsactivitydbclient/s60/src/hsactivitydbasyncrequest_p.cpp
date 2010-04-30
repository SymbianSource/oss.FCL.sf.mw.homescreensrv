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
#include <xqconversions>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivityDbAsyncRequestPrivate*
HsActivityDbAsyncRequestPrivate::NewL(HsActivityDbAsyncRequestObserver &observer,
                                      HsActivityDbClientPrivate &session, 
                                      TAsyncRequest requestType,
                                      void* userData)
{
    HsActivityDbAsyncRequestPrivate *self = 
    new(ELeave)HsActivityDbAsyncRequestPrivate(observer, session, requestType, userData);
    CleanupStack::PushL(self);
    self->mDataBuf.CreateL(64);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivityDbAsyncRequestPrivate*
HsActivityDbAsyncRequestPrivate::newWaitActivityL(HsActivityDbAsyncRequestObserver & observer, 
            HsActivityDbClientPrivate & session)
    {
    return HsActivityDbAsyncRequestPrivate::NewL(observer, session, EWaitActivity);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbAsyncRequestPrivate::getThumbnailLD(HsActivityDbAsyncRequestObserver &observer,
                                      HsActivityDbClientPrivate &session, 
                                      QSize size, 
                                      QString imagePath, 
                                      QString  mimeType, 
                                      void *userDdata)
{
    HsActivityDbAsyncRequestPrivate *instance = 
        HsActivityDbAsyncRequestPrivate::NewL(observer, session, EWaitGetThumbnail, userDdata);
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
        mSession.cancelWaitActivity();
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbAsyncRequestPrivate::RunL()
{
    int requestResult(iStatus.Int());
    if (KErrNone == requestResult) {
        switch (mRequestType){
        case WaitActivity: {
            RBuf8 buff;
            CleanupClosePushL(buff);
            QString data;
            if (0 < mDataSize()) {
                buff.CreateL(mDataSize());
            }
            mSession.getData(mTaskId(), buff);
            data = QString::fromAscii(reinterpret_cast<const char *>(buff.Ptr()),
                                      buff.Length());
            buff.Close();
            mObserver.asyncRequestCompleated(requestResult, mRequestType, data);
            CleanupStack::PopAndDestroy(&buff);
            break;
            }
        case EWaitGetThumbnail: {
            CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
            CleanupStack::PushL(bitmap);
            User::LeaveIfError(bitmap->Duplicate(mBitmapId()));
            mSession.getData(mTaskId(), mDataBuf);//ACK Bitmap copy
            mObserver.asyncRequestCompleated(requestResult, mRequestType, QPixmap::fromSymbianCFbsBitmap(bitmap), mUserData);
            CleanupStack::PopAndDestroy(bitmap);
            break;
            }
        }
        mSession.Pop(this);
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
