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
 * Description: caitemmodel.cpp
 *
 */

#include <QIcon>

#include "caitemmodel.h"
#include "caitemmodel_p.h"
#include "canotifier.h"
#include "canotifierfilter.h"

// Constants
const QSize defaultIconSize(30, 30);

// ======== MEMBER FUNCTIONS ========

/*!
 *   \class CaItemModel
 *
 *   \brief This class provides model containing CaEntry class objects.
 *
 *   To create instance of CaItemModel object, you need to pass CaQuery
 *   object in constructor. CaQuery should describe items that you want
 *   to have in a model.
 *
 *   \example
 *   \code
 *
 *   CaQuery query;
 *   query.setFlagsOn(VisibleEntryFlag);
 *   query.setParentId(collectionId);
 *   CaItemModel* model = new CaItemModel(query, this);
 *
 *   \endcode
 */

/*!
 Constructor.
 \param query query describing entries that should be present in a model.
 \param parent parent of a model
 */
CaItemModel::CaItemModel(const CaQuery& query, QObject * parent) :
    QAbstractItemModel(parent), m_d(new CaItemModelPrivate(query, this))
{

}

/*!
 Destructor
 */
CaItemModel::~CaItemModel()
{
    delete m_d;
}

/*!
 Returns number of columns
 \param parent not used
 \retval 1

 \code
 ...
 // to get number of columns in a model
 int columns = model->columnCount();
 ...
 \endcode

 */
int CaItemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    //model keeps entries in a column 0 and a parent entry in a column 1
    //parent entry is treated as an invisible root item,
    //so column count is always 1
    return 1;
}

/*!
 Returns number of rows
 \param parent not used
 \retval number of rows

 \code
 ...
 // to get number of rows in a model
 int rows = model->rowCount();
 ...
 \endcode
 */
int CaItemModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_d->rowCount();
}

/*!
 Returns QModelIndex of an item
 \param row row in which an item is placed
 \param column not used
 \param parent not used
 \retval index of item in model

 \code
 ...
 // to get model index of an item
 QModelIndex modelIndex = model->index(5);
 ...
 \endcode

 */
QModelIndex CaItemModel::index(int row, int column,
    const QModelIndex &parent) const
{
    Q_UNUSED(column);
    Q_UNUSED(parent);
    return m_d->index(row);
}

/*!
 Returns parent item
 \param child index (ignored)
 \retval parent index. in case of CaItemModel it is always QModelIndex()

 \code
 ...
 // to get model index of a parent of an item
 QModelIndex parentModelIndex = model->parent(childModelIndex);
 ...
 \endcode
 */
QModelIndex CaItemModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

/*!
 Returns root item model index
 \retval root item model index

 \code
 ...
 // to get model index of a root item
 QModelIndex rootIndex = model->root();
 ...
 \endcode
 */
QModelIndex CaItemModel::root() const
{
    return m_d->root();
}

/*!
 Returns appropiate model's data
 \param index model index
 \param role which data role to return
 \retval models data

 \code
 ...
 // to get data for model item
 QVariant = model->data(itemModelIndex, Qt::DisplayRole);
 ...
 \endcode

 */
QVariant CaItemModel::data(const QModelIndex &index, int role) const
{
    return m_d->data(index, role);
}

/*!
 Disables or enables auto-update feature of the model
 \param autoUpdate true to enable autoupdate, false to disable

 \code
 ...
 // to enable model auto update
 model->setAutoUpdate(true);
 ...
 \endcode

 */
void CaItemModel::setAutoUpdate(bool autoUpdate)
{
    m_d->setAutoUpdate(autoUpdate);
}

/*!
 Disables or enables secondline feature of the model
 \param secondLine enable or disable second line

 \code
 ...
 // to enable model second line visibility
 model->setSecondLineVisibility(true);
 ...
 \endcode

 */
void CaItemModel::setSecondLineVisibility(bool secondLineVisible)
{
    m_d->setSecondLineVisibility(secondLineVisible);
}

/*!
 Gets second line visibility attribute
 \retrun second line visibility attribute

 \code
 ...
 // to check second line visibility attribute
 bool visibility = model->secondLineVisibility();
 ...
 \endcode

 */
bool CaItemModel::secondLineVisibility() const
{
    return m_d->secondLineVisibility();
}
/*!
 Returns auto update status
 \retval true if autoupdate is on, false if not

 \code
 ...
 // to check auto update attribute
 bool autoUpdate = model->isAutoUpdate();
 ...
 \endcode

 */
