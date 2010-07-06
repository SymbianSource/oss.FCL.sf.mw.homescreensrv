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

#include "afstorageclient_p.h"
#include "afasyncrequest_p.h"
#include "afstorageglobals.h"
#include "afserializer.h"
#include "afentry.h"
#include "afstorageentry.h"


// -----------------------------------------------------------------------------
template <class T>
void RPointerArrayCleanupMethod(TAny *aPtr)
{
    static_cast< RPointerArray<T>* >(aPtr)->ResetAndDestroy();
}

// -----------------------------------------------------------------------------
template <class T>
void CleanupResetAndDestroyPushL(RPointerArray<T> &array) {
    CleanupStack::PushL(TCleanupItem(&RPointerArrayCleanupMethod<T>, &array));
} 
// -----------------------------------------------------------------------------
/**
 * Constructor
 */
AfStorageClientPrivate::AfStorageClientPrivate(MAfAsyncRequestObserver & observer)
:
mImplementation(observer)
{
}

// -----------------------------------------------------------------------------
/**
 * Destructor
 */
AfStorageClientPrivate::~AfStorageClientPrivate()
{
    mImplementation.Close();
}

// -----------------------------------------------------------------------------
/**
 * Function implementation
 * @see AfStorageClient::connect
 */
int AfStorageClientPrivate::connect()
{
    TRAPD(errNo, mImplementation.connectL());
    return errNo;
}

// -----------------------------------------------------------------------------
/**
 * Function implementation
 * @see AfStorageClient::addActivity(const AfStorageEntry &,const QPixmap&)
 */
int AfStorageClientPrivate::addActivity(const AfStorageEntry &entry, 
                                        const QPixmap& imageSrc)
{
    return execute(AddActivity, entry, imageSrc);
}

// -----------------------------------------------------------------------------
/**
 * Function implementation
 * @see AfStorageClient::updateActivity(const AfStorageEntry &,const QPixmap&)
 */
int AfStorageClientPrivate::updateActivity(const AfStorageEntry &entry, 
                                           const QPixmap& imageSrc)
{
    return execute(UpdateActivity, entry, imageSrc);
}

// -----------------------------------------------------------------------------
/**
 * Function implementation
 * @see AfStorageClient::removeActivity(const AfStorageEntry &)
 */
int AfStorageClientPrivate::removeActivity(const AfStorageEntry &entry)
{
    return execute(RemoveActivity, entry, QPixmap());
}

// -----------------------------------------------------------------------------
/**
 * Function implementation
 * @see AfStorageClient::removeApplicationActivities(const AfStorageEntry &)
 */
int AfStorageClientPrivate::removeApplicationActivities(const AfStorageEntry &entry)
{
    return execute(RemoveApplicationActivities, entry, QPixmap());
}

// -----------------------------------------------------------------------------
/**
 * Function implementation
 * @see AfStorageClient::activities(QList<AfStorageEntry> &)
 */
int AfStorageClientPrivate::activities(QList<AfStorageEntry> &results)
{
    return execute(Activities, results, AfStorageEntry());
}

// -----------------------------------------------------------------------------
/**
 * Function implementation
 * @see AfStorageClient::applicationActivities(QList<AfStorageEntry> &, const AfStorageEntry &)
 */
int AfStorageClientPrivate::applicationActivities(QList<AfStorageEntry> &results,
                                                  const AfStorageEntry &templateEntry)
{
    return execute(ApplicationActivities, results, templateEntry);
}

// -----------------------------------------------------------------------------
/**
 * Function implementation
 * @see AfStorageClient::activityData(AfStorageEntry &, const AfStorageEntry &)
 */
int AfStorageClientPrivate::activityData(AfStorageEntry &resultEntry, 
                                         const AfStorageEntry &templateEntry)
{
    
    QList<AfStorageEntry> results;
    int errNo(execute(ApplicationActivity, results, templateEntry));
    if (0 == errNo) {
        resultEntry = results.first();
    }
    return errNo;
}

