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

#include "afstorageproxy_p.h"

#include <fbs.h>

#include <QSize>
#include <QPixmap>

#include <XQConversions>

#include <afstorageglobals.h>
#include <afstorageclient.h>
#include <afentry.h>

#include "afstorageproxy.h"
#include "afserializer.h"

AfStorageProxyPrivate::AfStorageProxyPrivate(AfStorageProxy *q) : mClient(0), q_ptr(q)
{    
    QT_TRAP_THROWING(mClient = CAfStorageClient::NewL(*this));
}

AfStorageProxyPrivate::~AfStorageProxyPrivate()
{
    delete mClient;
}

bool AfStorageProxyPrivate::addActivity(int applicationId, const QString &activityId, const QVariant &activityData, const QVariantHash &metadata, const QPixmap &screenshot)
{    
    int screenshotHandle(-1);
    CFbsBitmap* bitmap(screenshot.toSymbianCFbsBitmap());
    if (bitmap) {
        screenshotHandle = bitmap->Handle();
    }
    
    CAfEntry *entry = createSaveEntry(applicationId, activityId, activityData, metadata);
    int result = mClient->addActivity(*entry, screenshotHandle);    
    delete entry;
    delete bitmap;
    
    return KErrNone == result;
}

bool AfStorageProxyPrivate::updateActivity(int applicationId, const QString &activityId, const QVariant &activityData, const QVariantHash &metadata, const QPixmap &screenshot)
{
    int screenshotHandle(-1);
    CFbsBitmap* bitmap(screenshot.toSymbianCFbsBitmap());
    if (bitmap) {
        screenshotHandle = bitmap->Handle();
    }
    
    CAfEntry *entry = createSaveEntry(applicationId, activityId, activityData, metadata);
    int result = mClient->updateActivity(*entry, screenshotHandle);    
    delete entry;
    delete bitmap;
    
    return KErrNone == result;
}

bool AfStorageProxyPrivate::saveActivity(int applicationId, const QString &activityId, const QVariant &activityData, const QVariantHash &metadata, const QPixmap &screenshot)
{
    int screenshotHandle(-1);
    CFbsBitmap* bitmap(screenshot.toSymbianCFbsBitmap());
    if (bitmap) {
        screenshotHandle = bitmap->Handle();
    }
    
    CAfEntry *entry = createSaveEntry(applicationId, activityId, activityData, metadata);
    int result = mClient->saveActivity(*entry, screenshotHandle);    
    delete entry;
    delete bitmap;
    
    return KErrNone == result;
}

bool AfStorageProxyPrivate::removeActivity(int applicationId, const QString &activityId)
{
    CAfEntry *entry = createFilterEntry(applicationId, activityId);
    int result = mClient->removeActivity(*entry);
    delete entry;
    return KErrNone == result;
}
   
bool AfStorageProxyPrivate::removeApplicationActivities(int applicationId)
{
    CAfEntry *entry = createFilterEntry(applicationId);
    int result = mClient->removeApplicationActivities(*entry);
    delete entry;
    return KErrNone == result;
}

bool AfStorageProxyPrivate::activities(QList<QVariantHash> &list)
{
    RPointerArray<CAfEntry> results;
    
    int result = mClient->activities(results);
    list.clear();
    for (int i=0; i < results.Count(); ++i) {
        list.append(extractMetadata(results[i]));
    }
   
    results.ResetAndDestroy();
    return KErrNone == result;
}

bool AfStorageProxyPrivate::applicationActivities(QStringList &list, int applicationId)
{
    RPointerArray<CAfEntry> results;
    
    CAfEntry *entry = createFilterEntry(applicationId);
    int result = mClient->applicationActivities(results, *entry);
    delete entry;
    
    list.clear();
    for (int i=0; i < results.Count(); ++i) {
        list.append(XQConversions::s60DescToQString(results[i]->ActivityId()));
    }
   
    results.ResetAndDestroy();
    return KErrNone == result;
}

bool AfStorageProxyPrivate::activityData(QVariant &data, int applicationId, const QString &activityId)
{
    CAfEntry *entry = getEntry(applicationId, activityId);
    
    if (!entry) {
        return false;
    }
    
    QVariantHash deserializedData;
    deserializedData << entry->Data(CAfEntry::Private);
    data = deserializedData[ActivityDataKeyword];
    delete entry;
    return true;
}

bool AfStorageProxyPrivate::activityMetaData(QVariantHash &metadata, int applicationId, const QString &activityId)
{
    CAfEntry *entry = getEntry(applicationId, activityId);
    
    if (entry) {
        metadata = extractMetadata(entry);
        delete entry;
        return true;
    }    
    
    return false;
}

bool AfStorageProxyPrivate::waitActivity()
{
    return KErrNone == mClient->waitActivity();
}
    
