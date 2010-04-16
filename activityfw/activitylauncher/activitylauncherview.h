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
#ifndef ACTIVITYLAUNCHERVIEW_H
#define ACTIVITYLAUNCHERVIEW_H

#include <hbview.h>
#include <QVariant>

class HbLabel;
class QStandardItemModel;
    
class ActivityLauncherView : public HbView
{
    Q_OBJECT

public:
    ActivityLauncherView(QGraphicsItem *parent = 0);
    ~ActivityLauncherView();

private slots:
	void itemPressed(const QModelIndex& index);

protected:
    bool eventFilter(QObject* obj, QEvent* event);

private:
    void getActivitiesList();

private:
    QStandardItemModel* mModel;
    QList<QVariantHash> mCurrentActivities;
    HbLabel* mStatusLabel;
    
    QObject* mActivityManager;
};

#endif // ACTIVITYLAUNCHERVIEW_H
