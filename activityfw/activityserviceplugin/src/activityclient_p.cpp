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
#include <hsactivitydbclient.h>

#include <QCoreApplication>
#include <QStringList>
#include <QTimer>
#include <QUrl>
#include <QDebug>
#include <QDir>

ActivityClientPrivate::ActivityClientPrivate(ActivityClient *q) : QObject(q), mIsconnected(false)
{
    mServerClient = new HsActivityDbClient();
    mIsconnected = ( KErrNone == mServerClient->connect());
    if( mIsconnected) {
        mServerClient->waitActivity(QVariantHash());
    }
    connect(mServerClient, SIGNAL(activityRequested(QString)), q, SIGNAL(activityRequested(QString)));
}

ActivityClientPrivate::~ActivityClientPrivate()
{
    delete mServerClient;
}

bool ActivityClientPrivate::addActivity(const QString &activityId, const QVariant &data, const QVariantHash &parameters)
{
    bool result(false);
    if (mIsconnected) {
        QVariantHash privateData;
        privateData.insert(ActivityDataKeyword, data);
        QVariantHash publicData(parameters);
        RProcess process;
        publicData.insert(ActivityApplicationKeyword, static_cast<int>(process.SecureId().iId));
        publicData.insert(ActivityActivityKeyword, activityId);
        result = (KErrNone == mServerClient->addActivity(privateData, publicData));
    }
    return result;
}

bool ActivityClientPrivate::removeActivity(const QString &activityId)
{
    bool result(false);
    if (mIsconnected) {
        QVariantHash activity;
        RProcess process;
        activity.insert(ActivityApplicationKeyword, static_cast<int>(process.SecureId().iId));
        activity.insert(ActivityActivityKeyword, activityId);
        result = (KErrNone == mServerClient->removeActivity(activity));
    }
    return result;
}

bool ActivityClientPrivate::updateActivity(const QString &activityId, const QVariant &data, const QVariantHash &parameters)
{
    bool result(false);
    if (mIsconnected) {
        QVariantHash privateData;
        privateData.insert(ActivityDataKeyword, data);
        QVariantHash publicData(parameters);
        RProcess process;
        publicData.insert(ActivityApplicationKeyword, static_cast<int>(process.SecureId().iId));
        publicData.insert(ActivityActivityKeyword, activityId);
        result = (KErrNone == mServerClient->updateActivity(privateData, publicData));
    }
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
    QVariant data;
    if (mIsconnected) {
        QVariantHash activity;
        RProcess process;
        activity.insert(ActivityApplicationKeyword, static_cast<int>(process.SecureId().iId));
        activity.insert(ActivityActivityKeyword, activityId);
        mServerClient->activityData(data, activity);
    }
    return data;
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