bool CaItemModel::isAutoUpdate() const
{
    return m_d->notifierExists();
}

/*!
 Method for setting sorting order on model
 \param sortAttribute sort attribute (by name, timestamp, ect...)
 \param sortOrder sort order (ascending, descending)

 \code
 ...
 // to set sort order in a model
 model->setSort(NameSortAttribute, Qt::Ascending);
 ...
 \endcode

 */
void CaItemModel::setSort(SortAttribute sortAttribute,
    Qt::SortOrder sortOrder)
{
    m_d->setSort(sortAttribute, sortOrder);
}

/*!
 Method for setting icon size
 \param  size icon size to display

 \code
 ...
 // to set an icon size in a model
 QSize iconSize(50,50);
 model->setIconSize(iconSize);
 ...
 \endcode

 */
void CaItemModel::setIconSize(const QSize &size)
{
    m_d->setIconSize(size);
}

/*!
 Method for getting icon size
 \param  size icon size to display
 */
QSize CaItemModel::getIconSize() const
{
    return m_d->getIconSize();
}

/*!
 Updates model with fresh entries

 \code
 ...
 // to refresh a model
 model->updateModel();
 ...
 \endcode

 */
void CaItemModel::updateModel()
{
    m_d->updateModel();
}

/*!
 Sets parent and fetch children items from a storage
 \param parentId id of a collection

 \code
 ...
 // to set a new parent id
 int newParentId = 10;
 model->setParentId(newParentId);
 ...
 \endcode

 */
void CaItemModel::setParentId(int parentId)
{
    m_d->setParentId(parentId);
}

/*!
 Returns an entry from model
 \param index of entry in model
 \retval pointer to an entry

 \code
 ...
 // to get an entry from a model
 CaEntry* entry = model->entry(entryModelIndex);
 ...
 delete entry;
 \endcode

 */
CaEntry* CaItemModel::entry(const QModelIndex &index) const
{
    return m_d->entry(index);
}

/*!
 Constructor
 \param query needed to create model
 \param pointer to public implementation object connected
 */
CaItemModelPrivate::CaItemModelPrivate(const CaQuery &query,
    CaItemModel *itemModelPublic) :
    QObject(), m_q(itemModelPublic), mParentEntry(0), mQuery(query),
    mService(CaService::instance()), mEntries(mService), mNotifier(NULL),
    mSize(defaultIconSize), mSecondLineVisibility(true)
{
    updateModel();
    setAutoUpdate(true);
}

/*!
 Destructor
 */
CaItemModelPrivate::~CaItemModelPrivate()
{
    mEntries.clear();
    delete mParentEntry;
    delete mNotifier;
}

/*!
 Returns count of rows in model
 \retval number of rows
 */
int CaItemModelPrivate::rowCount() const
{
    return mEntries.count();
}

/*!
 Returns QModelIndex of an item
 \param row row
 \retval QModelIndex of item in model
 */
QModelIndex CaItemModelPrivate::index(int row)
{
    if ((row >= 0) && (row < mEntries.count())) {
        return m_q->createIndex(row, 0);
    } else {
        return QModelIndex();
    }
}

/*!
 Returns appropiate model's data
 \param modelIndex model index
 \param role which data role to return
 \retval models data as QVariant
 */
QVariant CaItemModelPrivate::data(const QModelIndex &modelIndex,
    int role) const
{
    if (!modelIndex.isValid())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        return displayRole(modelIndex);
    case Qt::DecorationRole:
        return QVariant(QIcon(entry(modelIndex)->makeIcon(mSize)));
    case CaItemModel::IdRole:
        return QVariant(entry(modelIndex)->id());
    case CaItemModel::TypeRole:
        return QVariant(entry(modelIndex)->entryTypeName());
    case CaItemModel::FlagsRole:
        return qVariantFromValue(entry(modelIndex)->flags());
    case CaItemModel::TextRole:
        return QVariant(entry(modelIndex)->text());
    default:
        return QVariant(QVariant::Invalid);
    }
}

/*!
 Disables or enables auto-update feature of the model
 \param autoUpdate (HsMenuAutoUpdate)
 */
void CaItemModelPrivate::setAutoUpdate(bool autoUpdate)
{
    if (autoUpdate) {
        if (!mNotifier) {
            CaNotifierFilter filter(mQuery);
            mNotifier = mService->createNotifier(filter);
            connectSlots();
        }
    } else {
        disconnectSlots();
        delete mNotifier;
        mNotifier = NULL;
    }
}

