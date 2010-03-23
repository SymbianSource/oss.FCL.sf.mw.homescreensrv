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
 * Description: canotifier.cpp
 *
 */

#include "canotifier.h"
#include "canotifier_p.h"
#include "canotifierfilter.h"
#include "caclientnotifierproxy.h"

/*!
 \class CaNotifier.
 \brief This class describes notifier.

 \example
 It's example how client can be notified about changes on data specified by client's filter

 \code
 // example Client class which wants to be notified about changes

 class Client: public QObject
 {
 Q_OBJECT
 ...
 public slots:

 updateModelForEntryChanged(int,ChangeType);
 updateModelForEntryChanged(const CaEntry &,ChangeType);
 updateModelForEntryTouched(int);
 updateModelForgroupContentChanged(int);
 ...
 };
 \endcode

 \code
 // example code showed how to register for notifications
 Client * client;
 ...
 QSharedPointer<CaService> service = CaService::instance();
 CaNotifierFilter notifierFilter();
 CaNotifier * notifier = service->createNotifier(notifierfilter);

 // Connections cause that notifier is registered to server distributed
 // notifications while data changes.
 if( notifier )
 {
 connect( notifier, SIGNAL(entryChanged(int,ChangeType)),
 client, SLOT(updateModelForEntryChanged(int,ChangeType)) );
 connect( notifier, SIGNAL(entryChanged(CaEntry,ChangeType)),
 client, SLOT(updateModelForEntryChanged(CaEntry,ChangeType)) );
 connect( notifier, SIGNAL(entryTouched(int)),
 client, SLOT(updateModelForEntryTouched(int)) );
 connect( notifier, SIGNAL(groupContentChanged(int)),
 client, SLOT(updateModelForgroupContentChanged(int)) );
 }
 ...
 // creation new entry causes sent notification to the client
 CaEntry entry;
 entry.setText( "Text" );
 entry.setTypeName( "TypeName" );
 CaEntry * newEntry = service->createEntry( entry );
 ...
 \endcode

 \code
 // here client is being get notifications new entry was created
 ...
 Client::updateModelForEntryChanged(int entryId ,ChangeType changeType)
 {
 ...
 changeType == AddChangeType;
 ...
 }

 \endcode

 */

/*!
 Constructor.
 \param entryPrivate pointer to private implementation.
 */
CaNotifier::CaNotifier(CaNotifierPrivate * const notifierPrivate) :
    QObject(0), m_d(notifierPrivate)
{
    m_d->m_q = this;
    m_d->makeConnect();
}

/*!
 Destructor.
 */
CaNotifier::~CaNotifier()
{
    m_d->makeDisconnect();
    delete m_d;
}

/*!
    This method is called when something has been
    connected to signal in this object.
    \param signal which is used to register an appropriate notifier.
*/
void CaNotifier::connectNotify(const char *signal)
{
    qDebug("CaNotifier::connectNotify");
    qDebug("\tsignal: %s", signal);
    if (QLatin1String(signal)
        == SIGNAL(entryChanged(int,ChangeType))) {
         // signal is entryChanged(int, ChangeType)
        if (receivers(SIGNAL(entryChanged(int,ChangeType))) == 1) {
            m_d->registerNotifier(
                CaNotifierPrivate::EntryChangedWithIdNotifierType);
        }
     }
    else if (QLatin1String(signal)
        == SIGNAL(entryChanged(CaEntry,ChangeType))) {
        // signal is entryChanged(const CaEntry &, ChangeType)
        if (receivers(SIGNAL(entryChanged(const CaEntry &,ChangeType))) == 1) {
            m_d->registerNotifier(
                CaNotifierPrivate::EntryChangedWithEntryNotifierType);
        }
    }
    else if (QLatin1String(signal)
        == SIGNAL(entryTouched(int))) {
        // signal is entryTouched(int)
        if (receivers(SIGNAL(entryTouched(int)))) {
            m_d->registerNotifier(
                CaNotifierPrivate::EntryTouchedNotifierType);
        }
    }
    else if (QLatin1String(signal)
        == SIGNAL(groupContentChanged(int))) {
        // signal is groupContentChanged(int)
        if (receivers(SIGNAL(groupContentChanged(int)))) {
            m_d->registerNotifier(
                CaNotifierPrivate::GroupContentChangedNotifierType);
        }
    }
}

