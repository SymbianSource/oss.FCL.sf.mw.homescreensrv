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
 * Description: caitemmodel.h
 *
 */

#ifndef CASTANDARDITEMMODEL_H
#define CASTANDARDITEMMODEL_H

// System includes
#include <QAbstractItemModel>
#include <QAbstractItemView>

// User includes
#include "caclient_global.h"
#include "caquery.h"

// Forward declarations
class CaEntry;
class CaItemModelPrivate;
CA_CLIENT_TEST_CLASS(TestCaClient)

// Class declaration
class CACLIENT_EXPORT CaItemModel: public QAbstractItemModel
{

Q_OBJECT
    CA_CLIENT_TEST_FRIEND_CLASS(TestCaClient)

public:

    // Data types
    enum CaItemRole
    {
        IdRole = Qt::UserRole + 1,
        ParentIdRole,
        TypeRole,
        FlagsRole,
        TextRole,//contains title represented as QString,
        FullTextRole //'title description' convinient for operations such as search
    };

    // Function declarations
    CaItemModel(const CaQuery &query, QObject *parent = 0);
    ~CaItemModel();

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex index(int row, int column = 0,
        const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &index) const;
    QModelIndex root() const;
    virtual QVariant data(const QModelIndex &index, int role =
        Qt::DisplayRole) const;
    void setAutoUpdate(bool autoUpdate);
    void setSecondLineVisibility(bool secondLineVisible);
    bool secondLineVisibility() const;
    bool isAutoUpdate() const;
    void setSort(SortAttribute sortAttribute, Qt::SortOrder sortOrder =
        Qt::AscendingOrder);
    void setIconSize(const QSize &size);
    QSize getIconSize() const;
    void updateModel();
    void setParentId(int parentId);
    void setFlagsOn(const EntryFlags &onFlags);
    void setFlagsOff(const EntryFlags &offFlags);
    QSharedPointer<CaEntry> entry(const QModelIndex &index) const;

    signals:

    void scrollTo(int row, QAbstractItemView::ScrollHint hint =
        QAbstractItemView::EnsureVisible);

protected:
    // data

    CaItemModelPrivate * const m_d;
    friend class CaItemModelPrivate;

};

#endif // CASTANDARDITEMMODEL_H
