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

#include "activitymanager.h"
#include "activitymanager_p.h"

ActivityManager::ActivityManager(QObject *parent) : QObject(parent), d_ptr(new ActivityManagerPrivate(this))
{
}

ActivityManager::~ActivityManager()
{
}

QList<QVariantHash> ActivityManager::activitiesList()
{
    return d_ptr->activitiesList();
}

void ActivityManager::launchActivity(const QString &uri)
{
    d_ptr->launchActivity(uri);
}

void ActivityManager::launchActivity(int applicationId, const QString &activityId)
{
    d_ptr->launchActivity(applicationId, activityId);
}

void ActivityManager::getThumbnail(QSize resolution,const QString &thumbnailPath, void *data)
{
   d_ptr->getThumbnail(resolution, thumbnailPath, data);
}
