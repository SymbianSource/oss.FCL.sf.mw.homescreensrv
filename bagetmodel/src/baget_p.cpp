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
* Description: Private implementation of Baget.
*
*/

#include "baget_p.h"

/*!
    \class BagetPrivate
    \brief Private implementation of Baget.
 */

/*!
    Constructs a new BagetPrivate with \a bagetPublic.
 */
BagetPrivate::BagetPrivate(Baget * const bagetPublic) :
    m_q(bagetPublic), mState(BagetStateConstructed)
{
}

/*!
    Destructs the class.
 */
BagetPrivate::~BagetPrivate()
{
}

/*!
    Manages Baget's start.
 */
void BagetPrivate::start()
{
    switch (mState)
    {
    case BagetStateConstructed:
    case BagetStateStopped:
        break;

    default:
        return;
    }

    mState = BagetStateRunning;
    if (!m_q->onStart()) {
        emit m_q->faulted();
    }
}

/*!
    Manages Baget's stop.
 */
void BagetPrivate::stop()
{
    switch (mState)
    {
    case BagetStateRunning:
    case BagetStateSuspended:
        break;

    default:
        return;
    }

    mState = BagetStateStopped;
    if (!m_q->onStop()) {
        emit m_q->faulted();
    }
}

/*!
    Manages Baget's resume.
 */
void BagetPrivate::resume()
{
    switch (mState)
    {
    case BagetStateSuspended:
        break;

    default:
        return;
    }

    mState = BagetStateRunning;
    if (!m_q->onResume()) {
        emit m_q->faulted();
    }
}

/*!
    Manages Baget's suspend.
 */
void BagetPrivate::suspend()
{
    switch (mState)
    {
    case BagetStateRunning:
        break;

    default:
        return;
    }

    mState = BagetStateSuspended;
    if (!m_q->onSuspend()) {
        emit m_q->faulted();
    }
}

/*!
    Returns Baget's current state.
    \return The current state.
 */
BagetState BagetPrivate::currentState()
{
    return mState;
}
