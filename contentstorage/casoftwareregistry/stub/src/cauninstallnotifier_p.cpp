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
#include "cauninstallobserver.h"
#include "casoftwareregistry.h"

/*!
 Constructor.
 \param notifierFilter descrbies entries to observe.
 */
CaUninstallNotifierPrivate::CaUninstallNotifierPrivate() :
    mUninstallObserver(NULL),
    mCaSoftwareRegistry(CaSoftwareRegistry::create()),
    m_q(NULL)
{
    mUninstallObserver = new CaUninstallObserver();
}

/*!
 Destructor.
 */
CaUninstallNotifierPrivate::~CaUninstallNotifierPrivate()
{
    delete mUninstallObserver;
}

/*!
 Makes connections witch uninstall observer.
 */
void CaUninstallNotifierPrivate::makeConnect()
{
}

/*!
 Slot to cache and forward progress notification.
 */
void CaUninstallNotifierPrivate::progressChange(int componentId,int valueOfProgress){
}

/*!
 Returns last progress notification.
 */
QVariantMap CaUninstallNotifierPrivate::getLastNotification()
{
    QVariantMap lastNote;
    lastNote.insert(uninstallNotifierComponentIdKey, 0);
    lastNote.insert(uninstallNotifierValueOfProgressKey, 0);
    return lastNote;
}
