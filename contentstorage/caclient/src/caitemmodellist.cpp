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
 * Description: caitemmodel_p.cpp
 *
 */

#include <QIcon>

#include "caitemmodellist.h"
#include "caitemmodel_p.h"
#include "canotifier.h"
#include "canotifierfilter.h"
#include "caclienttest_global.h"

// ======== MEMBER FUNCTIONS ========

/*!
 Constructor
 \param aService content arsenall service provider
 */
CaItemModelList::CaItemModelList(QSharedPointer<CaService> service) :
    mOrderedList(), mEntriesHash(), mService(service)
{
}

/*!
 Destructor
 */
CaItemModelList::~CaItemModelList()
{
    clear();
}

/*!
 Clears list
 */
void CaItemModelList::clear()
{
    CACLIENTTEST_FUNC_ENTRY("CaItemModelList::clear");
    mOrderedList.clear();
    qDeleteAll(mEntriesHash);
    mEntriesHash.clear();
    CACLIENTTEST_FUNC_EXIT("CaItemModelList::clear");
}

/*!
 List count
 \retval list count
 */
int CaItemModelList::count() const
{
    return mOrderedList.count();
}

/*!
 Returns entry at aRow
 \param row of model list
 \retval entry at row
 */
CaEntry *CaItemModelList::at(int row) const
{
    int id = mOrderedList[row];
    return mEntriesHash.value(id);
}

/*!
 Reloads entries
 \param query needed to reload model list
 */
void CaItemModelList::reloadEntries(const CaQuery &query)
{
    CACLIENTTEST_FUNC_ENTRY("CaItemModelList::reloadEntries");
    clear();
    int id=0;
    QList<CaEntry *> eList = mService->getEntries(query);
    for (int i = 0; i < eList.count(); i++) {
        id = eList[i]->id();
        mOrderedList << id;
        mEntriesHash.insert(id, eList[i]);
    }
    CACLIENTTEST_FUNC_EXIT("CaItemModelList::reloadEntries");
}

/*!
 Updates entry with given id
 \param id of item in the list
 */
void CaItemModelList::updateEntry(int id)
{
    CACLIENTTEST_FUNC_ENTRY("CaItemModelList::updateEntry");
    if (mEntriesHash.contains(id)) {
        delete mEntriesHash.take(id);
        mEntriesHash.insert(id, mService->getEntry(id));
    }
    CACLIENTTEST_FUNC_EXIT("CaItemModelList::updateEntry");
}

/*!
 Updates entries
 \param query with sort order
 */
void CaItemModelList::updateEntries(const CaQuery &query)
{
    CACLIENTTEST_FUNC_ENTRY("CaItemModelList::updateEntries");
    mOrderedList = mService->getEntryIds(query);
    for (int i = 0; i < mOrderedList.count(); i++) {
        if (!mEntriesHash.contains(mOrderedList[i])) {
            mEntriesHash.insert(mOrderedList[i], mService->getEntry(
                                    mOrderedList[i]));
        }
    }
    CACLIENTTEST_FUNC_EXIT("CaItemModelList::updateEntries");
}

/*!
 Returns index with given id value
 \param id item id
 \retval index of item with given aId
 */
int CaItemModelList::indexOf(const int &id) const
{
    return mOrderedList.indexOf(id);
}

/*!
 Inserts item with given id at row
 (entry for given id is created and inserted in list)
 \param row at which item is to be inserted
 \param id of item to be inserted
 */
void CaItemModelList::insert(int row, int id)
{
    CACLIENTTEST_FUNC_ENTRY("CaItemModelList::insert");
    mOrderedList.insert(row, id);
    mEntriesHash.insert(id, mService->getEntry(id));
    CACLIENTTEST_FUNC_EXIT("CaItemModelList::insert");
}

/*!
 Removes item with given id
 \param id of item to remove
 */
void CaItemModelList::remove(int id)
{
    CACLIENTTEST_FUNC_ENTRY("CaItemModelList::remove");
    if (mEntriesHash.contains(id)) {
        delete mEntriesHash.take(id);
    }
    mOrderedList.removeOne(id);
    CACLIENTTEST_FUNC_EXIT("CaItemModelList::remove");
}

/*!
 Returns the item id at row position - aRow
 \param row item row
 \retval id of item at given row
 */
const int &CaItemModelList::operator[](int row) const
{
    return mOrderedList[row];
}
