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
#include "chfactorysettingsinstallnotifier.h"
#include "chfactorysettingsplugin.h"
#include "cpdebug.h"

// ============================ MEMBER FUNCTIONS =============================

// -----------------------------------------------------------------------------
// CCHFactorySettingsInstallNotifier::CCHFactorySettingsInstallNotifier
// C++ default constructor
// -----------------------------------------------------------------------------
//
CCHFactorySettingsInstallNotifier::CCHFactorySettingsInstallNotifier(
    CCHFactorySettings* aCallback, TUid aCategory, TUint aKey ) :
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
// CCHFactorySettingsInstallNotifier::ConstructL
// S2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CCHFactorySettingsInstallNotifier::ConstructL()
    {

    }

// ---------------------------------------------------------------------------
// CCHFactorySettingsInstallNotifier::NewL
// ---------------------------------------------------------------------------
//
CCHFactorySettingsInstallNotifier* CCHFactorySettingsInstallNotifier::NewL(
    CCHFactorySettings* aCallback, TUid aCategory, TUint aKey )
    {
    CCHFactorySettingsInstallNotifier* self = 
            new (ELeave) CCHFactorySettingsInstallNotifier( aCallback,
            aCategory, aKey );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// CCHFactorySettingsInstallNotifier::~CCHFactorySettingsInstallNotifier
// ---------------------------------------------------------------------------
//
CCHFactorySettingsInstallNotifier::~CCHFactorySettingsInstallNotifier()
    {
    Cancel( );
    iProperty.Close( );
    }

// ---------------------------------------------------------------------------
// CCHFactorySettingsInstallNotifier::DoCancel
// ---------------------------------------------------------------------------
//
void CCHFactorySettingsInstallNotifier::DoCancel()
    {
    iProperty.Cancel( );
    }

// ---------------------------------------------------------------------------
// CCHFactorySettingsInstallNotifier::RunError
// ---------------------------------------------------------------------------
//
TInt CCHFactorySettingsInstallNotifier::RunError( TInt /*aError*/)
    {

    // No need to do anything      
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CCHFactorySettingsInstallNotifier::RunL
// ---------------------------------------------------------------------------
//
void CCHFactorySettingsInstallNotifier::RunL()
    {
    CP_DEBUG(_L8("CCHFactorySettingsInstallNotifier::RunL" ));
    // Re-issue request before notifying
    SetActive( );
    iProperty.Subscribe( iStatus );

    TInt status;
    User::LeaveIfError( iProperty.Get( KUidSystemCategory,
        KSAUidSoftwareInstallKeyValue, status ) );

    if ( (status & EInstOpInstall )||(status & EInstOpUninstall ) )
        {
        iCallback->UpdateL( );
        }

    }
// End of File
