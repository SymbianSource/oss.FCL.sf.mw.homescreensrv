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
#ifndef CA_HANDLER_H
#define CA_HANDLER_H

#include <QObject>
#include <cadefs.h>
#include <caclient_global.h>

class CCaInnerEntry;
class QString;

class CaHandler
{
public:
    virtual ~CaHandler() {}
    virtual int execute(CCaInnerEntry &innerEntry,
                        const QString &commandName) = 0;
};

Q_DECLARE_INTERFACE(CaHandler, "com.nokia.homescreen.ICommandHandler")

#endif