/*!
 Method for setting sorting order on model
 (probably will be moved to MenuService)
 \param  sortOrder sorting order (SortAttribute)
 */
void CaItemModelPrivate::setSort(SortAttribute sortAttribute,
    Qt::SortOrder sortOrder)
{
    mQuery.setSort(sortAttribute, sortOrder);
    updateLayout();
}

/*!
 Method for setting icon size
 \param size icon size to display
 */
void CaItemModelPrivate::setIconSize(const QSize &size)
{
    if (mSize == size)
        return;
    m_q->layoutAboutToBeChanged();
    mSize = size;
    m_q->layoutChanged();
}

/*!
 Method for getting icon size
 \retval icon size to display
 */
QSize CaItemModelPrivate::getIconSize() const
{
    return mSize;
}


/*!
 Gets index of the parent item
 \retval QModelIndex representing root item
 */
QModelIndex CaItemModelPrivate::root()
{
    if (mQuery.parentId()) {
        return m_q->createIndex(0, 1);
    } else {
        return QModelIndex();
    }
}

/*!
 Returns an entry from model
 \param modelIndex index of entry in model
 \retval pointer to an entry
 */
CaEntry* CaItemModelPrivate::entry(const QModelIndex &modelIndex) const
{
    if (modelIndex.column() == 1) {
        return mParentEntry;
    } else {
        return mEntries.at(modelIndex.row());
    }
}

/*!
 Disables or enables secondline feature of the model
 \param secondLine disables or enables second line
 */
void CaItemModelPrivate::setSecondLineVisibility(bool secondLineVisibility)
{
    if (mSecondLineVisibility == secondLineVisibility)
        return;
    m_q->layoutAboutToBeChanged();
    mSecondLineVisibility = secondLineVisibility;
    m_q->layoutChanged();
}

/*!
 Gets second line visibility attribute
 \retrun second line visibility attribute
 */
bool CaItemModelPrivate::secondLineVisibility() const
{
    return mSecondLineVisibility;
}

/*!
 Returns proper display role for item
 \param modelIndex item index
 \retval QVariant containing display role
 */
QVariant CaItemModelPrivate::displayRole(const QModelIndex &modelIndex) const
{
    QVariant result;
    if (mSecondLineVisibility) {
        if (entry(modelIndex)->description().isEmpty()) {
            result = entry(modelIndex)->text();
        } else {
            QList<QVariant> text;
            text << entry(modelIndex)->text();
            text << entry(modelIndex)->description();
            result = QVariant(text);
        }
    } else {
        result = entry(modelIndex)->text();
    }
    return result;
}

/*!
 Sets parent
 \param parentId
 */
void CaItemModelPrivate::setParentId(int parentId)
{
    mQuery.setParentId(parentId);
    if (mNotifier) {
        delete mNotifier;
        mNotifier = mService->createNotifier(CaNotifierFilter(mQuery));
        reconnectSlots();
    }
    updateModel();
}


/*!
 Checks if notifier exists
  \retval true if notifier exists otherwise false
 */
bool CaItemModelPrivate::notifierExists() const
{
    if (mNotifier) {
        return true;
    }
    return false;
}


/*!
 Updates model with fresh entries and resets model
 */
void CaItemModelPrivate::updateModel()
{
    mEntries.reloadEntries(mQuery);
    updateParentEntry();
    m_q->reset();
}

/*!
 Updates parent entry
 */
void CaItemModelPrivate::updateParentEntry()
{
    if (mQuery.parentId()) {
        delete mParentEntry;
        mParentEntry = mService->getEntry(mQuery.parentId());
    }
}

/*!
 Updates model item with fresh data
 \param id id of item to update
 */
void CaItemModelPrivate::updateItemData(int id)
{
    mEntries.updateEntry(id);

    QList<int> ids = mService->getEntryIds(mQuery);
    if (mEntries.indexOf(id) >= 0 && ids.indexOf(id)
        == mEntries.indexOf(id)) {
        emit m_q->dataChanged(index(mEntries.indexOf(id)), index(
            mEntries.indexOf(id)));
    } else {
        if (mParentEntry && id == mParentEntry->id()) {
            updateParentEntry();
            m_q->reset();
        } else {
            updateLayout();
        }
    }
}

/*!
 Adds new item to model
 \param id id of item to add
 */
void CaItemModelPrivate::addItem(int id)
{
    int row = itemRow(id);
    //we use beginInsertRows and endInsertRows to emit proper signal
    //(see Qt documentation of QAbstractItemModel)
    if (mEntries.indexOf(id) < 0 && row >= 0) {
        m_q->beginInsertRows(QModelIndex(), row, row);
        mEntries.insert(row, id);
        m_q->endInsertRows();
    }
}

