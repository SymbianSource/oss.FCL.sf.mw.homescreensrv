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
 * Description: caservice_p.h
 *
 */

#ifndef CASERVICE_PRIVATE_H
#define CASERVICE_PRIVATE_H

#include <QList>

class QString;
class CaClientProxy;
class CaEntry;
class CaQuery;
class CaService;
class CaNotifier;

class CaServicePrivate
{

public:

    explicit CaServicePrivate(CaService *servicePublic);
    ~CaServicePrivate();

    QList<CaEntry *> getEntries(const QList<int> &entryIdList) const;
    QList<CaEntry *> getEntries(const CaQuery &query) const;
    QList<int> getEntryIds(const CaQuery &query) const;

    CaEntry *createEntry(const CaEntry &entry);

    bool removeEntries(const QList<int> &entryIdList);

    bool updateEntry(const CaEntry &entry);

    bool touch(const CaEntry &entry);

    bool insertEntriesIntoGroup(int groupId, const QList<int> &entryIdList,
                                int beforeEntryId);

    bool removeEntriesFromGroup(int groupId, const QList<int> &entryIdList);

    bool appendEntriesToGroup(int groupId, const QList<int> &entryIdList);

    bool prependEntriesToGroup(int groupId, const QList<int> &entryIdList);

    bool executeCommand(const CaEntry &entry, const QString &command);

    CaNotifier *createNotifier(const CaNotifierFilter &filter);

    bool customSort(int groupId, QList<int> &entryIdList);

    ErrorCode lastError() const;

private:

    /**
     * Points to the CaService instance that uses this private implementation.
     */
    CaService *const m_q;

    /**
     * Proxy to communicate with Symbian server.
     */
    CaClientProxy *mProxy;

    /**
     * code of error caused by last operation.
     */
    mutable ErrorCode mErrorCode;

};

#endif //CASERVICE_PRIVATE_H
