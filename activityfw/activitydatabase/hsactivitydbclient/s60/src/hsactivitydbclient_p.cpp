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
#include <fbs.h>
#include <s32mem.h>

#include <qvariant.h>
#include <XQConversions>

#include "hsactivitydbclient_p.h"
#include "hsactivitydbasyncrequest_p.h"
#include "hsactivityglobals.h"
#include "hsserializer.h"
#include "afentry.h"



// -----------------------------------------------------------------------------
/**
 * Constructor
 */
HsActivityDbClientPrivate::HsActivityDbClientPrivate(HsActivityDbAsyncRequestObserver &observer):
    mObserver(observer)
{

}

// -----------------------------------------------------------------------------
/**
 * Destructor
 */
HsActivityDbClientPrivate::~HsActivityDbClientPrivate()
{
    mAsyncTasks.ResetAndDestroy();
    Close();
}

// -----------------------------------------------------------------------------
/**
 * Function establish connection to activity server
 * @return 0 on succees, error code otherwise
 */
int HsActivityDbClientPrivate::connect()
{
    TRAPD(errNo, connectL());
    return errNo;
}

// -----------------------------------------------------------------------------
/**
 * Interface implementation.
 * @see int HsActivityDbClientInterface::addActivity(const QVariantHash &)
 */
int HsActivityDbClientPrivate::addActivity(const QVariantHash &privateData,
                                           const QVariantHash &publicData)
{
    return execute(AddActivity, privateData, publicData);
    
}

// -----------------------------------------------------------------------------
/**
 * Interface implementation.
 * @see int HsActivityDbClientInterface::updateActivity(const QVariantHash &)
 */
int HsActivityDbClientPrivate::updateActivity(const QVariantHash &privateData,
                                              const QVariantHash &publicData)
{
    return execute(UpdateActivity, privateData, publicData);
}

// -----------------------------------------------------------------------------
/**
 * Interface implementation.
 * @see int HsActivityDbClientInterface::removeActivity(const QVariantHash &)
 */
int HsActivityDbClientPrivate::removeActivity(const QVariantHash &activity)
{
    return execute(RemoveActivity, QVariantHash(), activity);
}

// -----------------------------------------------------------------------------
/**
 * Interface implementation.
 * @see int HsActivityDbClientInterface::removeApplicationActivities(const QVariantHash &)
*/

int HsActivityDbClientPrivate::removeApplicationActivities(const QVariantHash &activity)
{
    return execute(RemoveApplicationActivities, QVariantHash(), activity);
}

// -----------------------------------------------------------------------------
/**
 * Interface implementation.
 * @see int HsActivityDbClientInterface::activities(QList<QVariantHash> &);
 */
int HsActivityDbClientPrivate::activities(QList<QVariantHash>& result)
{
    return execute(Activities, result, QVariantHash());
}

// -----------------------------------------------------------------------------
/**
 * Interface implementation.
 * @see int HsActivityDbClientInterface::applicationActivities(QList<QVariantHash> &, const QVariantHash &)
 */
int HsActivityDbClientPrivate::applicationActivities(QList<QVariantHash>& result,
        const QVariantHash &condition)
{
    return execute(ApplicationActivities, result, condition);
}

// -----------------------------------------------------------------------------
/**
 * Interface implementation.
 * @see int HsActivityDbClientInterface::activityData(QVariant &, const QVariantHash &)
 */
int HsActivityDbClientPrivate::activityData(QVariant &result, const QVariantHash &activity)
{
    TRAPD(errNo,
    User::LeaveIfError(checkDataConstraint(ApplicationActivity, activity));
    activityDataL(result, activity);)
    return errNo;
}

// -----------------------------------------------------------------------------
/**
 * Interface implementation.
 * @see int HsActivityDbClientInterface::waitActivity(const QVariantHash &)
 */
int HsActivityDbClientPrivate::waitActivity(const QVariantHash &activity)
{
    return execute(WaitActivity, QVariantHash(), activity);
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
    return execute(NotifyChange, QVariantHash(), QVariantHash());
}

