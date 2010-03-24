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

#ifndef CA_S60_HANDLER_ADAPTER_H
#define CA_S60_HANDLER_ADAPTER_H

#include <e32des8.h>
#include <cadefs.h>
#include <caentry.h>
#include <QScopedPointer>
#include <QString>
#include <utf.h>
#include <XQConversions>

#include "cainnerentry.h"
#include "cahandler.h"
#include "caobjectadapter.h"

template <typename Plugin>
class CaS60HandlerAdapter: public CaHandler
{
public:
    int execute(const CaEntry &entry, const QString &commandName) {

        QScopedPointer<HBufC8> commandNameDesc8(XQConversions::qStringToS60Desc8(commandName));
        QScopedPointer<CCaInnerEntry> innerEntry(NULL);

        TRAPD(result,
             innerEntry.reset(CCaInnerEntry::NewL());
             CaObjectAdapter::convertL(entry, *innerEntry);

             static QScopedPointer<Plugin> plugin(Plugin::NewL());
             plugin->HandleCommandL(*innerEntry, commandNameDesc8->Des());
            );

        return result;
    }
};


#endif
