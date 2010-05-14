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

#include "activitydatabase_p.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QSettings>



ActivityDatabasePrivate::ActivityDatabasePrivate()
{
    mClient.connect();
}

ActivityDatabasePrivate::~ActivityDatabasePrivate()
{
}

void ActivityDatabasePrivate::saveActivity(const QString &applicationName, const QString &activityName, const QVariantHash &activityMetadata)
{
    QVariantHash request(activityMetadata);
    request.insert(ActivityApplicationKeyword,applicationName);
    request.insert(ActivityActivityKeyword,activityName);
    mClient.saveActivity(request);

}

void ActivityDatabasePrivate::deleteActivity(const QString &applicationName, const QString &activityName)
{
    QVariantHash request;
    request.insert(ActivityApplicationKeyword,applicationName);
    request.insert(ActivityActivityKeyword,activityName);
    mClient.deleteActivity(request);
}

void ActivityDatabasePrivate::deleteApplicationActivities(const QString &applicationName)
{
    QVariantHash request;
    request.insert(ActivityApplicationKeyword,applicationName);
    mClient.deleteApplicationActivities(request);
}

QList<QVariantHash> ActivityDatabasePrivate::allActivitiesList()
{
    QList<QVariantHash> retVal;
    mClient.activities(retVal);
    return retVal;
}

QList<QVariantHash> ActivityDatabasePrivate::applicationActivitiesList(const QString &applicationName)
{
    QList<QVariantHash> result;
    QVariantHash request;
    request.insert(ActivityApplicationKeyword,applicationName);
    mClient.applicationActivities(result, request);
    return result;
}


QString ActivityDatabasePrivate::requestedActivityName(const QString &applicationName)
{
    QString result;
    QVariantHash request;
    request.insert(ActivityApplicationKeyword,applicationName);
    mClient.requestedActivityName(result, request);
    return result;
}

void ActivityDatabasePrivate::setActivityRequestFlag(const QString &applicationName, const QString &activityName)
{
    QVariantHash request;
    request.insert(ActivityApplicationKeyword,applicationName);
    request.insert(ActivityActivityKeyword,activityName);
    mClient.setActivityRequestFlag(request);
}

void ActivityDatabasePrivate::clearActivityRequestFlag(const QString &applicationName, const QString &activityName)
{
    QVariantHash request;
    request.insert(ActivityApplicationKeyword,applicationName);
    request.insert(ActivityActivityKeyword,activityName);
    mClient.clearActivityRequestFlag(request);
}

