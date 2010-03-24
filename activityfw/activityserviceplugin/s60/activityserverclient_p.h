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
#ifndef ACTIVITYSERVERCLIENT_P_H
#define ACTIVITYSERVERCLIENT_P_H

#include <QString>
#include <QVariantHash>

class ActivityServerClient;

class ActivityServerClientPrivate
{

public:
    ActivityServerClientPrivate();
  
public: // API used by ActivityClient
    bool addActivity(const QString &activityId, const QVariantHash &parameters);
    bool removeActivity(const QString &activityId);
    bool updateActivity(const QString &activityId, const QVariantHash &parameters);
    QList<QVariantHash> applicationActivities();

public: // API  used by ActivityManager
    bool removeActivity(int applicationId, const QString &activityId);
    bool removeApplicationActivities(int applicationId);
    QList<QVariantHash> activities();    
    
public:
    ActivityServerClient* q_ptr;    
    
};

#endif // ACTIVITYSERVERCLIENT_P_H
