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
#ifndef ACTIVITYDATASTORAGE_H
#define ACTIVITYDATASTORAGE_H

#include <QVariant>
#include <QString>

class ActivityDataStorage 
{
  
public:
    ActivityDataStorage();
    ~ActivityDataStorage();
  
public:
    bool addActivity(const QString &activityId, const QVariant &data);
    bool removeActivity(const QString &activityId);
    bool updateActivity(const QString &activityId, const QVariant &data);
    QVariant activityData(const QString &activityId) const;
  
private:
    bool checkTables();
    void recreateTables();
    
};

#endif // ACTIVITYDATASTORAGE_H
