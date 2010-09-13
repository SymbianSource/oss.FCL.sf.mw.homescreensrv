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
* Description: tsmodel.cpp
*
*/
#include "tsmodel.h"

#include <QVariant>
#include <QList>

#include <HbIcon>

#include <afstorageglobals.h>

#ifdef Q_OS_SYMBIAN
#include <XQSettingsManager>
#include <apaid.h>

const int TSDeviceDialogUid = 0x2002677F;
const int ItemsLimit = 0x00000001;
#endif

#include "tsmodelitem.h"
#include "tsentrymodelitem.h"
#include "tsactivitymodelitem.h"
#include "tsdataroles.h"
#include "tstaskchangeinfo.h"

const int maxItems(10);
/*!
    \class TsModel
    \ingroup group_tsdevicedialogplugin
    \brief Model storing running application and activieties.
*/

/*!
    Constructor
    \param query used to create model
    \param pointer to parent object
*/
TsModel::TsModel(TsTaskMonitor &applicationSrv,
                 QObject &activitySrv,
                 QObject *parent) :
    QAbstractListModel(parent),
    mEntries(),
    mApplicationService(applicationSrv),
    mActivityService(activitySrv),
    mMaxItems(maxItems)
{

#ifdef Q_OS_SYMBIAN
    XQSettingsManager *crManager = new XQSettingsManager;
    XQCentralRepositorySettingsKey itemsNumberKey(TSDeviceDialogUid, ItemsLimit);
    QVariant itemsNumberVariant =
        crManager->readItemValue(itemsNumberKey, XQSettingsManager::TypeInt);
    if (!itemsNumberVariant.isNull()) {
        int number = itemsNumberVariant.toInt();
        if (number > 0) {
            mMaxItems = number;
        }
    }
#endif

    connect(&activitySrv,
            SIGNAL(dataChanged()),
            this,
            SLOT(updateActivities()));
    connect(&applicationSrv,
            SIGNAL(taskListChanged()),
            this,
            SLOT(updateApplications()));

    fullUpdate();
}

/*!
    Destructor
*/
TsModel::~TsModel()
{
    qDeleteAll(mEntries);
}

/*!
    Returns count of rows in model
    \retval number of rows
*/
int TsModel::rowCount(
    const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mEntries.count();
}

bool TsModel::insertRows(int row, int count, TsModelItem *item, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row+count-1);
    mEntries.insert(row, item);
    endInsertRows();
    return true;
}

bool TsModel::removeRows(int row, int count, const QModelIndex &parent)
{
    TsModelItem *oldItem = mEntries.at(row);
    beginRemoveRows(parent, row, row + count - 1);
    mEntries.removeAt(row);
    delete oldItem;
    endRemoveRows();
    return true;
}


bool TsModel::moveRows(int oldPosition, int newPosition, const QModelIndex &parent)
{
    beginMoveRows(parent, oldPosition, oldPosition, parent, newPosition);
    mEntries.move(oldPosition, newPosition);
    endMoveRows();
    return true;
}


bool TsModel::updateRows(int row, TsModelItem *item)
{
    TsModelItem *oldItem = mEntries.at(row);
    mEntries[row] = item;
    delete oldItem;

    emit dataChanged(index(row),index(row));
    return true;
}

/*!
    Returns appropiate model's data
    \param index model index
    \param role which data role to return
    \retval models data
*/
QVariant TsModel::data(const QModelIndex &index,
                       int role) const
{
    return index.isValid() ? entry(index)->data(role) : QVariant();
}

/*!
    Returns maximum anount of data allowed for model
    \retval maximum data count
*/

int TsModel::maxRowCount()const
{
    return mMaxItems;
}

/*!
    Activate one of model entries
*/
void TsModel::openApplication(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    entry(index)->open();
}

/*!
    Close one of moder entries
*/
void TsModel::closeApplication(const QModelIndex &index)
{
    if (!index.isValid() ||
            !entry(index)->data(TsDataRoles::Closable).toBool()) {
        return;
    }
    entry(index)->close();
}

