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
#ifndef AFACTIVITYSTORAGE_P_H
#define AFACTIVITYSTORAGE_P_H

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QSharedPointer>

#include "afstorageproxy.h"

class AfActivityStoragePrivate
{

public:
    AfActivityStoragePrivate(const QSharedPointer<AfStorageProxy> &connection);

public:
    bool saveActivity(const QString &activityId, const QVariant &activityData, const QVariantHash &metadata);
    bool removeActivity(const QString &activityId);

    QStringList allActivities() const;

    QVariant activityData(const QString &activityId) const;
    QVariantHash activityMetaData(const QString &activityId) const;

private: // helper methods
    static int applicationId();
    
private:
    QSharedPointer<AfStorageProxy> mConnection;
    
};

#endif // AFACTIVITYSTORAGE_P_H
