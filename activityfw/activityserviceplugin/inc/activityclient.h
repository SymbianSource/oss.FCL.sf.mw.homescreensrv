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
#ifndef ACTIVITYCLIENT_H
#define ACTIVITYCLIENT_H

#include <QObject>
#include <QVariant>
#include <QString>

#include <afstorageclient.h>

class ActivityClient : public QObject
{

    Q_OBJECT

public:
    ActivityClient(const QSharedPointer<AfStorageClient> &serviceProvider,QObject *parent = 0);
    ~ActivityClient();

public slots:
    bool addActivity(const QString &activityId, const QVariant &data, const QVariantHash &parameters);
    bool removeActivity(const QString &activityId);
    bool updateActivity(const QString &activityId, const QVariant &data, const QVariantHash &parameters);
    QList<QVariantHash> activities() const;
    QVariant activityData(const QString &activityId) const;
    QVariantHash parseCommandLine(const QStringList &commandLineParams) const;

signals:
    void activityRequested(const QString &activityId);

private:
    QSharedPointer<AfStorageClient> mServiceProvider;
    bool mIsconnected;
};

#endif // ACTIVITYCLIENT_H
