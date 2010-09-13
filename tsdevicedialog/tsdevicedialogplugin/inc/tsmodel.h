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

#include <QAbstractListModel>
#include <QList>
#include <QSize>
#include <QSharedPointer>

#include <tstaskmonitor.h>

class CaNotifier;
class TsModelItem;

class TsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    TsModel(TsTaskMonitor &applicationSrv,
            QObject &activitySrv,
            QObject *parent = 0);
    ~TsModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const;
    int maxRowCount()const;
    // from QAbstractModel
    virtual bool insertRows(int row, int count, TsModelItem *item,
                            const QModelIndex &parent = QModelIndex());
    virtual bool removeRows(int row, int count,
                            const QModelIndex &parent = QModelIndex());


    bool moveRows(int oldPosition, int newPosition,
                  const QModelIndex &parent = QModelIndex());
    bool updateRows(int row, TsModelItem *item);
    void fullUpdate();

public slots:
    void openApplication(const QModelIndex &index);
    void closeApplication(const QModelIndex &index);
    void updateApplications();
    void updateActivities();
    void entryChanged(TsModelItem *);
private:
    TsModelItem *entry(const QModelIndex &index) const;
    void getApplications();
    void getActivities(bool fullUpdate = true);

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

    /*
     * Maximum number of items (only for running apps + activities)
     */
    int mMaxItems;
    int mActivitiesCount;

    Q_DISABLE_COPY(TsModel)
};

#endif // TSMODEL_H