bool AfStorageProxyPrivate::launchActivity(int applicationId, const QString &activityUri)
{
    CAfEntry *entry = createFilterEntry(applicationId, activityUri);
    int result = mClient->launchActivity(*entry);
    delete entry;
    return KErrNone == result;
}

bool AfStorageProxyPrivate::getThumbnail(const QSize &size, const QString &imagePath, void *userData)
{
    HBufC *source = XQConversions::qStringToS60Desc(imagePath);
    int result = mClient->getThumbnail(TSize(size.width(), size.height()), *source, userData);
    delete source;
    return KErrNone == result;
}

bool AfStorageProxyPrivate::notifyDataChange()
{
    return KErrNone == mClient->notifyDataChange();
}


void AfStorageProxyPrivate::waitActivityRequestCompleted(int result, const TDesC8 &data)
{
    if (KErrCancel != result) {
        waitActivity();
    }
    if (KErrNone == result) {
        emit q_ptr->activityRequested(XQConversions::s60Desc8ToQString(data));
    }
}

void AfStorageProxyPrivate::getThumbnailRequestCompleted(int result, int bitmapHandle, void *userData)
{
    if (KErrNone == result) {
        CFbsBitmap* bitmap = new CFbsBitmap();
        if (KErrNone == bitmap->Duplicate(bitmapHandle)) {
            emit q_ptr->thumbnailRequested(QPixmap::fromSymbianCFbsBitmap(bitmap), userData);
        }
        delete bitmap;
    }
}

void AfStorageProxyPrivate::dataChangeNotificationCompleted(int result)
{
    if (KErrCancel != result) {
        notifyDataChange();
    }
    if (KErrNone == result) {
        emit q_ptr->dataChanged();
    }
}
 
CAfEntry *AfStorageProxyPrivate::createFilterEntry(int applicationId, const QString &activityId)
{   
    CAfEntry *entry(0); 
    QT_TRAP_THROWING(entry = CAfEntry::NewL(0, applicationId, TPtrC(static_cast<const TUint16*>(activityId.utf16())), KNullDesC(), KNullDesC8(), KNullDesC8()));
    return entry;
}

CAfEntry *AfStorageProxyPrivate::createSaveEntry(int applicationId, const QString &activityId, const QVariant &activityData, const QVariantHash &metadata)
{   
    CAfEntry *entry(0); 
    
    QT_TRAP_THROWING(
        int flags(0);
        QVariantHash privData;
        privData.insert(ActivityDataKeyword, activityData);
        RBuf8 privateBuff;
        RBuf8 publicBuff;
        CleanupClosePushL(privateBuff);
        CleanupClosePushL(publicBuff);
        privateBuff << privData;
        publicBuff << metadata;
        
        if (metadata.contains(ActivityPersistence) && metadata[ActivityPersistence].toBool()) {
            flags |= CAfEntry::Persistent;
        }
        
        if (metadata.contains(ActivityVisibility) && !metadata[ActivityVisibility].toBool()) {
            flags |= CAfEntry::Invisible;
        }
        
        HBufC *actBuff = XQConversions::qStringToS60Desc(activityId);
        CleanupStack::PushL(actBuff);
        entry = CAfEntry::NewL(flags, 
                               applicationId, 
                               *actBuff, 
                               KNullDesC, 
                               privateBuff, 
                               publicBuff);
        CleanupStack::PopAndDestroy(actBuff);
        CleanupStack::PopAndDestroy(&publicBuff);
        CleanupStack::PopAndDestroy(&privateBuff);
    );
    return entry;
}

CAfEntry *AfStorageProxyPrivate::getEntry(int applicationId, const QString &activityId)
{
    CAfEntry *entry = createFilterEntry(applicationId, activityId);
    CAfEntry *resultEntry(0);
    int result = mClient->activityData(resultEntry, *entry);
    delete entry;
    
    if (KErrNone != result) {
        delete resultEntry;
        resultEntry = 0;
    }
    
    return resultEntry;
}

QVariantHash AfStorageProxyPrivate::extractMetadata(CAfEntry *entry)
{
    QVariantHash metadata;
    metadata << entry->Data(CAfEntry::Public);
    metadata.insert(ActivityApplicationKeyword, entry->ApplicationId());
    metadata.insert(ActivityActivityKeyword, XQConversions::s60DescToQString(entry->ActivityId()));
    metadata.insert(ActivityScreenshotKeyword, XQConversions::s60DescToQString(entry->ImageSrc()));
    metadata.insert(ActivityPersistence, (entry->Flags() & CAfEntry::Persistent) ? true : false);
    metadata.insert(ActivityVisibility, (entry->Flags() & CAfEntry::Invisible) ? false : true); 
    return metadata;
}

