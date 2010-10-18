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

#include "cauninstallnotifier.h"
#include "cauninstallnotifier_p.h"
#include "caprogresscanner.h"
#include "cauninstallobserver.h"
#include "casoftwareregistry.h"

/*!
 Constructor.
 \param notifierFilter descrbies entries to observe.
 */
CaUninstallNotifierPrivate::CaUninstallNotifierPrivate() :
    mUninstallObserver(NULL), mProgressScanner(NULL),
    mCaSoftwareRegistry(CaSoftwareRegistry::create()),
    m_q(NULL),
    mLastComponentId(0),
    mLastValueOfProgress(0)
{
    mUninstallObserver = new CaUninstallObserver();
    TRAP_IGNORE(mProgressScanner = CCaProgresScanner::NewL(mUninstallObserver));
}

/*!
 Destructor.
 */
CaUninstallNotifierPrivate::~CaUninstallNotifierPrivate()
{
    delete mProgressScanner;
    delete mUninstallObserver;
}

/*!
 Makes connections witch uninstall observer.
 */
void CaUninstallNotifierPrivate::makeConnect()
{
    connect(mUninstallObserver, SIGNAL(signalprogressChange(int, int)), 
            this, SLOT(progressChange(int, int)));
}

/*!
 Slot to cache and forward progress notification.
 */
void CaUninstallNotifierPrivate::progressChange(int componentId,int valueOfProgress){
    mLastComponentId = componentId;
    mLastValueOfProgress = valueOfProgress;
    emit m_q->progressChange(componentId, valueOfProgress);
}

/*!
 Returns last progress notification.
 */
QVariantMap CaUninstallNotifierPrivate::getLastNotification()
{
    QVariantMap lastNote;
    if(mLastValueOfProgress < 0){
    	mLastComponentId = 0;
    }
    lastNote.insert(uninstallNotifierComponentIdKey, mLastComponentId);
    lastNote.insert(uninstallNotifierValueOfProgressKey, mLastValueOfProgress);
    return lastNote;
}
