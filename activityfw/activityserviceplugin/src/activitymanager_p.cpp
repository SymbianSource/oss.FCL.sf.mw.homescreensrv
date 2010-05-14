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

#include <QStringList>
#include <QUrl>

ActivityManagerPrivate::ActivityManagerPrivate(ActivityManager *q) : QObject(q), q_ptr(q)
{
    mServerClient = new HsActivityDbClient();
    if(0 == mServerClient->connect()){
        mServerClient->notifyDataChange();
    }
    connect(mServerClient,
            SIGNAL(thumbnailRequested(QPixmap, void *)),
            q,
            SIGNAL(thumbnailReady(QPixmap, void *)));
    connect(mServerClient,
            SIGNAL(dataChanged()),
            q,
            SIGNAL(dataChanged()));
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

void ActivityManagerPrivate::launchActivity(const QUrl &uri)
{
    if (uri.scheme() != "appto")
        return;

    bool conversionOk(false);
    int applicationId = uri.host().toUInt(&conversionOk, 16);   
    if (!conversionOk)
        return;
        
    QVariantHash activity;
    activity.insert(ActivityApplicationKeyword, applicationId);
    
    QVariantHash parameters;
    QList<QPair<QString, QString> > uriParams = uri.queryItems();
    for (QList<QPair<QString, QString> >::const_iterator i = uriParams.constBegin(); i != uriParams.constEnd(); ++i) {
        parameters.insert(i->first, i->second);
    }

    if (parameters.contains("activityname")) {
        activity.insert(ActivityActivityKeyword, parameters.value("activityname").toString());
        parameters.remove("activityname");
    }
    
    activity.insert(ActivityParametersKeyword, parameters);
    
    launchActivity(activity);
}

void ActivityManagerPrivate::launchActivity(int applicationId, const QString &activityId, const QVariantHash& parameters)
{
    QVariantHash activity;
    activity.insert(ActivityApplicationKeyword, applicationId);
    activity.insert(ActivityActivityKeyword, activityId);
    activity.insert(ActivityParametersKeyword, parameters);
    
    launchActivity(activity);
}

void ActivityManagerPrivate::launchActivity(const QVariantHash &activity)
{
    ApplicationLauncher applicationLauncher;
    int applicationId = activity.value(ActivityApplicationKeyword).toInt();
    
    if (applicationLauncher.isRunning(applicationId)) {
        mServerClient->launchActivity(activity);
        applicationLauncher.bringToForeground(applicationId);
    } else {
        applicationLauncher.startApplication(applicationId, activityToUri(activity));
    }
}

QUrl ActivityManagerPrivate::activityToUri(const QVariantHash &activity) const
{
    QUrl uri;
    uri.setScheme("appto");
    uri.setHost(QString("%1").arg(activity.value(ActivityApplicationKeyword).toUInt(), 8, 16, QChar('0')));
    
    if (activity.contains(ActivityActivityKeyword))
        uri.addQueryItem("activityname", activity.value(ActivityActivityKeyword).toString());
    
    QVariantHash parameters = activity.value(ActivityParametersKeyword).toHash();
    foreach(const QString &key, parameters.keys()) {
        uri.addQueryItem(key, parameters.value(key).toString());
    } 
    
    return uri;
}

void ActivityManagerPrivate::getThumbnail(QSize resolution,const QString &thumbnailPath, void *data)
{
    mServerClient->getThumbnail(resolution, thumbnailPath, "image/png", data);
}
