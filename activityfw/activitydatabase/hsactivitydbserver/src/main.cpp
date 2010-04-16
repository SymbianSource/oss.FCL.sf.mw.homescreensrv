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
* Description: Application entry point
*
*/
#include <QtCore>
#include <QCoreApplication>

#include "hsactivityserver.h"
#include "hsactivitystorage.h"

#ifdef Q_OS_SYMBIAN
#include <coemain.h>
#include <coedef.h>
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_SYMBIAN
    CCoeEnv::Static()->RootWin().SetOrdinalPosition(0, ECoeWinPriorityNeverAtFront);
#endif

    QCoreApplication app(argc, argv);
    HsActivityStorage storage;
    HsActivityServer server(storage);
    int retVal(KErrGeneral);
    if (server.start()) {
        retVal = app.exec();
    }
    return retVal;
}
