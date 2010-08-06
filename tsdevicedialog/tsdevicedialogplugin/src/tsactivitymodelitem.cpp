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
* Description: tsactivitymodelitem.cpp
*
*/
#include <hbicon.h>
#include <afstorageglobals.h>

#include "tsactivitymodelitem.h"
#include "tsdataroles.h"

/*!
    \class TsActivityModelItem
    \ingroup group_tsdevicedialogplugin
    \brief Item presenting activity in the grid.
*/

/*!
    Standard C++ constructor
    /param model - reference to parent model
    /param service - reference to initialized ActivityManager instance
    /param entry - Activity data
*/
TsActivityModelItem::TsActivityModelItem(QAbstractListModel &model,
                                         QObject& service, 
                                         const QVariantHash & activity)
    :
    mService(service),
    mActivity(activity),
    mIcon(),
    mRequestPending(false)
{
    connect(this,
            SIGNAL(entryChanged(TsModelItem *)),
            &model,
            SLOT(entryChanged(TsModelItem *)));
    connect(&mService,
            SIGNAL(thumbnailReady(QPixmap, void *)),
            this,
            SLOT(thumbnailReady(QPixmap, void *)));
}

/*!
    Standard C++ destructor
    /param service - reference to initialized ActivityManager instance
    /param entry - Activity data
*/
TsActivityModelItem::~TsActivityModelItem()
{
}

/*!
    Returns the data stored under the given role.
    /param role - requested data role
    /return data encapulated by QVariant
*/
QVariant TsActivityModelItem::data(int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            return displayRole();
        case Qt::DecorationRole:
            return decorationRole();
        case TsDataRoles::Closable:
            return QVariant(false);
        case TsDataRoles::Active:
            return QVariant(false);
        default:
            return QVariant(QVariant::Invalid);
    }
}

/*!
    Empty interface implementation
*/
void TsActivityModelItem::close()
{
    //no implementation required
}

/*!
    Open or move to foreground activity repesented by entry
*/
void TsActivityModelItem::open()
{
    QVariant application(mActivity.value(ActivityApplicationKeyword));
    QVariant activity(mActivity.value(ActivityActivityKeyword));

    if (application.isValid() && activity.isValid()) {
        QMetaObject::invokeMethod(&mService,
                                  "launchActivity",
                                  Q_ARG(int, application.toInt()),
                                  Q_ARG(QString, activity.toString()));
    }
}

/*!
    Return application name keyword
*/
QString TsActivityModelItem::applicationKeyword()
{
    static QString applicationName(ActivityApplicationName);
    return applicationName;
}

/*!
    Return activity screenshot keyword
*/
QString TsActivityModelItem::screenshotKeyword()
{
    static QString screenshot(ActivityScreenshotKeyword);
    return screenshot;
}

/*!
    Returns the data stored under decoration role.
    /return data encapulated by QVariant
*/
QVariant TsActivityModelItem::decorationRole() const
{
    if (!mRequestPending) {
        mRequestPending = true;
        QVariant screenshot = mActivity.value(screenshotKeyword());
        if (screenshot.isValid()) {
            QMetaObject::invokeMethod(
                &mService,
                "getThumbnail",
                Q_ARG(QSize, QSize(128, 128)),
                Q_ARG(QString, screenshot.toString()),
                Q_ARG(void *, const_cast<TsActivityModelItem *>(this)));
        }
    }
    return QVariant::fromValue<HbIcon>(HbIcon(mIcon));
}

/*!
    Returns the data stored under display role.
    /return data encapulated by QVariant
*/
QVariant TsActivityModelItem::displayRole() const
{
    return mActivity.value(applicationKeyword());
}

/*!
    Slot retrieve thumbnail form TM, update content and notify observers
    /param pixmap - thumbnail content
    /ptr - user data provided to request
*/
void TsActivityModelItem::thumbnailReady(QPixmap pixmap, void *ptr)
{
    if (ptr == this) {
        mIcon = pixmap;
        emit entryChanged(this);
    }
}
