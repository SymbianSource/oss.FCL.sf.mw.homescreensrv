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
* Description: tsmodel.h
*
*/

#ifndef TSMODEL_H
#define TSMODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QSize>
#include <QSharedPointer>
#include <tstaskmonitor.h>
#ifdef Q_OS_SYMBIAN
#include <apgcli.h>
#endif

class TsTaskMonitor;
class ActivityManager;
class CaNotifier;
class TsModelItem;

class TsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit TsModel(TsTaskMonitor &applicationSrv,
                     QObject &activitySrv,
                     QObject *parent = 0);
    ~TsModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int maxRowCount()const;

public slots:
    void openApplication(const QModelIndex &index);
    void closeApplication(const QModelIndex &index);
    void updateModel();
    void entryChanged(TsModelItem *);
private:
    TsModelItem *entry(const QModelIndex &index) const;
    void getApplications();
    void getActivities();
    void prepareActivityEntry(QVariantHash &activity);
    QString getApplicationName(int id);

private:
    /*
     * List containing result
     */
    QList<TsModelItem *> mEntries;

    TsTaskMonitor &mApplicationService;

    /*
     * Activity Service
     */
    QObject &mActivityService;
    
#ifdef Q_OS_SYMBIAN
    RApaLsSession iAppArcSession;
#endif

    /*
     * Icon size in model
     */
    QSize mSize;
    
    /*
     * Maximum number of items (only for running apps + activities)
     */ 
    int mMaxItems;
};

#endif // TSMODEL_H
