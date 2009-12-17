/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
#ifndef __MENUUNINSTALLOPERATION_H__
#define __MENUUNINSTALLOPERATION_H__

#include "mcsmenuoperation.h"

#include <e32base.h>
#include <SWInstApi.h>

class RMenu;
class CMenuItem;

/**
* Uninstall CMenuOperation
* Encapsulates the functionality of uninstalling an application.
*/
NONSHARABLE_CLASS( CMenuUninstallOperation ) : public CMenuOperation
    {

public:     // construction

    virtual ~CMenuUninstallOperation();

    static CMenuUninstallOperation* NewL(
        RMenu &aMenu,
        TInt aPriority,
        TRequestStatus &aObserverStatus,
        CMenuItem& aItem );

private:    // construction

    CMenuUninstallOperation(
        RMenu &aMenu,
        TInt aPriority,
        TRequestStatus &aObserverStatus );

    void ConstructL( CMenuItem& aItem );

    void AppInfoL( const TUid& aAppUid, TPtrC8& aMimeType, TUid& aPackageUid );

    TBool GetInstallPkgUidL( const TDesC& aAppFullName, TUid& aPackageUid );
    void GetJavaSuitUidL( const TUid& aAppUid, TUid& aPackageUid );
    TBool IsWidgetL( const TUid& aAppUid );

private:    // from CActive

    void RunL();
    void DoCancel();
    TInt RunError( TInt aError );

private:

    enum TUninstState
        {
        EIdle,
        EUninstalling,
        ERemoving
        }                   iState;
    SwiUI::RSWInstLauncher  iUninstaller;
    CMenuOperation*         iRemoveOperation;
    TInt                    iId;
    };

#endif // __MENUUNINSTALLOPERATION_H__