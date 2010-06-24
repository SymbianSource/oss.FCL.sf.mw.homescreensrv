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
#include <QCoreApplication>
#include <QStringList>
#include <QTimer>
#include <QUrl>
#include <QDebug>
#include <QDir>

#include <afstorageentry.h>
#include <afstorageglobals.h>

#include "activityclient.h"
ActivityClient::ActivityClient(const QSharedPointer<AfStorageClient> &serviceProvider, QObject *parent) 
: 
    QObject(parent),
    mServiceProvider(serviceProvider),
    mIsconnected(false)
{
    mIsconnected = ( KErrNone == mServiceProvider->connect());
    if( mIsconnected) {
        mServiceProvider->waitActivity();
    }
    connect(mServiceProvider.data(), SIGNAL(activityRequested(QString)), this, SIGNAL(activityRequested(QString)));
}

ActivityClient::~ActivityClient()
{
}

bool ActivityClient::addActivity(const QString &activityId, const QVariant &data, const QVariantHash &parameters)
{
    bool result(mIsconnected);
    if (result) {
        QVariantHash publicData(parameters);
        
        QPixmap screenshot(publicData[ActivityScreenshotKeyword].value<QPixmap>());
        publicData.remove(ActivityScreenshotKeyword);
        
        RProcess process;
        publicData.insert(ActivityApplicationKeyword, static_cast<int>(process.SecureId().iId));
        publicData.insert(ActivityActivityKeyword, activityId);
        AfStorageEntry entry(process.SecureId().iId, activityId, data, publicData);
        result = (KErrNone == mServiceProvider->addActivity(entry, screenshot));
    }
    return result;
}

bool ActivityClient::removeActivity(const QString &activityId)
{
    bool result(mIsconnected);
    if (result) {
        RProcess process;
        AfStorageEntry entry(process.SecureId().iId, activityId);
        result = (KErrNone == mServiceProvider->removeActivity(entry));
    }
    return result;
}

bool ActivityClient::updateActivity(const QString &activityId, const QVariant &data, const QVariantHash &parameters)
{
    bool result(mIsconnected);
    if (result) {
        QVariantHash publicData(parameters);
        QPixmap screenshot(publicData[ActivityScreenshotKeyword].value<QPixmap>());
        publicData.remove(ActivityScreenshotKeyword);
        RProcess process;
        publicData.insert(ActivityApplicationKeyword, static_cast<int>(process.SecureId().iId));
        publicData.insert(ActivityActivityKeyword, activityId);
        AfStorageEntry entry(process.SecureId().iId, activityId, data, publicData);
        result = (KErrNone == mServiceProvider->updateActivity(entry, screenshot));
    }
    return result;
}

QList<QVariantHash> ActivityClient::activities() const
{
    QList<QVariantHash> retVal;
    if (mIsconnected) {
        RProcess process;
        AfStorageEntry entry(process.SecureId().iId);
        QList<AfStorageEntry> activities;
        mServiceProvider->applicationActivities(activities, entry);
        
        QList<AfStorageEntry>::iterator iter(activities.begin());
        for (; activities.end() != iter; iter = activities.erase(iter)) {
            retVal.append((*iter).publicData());
        }
    }
    return retVal;
}

QVariant ActivityClient::activityData(const QString &activityId) const
{
    QVariant data;
    if (mIsconnected) {
        RProcess process;
        AfStorageEntry entry(static_cast<int>(process.SecureId().iId), activityId), result;
        if (0 == mServiceProvider->activityData(result, entry)) {
            data = result.privateData();
        }
    }
    return data;
}

QVariantHash ActivityClient::parseCommandLine(const QStringList &commandLineParams) const
{
     QVariantHash activityParams;
    int activityMarkerIndex = commandLineParams.indexOf("-activity");
    if (activityMarkerIndex != -1 && commandLineParams.count() - 1 > activityMarkerIndex) {
        QUrl activityUri = QUrl::fromEncoded(commandLineParams.at(activityMarkerIndex+1).toAscii());
        if (activityUri.scheme() == "appto") {
            QList<QPair<QString, QString> > parameters = activityUri.queryItems();
            for (QList<QPair<QString, QString> >::const_iterator i = parameters.constBegin(); i != parameters.constEnd(); ++i) {
                activityParams.insert(i->first, i->second);
            }

            if (activityParams.contains("activityname") && !activityParams.value("activityname").toString().isEmpty()) {
                return activityParams;
            }
        }
    }
    return QVariantHash();
}
