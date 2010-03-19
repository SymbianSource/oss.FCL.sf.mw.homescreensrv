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
// CCaWidgetScannerInstallNotifier::CCaWidgetScannerInstallNotifier
// C++ default constructor
// -----------------------------------------------------------------------------
//
CCaWidgetScannerInstallNotifier::CCaWidgetScannerInstallNotifier(
    CCaWidgetScannerPlugin* aCallback, TUid aCategory, TUint aKey ) :
    CActive(EPriorityNormal)
    {
    CActiveScheduler::Add( this );

    iCallback = aCallback;
    iCategory = aCategory;
    iKey = aKey;
    // Prepare automatically
    iProperty.Attach( iCategory, iKey );
    SetActive( );
    iProperty.Subscribe( iStatus );
    }

// -----------------------------------------------------------------------------
// CCaWidgetScannerInstallNotifier::ConstructL
// S2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CCaWidgetScannerInstallNotifier::ConstructL()
    {
    iHackFlag = EFalse;
    }

// ---------------------------------------------------------------------------
// CCaWidgetScannerInstallNotifier::NewL
// ---------------------------------------------------------------------------
//
CCaWidgetScannerInstallNotifier* CCaWidgetScannerInstallNotifier::NewL(
    CCaWidgetScannerPlugin* aCallback, TUid aCategory, TUint aKey )
    {
    CCaWidgetScannerInstallNotifier* self = 
            new (ELeave) CCaWidgetScannerInstallNotifier( aCallback,
            aCategory, aKey );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CCaWidgetScannerInstallNotifier::~CCaWidgetScannerInstallNotifier
// ---------------------------------------------------------------------------
//
CCaWidgetScannerInstallNotifier::~CCaWidgetScannerInstallNotifier()
    {
    Cancel( );
    iProperty.Close( );
    }

// ---------------------------------------------------------------------------
// CCaWidgetScannerInstallNotifier::DoCancel
// ---------------------------------------------------------------------------
//
void CCaWidgetScannerInstallNotifier::DoCancel()
    {
    iProperty.Cancel( );
    }

// ---------------------------------------------------------------------------
// CCaWidgetScannerInstallNotifier::RunError
// ---------------------------------------------------------------------------
//
TInt CCaWidgetScannerInstallNotifier::RunError( TInt /*aError*/)
    {
    // No need to do anything      
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCaWidgetScannerInstallNotifier::RunL
// ---------------------------------------------------------------------------
//
void CCaWidgetScannerInstallNotifier::RunL()
    {
    // Re-issue request before notifying
    SetActive( );
    iProperty.Subscribe( iStatus );

    TInt status;
    User::LeaveIfError( iProperty.Get( KUidSystemCategory,
        KSAUidSoftwareInstallKeyValue, status ) );
    
    if( iHackFlag )
        {
        iHackFlag = EFalse;
        iCallback->SynchronizeL();
        }
    else if ( (status & EInstOpInstall )||(status & EInstOpUninstall ) )
        {
        iHackFlag = ETrue;
        }
    }
// End of File
