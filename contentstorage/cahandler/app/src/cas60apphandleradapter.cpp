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

#include <XQConversions>
#include <utf.h>
#include <QScopedPointer>

#include "cas60apphandleradapter.h"
#include "caapphandler.h"
#include "cainnerentry.h"

int CaS60AppHandlerAdapter::execute(CCaInnerEntry &innerEntry, const QString &commandName)
{
    QScopedPointer<HBufC8> commandNameDesc8(XQConversions::qStringToS60Desc8(commandName));
    TRAPD(result,
        static QScopedPointer<CCaAppHandler> plugin(CCaAppHandler::NewL());
        plugin->HandleCommandL(innerEntry, commandNameDesc8->Des());
        );
    return result;
}
