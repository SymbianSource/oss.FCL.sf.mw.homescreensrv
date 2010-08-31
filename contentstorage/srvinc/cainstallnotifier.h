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

#ifndef CAINSTALLNOTIFIER_H
#define CAINSTALLNOTIFIER_H

#include <sacls.h>
#include <e32base.h>
#include <e32property.h>
#include "castorage_global.h"

class CNotifierStrategy;

/**
 * Interface for updating after installer events.
 *
 * @since S60 v5.0
 */
class MCaInstallListener
    {

public:

    /**
     * Pure virtual method.
     * @param aEvent event type.
     */
    virtual void HandleInstallNotifyL(TInt aEvent) = 0;
    };

/**
 *  CA Install notifier.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CCaInstallNotifier ) : public CActive
    {

public:

    /**
     * Enum defining notification type.
     */
    enum TNotificationType
        {
        ENoNotification,            ///< No notification.
        ESisInstallNotification,    ///< System installation notification.
        EJavaInstallNotification,   ///< Java instalation and uninstallation notification.
        EUsifUninstallNotification, ///< Usif's uninstalation notification.
        };

    /**
     * Creates an instance of CCaInstallNotifier.
     * @param aNotifier Reference to notifier interface.
     * @param aNotificationType Notification type.
     */
IMPORT_C static CCaInstallNotifier* NewL( MCaInstallListener& aListener,
            TNotificationType aNotificationType );

    /**
     * Destructor.
     */
    virtual ~CCaInstallNotifier();

private:

    /**
     * Constructor.
     * @param aNotifier Reference to notifier interface.
     */
    CCaInstallNotifier( MCaInstallListener& aListener );

    /**
     * Symbian 2nd phase constructor.
     * @param aNotificationType Notification type.
     */
    void ConstructL( TNotificationType aNotificationType );

    /**
     * From CActive.
     */
    void DoCancel();

    /**
     * From CActive.
     */
    void RunL();

    /**
     * From CActive.
     */
    TInt RunError( TInt aError );

private:

    /**
     * RProperty.
     * Own.
     */
    RProperty iProperty;

    /**
     * Interface for notifying changes in folder.
     * Not Own.
     */
    MCaInstallListener& iListener;

    /*
     * Notification strategy.
     */
    CNotifierStrategy* iNotifierStrategy;

    };

#endif /* CAINSTALLNOTIFIER_H */

// End of File

