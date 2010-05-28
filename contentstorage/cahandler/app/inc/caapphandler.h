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

/**
 *  Command handler for application entries.
 *
 *  @lib caclient.lib
 */
class CaAppHandler: public QObject, public CaHandler
{
    Q_OBJECT
    Q_INTERFACES(CaHandler)

public:

    explicit CaAppHandler(QObject *parent = 0);

    /**
     * Destructor.
     */
    virtual ~CaAppHandler();

public:

    int execute(const CaEntry &entry, const QString &command);

private:

    /**
     * Launches application
     * @param aUid UID of the application to launch
     * @param aParam command parameters
     * @param aViewId id of the view the application is to start in
     */
    void launchApplicationL(const TUid aUid, TInt aViewId);

    /**
     * Closes application
     * @param aEntry the entry represeting application to close
     */
    int closeApplication(const EntryFlags &flags, TInt windowGroupId);

    /**
     * Uninstall application 
     * @param aEntry the entry represeting application to uninstall
     */
    int handleRemove(const EntryFlags &flags,
        const QString &typeName,
        const QString &componentId);
    
    /**
     * Start uninstall operation via usif 
     * @param aComponentId component id
     */
    void startUsifUninstallL(TInt aComponentId);
    
private:
    CEikonEnv *iEikEnv;
    CCaUsifUninstallOperation *iUsifUninstallOperation;
};

#endif // CAAPPHANDLER_H
