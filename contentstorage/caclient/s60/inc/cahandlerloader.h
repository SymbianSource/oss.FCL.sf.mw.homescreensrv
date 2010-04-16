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

#ifndef CA_HANDLER_LOADER_H
#define CA_HANDLER_LOADER_H

#include <caclient_global.h>

class QString;
class CaHandler;

class CaHandlerLoader
{
public:
    virtual ~CaHandlerLoader();
    virtual CaHandler *loadHandler(const QString &entryTypeName,
                                   const QString &commandName) = 0;
};

#endif
