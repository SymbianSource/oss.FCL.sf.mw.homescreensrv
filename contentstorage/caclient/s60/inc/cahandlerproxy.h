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

#ifndef CA_HANDLER_PROXY_H
#define CA_HANDLER_PROXY_H

#include <e32def.h>
#include <QMap>
#include <QString>
#include <QSharedPointer>
#include <cadefs.h>

#include "cahandlerloader.h"

class CaEntry;

class CaHandlerProxy
{
public:

    ~CaHandlerProxy();

    explicit CaHandlerProxy(CaHandlerLoader *loader);

    TInt execute(const CaEntry &entry, const QString &commandName);

private:
    CaHandler *getHandler(const CaEntry &entry,
                          const QString &commandName);

    typedef QMap<QString, QSharedPointer<CaHandler> > ImplementationMap;
    typedef ImplementationMap::iterator ImplementationMapIterator;

    QSharedPointer<CaHandlerLoader> mLoader;
    ImplementationMap mImplementationMap;
};

#endif
