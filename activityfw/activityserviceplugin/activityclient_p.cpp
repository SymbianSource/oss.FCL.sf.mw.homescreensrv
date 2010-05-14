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
#include "activityclient_p.h"
#include "activityclient.h"
#include "activitydatastorage.h"
#include <hsactivitydbclient.h>

#include <QCoreApplication>
#include <QStringList>
#include <QTimer>
#include <QUrl>
#include <QDebug>
#include <QDir>

ActivityClientPrivate::ActivityClientPrivate(ActivityClient *q) : QObject(q), mIsconnected(false)
{
    mDataStorage = new ActivityDataStorage();
    mServerClient = new HsActivityDbClient();
    mIsconnected = ( KErrNone == mServerClient->connect());
    connect(mServerClient, SIGNAL(activityRequested(QString)), q, SIGNAL(activityRequested(QString)));
}

ActivityClientPrivate::~ActivityClientPrivate()
{
    delete mServerClient;
    delete mDataStorage;
}

bool ActivityClientPrivate::addActivity(const QString &activityId, const QVariant &data, const QVariantHash &parameters)
{
    bool result(false);
    if (mIsconnected) {
        result = mDataStorage->addActivity(activityId, data);
        if ( result ) {
            QVariantHash activity(parameters);
            RProcess process;
            registerThumbnail(activityId, activity);
            activity.insert(ActivityApplicationKeyword, static_cast<int>(process.SecureId().iId));
            activity.insert(ActivityActivityKeyword, activityId);
            int error = mServerClient->addActivity(activity);
            result = error == KErrNone ? true : false;
        }
    }
    // @todo make those operations atomic
    return result;
}

bool ActivityClientPrivate::removeActivity(const QString &activityId)
{
    bool result(false);
    if (mIsconnected) {
        result = mDataStorage->removeActivity(activityId);
        if ( result ) {            
            QVariantHash activity;
            RProcess process;
            activity.insert(ActivityApplicationKeyword, static_cast<int>(process.SecureId().iId));
            activity.insert(ActivityActivityKeyword, activityId);
            int error = mServerClient->removeActivity(activity);
            result = error == KErrNone ? true : false;
            if (result) {
                result = QFile::remove(thumbnailName(activityId));
            }
        }
    }
    // @todo make those operations atomic
    return result;
}

bool ActivityClientPrivate::updateActivity(const QString &activityId, const QVariant &data, const QVariantHash &parameters)
{
    bool result(false);
    if (mIsconnected) {
        result = mDataStorage->updateActivity(activityId, data);
        if ( result ) {
            QVariantHash activity(parameters);
            RProcess process;
            registerThumbnail(activityId, activity);
            activity.insert(ActivityApplicationKeyword, static_cast<int>(process.SecureId().iId));
            activity.insert(ActivityActivityKeyword, activityId);
            int error = mServerClient->updateActivity(activity);
            result = error == KErrNone ? true : false;
        }
    }
    // @todo make those operations atomic
    return result;
}

QList<QVariantHash> ActivityClientPrivate::activities() const
{
    QList<QVariantHash> activities;
    if (mIsconnected) {
        QVariantHash activity;
        RProcess process;
        activity.insert(ActivityApplicationKeyword, static_cast<int>(process.SecureId().iId));
        mServerClient->applicationActivities(activities, activity);
    }
    return activities;
}

QVariant ActivityClientPrivate::activityData(const QString &activityId) const
{
    return mIsconnected ? mDataStorage->activityData(activityId) : QVariant();
}

bool ActivityClientPrivate::waitActivity()
{
    bool retVal(mIsconnected);
    if (mIsconnected) {
        RProcess process;
        QVariantHash activity;
        activity.insert(ActivityApplicationKeyword, static_cast<int>(process.SecureId().iId));
        retVal = !mServerClient->waitActivity(activity);
    }
    return retVal;
}

QVariantHash ActivityClientPrivate::parseCommandLine(const QStringList &commandLineParams) const
{
    QVariantHash activityParams;
    int activityMarkerIndex = commandLineParams.indexOf("-activity");
    if (activityMarkerIndex != -1 && commandLineParams.count() - 1 > activityMarkerIndex) {
        QUrl activityUri(commandLineParams.at(activityMarkerIndex+1));
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

void ActivityClientPrivate::registerThumbnail(const QString &name, QVariantHash &activity)
{
    QVariantHash::const_iterator findIterator(activity.constFind(ActivityScreenshotKeyword));
    if (activity.constEnd() != findIterator   &&
        findIterator.value().canConvert<QPixmap>()) {
        const QString thumbnailManagerName = thumbnailName(name);
        if (findIterator.value().value<QPixmap>().save(thumbnailManagerName)) {
            activity.insert(ActivityScreenshotKeyword, thumbnailManagerName);
        } else {
            activity.remove(ActivityScreenshotKeyword);
        }
    }
}

QString ActivityClientPrivate::thumbnailName(const QString &activityId) const
{
    return QDir::toNativeSeparators(qApp->applicationDirPath() + 
                                    "/" + 
                                    QString::number(qHash(activityId), 16) + 
                                    ".png");
}

