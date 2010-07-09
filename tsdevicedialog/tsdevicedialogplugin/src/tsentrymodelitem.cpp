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
* Description: tsentrymodelitem.cpp
*
*/
#include "tsentrymodelitem.h"
#include "tsdataroles.h"

#include <tstask.h>
#include <HbIcon>

/*!
    \class TsEntryModelItem
    \ingroup group_tsdevicedialogplugin
    \brief Item presenting running apps in the grid.
*/

/*!
    Standard C++ constructor
    /param entry - Task Monitor data
*/
TsEntryModelItem::TsEntryModelItem(QSharedPointer<TsTask> entry)
    :
    mEntry(entry)
{
    //no implementation required
}

/*!
    Standard C++ destructor
*/
TsEntryModelItem::~TsEntryModelItem()
{
}

/*!
    Returns the data stored under the given role.
    /param role - requested data role
    /return data encapulated by QVariant
*/
QVariant TsEntryModelItem::data(int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            return QVariant(mEntry->name());
        case Qt::DecorationRole:
            {
            QPixmap icon = mEntry->screenshot();
            return QVariant::fromValue<HbIcon>(HbIcon(icon));
            }
        case TsDataRoles::Closable:
            return QVariant(mEntry->isClosable());
        case TsDataRoles::Active:
            return QVariant(mEntry->isActive());
        default:
            return QVariant(QVariant::Invalid);
    }
}

/*!
    Close running application repesented by entry
*/
void TsEntryModelItem::close()
{
    mEntry->close();
}

/*!
    Open or move to foreground application repesented by entry
*/
void TsEntryModelItem::open()
{
    mEntry->open();
}