/*!
    Updates model with fresh entries
*/
void TsModel::updateApplications()
{
    QList<TsTaskChange>  changes(mApplicationService.changeList());

    if (changes.count() == 0) {
        //no applications - only activities on list
        return;
    }
    //check 1st item whether we have cancel change - if so reset model
    if (changes[0].first.changeType() == TsTaskChangeInfo::EChangeCancel) {
        fullUpdate();
        return;
    }
    for (int iter(0); iter < changes.count(); iter++) {
        switch (changes[iter].first.changeType()) {
            case TsTaskChangeInfo::EChangeDelete :
                removeRows(changes[iter].first.oldOffset(), 1);
                //we had max rows before delete - so there is possibility to add 
                //one activity
                if(mEntries.count() + 1 == maxRowCount()) {
                    beginInsertRows(QModelIndex(), mEntries.count(), mEntries.count());
                    getActivities(false);
                    endInsertRows();
                }
                break;
            case TsTaskChangeInfo::EChangeInsert :
                insertRows(changes[iter].first.newOffset(), 1,
                           new TsEntryModelItem(changes[iter].second));
                //we have too many items - delete some activities if we can
                while(mEntries.count() > maxRowCount() && mActivitiesCount > 0) {
                    removeRows(mEntries.count()-1, 1);
                    mActivitiesCount--;
                }
                break;
            case TsTaskChangeInfo::EChangeMove :
                moveRows(changes[iter].first.oldOffset(), changes[iter].first.newOffset());
                break;
            case TsTaskChangeInfo::EChangeUpdate :
                updateRows(changes[iter].first.oldOffset(),
                           new TsEntryModelItem(changes[iter].second));
                break;
            default:
                break;
        }
    }

    //because delete entries are at end of changelist - iterate backwards

}

/*!
    Updates model with fresh entries
*/
void TsModel::updateActivities()
{
    //as for now we need full update when activities change
    fullUpdate();
}

/*!
    reset model using full application and activities lists
*/
void TsModel::fullUpdate()
{
    beginResetModel();
    qDeleteAll(mEntries);
    mEntries.clear();
    mActivitiesCount = 0;
    getApplications();
    getActivities();
    endResetModel();

}

/*!
    Read list of running applications
*/
void TsModel::getApplications()
{
    //get all running applications and append to entries list
    TsModelItem *entry(0);
    QList< TsTaskChange> tasks(mApplicationService.changeList(true));
    foreach(TsTaskChange taskData, tasks) {
        if (!taskData.second.isNull()) {
            entry = new TsEntryModelItem(taskData.second);
            if (entry) {
                mEntries.append(entry);
            }
        }
    }
}

/*!
    Read current activities
*/
void TsModel::getActivities(bool fullUpdate)
{
    //get activities
    int maxActivitiesCount = maxRowCount() - mEntries.count() + mActivitiesCount;
    if (maxActivitiesCount > 0) {
        QList<QVariantHash> activities;
        QMetaObject::invokeMethod(&mActivityService,
                                  "activitiesList",
                                  Q_RETURN_ARG(QList<QVariantHash>, activities),
                                  Q_ARG(int, maxActivitiesCount));
        int iterPos = fullUpdate ? 0 : mActivitiesCount;
        for(int iter(iterPos); iter< activities.count(); iter++) {
            mEntries.append(new TsActivityModelItem(*this, mActivityService, activities[iter]));
            mActivitiesCount++;
        }
    }
}

/*!
    Called when some item was changed
    \param itemPtr - address of updated item
*/
void TsModel::entryChanged(TsModelItem *itemPtr)
{
    const int itemIndex = mEntries.indexOf(itemPtr);
    if (itemIndex != -1) {
        emit dataChanged(index(itemIndex, 0), index(itemIndex, 0));
    }
}

/*!
    Returns an entry from model
    \param index of entry in model
    \retval pointer to an entry
*/
TsModelItem *TsModel::entry(const QModelIndex &index) const
{
    return mEntries.at(index.row());
}
