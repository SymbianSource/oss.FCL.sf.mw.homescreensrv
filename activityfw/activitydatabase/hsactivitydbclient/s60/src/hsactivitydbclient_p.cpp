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

#include "hsactivitydbclient_p.h"
#include "hsactivitydbasyncrequest_p.h"
#include "hsactivityglobals.h"
#include "hsserializer.h"
#include <qvariant.h>
#include <XQConversions>


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivityDbClientPrivate::HsActivityDbClientPrivate(HsActivityDbAsyncRequestObserver &observer):
    mObserver(observer)
{
    
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HsActivityDbClientPrivate::~HsActivityDbClientPrivate()
{
    mAsyncTasks.ResetAndDestroy();
    Close();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::connect()
{
    TRAPD(errNo, connectL());
    return errNo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::addActivity(const QVariantHash &activity)
{
    int errNo(KErrCorrupt);
    if (activity.end() != activity.find(ActivityApplicationKeyword) &&
        activity.end() != activity.find(ActivityActivityKeyword)) {
        TRAP(errNo, execSimpleRequestL(AddActivity, activity);)
    }
    return errNo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::updateActivity(const QVariantHash &activity)
{
    int errNo(KErrCorrupt);
    if (activity.end() != activity.find(ActivityApplicationKeyword) &&
        activity.end() != activity.find(ActivityActivityKeyword)) {
        TRAP(errNo, execSimpleRequestL(UpdateActivity, activity);)
    }
    return errNo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::removeActivity(const QVariantHash &activity)
{
    int errNo(KErrCorrupt);
    if (activity.end() != activity.find(ActivityApplicationKeyword) &&
        activity.end() != activity.find(ActivityActivityKeyword)) {
        TRAP(errNo, execSimpleRequestL(RemoveActivity, activity);)
    }
    return errNo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::removeApplicationActivities(const QVariantHash &activity)
{
    int errNo(KErrCorrupt);
    if (activity.end() != activity.find(ActivityApplicationKeyword)) {
        TRAP(errNo, execSimpleRequestL(RemoveApplicationActivities, activity);)
    }
    return errNo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::activities(QList<QVariantHash>& result)
{
    TRAPD(errNo, activitiesL(result));
    return errNo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::applicationActivities(QList<QVariantHash>& result,
        const QVariantHash &condition)
{
    TRAPD(errNo, applicationActivitiesL(result, condition));
    return errNo;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::waitActivity(const QVariantHash &activity)
{
    TRAPD(errNo, waitActivityL(activity);)
    return errNo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::getThumbnail(QSize size, QString imagePath, QString  mimeType, void *userDdata)
{
    TRAPD(errNo, getThumbnailL(size, imagePath, mimeType, userDdata);)
    return errNo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::notifyDataChange()
{
    TRAPD(errNo,HsActivityDbAsyncRequestPrivate::notifyDataChangeLD(mObserver, *this);)
    return errNo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::launchActivity(const QVariantHash &activity)
{
    TRAPD(errNo, execSimpleRequestL(LaunchActivity, activity);)
    return errNo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::cancelWaitActivity()
{
    return SendReceive(CancelWait, TIpcArgs());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::cancelNotifyDataChange()
{
    return SendReceive(CancelNotify, TIpcArgs());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::startServerL()
{
    RProcess server;
    const TUidType uid(KNullUid, KNullUid, KActivityServerUid);
    User::LeaveIfError(server.Create(KActivityServerName, KNullDesC, uid));
    TRequestStatus stat;
    server.Rendezvous(stat);
    if (stat != KRequestPending) {
        server.Kill(0);
    } else {
        server.Resume();
    }
    User::WaitForRequest(stat);
    int errNo = (EExitPanic == server.ExitType()) ?
                KErrGeneral : stat.Int();
    if (KErrCancel == errNo) {
        errNo = KErrNone;
    }
    server.Close();
    User::LeaveIfError(errNo);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::connectL()
{
    const int asyncMessageSlots(12);
    const int maxRetry(4);

    TInt retry = maxRetry;
    TInt errNo(KErrNone);
    do {
        errNo = CreateSession(KActivityServerName, TVersion(0, 0, 0), asyncMessageSlots);
        if (KErrNotFound != errNo && KErrServerTerminated != errNo) {
            retry =0;
        } else {
            TRAP(errNo, startServerL());
            if (KErrNone != errNo && KErrAlreadyExists != errNo) {
                retry =0;
                errNo = CreateSession(KActivityServerName, TVersion(0, 0, 0), asyncMessageSlots);
            }
        }
    } while (--retry > 0);
    User::LeaveIfError(errNo);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::execSimpleRequestL(int function, const QVariantHash &activity)
{
    TPckgBuf<TInt> appId( activity.find(ActivityApplicationKeyword).value().toInt() );
    HBufC8 *actId = XQConversions::qStringToS60Desc8(activity.find(ActivityActivityKeyword).value().toString());
    CleanupStack::PushL(actId);
    TPckgBuf<TInt> taskId(0);
    RBuf8 data;
    CleanupClosePushL(data);
    data.CreateL(256);
    data << activity;
    User::LeaveIfError(SendReceive(function, TIpcArgs(&appId, actId, &data, &taskId)));
    CleanupStack::PopAndDestroy(&data);
    CleanupStack::PopAndDestroy(actId);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::activitiesL(QList<QVariantHash>& result)
{
    TPckgBuf<TInt> appId(0);
    TPtrC8 actId(KNullDesC8);
    TPtrC8 desc(KNullDesC8);
    TPckgBuf<TInt> taskId(0);
    User::LeaveIfError(SendReceive(Activities, 
                       TIpcArgs(&appId, &actId, &desc, &taskId)));

    int sizeBuf(appId());
    RBuf8 data;
    CleanupClosePushL(data);
    data.Create(sizeBuf);
    User::LeaveIfError(SendReceive(GetData, TIpcArgs(&taskId, &data)));
    
    result << data;
    CleanupStack::PopAndDestroy(&data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::applicationActivitiesL(QList<QVariantHash>& result,
                                                     const QVariantHash & condition)
{
    TPckgBuf<TInt> appId = condition.find(ActivityApplicationKeyword).value().toInt();
    TPtrC8 actId(KNullDesC8);
    TPtrC8 desc(KNullDesC8);
    TPckgBuf<TInt> taskId(0);
    User::LeaveIfError(SendReceive(ApplicationActivities, 
                       TIpcArgs(&appId, &actId, &desc, &taskId)));

    int sizeBuf(appId());
    RBuf8 data;
    CleanupClosePushL(data);
    data.Create(sizeBuf);
    User::LeaveIfError(SendReceive(GetData, TIpcArgs(&taskId, &data)));
    
    result << data;
    CleanupStack::PopAndDestroy(&data);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::waitActivityL(const QVariantHash &activity)
{
    HsActivityDbAsyncRequestPrivate::waitActivityLD(mObserver,*this, activity);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::getThumbnailL(QSize size, QString imagePath, QString  mimeType, void *userDdata)
{
    HsActivityDbAsyncRequestPrivate::getThumbnailLD(mObserver,
                              *this, size, imagePath, mimeType, userDdata);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::getData(int taskId, RBuf8 &data)
{
    TPckgBuf<int> requestId(taskId);
    SendReceive(GetData, TIpcArgs(&requestId, &data));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::sendDataAsync(int func,
                                              const TIpcArgs &data, 
                                              TRequestStatus& status)
{
    SendReceive(func, data, status);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::PushL(HsActivityDbAsyncRequestPrivate * task)
{
    (KErrNotFound == mAsyncTasks.Find(task)) ?
        mAsyncTasks.AppendL(task):
        User::Leave(KErrAlreadyExists);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::Pop(HsActivityDbAsyncRequestPrivate *task)
{
    const TInt offset(mAsyncTasks.Find(task));
    if (KErrNotFound != offset) {
        mAsyncTasks.Remove(offset);
    }
}