/*!
    This method is called when something has been
    disconnected from signal in this object.
    \param signal which is used to unregister an appropriate notifier.
*/
void CaNotifier::disconnectNotify(const char *signal)
{
    qDebug("CaNotifier::disconnectNotify");
    qDebug("\tsignal: %s", signal);
    if (QLatin1String(signal)
        == SIGNAL(entryChanged(int,ChangeType))) {
         // signal is entryChanged(int, ChangeType)
        if(receivers(SIGNAL(entryChanged(int,ChangeType)))==0) {
            m_d->unregisterNotifier(
                CaNotifierPrivate::EntryChangedWithIdNotifierType);
        }
     }
    else if (QLatin1String(signal)
        == SIGNAL(entryChanged(CaEntry,ChangeType))) {
        // signal is entryChanged(const CaEntry &, ChangeType)
        if (receivers(SIGNAL(entryChanged(const CaEntry &,ChangeType)))==0) {
            m_d->unregisterNotifier(
                CaNotifierPrivate::EntryChangedWithEntryNotifierType);
        }
    }
    else if (QLatin1String(signal)
        == SIGNAL(entryTouched(int))) {
        // signal is entryTouched(int)
        if (receivers(SIGNAL(entryTouched(int))) == 0) {
            m_d->unregisterNotifier(
                CaNotifierPrivate::EntryTouchedNotifierType);
        }
    }
    else if (QLatin1String(signal)
        == SIGNAL(groupContentChanged(int))) {
        // signal is groupContentChanged(int)
        if (receivers(SIGNAL(groupContentChanged(int))) == 0) {
            m_d->unregisterNotifier(
                CaNotifierPrivate::GroupContentChangedNotifierType);
        }
    }
}

/*!
 Constructor.
 \param notifierFilter descrbies entries to observe.
 */
CaNotifierPrivate::CaNotifierPrivate(
    const CaNotifierFilter &notifierFilter) :
    m_q(NULL),
    mNotifierFilter(NULL),
    mNotifierProxy(NULL)
{
        mNotifierFilter = new CaNotifierFilter(notifierFilter);
}

/*!
 Destructor.
 */
CaNotifierPrivate::~CaNotifierPrivate()
{
    if (mNotifierProxy) {
        mNotifierProxy->unregisterNotifier(*mNotifierFilter,
            EntryChangedWithIdNotifierType);
        mNotifierProxy->unregisterNotifier(*mNotifierFilter,
            EntryChangedWithEntryNotifierType);
        mNotifierProxy->unregisterNotifier(*mNotifierFilter,
            EntryTouchedNotifierType);
        mNotifierProxy->unregisterNotifier(*mNotifierFilter,
            GroupContentChangedNotifierType);
    }
    delete mNotifierFilter;
    delete mNotifierProxy;
}
/*!
 Registers notifier
 \param notifierType type of notifier to register (class of events to listen)
 */
int CaNotifierPrivate::registerNotifier(NotifierType notifierType)
{
    if (mNotifierProxy) {
        return mNotifierProxy->registerNotifier(
            mNotifierFilter,
            notifierType,
            mNotifierProxy);
    }
    return 0;
}
/*!
 Unregisters notifier
 \param notifierType type of notifier event type to unregister
     (class of events to listen)
 */
void CaNotifierPrivate::unregisterNotifier(NotifierType notifierType)
{
    if (mNotifierProxy) {
        mNotifierProxy->unregisterNotifier(*mNotifierFilter, notifierType);
    }
}

/*!
 Connects this Notifier
 */
void CaNotifierPrivate::makeConnect()
{
    if (m_q) {
        if (!mNotifierProxy) {
            mNotifierProxy = new CaClientNotifierProxy();
        }
        m_q->connect(mNotifierProxy,
            SIGNAL(signalEntryChanged(int,ChangeType)),
            m_q,
            SIGNAL(entryChanged(int,ChangeType)));
        m_q->connect(mNotifierProxy,
            SIGNAL(signalEntryChanged(const CaEntry &,ChangeType)),
            m_q,
            SIGNAL(entryChanged(const CaEntry &,ChangeType)));
        m_q->connect(mNotifierProxy,
            SIGNAL(signalEntryTouched(int)),
            m_q,
            SIGNAL(entryTouched(int)));
        m_q->connect(mNotifierProxy,
            SIGNAL(signalGroupContentChanged(int)),
            m_q,
            SIGNAL(groupContentChanged(int)));
    }
}
/*!
 Disconnects this Notifier
 */
void CaNotifierPrivate::makeDisconnect()
{
    if (m_q && mNotifierProxy) {
        m_q->disconnect(mNotifierProxy,
            SIGNAL(signalEntryChanged(int,ChangeType)),
            m_q,
            SIGNAL(entryChanged(int,ChangeType)));
        m_q->disconnect(mNotifierProxy,
            SIGNAL( signalEntryChanged(const CaEntry &,ChangeType)),
            m_q,
            SIGNAL(entryChanged(const CaEntry &,ChangeType)));
        m_q->disconnect(mNotifierProxy,
            SIGNAL(signalEntryTouched(int)),
            m_q,
            SIGNAL(entryTouched(int)));
        m_q->disconnect(mNotifierProxy,
            SIGNAL(signalGroupContentChanged(int)),
            m_q,
            SIGNAL(groupContentChanged(int)));
    }
}
