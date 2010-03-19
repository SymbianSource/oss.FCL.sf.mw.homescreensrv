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
 * Description: canotifier_p.h
 *
 */

#ifndef CANOTIFIER_PRIVATE_H
#define CANOTIFIER_PRIVATE_H

class CaClientNotifierProxy;
class CaNotifier;
class CaNotifierFilter;

class CaNotifierPrivate
{
public:

    enum NotifierType {
        EntryChangedWithIdNotifierType = 0,
        EntryChangedWithEntryNotifierType,
        EntryTouchedNotifierType,
        GroupContentChangedNotifierType
    };

    CaNotifierPrivate(const CaNotifierFilter &notifierFilter);
    ~CaNotifierPrivate();

    int registerNotifier(NotifierType notifierType);
    void unregisterNotifier(NotifierType notifierType);

    void makeConnect();

    void makeDisconnect();

private:

    /*!
     * Points to the CaNotifier instance that uses
     * this private implementation.
     */
    const CaNotifier *m_q;

    /*!
     * Own.
     */
    const CaNotifierFilter *mNotifierFilter;

    /*!
     * Own.
     */
    CaClientNotifierProxy *mNotifierProxy;

    friend class CaNotifier;

};

#endif // CANOTIFIER_PRIVATE_H
