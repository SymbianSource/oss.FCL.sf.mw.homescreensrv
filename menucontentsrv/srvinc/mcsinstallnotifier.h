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
* Description:  This class implements MMcsGetListCreatorInterface. It builds tree
*   output list for getlist operation
*
*
*/

#ifndef MCSINSTALLNOTIFIER_H_
#define MCSINSTALLNOTIFIER_H_

#include <sacls.h>
#include <e32base.h>
#include <e32property.h>

class CMcsNotifierStrategy;
class MMcsInstallListener;

/**
 *  MCS Install notifier.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMcsInstallNotifier ) : public CActive
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
        };

    /**
     * Creates an instance of CCaInstallNotifier.
     * @param aNotifier Reference to notifier interface.
     * @param aNotificationType Notification type.
     */
	static CMcsInstallNotifier* NewL( MMcsInstallListener& aListener,
			TNotificationType aNotificationType );

	/**
	 * Destructor.
	 */
	virtual ~CMcsInstallNotifier();

private:

    /**
     * Constructor.
     * @param aListener Reference to listener interface.
     */
	CMcsInstallNotifier( MMcsInstallListener& aListener );

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
     * RProperty - own.
     */
    RProperty iProperty;

    /**
     * MMcsInstallListener - own.
     */
    MMcsInstallListener& iListener;

    /*
     * Notification strategy - own.
     */
    CMcsNotifierStrategy* iNotifierStrategy;

    };

/**
 * Interface for updating after installer events.
 *
 * @since S60 v5.0
 */
class MMcsInstallListener
    {
public:

    /**
     * Pure virtual method.
     * @param aUid uid of installed application.
     * @param aOperation operation type, see TSASwisOperation.
     */
    virtual void HandleInstallNotifyL( TUid aUid,
    		CMcsInstallNotifier::TNotificationType aNotificationType ) = 0;
    };


#endif /* MCSINSTALLNOTIFIER_H_ */
