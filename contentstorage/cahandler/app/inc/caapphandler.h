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

#ifndef CAAPPHANDLER_H
#define CAAPPHANDLER_H

#include <QObject>

#include "cahandler.h"

class CEikonEnv;
class CCaUsifUninstallOperation;
class CaEntry;

class CaAppHandler: public QObject, public CaHandler
{
    Q_OBJECT
    Q_INTERFACES(CaHandler)

public:

    explicit CaAppHandler(QObject *parent = 0);

    virtual ~CaAppHandler();

public:

    int execute(const CaEntry &entry, const QString &command);

private:

    void launchApplicationL(const TUid uid, TInt viewId);

    int closeApplication(const EntryFlags &flags, TInt windowGroupId);

    int handleRemove(const EntryFlags &flags,
        const QString &typeName,
        const QString &componentId);
    
    void startUsifUninstallL(TInt componentId);
    
private:
    CEikonEnv *iEikEnv;
    CCaUsifUninstallOperation *iUsifUninstallOperation;
};

#endif // CAAPPHANDLER_H
