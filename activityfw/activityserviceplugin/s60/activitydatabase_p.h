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
#ifndef ACTIVITYDATABASE_P_H
#define ACTIVITYDATABASE_P_H

#include <QVariant>
#include <QList>
#include <hsactivitydbclient.h>
class ActivityDatabasePrivate
{

public:
    ActivityDatabasePrivate();
    ~ActivityDatabasePrivate();

public:
    void saveActivity(const QString &applicationName, const QString &activityName, const QVariantHash &activityMetadata);
    void deleteActivity(const QString &applicationName, const QString &activityName);
    void deleteApplicationActivities(const QString &applicationName);
    QList<QVariantHash> allActivitiesList();
    QList<QVariantHash> applicationActivitiesList(const QString &applicationName);
    QString requestedActivityName(const QString &applicationName);
    void setActivityRequestFlag(const QString &applicationName, const QString &activityName);
    void clearActivityRequestFlag(const QString &applicationName, const QString &activityName);

private:
    HsActivityDbClient mClient;
};

#endif // ACTIVITYDATABASE_P_H
