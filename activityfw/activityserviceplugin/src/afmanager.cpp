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

#include <QStringList>
#include <QUrl>

#include <afstorageclient.h>
#include <afstorageentry.h>
#include <afstorageglobals.h>

#include "afmanager.h"
#include "aflauncher.h"

AfManager::AfManager(const QSharedPointer<AfStorageClient> &serviceProvider, 
                     QObject *parent) 
: 
    QObject(parent), 
    mServiceProvider(serviceProvider)
{
    if(0 == mServiceProvider->connect()){
        mServiceProvider->notifyDataChange();
    }
    connect(mServiceProvider.data(),
            SIGNAL(thumbnailRequested(QPixmap, void *)),
            this,
            SIGNAL(thumbnailReady(QPixmap, void *)));
    connect(mServiceProvider.data(),
            SIGNAL(dataChanged()),
            this,
            SIGNAL(dataChanged()));
}

AfManager::~AfManager()
{
}

QList<QVariantHash> AfManager::activitiesList()
{
    QList<AfStorageEntry> results;
    mServiceProvider->activities(results);
    
    QList<QVariantHash> retVal;
    QList<AfStorageEntry>::iterator iter(results.begin());
    for (; iter != results.end(); iter = results.erase(iter)) {
        retVal.append((*iter).publicData());
    }
    return retVal;
}

void AfManager::launchActivity(const QUrl &uri)
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

void AfManager::launchActivity(const QString &uri)
{
    qWarning("AfManager::launchActivity(const QString &uri) is deprecated, use AfManager::launchActivity(const QUrl &uri) instead");
    launchActivity(QUrl(uri));
}

void AfManager::launchActivity(int applicationId, const QString &activityId, const QVariantHash& parameters)
{
    QVariantHash activity;
    activity.insert(ActivityApplicationKeyword, applicationId);
    activity.insert(ActivityActivityKeyword, activityId);
    activity.insert(ActivityParametersKeyword, parameters);
    
    launchActivity(activity);
}

void AfManager::getThumbnail(QSize resolution,const QString &thumbnailPath, void *data)
{
    mServiceProvider->getThumbnail(resolution, thumbnailPath, data);
}

void AfManager::launchActivity(const QVariantHash& activity)
{
    AfLauncher applicationLauncher;
    int applicationId = activity.value(ActivityApplicationKeyword).toInt();
    AfStorageEntry entry(applicationId, 
                         activity[ActivityActivityKeyword].toString());
    if (applicationLauncher.isRunning(applicationId)) {
        mServiceProvider->launchActivity(entry);
    } else {
        applicationLauncher.startApplication(applicationId, activityToUri(activity));
    }
}

QUrl AfManager::activityToUri(const QVariantHash& activity) const
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
