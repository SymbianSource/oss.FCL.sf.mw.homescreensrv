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
 * Description:  Default implementation of the tapp handler
 *
 */

#ifndef CATAPPHANDLER_H
#define CATAPPHANDLER_H

#include <QObject>
#include "cahandler.h"
#include "cadefs.h"

class CaEntry;
class XQApplicationManager;

const QString hsitemLaunchUri("item:launchuri");

class CaTappHandler: public CaHandler
{
Q_OBJECT

public:

explicit    CaTappHandler(QObject *parent = 0);
    ~CaTappHandler();

    Q_INVOKABLE
    int execute(const CaEntry& entry, const QString& command);

private:
    XQApplicationManager* mAiwMgr;

};

#endif
