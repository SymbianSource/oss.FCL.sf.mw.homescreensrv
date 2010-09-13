/*
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  ?Description
 *
 */

#ifndef CAURLHANDLER_H
#define CAURLHANDLER_H

#include "cahandler.h"

class QString;

class CaUrlHandler: public QObject, public CaHandler
{
    Q_OBJECT
    Q_INTERFACES(CaHandler)

public:

    explicit CaUrlHandler(QObject *parent = 0);

    virtual ~CaUrlHandler();

    int execute(const CaEntry &entry, const QString &command, 
            QObject* receiver = NULL, const char* member = NULL);

};

#endif // CAURLHANDLER_H
