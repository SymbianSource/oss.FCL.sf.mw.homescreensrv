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
#include <SWinstApi.h>

/**
 * Uninstall CMenuOperation
 * Encapsulates the functionality of uninstalling an application.
 */
NONSHARABLE_CLASS( CCaUninstallOperation ): public CActive
    {

public:
    // construction

    virtual ~CCaUninstallOperation();

    static CCaUninstallOperation* NewL( CCaInnerEntry &aEntry,
            TInt aPriority = CActive::EPriorityStandard );

private:
    // construction

    CCaUninstallOperation( CCaInnerEntry &aEntry, TInt aPriority );

    void ConstructL( CCaInnerEntry &aEntry );

    void AppInfoL( const TUid& aAppUid, TPtrC8& aMimeType,
            TUid& aPackageUid );

    TBool GetInstallPkgUidL( const TDesC& aAppFullName, TUid& aPackageUid );

private:
    // from CActive

    void RunL();
    void DoCancel();
    TInt RunError( TInt aError );

private:

    CCaInnerEntry& iEntry;
    SwiUI::RSWInstLauncher iUninstaller;
    TInt iId;
    };

#endif // C_CAUNINSTALLOPERATION_H
