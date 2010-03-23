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
#include <xqconversions.h>

#include "cainnerentry.h"
#include "cahandler.h"
#include "caobjectadapter.h"

template <typename Plugin>
class CaS60HandlerAdapter: public CaHandler
{
public:
    int execute(const CaEntry &entry, const QString &commandName) {

        TPtrC16 commandNameDesC16(
            reinterpret_cast<const TUint16 *>(commandName.utf16()));

        QScopedPointer<HBufC8> commandNameDesc8;

        TRAPD(result,
              commandNameDesc8.reset(CnvUtfConverter::ConvertFromUnicodeToUtf7L(
                                         commandNameDesC16, false)));

        QScopedPointer<CCaInnerEntry> innerEntry(NULL);

        TRAP(result,
             innerEntry.reset(CCaInnerEntry::NewL());
             CaObjectAdapter::convertL(entry, *innerEntry);

             static QScopedPointer<Plugin> plugin(Plugin::NewL());
             plugin->HandleCommandL(*innerEntry, commandNameDesc8->Des());
            );

        return result;
    }
};

#endif
