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
#include "activitymanager_p.h"
#include "activitymanager.h"
#include <hsactivitydbclient.h>

#include <QRegExp>
#include <QStringList>

ActivityManagerPrivate::ActivityManagerPrivate(ActivityManager *q) : QObject(q), q_ptr(q)
{
    mServerClient = new HsActivityDbClient();
    mServerClient->connect();
}

ActivityManagerPrivate::~ActivityManagerPrivate()
{
    delete mServerClient;
}

QList<QVariantHash> ActivityManagerPrivate::activitiesList()
{
    QList<QVariantHash> retVal;
    mServerClient->activities(retVal);
    return retVal;
}

void ActivityManagerPrivate::launchActivity(const QString &uri)
{
    // @todo use the same parser as in HbApplicationPrivate (if possible)
    QRegExp uriMatcher("^appto://(.+)\\?activityname=(.+)$");
    if (uriMatcher.indexIn(uri) != -1) {
        QStringList list = uriMatcher.capturedTexts();
        Q_ASSERT(list.count() == 3);
        launchActivity(list.at(1).toInt(), list.at(2));
    } else {
        qWarning("Activity URI parsing error");
    }
}

void ActivityManagerPrivate::launchActivity(int applicationId, const QString &activityId)
{
    ApplicationLauncher applicationLauncher;
    if (applicationLauncher.isRunning(applicationId)) {
        QVariantHash activity;
        activity.insert(ActivityApplicationKeyword, applicationId);
        activity.insert(ActivityActivityKeyword, activityId);
        mServerClient->launchActivity(activity);
        applicationLauncher.bringToForeground(applicationId);
    } else {
        applicationLauncher.startApplication(applicationId, activityId);
    }
}

void ActivityManagerPrivate::removeActivity(int applicationId, const QString &activityId)
{
    QVariantHash activity;
    activity.insert(ActivityApplicationKeyword, applicationId);
    activity.insert(ActivityActivityKeyword, activityId);
    mServerClient->removeActivity(activity);
}

void ActivityManagerPrivate::removeApplicationActivities(int applicationId)
{
    QVariantHash activity;
    activity.insert(ActivityApplicationKeyword, applicationId);
    mServerClient->removeApplicationActivities(activity);
}

void ActivityManagerPrivate::getThumbnail(const QString &thumbnailId, void *userData)
{
    QPixmap ico;
    mServerClient->getThumbnail(ico, thumbnailId);
    emit q_ptr->thumbnailReady(ico, userData);
}