// -----------------------------------------------------------------------------
/**
 * Function implementation
 * @see AfStorageClient::waitActivity()
 */
int AfStorageClientPrivate::waitActivity()
{
    return execute(WaitActivity, AfStorageEntry(), QPixmap());
}

// -----------------------------------------------------------------------------
/**
 * Function implementation
 * @see AfStorageClient::getThumbnail(QSize, QString, void *)
 */
int AfStorageClientPrivate::getThumbnail(QSize size, QString imagePath, void *userData)
{
    HBufC *source = XQConversions::qStringToS60Desc(imagePath);
    TRAPD(errNo, mImplementation.getThumbnailL(TSize(size.width(), size.height()), *source, userData);)
    delete source;
    return errNo;
    
}

// -----------------------------------------------------------------------------
/**
 * Function implementation
 * @see AfStorageClient::notifyDataChange()
 */
int AfStorageClientPrivate::notifyDataChange()
{
    return execute(NotifyChange, AfStorageEntry(), QPixmap());
}

// -----------------------------------------------------------------------------
/**
 * Function implementation
 * @see AfStorageClient::launchActivity(const AfStorageEntry &)
 */
int AfStorageClientPrivate::launchActivity(const AfStorageEntry &templateEntry)
{
    return execute(LaunchActivity, templateEntry, QPixmap());
}

// -----------------------------------------------------------------------------
/**
 * Function implementation
 * @see AfStorageClient::cancelWaitActivity()
 */
int AfStorageClientPrivate::cancelWaitActivity()
{
    return execute(CancelWait, AfStorageEntry(), QPixmap());
}

// -----------------------------------------------------------------------------
/**
 * Function implementation
 * @see AfStorageClient::cancelNotifyDataChange()
 */
int AfStorageClientPrivate::cancelNotifyDataChange()
{
    return execute(CancelNotify, AfStorageEntry(), QPixmap());
}

// -----------------------------------------------------------------------------
/**
 * Function execute activity framework functinality and return results 
 */
int AfStorageClientPrivate::execute(int function, const AfStorageEntry &sourceEntry, const QPixmap& sourceImage)
{
    TRAPD(errNo,
    const CAfEntry *entry(sourceEntry.data());
    int userData(-1);
    CFbsBitmap* bitmap(sourceImage.toSymbianCFbsBitmap());
    if(0 != bitmap) {
        CleanupStack::PushL(bitmap);
        userData = bitmap->Handle();
    }
    switch (function) {
    case AddActivity:
    case UpdateActivity:
    case RemoveActivity:
    case RemoveApplicationActivities:
    case LaunchActivity:
        User::LeaveIfNull(entry);
        mImplementation.executeL(function, *entry, userData);
        break;
    
    case CancelNotify:
    case CancelWait:
    case WaitActivity:
    case NotifyChange:
        mImplementation.executeL(function);
        break;
    }
    if(0 != bitmap) {
        CleanupStack::PopAndDestroy(bitmap);
    }
    )
    return errNo;
}

// -----------------------------------------------------------------------------
/**
 * Function execute activity framework functinality and return results 
 */
int AfStorageClientPrivate::execute(int function, QList<AfStorageEntry>& resultsList,const AfStorageEntry& templateEntry)
{
    TRAPD(errNo,
    RPointerArray<CAfEntry> results;
    CleanupResetAndDestroyPushL<CAfEntry>(results);
    
    CAfEntry *entry(const_cast<CAfEntry *>(templateEntry.data()));
    if(0 == entry) {
        entry = CAfEntry::NewLC();
    }
    mImplementation.executeL(function, results, *entry);
    if(templateEntry.data() != entry) {
        CleanupStack::PopAndDestroy(entry);
    }
    
    //copy data
    while (results.Count()) {
        resultsList.append(AfStorageEntry(QSharedPointer<CAfEntry>(results[0])));
        results.Remove(0);
    }
    CleanupStack::PopAndDestroy(&results);
    )
    return errNo;
}
