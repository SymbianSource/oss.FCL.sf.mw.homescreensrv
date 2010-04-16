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
#ifndef APPLICATIONLAUNCHER_H
#define APPLICATIONLAUNCHER_H

class ApplicationLauncherPrivate;

#include <QString>

class ApplicationLauncher
{

public:
    ApplicationLauncher();
    ~ApplicationLauncher();

public:
    bool isRunning(int applicationId);
    void startApplication(int applicationId, const QString &activityId);
    void bringToForeground(int applicationId);

private:
    ApplicationLauncherPrivate *d_ptr;

};

#endif // APPLICATIONLAUNCHER_H
