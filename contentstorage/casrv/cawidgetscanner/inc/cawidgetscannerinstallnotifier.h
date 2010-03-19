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
* Description:  Sis package installation event listener
 *
*/


#ifndef C_CAWIDGETSCANNERINSTALLNOTIFIER_H
#define C_CAWIDGETSCANNERINSTALLNOTIFIER_H

#include <e32base.h>
#include <e32property.h>

class CCaWidgetScannerPlugin;

/**
 *  Central Repository notifier.
 *
 *  @since S60 S60 v5.x
 */
class CCaWidgetScannerInstallNotifier : public CActive
    {
    /**
     * Enum defining the purpouse of the installation event.
     */
    enum TInstOp
        {
        EInstOpNone = 0x00000000,
        EInstOpInstall = 0x00000001,
        EInstOpUninstall = 0x00000002,
        EInstOpRestore = 0x00000004
        };

    /**
     * Enum defining the exit status of the installation event.
     */
    enum TInstOpStatus
        {
        EInstOpStatusNone = 0x00000000,
        EInstOpStatusSuccess = 0x00000100,
        EInstOpStatusAborted = 0x00000200
        };

public:

    /**
     * Creates an instance of CCaWidetScannerInstallNotifier implementation.
     * @param aCallBack Reference to notifier interface.
     * @param aCategory Package uid.
     * @param aKey Key for central repository.
     */
    static CCaWidgetScannerInstallNotifier* NewL(
        CCaWidgetScannerPlugin* aCallback, TUid aCategory, TUint aKey );

    /**
     * Destructor.
     */
    virtual ~CCaWidgetScannerInstallNotifier ();

private:
    /**
     * Constructor.
     * @param aCallBack Reference to notifier interface.
     * @param aCategory Package uid.
     * @param aKey Key for central repository.
     */
    CCaWidgetScannerInstallNotifier( CCaWidgetScannerPlugin* aCallback,
        TUid aCategory, TUint aKey );

    /**
     * Symbian 2nd phase constructor.
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
     * User side interface to Publish & Subscribe.
     * Own.
     */
    RProperty iProperty;

    /**
     * Interface for notifying changes in SWI.
     * Not own.
     */
    CCaWidgetScannerPlugin* iCallback;

    /**
     * Category uid.
     * Own.
     */
    TUid iCategory;

    /**
     * Key identifier.
     * Own.
     */
    TUint iKey;

    /**
     * flag provides workaround for a problem with missing notifications after install
     *  
     */
    TBool iHackFlag; // TODO find another solution

    };

#endif // C_CAWIDGETSCANNERINSTALLNOTIFIER_H
// End of File