// -----------------------------------------------------------------------------
/**
 * Interface implementation.
 * @see int HsActivityDbClientInterface::launchActivity(const QVariantHash &)
 */
int HsActivityDbClientPrivate::launchActivity(const QVariantHash &activity)
{
    return execute(LaunchActivity, QVariantHash(), activity);
}

// -----------------------------------------------------------------------------
/**
 * Interface implementation.
 * @see int HsActivityDbClientInterface::cancelWaitActivity()
 */
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
/**
 * Function start activity server process.
 * Function can leave on failure.
 */
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
/**
 * Function establish connection to activity server.
 * Function can leave on failure
 */
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
/**
 * Function execute remote call request.
 * @param function - remote function identyfier
 * @param activity - remote function parameters
 */
void HsActivityDbClientPrivate::execSimpleRequestL(int function,
                                                   const QVariantHash &privateData,
                                                   const QVariantHash &publicData)
{

    TPckgBuf<TInt> bitmapHdl(0);
    int flags(0);
    if(publicData.end() != publicData.find(ActivityPersistence) &&
       publicData[ActivityPersistence].toBool()) {
        flags |= CAfEntry::Persistent;
    }
    if(publicData.end() != publicData.find(ActivityVisibility) &&
       !publicData[ActivityVisibility].toBool()) {
        flags |= CAfEntry::Invisible;
    }
    CFbsBitmap* bitmap(0);
    if (publicData.end() != publicData.find(ActivityScreenshotKeyword)) {
        bitmap = publicData[ActivityScreenshotKeyword].value<QPixmap>().toSymbianCFbsBitmap();
        if (bitmap) {
            CleanupStack::PushL(bitmap);
            bitmapHdl = bitmap->Handle();
        }
        
        
    }

    RBuf8 prvBuffer, pubBuffer, data;
    CleanupClosePushL(data);
    CleanupClosePushL(prvBuffer);
    CleanupClosePushL(pubBuffer);

    prvBuffer << privateData;
    if (publicData.end() != publicData.find(ActivityScreenshotKeyword)) {
        QVariantHash localData(publicData);
        localData.remove(ActivityScreenshotKeyword);
        pubBuffer << localData;
    } else {
        pubBuffer << publicData;
    }
    
    HBufC *actId = XQConversions::qStringToS60Desc(publicData[ActivityActivityKeyword].toString());
    CleanupStack::PushL(actId);
    CAfEntry *entry = CAfEntry::NewL(flags,
                                     publicData[ActivityApplicationKeyword].toInt(),
                                     *actId,
                                     KNullDesC,
                                     prvBuffer,
                                     pubBuffer);
    CleanupStack::PopAndDestroy(actId);
    CleanupStack::PopAndDestroy(&pubBuffer);
    CleanupStack::PopAndDestroy(&prvBuffer);

    CleanupStack::PushL(entry);
    data.CreateL(entry->Size());
    RDesWriteStream stream(data);
    CleanupClosePushL(stream);
    stream << (*entry);
    CleanupStack::PopAndDestroy(&stream);
    CleanupStack::PopAndDestroy(entry);
    User::LeaveIfError(SendReceive(function, TIpcArgs(&data, &bitmapHdl)));
    
    CleanupStack::PopAndDestroy(&data);
    if (0 != bitmap) {
        CleanupStack::PopAndDestroy(bitmap);
    }

}

// -----------------------------------------------------------------------------
/**
 * Function retrieve all stored activity
 * Function can leave on failure
 * @param result - list of activity
 */
