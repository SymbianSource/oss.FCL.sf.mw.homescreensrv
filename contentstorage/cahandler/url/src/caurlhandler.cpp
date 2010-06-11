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

#include <QDesktopServices>
#include <QUrl>

#include <caentry.h>

#include "caurlhandler.h"

static const QString caAttrUrl("url");

CaUrlHandler::CaUrlHandler(QObject *parent)
{
    Q_UNUSED(parent);
}

CaUrlHandler::~CaUrlHandler()
{
}

int CaUrlHandler::execute(const CaEntry &entry, const QString &command)
{
    int err(KErrGeneral);
    if (command == caCmdOpen) {
        QString attribute = entry.attribute(caAttrUrl);
        if (!attribute.isNull() &&
            QDesktopServices::openUrl(QUrl(attribute))) {
            err = KErrNone;
        }
    }
    return err;
}
