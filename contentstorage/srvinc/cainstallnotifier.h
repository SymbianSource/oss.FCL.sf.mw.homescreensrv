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
    virtual void HandleInstallNotifyL( TInt aEvent ) = 0;
    };

/**
 *  CA Install notifier.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CCaInstallNotifier ): public CActive
    {

public:

    /**
     * Enum defining the operation type.
     */
    enum TInstOp
        {
        EInstOpNone = 0x00000000,
        EInstOpInstall = 0x00000001,
        EInstOpUninstall = 0x00000002,
        EInstOpRestore = 0x00000004
        };

    /**
     * Enum defining the operation status.
     */
    enum TInstOpStatus
        {
        EInstOpStatusNone = 0x00000000,
        EInstOpStatusSuccess = 0x00000100,
        EInstOpStatusAborted = 0x00000200
        };

    /**
     * Enum defining notification type.
     */
    enum TNotificationType
        {
        ENoNotification = 0x00000000,
        ESisInstallNotification = KUidSwiLatestInstallation, ///< installation notification
        };

    /**
     * Creates an instance of CCaInstallNotifier implementation.
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
    CCaInstallNotifier( MCaInstallListener& aListener,
            TNotificationType aNotificationType );

    /**
     * Symbian 2nd phase constructor.
     * @param aNotificationType Notification type.
     */
    void ConstructL();

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
     * Notification type.
     */
    TInt iRPropertyKey;

    };

#endif /* CAINSTALLNOTIFIER_H */

// End of File

