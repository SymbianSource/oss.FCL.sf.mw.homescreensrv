/****************************************************************************
**
** Copyright (C) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (developer.feedback@nokia.com)
**
** This file is part of the HbCore module of the UI Extensions for Mobile.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at developer.feedback@nokia.com.
**
****************************************************************************/

#ifndef HBACTIVITYPLUGININTERFACE_P_H
#define HBACTIVITYPLUGININTERFACE_P_H

#include <QtPlugin>
#include <QVariant>
#include <QString>
#include <QList>
#include <QVariantHash>

class HbActivityPluginInterface
{
public:
    virtual ~HbActivityPluginInterface() {}

public:
    virtual bool addActivity(const QString &activityId, const QVariant &data, const QVariantHash &parameters) = 0;
    virtual bool removeActivity(const QString &activityId) = 0;
    virtual bool updateActivity(const QString &activityId, const QVariant &data, const QVariantHash &parameters) = 0;
    virtual QList<QVariantHash> activities() = 0;
    virtual QVariant activityData(const QString &activityId) = 0;
    virtual bool waitActivity() = 0;
    virtual QVariantHash parseCommandLine(const QStringList &commandLineParams) = 0;

signals:
    // signal must be re-declared in the plugin implementation
    void activityRequested(const QString &activityId);
};

Q_DECLARE_INTERFACE(HbActivityPluginInterface, "HbActivityPluginInterface/1.0")

#endif // HBACTIVITYPLUGININTERFACE_P_H