void HsActivityDbClientPrivate::activitiesL(QList<QVariantHash>& result)
{
    result.clear();

    RBuf8 buffer;
    CleanupClosePushL(buffer);

    TPckgBuf<int> emptyFilter(0), length(0), taskId(0);
    User::LeaveIfError(SendReceive(Activities,
                                   TIpcArgs(&emptyFilter, &length, &taskId)));

    CAfEntry::ReallocL(buffer, length());

    User::LeaveIfError(SendReceive(GetData, TIpcArgs(&taskId, &buffer)));

    RPointerArray<CAfEntry> entries;
    CleanupClosePushL(entries);

    entries << buffer;
    buffer.Close();//release unneeded resources


    while(entries.Count()) {
        QVariantHash publicData;
        publicData << entries[0]->Data(CAfEntry::Public);
        publicData.insert(ActivityScreenshotKeyword,
                          QString::fromUtf16(entries[0]->ImageSrc().Ptr(),
                                             entries[0]->ImageSrc().Length()));
        result.append(publicData);
        entries.Remove(0);
    }
    CleanupStack::PopAndDestroy(&entries);
    CleanupStack::PopAndDestroy(&buffer);
}

// -----------------------------------------------------------------------------
/**
 * Function retrieve all stored activity
 * Function can leave on failure
 * @param result - list of activity
 * @param cond - request conditions
 */
void HsActivityDbClientPrivate::applicationActivitiesL(QList<QVariantHash>& result,
                                                     const QVariantHash & condition)
{
    result.clear();

    RBuf8 buffer;
    CleanupClosePushL(buffer);
    CAfEntry *entry = CAfEntry::NewLC(0,
                                      condition[ActivityApplicationKeyword].toInt(),
                                      KNullDesC,
                                      KNullDesC,
                                      KNullDesC8,
                                      KNullDesC8);//filtering using application id only
    CAfEntry::ReallocL(buffer, entry->Size());
    RDesWriteStream writer(buffer);
    CleanupClosePushL(writer);
    writer << (*entry);
    CleanupStack::PopAndDestroy(&writer);
    CleanupStack::PopAndDestroy(entry);

    TPckgBuf<int> length(0), taskId(0);
    User::LeaveIfError(SendReceive(ApplicationActivities,
                                   TIpcArgs(&buffer, &length, &taskId)));

    CAfEntry::ReallocL(buffer, length());

    User::LeaveIfError(SendReceive(GetData, TIpcArgs(&taskId, &buffer)));

    RPointerArray<CAfEntry> entries;
    CleanupClosePushL(entries);

    entries << buffer;
    buffer.Close();//release unneeded resources


    while(entries.Count()) {
        QVariantHash publicData;
        publicData << entries[0]->Data(CAfEntry::Public);
        publicData.insert(ActivityScreenshotKeyword,
                          QString::fromUtf16(entries[0]->ImageSrc().Ptr(),
                                             entries[0]->ImageSrc().Length()));
        result.append(publicData);
        entries.Remove(0);
    }
    CleanupStack::PopAndDestroy(&entries);
    CleanupStack::PopAndDestroy(&buffer);
}

// -----------------------------------------------------------------------------
/**
 * Function retrieve private data of stored activity
 * Function can leave on failure
 * @param result - list of activity
 * @param activity - request conditions
 */
