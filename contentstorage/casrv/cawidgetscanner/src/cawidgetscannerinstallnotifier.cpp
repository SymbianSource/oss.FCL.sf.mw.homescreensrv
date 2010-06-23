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

#include <sacls.h>
#include "cawidgetscannerinstallnotifier.h"
#include "cawidgetscannerplugin.h"

// ============================ MEMBER FUNCTIONS =============================

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CCaWidgetScannerInstallNotifier::CCaWidgetScannerInstallNotifier(
        CCaWidgetScannerPlugin* aCallback, TUid aCategory, TUint aKey ) :
    CActive( EPriorityNormal )
    {
    CActiveScheduler::Add( this );

    iCallback = aCallback;
    iCategory = aCategory;
    iKey = aKey;
    iProperty.Attach( iCategory, iKey );
    SetActive();
    iProperty.Subscribe( iStatus );
    iHackFlag = EFalse;
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CCaWidgetScannerInstallNotifier::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaWidgetScannerInstallNotifier* CCaWidgetScannerInstallNotifier::NewL(
        CCaWidgetScannerPlugin* aCallback, TUid aCategory, TUint aKey )
    {
    CCaWidgetScannerInstallNotifier* self =
            new ( ELeave ) CCaWidgetScannerInstallNotifier( aCallback,
                    aCategory, aKey );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaWidgetScannerInstallNotifier::~CCaWidgetScannerInstallNotifier()
    {
    Cancel();
    iProperty.Close();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaWidgetScannerInstallNotifier::DoCancel()
    {
    iProperty.Cancel();
    }

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT (error is ignored)
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CCaWidgetScannerInstallNotifier::RunError( TInt /*aError*/)
    {
    // No need to do anything      
    return KErrNone;
    }
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaWidgetScannerInstallNotifier::RunL()
    {
    // Re-issue request before notifying
    SetActive();
    iProperty.Subscribe( iStatus );

    TInt status;
    User::LeaveIfError( iProperty.Get( KUidSystemCategory,
            KSAUidSoftwareInstallKeyValue, status ) );

    if ( iHackFlag )
        {
        iHackFlag = EFalse;
        iCallback->SynchronizeL();
        }
    else if ( ( status & EInstOpInstall ) || ( status & EInstOpUninstall ) )
        {
        iHackFlag = ETrue;
        }
    }
// End of File
