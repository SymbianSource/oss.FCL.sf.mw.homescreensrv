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

#include "activityserverclient_p.h"

ActivityServerClientPrivate::ActivityServerClientPrivate()
{
}

bool ActivityServerClientPrivate::addActivity(const QString &activityId, const QVariantHash &parameters)
{
    return true;
}

bool ActivityServerClientPrivate::removeActivity(const QString &activityId)
{
    return true;
}

bool ActivityServerClientPrivate::updateActivity(const QString &activityId, const QVariantHash &parameters)
{
    return true;
}

QList<QVariantHash> ActivityServerClientPrivate::applicationActivities()
{
    return QList<QVariantHash>();
}

bool ActivityServerClientPrivate::removeActivity(int applicationId, const QString &activityId)
{
    return true;
}

bool ActivityServerClientPrivate::removeApplicationActivities(int applicationId)
{
    return true;
}

QList<QVariantHash> ActivityServerClientPrivate::activities()
{
    return QList<QVariantHash>();
}
