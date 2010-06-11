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

#include "activityclient.h"
#include "activityclient_p.h"

ActivityClient::ActivityClient(QObject *parent) : QObject(parent)
{
    d_ptr = new ActivityClientPrivate(this);
}

ActivityClient::~ActivityClient()
{
}

bool ActivityClient::addActivity(const QString &activityId, const QVariant &data, const QVariantHash &parameters)
{
    return d_ptr->addActivity(activityId, data, parameters);
}

bool ActivityClient::removeActivity(const QString &activityId)
{
    return d_ptr->removeActivity(activityId);
}

bool ActivityClient::updateActivity(const QString &activityId, const QVariant &data, const QVariantHash &parameters)
{
    return d_ptr->updateActivity(activityId, data, parameters);
}

QList<QVariantHash> ActivityClient::activities() const
{
    return d_ptr->activities();
}

QVariant ActivityClient::activityData(const QString &activityId) const
{
    return d_ptr->activityData(activityId);
}

QVariantHash ActivityClient::parseCommandLine(const QStringList &commandLineParams) const
{
    return d_ptr->parseCommandLine(commandLineParams);
}
