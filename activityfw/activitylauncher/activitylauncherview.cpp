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
#include "activitylauncherview.h"

#include <QGraphicsLinearLayout>
#include <QStandardItemModel>
#include <QProcess>

#include <qservicemanager.h>

#include <hblabel.h>
#include <hbapplication.h>
#include <hbpushbutton.h>
#include <hbgridview.h>

#include <hsactivitydbclientinterface.h>

QTM_USE_NAMESPACE

ActivityLauncherView::ActivityLauncherView(QGraphicsItem *parent) : HbView(parent), mModel(NULL), mActivityManager(NULL)
{
    QServiceManager serviceManager;
    mActivityManager = serviceManager.loadInterface("com.nokia.qt.activities.ActivityManager");

    if (!mActivityManager) {
        qFatal("Cannot initialize critical com.nokia.qt.activities.ActivityManager service");
    }

    // create UI
    setTitle(tr("Activity launcher"));
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    QGraphicsLinearLayout *statusLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    mStatusLabel = new HbLabel(this);
    mStatusLabel->setAlignment(Qt::AlignLeft);
    statusLayout->setMaximumHeight(15);

    HbLabel *statusHeader = new HbLabel("Status: ");
    statusHeader->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    statusLayout->addItem(statusHeader);
    statusLayout->addItem(mStatusLabel);

    HbGridView *activities = new HbGridView();
    connect(activities, SIGNAL(activated(QModelIndex)), this, SLOT(itemPressed(QModelIndex)));
    activities->setRowCount(1);
    activities->setColumnCount(1);

    activities->setScrollDirections(Qt::Horizontal);

    mModel = new QStandardItemModel(this);
    activities->setModel(mModel);

    layout->addItem(statusLayout);
    layout->addItem(activities);

    setLayout(layout);
    qApp->installEventFilter(this);
    getActivitiesList();
}

ActivityLauncherView::~ActivityLauncherView()
{
    delete mActivityManager;
}

void ActivityLauncherView::getActivitiesList()
{
    mStatusLabel->setPlainText("Fetching activities");
    mCurrentActivities.clear();
    QMetaObject::invokeMethod(mActivityManager, "activitiesList", Q_RETURN_ARG(QList<QVariantHash>, mCurrentActivities));
    mModel->clear();

    foreach(const QVariantHash& activityEntry, mCurrentActivities) {
        QStandardItem *newItem = new QStandardItem(QIcon(activityEntry.value("screenshot").value<QPixmap>()), QString("%1").arg(activityEntry.value(ActivityActivityKeyword).toInt()));
        mModel->invisibleRootItem()->appendRow(newItem);
    }
    mStatusLabel->setPlainText(QString("Ready (%1 activities)").arg(mCurrentActivities.count()));
}

bool ActivityLauncherView::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::ApplicationActivate) {
        getActivitiesList();
    }
    return QObject::eventFilter(obj, event);
}

void ActivityLauncherView::itemPressed(const QModelIndex &index)
{
    QVariantHash activity = mCurrentActivities.at(index.row());
    int applicationId = activity.value(ActivityApplicationKeyword).toInt();
    QString activityName = activity.value(ActivityActivityKeyword).toString();
    QMetaObject::invokeMethod(mActivityManager, "launchActivity", Q_ARG(int, applicationId), Q_ARG(QString, activityName));
}