void HsActivityDbClientPrivate::activityDataL(QVariant &result, const QVariantHash &activity)
{
    result.clear();

    RBuf8 buffer;
    CleanupClosePushL(buffer);

    {   // prepare entry to send
        HBufC *activityId = XQConversions::qStringToS60Desc(activity[ActivityActivityKeyword].toString());
        CleanupStack::PushL(activityId);

        CAfEntry *entry = CAfEntry::NewLC(0,
                                          activity[ActivityApplicationKeyword].toInt(),
                                          *activityId,
                                          KNullDesC,
                                          KNullDesC8,
                                          KNullDesC8);//filtering using application id only
        CAfEntry::ReallocL(buffer, entry->Size());
        RDesWriteStream writer(buffer);
        CleanupClosePushL(writer);
        writer << (*entry);
        CleanupStack::PopAndDestroy(&writer);
        CleanupStack::PopAndDestroy(entry);
        CleanupStack::PopAndDestroy(activityId);
    }

    {   // get data
        TPckgBuf<int> length(0), taskId(0);
        User::LeaveIfError(SendReceive(ApplicationActivity, TIpcArgs(&buffer, &length, &taskId)));

        CAfEntry::ReallocL(buffer, length());
        User::LeaveIfError(SendReceive(GetData, TIpcArgs(&taskId, &buffer)));
    }

    {   // read data to Qt structure
        CAfEntry *entry = CAfEntry::NewLC();
        RDesReadStream reader(buffer);
        CleanupClosePushL(reader);
        entry->InternalizeL(reader);
        CleanupStack::PopAndDestroy(&reader);

        buffer.Close(); //release unneeded resources

        QVariantHash privateData;
        privateData << entry->Data(CAfEntry::Private);
        result = privateData.value(ActivityDataKeyword);

        CleanupStack::PopAndDestroy(entry);
    }
    CleanupStack::PopAndDestroy(&buffer);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void HsActivityDbClientPrivate::launchActivityL(const QVariantHash &activity)
{
    TPckgC<TInt> applicationId(activity[ActivityApplicationKeyword].toInt());
    HBufC8 *activityId = XQConversions::qStringToS60Desc8(activity[ActivityActivityKeyword].toString());
    CleanupStack::PushL(activityId);
    TPtrC8 empty(KNullDesC8);
    User::LeaveIfError(SendReceive(LaunchActivity,
                                   TIpcArgs(&applicationId, activityId, &empty, &empty)));
    CleanupStack::PopAndDestroy(activityId);

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
/**
 * Function get cached data from server
 * @param taskId - request task id
 * @param dst - destination, preallocated buffer
 */
void HsActivityDbClientPrivate::getData(int taskId, RBuf8 &data)
{
    TPckgBuf<int> requestId(taskId);
    SendReceive(GetData, TIpcArgs(&requestId, &data));
}

// -----------------------------------------------------------------------------
/**
 * Function initialize aync request
 * @param func - requested function
 * @param data - request data
 * @param status - request status
 */
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::execute(int func, const QVariantHash &privateData, const QVariantHash &publicData)
{
    TRAPD(errNo,
    User::LeaveIfError(checkDataConstraint(func, publicData));
    switch (func) {
    case AddActivity:
    case UpdateActivity:
    case RemoveActivity:
    case RemoveApplicationActivities:
        execSimpleRequestL(func, privateData, publicData);
        break;
    
    case LaunchActivity:
        launchActivityL(publicData);
        break;
    
    case NotifyChange:
        HsActivityDbAsyncRequestPrivate::notifyDataChangeLD(mObserver, *this);
        break;
    
    case WaitActivity:
        HsActivityDbAsyncRequestPrivate::waitActivityLD(mObserver,*this, publicData);
        break;
    
    }
    
    )
    return errNo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::execute(int func, 
                                       QList<QVariantHash>&dst, 
                                       const QVariantHash &src)
{
    TRAPD(errNo,
    User::LeaveIfError(checkDataConstraint(func, src));
    switch (func) {
    case Activities: activitiesL(dst);break;
    case ApplicationActivities: applicationActivitiesL(dst, src); break;
    }
    )
    return errNo;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
int HsActivityDbClientPrivate::checkDataConstraint(int func, const QVariantHash &data)
{
    int retVal(KErrNone);
    QStringList constraints;
    switch(func) {
    case AddActivity:
    case UpdateActivity:
        constraints << ActivityApplicationKeyword 
                    << ActivityActivityKeyword 
                    << ActivityScreenshotKeyword;
        break;
    
    case ApplicationActivity:
    case LaunchActivity:
    case RemoveActivity:
        constraints << ActivityApplicationKeyword
                    << ActivityActivityKeyword;
                    break;
    
    case WaitActivity:
    case RemoveApplicationActivities:
        constraints << ActivityApplicationKeyword;
        break;
    }
    foreach (QString constraint, constraints) {
        if (data.end() == data.find(constraint)) {
            retVal = KErrCorrupt;
            break;
        }
    }
    return retVal;
}
