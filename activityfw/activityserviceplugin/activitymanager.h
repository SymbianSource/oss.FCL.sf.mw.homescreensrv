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
#ifndef ACTIVITYMANAGER_H
#define ACTIVITYMANAGER_H

#include <QObject>
#include <QVariant>
#include <QList>
#include <QPixmap>
#include <QSize>

class ActivityManagerPrivate;

class ActivityManager : public QObject
{

    Q_OBJECT

public:
    ActivityManager(QObject *parent = 0);
    ~ActivityManager();

public slots:
    QList<QVariantHash> activitiesList();
    void launchActivity(const QString &uri);
    void launchActivity(int applicationId, const QString &activityId);
    void getThumbnail(QSize resolution,const QString &thumbnailPath, void *data =0);

signals:
    void thumbnailReady(QPixmap, void *);

private:
    ActivityManagerPrivate *d_ptr;

    friend class ActivityManagerPrivate;
};

#endif // ACTIVITYMANAGER_H
