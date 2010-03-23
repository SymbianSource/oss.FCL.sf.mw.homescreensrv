/*
 * Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QDebug>
#include <QMutexLocker>

#include "caclientnotifierproxy.h"
#include "caentry.h"
#include "caobjectadapter.h"
#include "canotifiers.h"

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
CaClientNotifierProxy::CaClientNotifierProxy(QObject *parent) :
    QObject(parent)
{
}
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
CaClientNotifierProxy::~CaClientNotifierProxy()
{
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CaClientNotifierProxy::entryChanged(int entryId,
        ChangeType changeType) const
{
    qDebug() << "CaClientProxy::entryChanged entryId:"
             << entryId << "changeType:" << changeType;
    emit signalEntryChanged(entryId, changeType);
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CaClientNotifierProxy::entryChanged(const CaEntry &entry,
        ChangeType changeType) const
{
    qDebug() << "CaClientProxy::entryChanged changeType:" << changeType;
    CaEntry *caEntry = new CaEntry(entry);
    emit signalEntryChanged(*caEntry, changeType);
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CaClientNotifierProxy::entryTouched(int id) const
{
    qDebug() << "CaClientProxy::entryTouched id:" << id;
    emit signalEntryTouched(id);
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CaClientNotifierProxy::groupContentChanged(int groupId) const
{
    qDebug() << "CaClientProxy::groupContentChanged groupId:" << groupId;
    emit signalGroupContentChanged(groupId);
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
int CaClientNotifierProxy::registerNotifier(
    const CaNotifierFilter *notifierFilter,
    CaNotifierPrivate::NotifierType notifierType,
    const CaClientNotifierProxy *notifierProxy)
{
    qDebug() << "CaClientProxy::registerNotifier notifierType:"
             << notifierType;
    QMutexLocker locker(&mMutex);
    return CaNotifiers::addNotifier(notifierFilter, notifierType, notifierProxy);
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CaClientNotifierProxy::unregisterNotifier(
    const CaNotifierFilter &notifierFilter,
    CaNotifierPrivate::NotifierType notifierType)
{
    qDebug() << "CaClientProxy::unregisterNotifier notifierType:"
             << notifierType;
    QMutexLocker locker(&mMutex);
    CaNotifiers::removeNotifier(&notifierFilter, notifierType);
}

