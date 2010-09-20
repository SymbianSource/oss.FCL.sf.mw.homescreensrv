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

#include "activitytsmodel.h"

#include <stdexcept>

#include <QPixmap>
#include <QUrl>

#include <QServiceManager>
#include <XQSettingsManager>

#include "afstorageglobals.h"

QTM_USE_NAMESPACE

namespace {
    const char KActivityManager[] = "com.nokia.qt.activities.ActivityManager";
    
    const int KTsDeviceDialogUid = 0x2002677F;
    const int KItemsLimit = 0x00000001;
}

ActivityTsModel::ActivityTsModel(QObject *parent) : QObject(parent), mAfManager(0) , mMaxItems(10)
{
    {
        QServiceManager serviceManager;
        mAfManager = serviceManager.loadInterface(KActivityManager);
        if (mAfManager) {
            mAfManager->setParent(this);
        } else {
            throw std::runtime_error("Cannot create critical com.nokia.qt.activities.ActivityManager service");
        }
    }
    
    {
        XQSettingsManager crManager;
        QVariant itemsNumberVariant = crManager.readItemValue(XQCentralRepositorySettingsKey(KTsDeviceDialogUid, KItemsLimit), XQSettingsManager::TypeInt);
        if (!itemsNumberVariant.isNull()) {
            int number = itemsNumberVariant.toInt();
            if (number > 0) {
                mMaxItems = number;
            }
        }
    }
    
    connect(mAfManager, SIGNAL(dataChanged()), SLOT(getActivities()));
    connect(mAfManager, SIGNAL(thumbnailReady(QPixmap,void*)), SLOT(convertScreenshotToThumbnail(QPixmap,void*)));
    
    getActivities();
}

ActivityTsModel::~ActivityTsModel()
{
    qDeleteAll(mData);
}

QList<QVariantHash> ActivityTsModel::taskList() const
{
    return taskList(mData.count());
}

QList<QVariantHash> ActivityTsModel::taskList(int limit) const
{
    QList<QVariantHash> result;
    for (int i(0); i < limit && i < mData.count(); ++i) {
        result.append(mData[i]->data());
    }
    return result;
}
    
bool ActivityTsModel::openTask(const QVariant &id)
{
    foreach (const ActivityTsEntry *entry, mData) {
        if (entry->data().value("TaskId") == id) {
            QMetaObject::invokeMethod(mAfManager,
                                  "launchActivity",
                                  Q_ARG(QUrl, entry->data().value("Uri").toUrl()));
            return true;
        }
    }
    return false;
}

bool ActivityTsModel::closeTask(const QVariant &id)
{
    // it's impossible to "close" activity
    Q_UNUSED(id);
    return false;
}

void ActivityTsModel::getActivities()
{
    qDeleteAll(mData);
    mData.clear();
    
    QList<QVariantHash> activities;
    QMetaObject::invokeMethod(mAfManager,
                              "activitiesList",
                              Q_RETURN_ARG(QList<QVariantHash>, activities),
                              Q_ARG(int, mMaxItems));
                              
    foreach (const QVariantHash &activityEntry, activities) {
        mData.append(new ActivityTsEntry(activityEntry));
        
        QMetaObject::invokeMethod(
                mAfManager,
                "getThumbnail",
                Q_ARG(QString, activityEntry.value(ActivityScreenshotKeyword).toString()),
                Q_ARG(void *, mData.last()));
    }
    
    emit dataChanged();
}

void ActivityTsModel::convertScreenshotToThumbnail(const QPixmap &thumbnail, void *userData)
{
    emit createThumbnail(thumbnail, 0, userData);
}

void ActivityTsModel::thumbnailCreated(const QPixmap &thumbnail, const void *userData)
{    
    foreach (ActivityTsEntry *activity, mData) {
        if (activity == userData) {
            activity->setThumbnail(thumbnail.toSymbianCFbsBitmap());
            emit dataChanged();
            break;
        }
    }
}
