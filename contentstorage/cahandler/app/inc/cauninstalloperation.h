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

#ifndef C_CAUNINSTALLOPERATION_H
#define C_CAUNINSTALLOPERATION_H

#include <e32base.h>
#include <SWInstApi.h>

/**
 * Uninstall CMenuOperation
 * Encapsulates the functionality of uninstalling an application.
 *
 *  @lib caclient.lib
 */
NONSHARABLE_CLASS(CCaUninstallOperation): public CActive {

public:
    /**
    * Destructor
    */
    virtual ~CCaUninstallOperation();
    
    /**
     * Two-phased constructor. Leaves on failure.
     * Allocates memory for and constructs an uninstaller object. After that
     * starts asynchronous uninnstallation of the requested entry
     * @param aEntry entry to be uninstalled
     * @param aPriority priority of the active scheduler responsible for handling
     * asynchronous installation operation
     * @return The constructed object.
     */
    static CCaUninstallOperation *NewL(CCaInnerEntry &aEntry,
    TInt aPriority = CActive::EPriorityStandard);

private:
    // construction
    /**
     * Constructor.
     * Starts active scheduler.
     * @param aEntry entry to be uninstalle
     * @param aPriority priority of the active scheduler responsible for handling
     * asynchronous installation operation
     */
    CCaUninstallOperation(CCaInnerEntry &aEntry, TInt aPriority);
    
    /**
     * Initializes the object and starts asynchronous uninstallation process.
     * @param aEntry entry to be uninstalle
     * asynchronous installation operation
     */
    void ConstructL(CCaInnerEntry &aEntry);
    
    /**
     * Retrieves package information for requested application uid
     * @param aAppUid application uid
       @param aMimeType placeholder for resulting mime type of the package
     * @param aPackageUid placeholder for resulting package uid of the application 
     */
    void AppInfoL(const TUid &aAppUid, TPtrC8 &aMimeType, TUid &aPackageUid);
    
    /**
     * Retrieves package uid 
     * @param aAppFullName application full name
       @param aPackageUid placeholder for resulting package uid of the application
       @return true if package uid was found
     */
    TBool GetInstallPkgUidL(const TDesC &aAppFullName, TUid &aPackageUid);

private:
    // from CActive

    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:

    CCaInnerEntry &iEntry;
    SwiUI::RSWInstLauncher iUninstaller;
    TInt iId;
};

#endif // C_CAUNINSTALLOPERATION_H
