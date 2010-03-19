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
 * Description: caquery.h
 *
 */

#ifndef CAQUERY_H
#define CAQUERY_H

#include <QStringList.h>
#include "caclient_global.h"
#include "cadefs.h"

class CaQueryPrivate;

class CACLIENT_EXPORT CaQuery
{

public:

    CaQuery();
    CaQuery(const CaQuery &query);
    CaQuery &operator=(const CaQuery &query);
    ~CaQuery();

    void setEntryRoles(EntryRoles entryRoles);
    EntryRoles entryRoles() const;

    void setParentId(int id);
    int parentId() const;

    void setEntryTypeNames(const QStringList &entryTypeNames);
    QStringList entryTypeNames() const;

    void addEntryTypeName(const QString &entryTypeName);

    void setFlagsOn(const EntryFlags &onFlags);
    EntryFlags flagsOn() const;
    void setFlagsOff(const EntryFlags &offFlags);
    EntryFlags flagsOff() const;

    void setSort(SortAttribute sortAttribute, 
        Qt::SortOrder sortOrder = Qt::AscendingOrder);
    void getSort(SortAttribute& sortAttribute, 
        Qt::SortOrder& sortOrder) const;

    unsigned int count() const;
    void setCount(unsigned int count);

    void clear();

private:

    /**
     * Pointer to a private implementation.
     */
    CaQueryPrivate * const m_d;
    friend class CaQueryPrivate;

};

#endif // CAQUERY_H
