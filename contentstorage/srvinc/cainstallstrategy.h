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

#ifndef CAINSTALLSTRATEGY_H
#define CAINSTALLSTRATEGY_H

#include "cainstallnotifier.h"
#include <e32base.h>
#include <SWInstallerInternalPSKeys.h>

/**
 * Strategy interface.
 *
 * @since S60 v5.0
 */
class CNotifierStrategy: public CBase
    {
public:

    /**
     * Notifies listener with changes in property.
     * @param aProperty to read values from.
     * @param aListener listener to notify.
     */
    virtual void NotifyListenerL( RProperty& aProperty,
            MCaInstallListener& aListener ) = 0;

    };

/**
 *  CA Install strategy.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CCaSwiInstallStrategy ): public CNotifierStrategy
    {
public:

    /**
     * Creates an instance of CCaInstallStrategy.
     * @param aProperty property to handle.
     */
    static CCaSwiInstallStrategy* NewL( RProperty& aProperty );

    /**
     * Notifies listener with changes in property.
     * @param aProperty to read values from.
     * @param aListener listener to notify.
     */
    void NotifyListenerL( RProperty& aProperty,
            MCaInstallListener& aListener );

private:

    /**
     * Constructor.
     * @param aProperty property to handle.
     */
    CCaSwiInstallStrategy( RProperty& aProperty );

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

    };

/**
 *  CA Java install strategy.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CCaJavaInstallStrategy ): public CNotifierStrategy
    {
public:

    /**
     * Creates an instance of CCaInstallStrategy.
     * @param aProperty property to handle.
     */
    static CCaJavaInstallStrategy* NewL( RProperty& aProperty );

    /**
     * Notifies listener with changes in property.
     * @param aProperty to read values from.
     * @param aListener listener to notify.
     */
    void NotifyListenerL( RProperty& aProperty,
            MCaInstallListener& aListener );

private:

    /**
     * Constructor.
     * @param aProperty property to handle.
     */
    CCaJavaInstallStrategy( RProperty& aProperty );

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

    };

/**
 *  CA Unninstal with USIF strategy.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CCaUsifUninstallStrategy ): public CNotifierStrategy
    {
public:

    /**
     * Creates an instance of CCaInstallStrategy.
     * @param aProperty property to handle.
     */
    static CCaUsifUninstallStrategy* NewL( RProperty& aProperty );

    /**
     * Notifies listener with changes in property.
     * @param aProperty to read values from.
     * @param aListener listener to notify.
     */
    void NotifyListenerL( RProperty& aProperty,
            MCaInstallListener& aListener );

private:

    /**
     * Constructor.
     * @param aProperty property to handle.
     */
    CCaUsifUninstallStrategy( RProperty& aProperty );

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

private:

    /**
     * Contains uninstallation status of application 
     * that was started to uninstall.
     */
    TInt iUninstallStatus;

    };

#endif /* CAINSTALLSTRATEGY_H */

// End of File

