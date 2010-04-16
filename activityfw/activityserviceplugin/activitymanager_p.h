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
#ifndef ACTIVITYMANAGER_P_H
#define ACTIVITYMANAGER_P_H

#include <QObject>
#include <QVariant>
#include <QList>
#include "applicationlauncher.h"

class ActivityManager;
class HsActivityDbClient;

class ActivityManagerPrivate : public QObject
{

    Q_OBJECT

public:
    ActivityManagerPrivate(ActivityManager *q);
    ~ActivityManagerPrivate();

public slots:
    QList<QVariantHash> activitiesList();
    void launchActivity(const QString &uri);
    void launchActivity(int applicationId, const QString &activityId);
    void removeActivity(int applicationId, const QString &activityId);
    void removeApplicationActivities(int applicationId);
    void getThumbnail(const QString &thumbnailId, void *);

private:
    ActivityManager *q_ptr;
    HsActivityDbClient *mServerClient;
};

#endif // ACTIVITYMANAGER_P_H
