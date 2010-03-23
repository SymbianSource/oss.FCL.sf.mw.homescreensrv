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
#include "caclientnotifierproxy.h"
#include "caentry.h"
#include "cainnerentry.h"
#include "caobjectadapter.h"

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
void CaClientNotifierProxy::entryChanged(TInt entryId,
        TChangeType changeType) const
{
    qDebug() << "CaClientProxy::entryChanged entryId:"
             << entryId << "changeType:" << changeType;

    int entryChangedId(entryId);
    ChangeType entryChangeType(AddChangeType);
    CaObjectAdapter::convert(changeType, entryChangeType);

    emit signalEntryChanged(entryChangedId, entryChangeType);
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CaClientNotifierProxy::entryChanged(const CCaInnerEntry &entry,
        TChangeType changeType) const
{
    qDebug() << "CaClientProxy::entryChanged changeType:" << changeType;

    CaEntry *caEntry = new CaEntry(static_cast<EntryRole>(entry.GetRole()));
    ChangeType entryChangeType(AddChangeType);
    CaObjectAdapter::convert(entry, *caEntry);
    CaObjectAdapter::convert(changeType, entryChangeType);

    emit signalEntryChanged(*caEntry, entryChangeType);
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CaClientNotifierProxy::entryTouched(TInt id) const
{
    qDebug() << "CaClientProxy::entryTouched id:" << id;
    emit signalEntryTouched(id);
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
void CaClientNotifierProxy::groupContentChanged(TInt groupId) const
{
    qDebug() << "CaClientProxy::groupContentChanged groupId:" << groupId;

    int groupChangedId(groupId);

    emit signalGroupContentChanged(groupChangedId);
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

    CCaInnerNotifierFilter::TNotifierType innerNotifierType(
        CCaInnerNotifierFilter::EEntryChangedWithId);

    CaObjectAdapter::convert(notifierType,
                             innerNotifierType);
    CCaInnerNotifierFilter *innerNotifierFilter(NULL);

    TRAPD(error,
          innerNotifierFilter =
              CCaInnerNotifierFilter::NewLC(innerNotifierType);
          CaObjectAdapter::convertL(*notifierFilter, *innerNotifierFilter);
          CleanupStack::Pop(innerNotifierFilter)
         );

    if (!error) {
        // Lock the access to mSessions.
        // It will be automatically unlocked at the end of the current range.
        QMutexLocker locker(&mMutex);
        RCaClientNotifierSession *session = findSession();
        if (!session) {
            session = newSession(error);
        }
        if (!error) {
            error = session->RegisterNotifier(innerNotifierFilter,
                                              notifierFilter, notifierProxy);
        }
    }

    qDebug() << "CaClientProxy::registerNotifier result:" << error;

    return error;
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

    CCaInnerNotifierFilter::TNotifierType innerNotifierType(
        CCaInnerNotifierFilter::EEntryChangedWithId);
    CaObjectAdapter::convert(
        notifierType,
        innerNotifierType);
    TInt pos = findSession(notifierFilter, innerNotifierType);
    if (pos != KErrNotFound) {
        // Lock the access to mSessions.
        // It will be automatically unlocked at the end of the current range.
        QMutexLocker locker(&mMutex);
        mSessions[pos].UnregisterNotifier(&notifierFilter,
                                          innerNotifierType);
        if (mSessions[pos].SubsessionsCount() == 0) {
            mSessions[pos].Close();
            mSessions.Remove(pos);
        }
    }
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
RCaClientNotifierSession *CaClientNotifierProxy::findSession()
{
    RCaClientNotifierSession *session(NULL);
    TInt count(mSessions.Count());
    for (int i = 0; i < count; i++) {
        if (mSessions[i].SubsessionsCount() < KDefaultMessageSlots) {
            session = &mSessions[i];
            break;
        }
    }
    return session;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
TInt CaClientNotifierProxy::findSession(
    const CaNotifierFilter &notifierFilter,
    CCaInnerNotifierFilter::TNotifierType innerNotifierType) const
{
    TInt count(mSessions.Count());
    TInt pos(KErrNotFound);
    for (int i = 0; i < count; i++) {
        if (mSessions[i].ContainsSubsession(&notifierFilter,
                                            innerNotifierType)) {
            pos = i;
            break;
        }
    }
    return pos;
}

//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
RCaClientNotifierSession *CaClientNotifierProxy::newSession(TInt &error)
{
    RCaClientNotifierSession session;
    error = session.Connect();
    if (!error) {
        error = mSessions.Append(session);
    }
    if (error) {
        session.Close();
        qDebug("CaClientProxy::newSession: error (%d)", error);
        return NULL;
    } else {
        return &mSessions[mSessions.Count() - 1];
    }
}
