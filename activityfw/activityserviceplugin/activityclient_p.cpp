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

#include <QStringList>
#include <QTimer>

ActivityClientPrivate::ActivityClientPrivate(ActivityClient *q) : QObject(q)
{
    mDataStorage = new ActivityDataStorage();
    mServerClient = new HsActivityDbClient();
    mServerClient->connect();
    connect(mServerClient, SIGNAL(activityRequested(QString)), q, SIGNAL(activityRequested(QString)));
}

ActivityClientPrivate::~ActivityClientPrivate()
{
    delete mServerClient;
    delete mDataStorage;
}

bool ActivityClientPrivate::addActivity(const QString &activityId, const QVariant &data, const QVariantHash &parameters)
{
    QVariantHash activity(parameters);
    mDataStorage->addActivity(activityId, data);
    RProcess process;
    
    activity.insert(ActivityApplicationKeyword, static_cast<int>(process.SecureId().iId));
    activity.insert(ActivityActivityKeyword, activityId);
    mServerClient->addActivity(activity);
    
    // @todo make those operations atomic
    // @todo return real result
    return true;
}

bool ActivityClientPrivate::removeActivity(const QString &activityId)
{
    QVariantHash activity;
    RProcess process;
    activity.insert(ActivityApplicationKeyword, static_cast<int>(process.SecureId().iId));
    activity.insert(ActivityActivityKeyword, activityId);
    mDataStorage->removeActivity(activityId); 
    mServerClient->removeActivity(activity); 
    // @todo make those operations atomic
    // @todo return real result
    return true;
}

bool ActivityClientPrivate::updateActivity(const QString &activityId, const QVariant &data, const QVariantHash &parameters)
{
    QVariantHash activity(parameters);
    RProcess process;
    activity.insert(ActivityApplicationKeyword, static_cast<int>(process.SecureId().iId));
    activity.insert(ActivityActivityKeyword, activityId);
    mDataStorage->updateActivity(activityId, data);
    mServerClient->updateActivity(activity);
    // @todo make those operations atomic
    // @todo return real result
    return true;
}

QList<QVariantHash> ActivityClientPrivate::activities() const
{
    QVariantHash activity;
    RProcess process;
    activity.insert(ActivityApplicationKeyword, static_cast<int>(process.SecureId().iId));

    QList<QVariantHash> activities;

    mServerClient->applicationActivities(activities, activity);
    return activities;
}

QVariant ActivityClientPrivate::activityData(const QString &activityId) const
{
    return mDataStorage->activityData(activityId);
}

bool ActivityClientPrivate::waitActivity()
{
    RProcess process;
    QVariantHash activity;
    activity.insert(ActivityApplicationKeyword, static_cast<int>(process.SecureId().iId));
    return !mServerClient->waitActivity(activity);
}
