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

#ifndef HBACTIVITYPLUGIN_H
#define HBACTIVITYPLUGIN_H

#include "hbactivityplugininterface.h"

class HbActivityPlugin : public HbActivityPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(HbActivityPluginInterface)
    
public:
    HbActivityPlugin(QObject *parent = 0);
    ~HbActivityPlugin();
    
public:
    virtual bool addActivity(const QString &activityId, const QVariant &data, const QVariantHash &parameters);
    virtual bool removeActivity(const QString &activityId);
    virtual bool updateActivity(const QString &activityId, const QVariant &data, const QVariantHash &parameters);
    virtual QList<QVariantHash> activities();
    virtual QVariant activityData(const QString &activityId);
    virtual bool waitActivity();

private:
    QObject *mActivityClient;
    
};

#endif //HBACTIVITYPLUGIN_H
