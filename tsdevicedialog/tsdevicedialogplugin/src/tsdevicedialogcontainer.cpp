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

#include <QAbstractListModel>
#include <HbDialog>
#include <HbMainWindow>
#include <HbLabel>
#include <tspropertydefs.h>

#include "tsdevicedialogcontainer.h"
#include "tstasksgrid.h"
#include "tstasksgriditem.h"

namespace {
    const char KDocmlPath[] = ":/resource/layout.docml";
}

TsDeviceDialogContainer::TsDeviceDialogContainer(QAbstractListModel *model, 
                                                 QObject *parent)
:
    QObject(parent), 
    mVisibilityPublisher(TsProperty::KTsPath)
{
    bool ok(true);
    mLoader.load(KDocmlPath, &ok);
    Q_ASSERT(ok);

    HbDialog *dialog = 
        qobject_cast<HbDialog*>(mLoader.findWidget("tsdevicedialog"));
    TsTasksGrid *grid = 
        qobject_cast<TsTasksGrid *>(mLoader.findWidget("taskgrid"));
    Q_ASSERT(dialog);
    Q_ASSERT(grid);

    grid->setModel(model);

    changeOrientation(dialog->mainWindow()->orientation());
    switchViewOnModelChange();

    // needed because of Qt::QueuedConnection used below
    // @todo: check if we actually need queued connections
    qRegisterMetaType<QModelIndex>("QModelIndex");

    // connect the grid and model
    connect(grid, 
            SIGNAL(activated(QModelIndex)), 
            model, 
            SLOT(openApplication(QModelIndex)));
    connect(grid, 
            SIGNAL(activated(QModelIndex)), 
            dialog, 
            SLOT(close()));
    connect(grid, 
            SIGNAL(deleteButtonClicked(QModelIndex)), 
            model, SLOT(closeApplication(QModelIndex)), 
            Qt::QueuedConnection);

    connect(dialog->mainWindow(), 
            SIGNAL(orientationChanged(Qt::Orientation)), 
            this, 
            SLOT(changeOrientation(Qt::Orientation)));
    connect(dialog, 
            SIGNAL(aboutToClose()), 
            this, 
            SIGNAL(deviceDialogClosed()));

    // note: connect to more signals when partial model updates are implemented
    connect(model, 
            SIGNAL(modelReset()), 
            this, 
            SLOT(switchViewOnModelChange()));

    connect(this, 
            SIGNAL(deviceDialogClosed()), 
            this, 
            SLOT(notifyDialogClosed()));
    mVisibilityPublisher.setValue(TsProperty::KVisibilityPath, 
                                  static_cast<int>(true));
    mVisibilityPublisher.sync();
}

TsDeviceDialogContainer::~TsDeviceDialogContainer()
{
    delete deviceDialogWidget();
}

bool TsDeviceDialogContainer::setDeviceDialogParameters(
                                  const QVariantMap &parameters)
{
    Q_UNUSED(parameters);
    return false;
}

int TsDeviceDialogContainer::deviceDialogError() const
{
    return 0;
}

void TsDeviceDialogContainer::closeDeviceDialog(bool byClient)
{
    Q_UNUSED(byClient)
    // @todo: should there be hide calls? deviceDialogWidget->hide();
    emit deviceDialogClosed();
}

HbPopup *TsDeviceDialogContainer::deviceDialogWidget() const
{
    HbDialog *widget = 
        qobject_cast<HbDialog*>(mLoader.findWidget("tsdevicedialog"));
    Q_ASSERT(widget);
    return widget;
}

QObject *TsDeviceDialogContainer::signalSender() const
{
    return const_cast<TsDeviceDialogContainer*>(this);
}

void TsDeviceDialogContainer::changeOrientation(Qt::Orientation orientation)
{
    bool ok(true);
    if (orientation == Qt::Horizontal) {
        mLoader.load(KDocmlPath, "landscape", &ok);
    } else {
        mLoader.load(KDocmlPath, "portrait", &ok);
    }
    Q_ASSERT(ok);
}

void TsDeviceDialogContainer::notifyDialogClosed()
{
    mVisibilityPublisher.setValue(TsProperty::KVisibilityPath, 
                                  static_cast<int>(false));
    mVisibilityPublisher.sync();
}

void TsDeviceDialogContainer::switchViewOnModelChange()
{
    TsTasksGrid *grid = 
        qobject_cast<TsTasksGrid *>(mLoader.findWidget("taskgrid"));
    HbLabel *noItemsLabel = 
        qobject_cast<HbLabel *>(mLoader.findWidget("noitemslabel"));
    Q_ASSERT(grid);
    Q_ASSERT(noItemsLabel);

    if (grid->model()->rowCount()) {
        noItemsLabel->hide();
        grid->show();
    } else {
        noItemsLabel->show();
        grid->hide();
    }
}
