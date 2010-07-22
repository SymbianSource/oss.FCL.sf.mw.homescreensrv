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

#include "applicationlauncher.h"
#include "applicationlauncher_p.h"

ApplicationLauncher::ApplicationLauncher() : d_ptr(new ApplicationLauncherPrivate())
{
}

ApplicationLauncher::~ApplicationLauncher()
{
    delete d_ptr;
}

bool ApplicationLauncher::isRunning(int applicationId)
{
    return d_ptr->isRunning(applicationId);
}

void ApplicationLauncher::startApplication(int applicationId, const QUrl &uri)
{
    d_ptr->startApplication(applicationId, uri);
}

void ApplicationLauncher::bringToForeground(int applicationId)
{
    d_ptr->bringToForeground(applicationId);
}
