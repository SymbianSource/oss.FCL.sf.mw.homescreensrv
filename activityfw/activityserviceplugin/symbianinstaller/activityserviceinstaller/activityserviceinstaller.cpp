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

#include <qservicemanager.h>
#include <QCoreApplication>

QTM_USE_NAMESPACE

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QServiceManager manager;
    bool ok = manager.addService(":/activityserviceplugin.xml");
    if (!ok) {
        int err = manager.error();
        qFatal("%d ini", err);
    }
}
