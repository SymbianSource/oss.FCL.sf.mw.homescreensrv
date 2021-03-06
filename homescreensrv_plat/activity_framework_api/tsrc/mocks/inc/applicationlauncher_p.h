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
#ifndef APPLICATIONLAUNCHER_P_H
#define APPLICATIONLAUNCHER_P_H

#include <QVariantHash>
#include <QUrl>

class ApplicationLauncherPrivate
{

public:
    ApplicationLauncherPrivate();
    ~ApplicationLauncherPrivate();

public:
    static ApplicationLauncherPrivate *instance;
    static bool isRunningReturnValue;

public:
    bool isRunning(int applicationId);
    void startApplication(int applicationId, const QUrl &uri);
    void bringToForeground(int applicationId);

public:
    QList<QVariantHash> isRunningCalls;
    QList<QVariantHash> startApplicationCalls;
    QList<QVariantHash> bringToForegroundCalls;

};

#endif // APPLICATIONLAUNCHER_P_H
