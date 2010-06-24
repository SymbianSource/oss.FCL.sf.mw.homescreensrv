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

#ifndef AFSTORAGECLIENTPRIVATE_H
#define AFSTORAGECLIENTPRIVATE_H

#include <e32base.h>
#include "afstorageglobals.h"
#include "afstorageclientimp.h"

class AfStorageEntry;
class AfAsyncRequestPrivate;

class AfStorageClientPrivate
{
public:
    AfStorageClientPrivate(MAfAsyncRequestObserver & observer);

    ~AfStorageClientPrivate();

    int connect();

    int addActivity(const AfStorageEntry &sourceEntry, const QPixmap& sourceImage);

    int updateActivity(const AfStorageEntry &sourceEntry, const QPixmap& sourceImage);

    int removeActivity(const AfStorageEntry &templateEntry);

    int removeApplicationActivities(const AfStorageEntry &templateEntry);

    int activities(QList<AfStorageEntry> &dst);

    int applicationActivities(QList<AfStorageEntry> &dst, 
                              const AfStorageEntry &entry);

    int activityData(AfStorageEntry &resultEntry, const AfStorageEntry &templateEntry);
    
    int waitActivity();
    
    int getThumbnail(QSize size, QString imagePath , void *userDdata);
    
    int notifyDataChange();

    int launchActivity(const AfStorageEntry &templateEntry);

    int cancelWaitActivity();

    int cancelNotifyDataChange();

private:
    int execute(int function, const AfStorageEntry &sourceEntry, const QPixmap& sourceImage);
    int execute(int function, QList<AfStorageEntry>& resultsList,const AfStorageEntry& templateEntry);

private:
    RAfStorageClientImplementation mImplementation;
};
#endif // AFSTORAGECLIENTPRIVATE_H
