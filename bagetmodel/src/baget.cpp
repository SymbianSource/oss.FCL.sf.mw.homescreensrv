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
* Description:  Base class for all bagets.
*
*/

#include "baget.h"
#include "baget_p.h"

/*!
    \enum BagetState
    Lists states that the baget can be in.
*/

/*
    \var BagetState BagetStateConstructed
    Baget is in this state right after construction.
*/

/*
    \var BagetState BagetStateRunning
    Baget is performing regular operations (animations, etc.).
    Baget is set to Running state after a call to start (if previously Constructed or Stopped) 
    or resume (if previously Suspended) methods.
    \sa Baget::start()
    \sa Baget::resume()
*/

/*
    \var BagetState BagetStateSuspended
    Baget operations are suspended (e.g. animation timers stopped).
    Baget is set to Suspended after a call to suspend (if previously Running) method.
    \sa Baget::suspend()
*/

/*
    \var BagetState BagetStateStopped 
    Baget finished all processing and released all resources.
    Baget is set to Stopped after a call to stop (if previously Running or Suspended) method.
    \sa Baget::stop()
*/

/*!
    \class Baget
    \brief Base class for all Bagets.

    Baget (BAckGround widgET) is a widget intended to be displayed in the background 
    of an application view. It does not respond to user input. It usually performs some 
    animations to make the view more appealing.
    Bagets are intended for use with HbStackedLayout as the bottom most QGraphicsWidget.
 */

/*!
    Constructs a new Baget with \a parent and \a flags.
 */
Baget::Baget(QGraphicsItem *parent, Qt::WindowFlags flags) :
    QGraphicsWidget(parent, flags), m_d(new BagetPrivate(this))
{
}

/*!
    Destructs the class.
 */
Baget::~Baget()
{
    delete m_d;
}

/*!
    \fn void Baget::faulted()

    This signal is emitted if a fault occurs when changing Baget's state.
 */

/*!
    Returns the state that the Baget is currently in.
    \return The current state.
 */
BagetState Baget::currentState()
{
    return m_d->currentState();
}

/*!
    Initiates Baget's processing.
 */
void Baget::start()
{
    m_d->start();
}

/*!
    Stops Baget's processing.
    After this call Baget should also free all resources.
 */
void Baget::stop()
{
    m_d->stop();
}

/*!
    Resumes Baget's processing.
 */
void Baget::resume()
{
    m_d->resume();
}

/*!
    Suspends Baget's processing.
 */
void Baget::suspend()
{
    m_d->suspend();
}

/*!
    \fn virtual bool Baget::onStart() = 0

    After a call it should start Baget's processing.
    Returns true if the operation secceeded, otherwise false - in this case 
    the faulted() signal will be emitted by the base class.
    \return Indicates if the operation succeeded.
 */

/*!
    \fn virtual bool Baget::onStop() = 0

    After a call it should stop Baget's processing and 
    free all resources.
    Returns true if the operation secceeded, otherwise false - in this case 
    the faulted() signal will be emitted by the base class.
    \return Indicates if the operation succeeded.
 */

/*!
    After a call it should resume Baget's processing.
    Returns true if the operation secceeded, otherwise false - in this case 
    the faulted() signal will be emitted by the base class.
    The default implementation does nothing and always returns true.
    \return Indicates if the operation succeeded.
 */
bool Baget::onResume()
{
    return true;
}

/*!
    After a call it should suspend Baget's processing.
    Returns true if the operation secceeded, otherwise false - in this case 
    the faulted() signal will be emitted by the base class.
    The default implementation does nothing and always returns true.
    \return Indicates if the operation succeeded.
 */
bool Baget::onSuspend()
{
    return true;
}
