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

#ifndef MCSINSTALLSTRATEGY_H
#define MCSINSTALLSTRATEGY_H

#include "mcsinstallnotifier.h"
#include <e32base.h>

namespace Swi
    {
    class RSisRegistryEntry;
    }

/**
 * Strategy interface.
 *
 * @since S60 v5.0
 */
class CMcsNotifierStrategy: public CBase
    {
public:

    /**
     * Notifies listener with changes in property.
     */
    virtual void NotifyListenerL() = 0;

protected:

    /**
     * Constructor.
     * @param aProperty to read values from.
     * @param aListener listener to notify.
     */
    CMcsNotifierStrategy(
            RProperty& aProperty,
            MMcsInstallListener& aListener );

protected:

    /**
     * Property. Not own.
     */
    RProperty& iProperty;

    /**
     * Listener. Not own.
     */
    MMcsInstallListener& iListener;

    };

/**
 *  Swi Install strategy.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMcsSwiInstallStrategy ): public CMcsNotifierStrategy
    {
public:

    /**
     * Creates an instance of CCaInstallStrategy.
     * @param aProperty to read values from.
     * @param aListener listener to notify.
     */
    static CMcsSwiInstallStrategy* NewL(
            RProperty& aProperty,
            MMcsInstallListener& aListener );

    /**
     * Notifies listener with changes in property.
     */
    void NotifyListenerL();

private:

    /**
     * Constructor.
     * @param aProperty to read values from.
     * @param aListener listener to notify.
     */
    CMcsSwiInstallStrategy(
            RProperty& aProperty,
            MMcsInstallListener& aListener );

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Handle Install Event.
     * @param aUid installed/uninstalled app uid.
     */
    void HandleInstallNotifyL( TInt aUid );

    /**
     * Notifies storage abaut change for aPackageEntry related app.
     * @param aPackageEntry application package entry.
     */
    void NotifyL( Swi::RSisRegistryEntry & aPackageEntry );

    };

/**
 *  Mcs Java install strategy.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMcsJavaInstallStrategy ): public CMcsNotifierStrategy
    {
public:

    /**
     * Creates an instance of CCaInstallStrategy.
     * @param aProperty to read values from.
     * @param aListener listener to notify.
     */
    static CMcsJavaInstallStrategy* NewL(
            RProperty& aProperty,
            MMcsInstallListener& aListener );

    /**
     * Notifies listener with changes in property.
     */
    void NotifyListenerL();

private:

    /**
     * Constructor.
     * @param aProperty to read values from.
     * @param aListener listener to notify.
     */
    CMcsJavaInstallStrategy(
            RProperty& aProperty,
            MMcsInstallListener& aListener );

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Handle Install Event.
     * @param aUid installed/uninstalled app uid.
     */
    void HandleInstallNotifyL( TUid aPackageUid );

    };

#endif /* MCSINSTALLSTRATEGY_H */
// End of File
