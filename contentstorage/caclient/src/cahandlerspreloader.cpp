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
 * Description: cahandlerspreloader.cpp
 *
 */

#include <QTimer>

#include <caentry.h>

#include "caclient_defines.h"
#include "cahandlerproxy.h"
#include "cahandlerspreloader.h"

/*!
 Constructor.
 \param pointer to a parent object.
 */
CaHandlersPreloader::CaHandlersPreloader(
    CaHandlerProxy *handlerProxy,
    QObject *parent):
    QObject(parent),
    mTimer(NULL),
    mHandlerProxy(handlerProxy)
{
    mHandlersToLoad <<
        URL_ENTRY_TYPE_NAME <<
        TEMPLATED_APPLICATION_ENTRY_TYPE_NAME <<
        APPLICATION_ENTRY_TYPE_NAME;

    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(preloadHandlersWhenIdle()));
    mTimer->start(0); // NOTE: zero for idle
}

/*!
 Destructor.
 */
CaHandlersPreloader::~CaHandlersPreloader()
{
    mTimer->stop();
    delete mTimer;
}

/*!
 Preload a single handler from a list during the idle time.
 */
void CaHandlersPreloader::preloadHandlersWhenIdle()
{
    if (!mHandlersToLoad.isEmpty()) {
        mHandlerProxy->getHandler(mHandlersToLoad.takeLast());
    } else {
    	mTimer->stop();
	}
}

