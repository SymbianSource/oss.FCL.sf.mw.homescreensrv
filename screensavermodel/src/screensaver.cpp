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
* Description:  Base class for all screensavers.
*
*/

#include "screensaver.h"
#include "screensaver_p.h"

/*!
    \enum ScreensaverState
    Lists states that the Screensaver can be in.
*/

/*
    \var ScreensaverState ScreensaverStateConstructed
    Screensaver is in this state right after construction.
*/

/*
    \var ScreensaverState ScreensaverStateInitialized

    All Screensaver resources are initialized.
    Screensaver is set to Initialized state after a call to initialize 
    (if previously Constructed or Closed) method.
*/

/*
    \var Screensaver ScreensaverStateBackground
    Screensaver is in background, its operations are suspended.
    Screensaver is set to Background after a call to background method.
*/

/*
    \var Screensaver ScreensaverStateForeground 
    Screensaver is in foreground and fully operational, showing the main visualization.
    Screensaver is set to Foreground after a call to foreground method.
*/

/*
    \var Screensaver ScreensaverStatePartialForeground 
    Screensaver has limited foreground (in OLED display cases).
    Screensaver is set to PartialForeground after a call to partialForeground method.
*/

/*
    \var Screensaver ScreensaverStatePowerSave 
    Device is in power save mode. Screensaver should limit all processing.
    Screensaver is set to PowerSave after a call to powerSave method.
*/

/*
    \var Screensaver ScreensaverStateClosed 
    Screensaver is closed. All resources should be frees.
    Screensaver is set to Closed after a call to close method.
*/

/*!
    \class Screensaver
    \brief Base class for all screensavers.

    Screensaver plug-ins provide the visualizations for different screensaver application states.
    A Screensaver is notified about state changes and in consequence it should emit a signal
    viewChanged() carrying a QGraphicsWidget which will be set as the current view.
    The application takse care about tracing device status so the Screensaver should be only
    concerned about the GUI.
 */

/*!
    Constructs a new Screensaver with \a parent.
 */
Screensaver::Screensaver(QObject *parent) :
    QObject(parent), m_d(new ScreensaverPrivate(this))
{
}

/*!
    Destructs the class.
 */
Screensaver::~Screensaver()
{
    delete m_d;
}

/*!
    \fn void Screensaver::faulted()

    This signal is emitted if a fault occurs when changing Screensaver's state.
 */

/*!
    \fn void Screensaver::viewChanged(QGraphicsWidget *widget)

    This signal should be emitted when the Screensaver needs to change its visualization
    after a state change.
    \param widget The graphics widget container holding the current visualization.
 */

/*!
    Returns the state that the Screensaver is currently in.
    \return The current state.
 */
ScreensaverState Screensaver::currentState()
{
    return m_d->currentState();
}

/*!
    Initializes the Screensaver
 */
void Screensaver::initialize()
{
    m_d->initialize();
}

/*!
    Called when the application is in foreground.
 */
void Screensaver::foreground()
{
    m_d->foreground();
}

/*!
    Called when the application gains limited foreground as with OLED display cases.
 */
void Screensaver::partialForeground()
{
    m_d->partialForeground();
}

/*!
    Called when the application goes to background.
 */
void Screensaver::background()
{
    m_d->background();
}

/*!
    Called when device enters power save mode.
 */
void Screensaver::powerSave()
{
    m_d->powerSave();
}

/*!
    Stops Screensaver's processing.
 */
void Screensaver::close()
{
    m_d->close();
}

/*!
    \fn virtual bool Screensaver::onForeground() = 0

    After a call the Screensaver should emit the foreground state visualization.
    Returns true if the operation secceeded, otherwise false - in this case 
    the faulted() signal will be emitted by the base class.
    \return Indicates if the operation succeeded.
 */

/*!
    \fn virtual bool Screensaver::onPartialForeground() = 0

    After a call the Screensaver should emit the partial foreground state visualization.
    This is valid for OLED display cases when the screensaver is displayed at all times
    with limited functionality.
    Returns true if the operation secceeded, otherwise false - in this case 
    the faulted() signal will be emitted by the base class.
    \return Indicates if the operation succeeded.
 */

/*!
    \fn virtual bool Screensaver::onBackground() = 0

    After a call the Screensaver should limit its processing.
    Returns true if the operation secceeded, otherwise false - in this case 
    the faulted() signal will be emitted by the base class.
    \return Indicates if the operation succeeded.
 */

/*!
    \fn virtual bool Screensaver::onPowerSave() = 0

    After a call the Screensaver should limit its processing as much as possible.
    Returns true if the operation secceeded, otherwise false - in this case 
    the faulted() signal will be emitted by the base class.
    \return Indicates if the operation succeeded.
 */

/*!
    After a call it should initialize the Screensaver.
    Returns true if the operation secceeded, otherwise false - in this case 
    the faulted() signal will be emitted by the base class.
    The default implementation does nothing and always returns true.
    \return Indicates if the operation succeeded.
 */
bool Screensaver::onInitialize()
{
    return true;
}

/*!
    After a call it should close the Screensaver.
    The Screensaver should also free all resources.
    Returns true if the operation secceeded, otherwise false - in this case 
    the faulted() signal will be emitted by the base class.
    The default implementation does nothing and always returns true.
    \return Indicates if the operation succeeded.
 */
bool Screensaver::onClose()
{
    return true;
}
