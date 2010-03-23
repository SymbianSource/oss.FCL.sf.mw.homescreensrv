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
 * Description:
 *
 */

#ifndef CACLIENTNOTIFIERPROXY_H
#define CACLIENTNOTIFIERPROXY_H

#include <QObject>
#include <QMutex>
#include "cadef.h"

#include "caclient_global.h"
#include "cadefs.h"
#include "canotifier_p.h"
#include "caclientnotifiersession.h"

class CaEntry;
class CaNotifierFilter;

/**
 *  IDataObserver
 *
 */
class IDataObserver
{
public:
    /**
     * Method invoked when entry is changed.
     * @param entryId entry id.
     * @param changeType change type.
     */
    virtual void entryChanged(TInt entryId,
                              TChangeType changeType) const = 0;

    /**
     * Method invoked when entry is changed.
     * @param entry entry.
     * @param changeType change type.
     */
    virtual void entryChanged(const CCaInnerEntry &entry,
                              TChangeType changeType) const = 0;

    /**
     * Method invoked when entry was changed.
     * @param entryId entry id.
     */
    virtual void entryTouched(TInt id) const = 0;

    /**
     * Method invoked when entry was changed.
     * @param groupId group id.
     */
    virtual void groupContentChanged(TInt groupId) const = 0;
};

class CACLIENT_EXPORT CaClientNotifierProxy: public QObject,
    public IDataObserver
{
    Q_OBJECT

public:

    /**
     * Constructor.
     * @param parent pointer to parent. Defaults to NULL.
     */
    explicit CaClientNotifierProxy(QObject *parent = 0);

    /**
     * Destructor.
     */
    ~CaClientNotifierProxy();

    /**
     * Method invoked when entry is changed.
     * @param entryId entry id.
     * @param changeType change type.
     */
    virtual void entryChanged(TInt entryId, TChangeType changeType) const;

    /**
     * Method invoked when entry is changed.
     * @param entry entry.
     * @param changeType change type.
     */
    virtual void entryChanged(const CCaInnerEntry &entry,
                              TChangeType changeType) const;

    /**
     * Method invoked when entry was changed.
     * @param entryId entry id.
     */
    virtual void entryTouched(TInt entryId) const;

    /**
     * Method invoked when entry was changed.
     * @param groupId group id.
     */
    virtual void groupContentChanged(TInt groupId) const;

    /**
     * Method for registering notifier
     *
     * @param notifierFilter notifierFilter used to register notifier.
     * @param notifierType type of notifier
     * @return error code
     */
    int registerNotifier(
        const CaNotifierFilter *notifierFilter,
        CaNotifierPrivate::NotifierType notifierType,
        const CaClientNotifierProxy *notifierProxy);

    /**
     * Method for unregistering notifier.
     *
     * @param notifierFilter notifierFilter used to unregister notifier.
     * @param notifierType type of notifier
     */
    void unregisterNotifier(
        const CaNotifierFilter &notifierFilter,
        CaNotifierPrivate::NotifierType notifierType);

signals:

    void signalEntryChanged(int entryId, ChangeType changeType) const;
    void signalEntryChanged(const CaEntry &entry,
                            ChangeType changeType) const;
    void signalEntryTouched(int id) const;
    void signalGroupContentChanged(int groupId) const;

private:
    /**
     * Method for finding a session with free slots.
     *
     * @return a pointer to a session.
     */
    RCaClientNotifierSession *findSession();

    /**
     * Method for finding a position of a session with a subsession
     * associated with given parameters.
     *
     * @param notifierFilter notifierFilter used to unregister notifier.
     * @param innerNotifierType inner type of notifier
     * @return a position of a pointer to a session,
     * KErrNotFound when not found.
     */
    TInt findSession(
        const CaNotifierFilter &notifierFilter,
        CCaInnerNotifierFilter::TNotifierType innerNotifierType) const;

    /**
     * Method for creating a new session.
     *
     * @param[out] error error code.
     * @return a pointer to a newly created session, NULL in case of error.
     */
    RCaClientNotifierSession *newSession(TInt &error);

private:

    // Sessions to server.
    RArray<RCaClientNotifierSession> mSessions;

    // Mutex to serialize access to mSessions.
    QMutex mMutex;

    Q_DISABLE_COPY(CaClientNotifierProxy)

    friend class CaClientProxy;
    friend class CaClientObserver;

};

#endif //_CACLIENTNOTIFIERPROXY_H