/*!
 Adds new items to model
 \param itemsList current items list
 */
void CaItemModelPrivate::handleAddItems(QList<int> &itemsList)
{
    int entriesCount = mEntries.count();
    if (entriesCount) {
        int lastRow = itemsList.indexOf(mEntries[entriesCount - 1]);
        if (itemsList.count() == entriesCount && lastRow == (entriesCount
            - 1)) {
            //count is same and last item is in same position
            //so we update whole model
            updateModel();
        } else if ((itemsList.count() - entriesCount) == 1 && lastRow
            == entriesCount) {
            //just one item added - collection
            int i = 0;
            for (i = 0; i < entriesCount; i++) {
                if (itemsList[i] != mEntries[i]) {
                    addItem(itemsList[i]);
                }
            }
            while (i < itemsList.count()) {
                addItem(itemsList[i]);
                i++;
            }
        } else {
            //some items were inserted or reordered,
            //so we update layout and emit signal with row number
            //of first moved/added item
            //signal is needed to scroll a view to proper position after
            //some items were added
            updateLayout();
            emit m_q->scrollTo(lastRow + 1,
                QAbstractItemView::PositionAtTop);
        }
    } else {
        updateModel();
    }
}

/*!
 Gets index/row for new item
 \param id of item to add
 \retval row in model list for new item to insert
 */
int CaItemModelPrivate::itemRow(int id)
{
    QList<int> ids = mService->getEntryIds(mQuery);
    return ids.indexOf(id);
}

/*!
 Removes item from model
 \param id of item to remove
 */
void CaItemModelPrivate::removeItem(int id)
{
    int row = mEntries.indexOf(id);
    if (row >= 0) {
        m_q->beginRemoveRows(QModelIndex(), mEntries.indexOf(id),
            mEntries.indexOf(id));
        mEntries.remove(id);
        m_q->endRemoveRows();
    }
}

/*!
 Removes missing items from model
 \param itemsList current items list
 */
void CaItemModelPrivate::removeItems(const QList<int> &itemsList)
{
    int i = 0;
    for (i = 0; i < itemsList.count(); i++) {
        if (itemsList[i] != mEntries[i]) {
            removeItem(mEntries[i]);
        }
    }
    while (i < mEntries.count()) {
        removeItem(mEntries[i]);
        i++;
    }
}

/*!
 Layout update
 */
void CaItemModelPrivate::updateLayout()
{
    m_q->layoutAboutToBeChanged();
    mEntries.updateEntries(mQuery);
    updateParentEntry();
    m_q->layoutChanged();
}

/*!
 Connects slots
 */
void CaItemModelPrivate::connectSlots()
{
    connect(mNotifier, SIGNAL(entryChanged(int,ChangeType)),
    this, SLOT(updateModelItem(int,ChangeType)) );

    if (mQuery.parentId() > 0) {
        connect(mNotifier, SIGNAL(groupContentChanged(int)),
        this, SLOT(updateModelContent(int)) );
    }
}

/*!
 Disconnects slots
 */
void CaItemModelPrivate::disconnectSlots()
{
    disconnect(mNotifier, SIGNAL(entryChanged(int,ChangeType)),
    this, SLOT(updateModelItem(int,ChangeType)) );
    if (mQuery.parentId() > 0) {
        disconnect(mNotifier, SIGNAL(groupContentChanged(int)),
        this, SLOT(updateModelContent(int)) );
    }
}

/*!
 Reconnects slots
 */
void CaItemModelPrivate::reconnectSlots()
{
    disconnectSlots();
    connectSlots();
}

/*!
 Updates model with fresh entries
 \param id of item to handle
 \param changeType change type
 */
void CaItemModelPrivate::updateModelItem(int id, ChangeType changeType)
{
    switch (changeType) {
    case AddChangeType:
        addItem(id);
        break;
    case RemoveChangeType:
        removeItem(id);
        break;
    default:
        updateItemData(id);
        break;
    }
}

/*!
 Updates model with fresh entries
 \param id of parent
 */
void CaItemModelPrivate::updateModelContent(int id)
{
    Q_UNUSED(id);

    QList<int> ids = mService->getEntryIds(mQuery);

    if (ids.count() >= mEntries.count()) {
        handleAddItems(ids);
    } else {
        removeItems(ids);
    }
}
