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
#include <e32property.h>
#include <SWInstallerInternalPSKeys.h>
#include <app/javadomainpskeys.h>
#include <swi/swispubsubdefs.h>

#include "cainstallstrategy.h"

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaSwiInstallStrategy* CCaSwiInstallStrategy::NewL( RProperty& aProperty )
    {
    CCaSwiInstallStrategy* self = new ( ELeave ) CCaSwiInstallStrategy(
            aProperty );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSwiInstallStrategy::NotifyListenerL(
		RProperty& aProperty, MCaInstallListener& aListener )
    {
    TInt appUid;
    User::LeaveIfError( aProperty.Get( KUidSystemCategory,
            KUidSwiLatestInstallation, appUid ) );
    if( appUid )
        {
        aListener.HandleInstallNotifyL(appUid );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaSwiInstallStrategy::CCaSwiInstallStrategy( RProperty& aProperty )
    {
    aProperty.Attach( KUidSystemCategory, KUidSwiLatestInstallation );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaSwiInstallStrategy::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaJavaInstallStrategy* CCaJavaInstallStrategy::NewL( RProperty& aProperty )
    {
    CCaJavaInstallStrategy* self =
            new ( ELeave ) CCaJavaInstallStrategy( aProperty );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaJavaInstallStrategy::NotifyListenerL(
		RProperty& aProperty, MCaInstallListener& aListener )
    {
//TODO: temporary sollution until new notification mechanism from usif in wk14
//    TInt state;
//    User::LeaveIfError( aProperty.Get( KUidSystemCategory,
//            KPSUidJavaLatestInstallationState, state ) );
//	if( ( ( state & ESASwisInstall ) || ( state & ESASwisUninstall ) )
//            && ( state & ESASwisStatusSuccess ) )
//        {
        TInt appUid;
        User::LeaveIfError( aProperty.Get( KUidSystemCategory,
                KPSUidJavaLatestInstallation, appUid ) );
        aListener.HandleInstallNotifyL(appUid );
//        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaJavaInstallStrategy::CCaJavaInstallStrategy( RProperty& aProperty )
    {
    aProperty.Attach( KUidSystemCategory, KPSUidJavaLatestInstallationState );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaJavaInstallStrategy::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaUsifUninstallStrategy* CCaUsifUninstallStrategy::NewL(
        RProperty& aProperty )
    {
    CCaUsifUninstallStrategy* self =
            new ( ELeave ) CCaUsifUninstallStrategy( aProperty );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaUsifUninstallStrategy::NotifyListenerL(
		RProperty& aProperty, MCaInstallListener& aListener )
    {
    TInt status;
    // TODO: Temporary solution for incorrect USIF notifications     
    User::LeaveIfError( aProperty.Get( KUidSystemCategory,
            Swi::KUidSoftwareInstallKey, status ) );
    //if( status == ( Swi::ESwisStatusSuccess | Swi::ESwisUninstall ) )
    // notify subscribers on begenning and at the end of uninstallation
    if( status == Swi::ESwisUninstall || iUninstallStatus == Swi::ESwisUninstall /*&& !status*/ )
        {
        aListener.HandleInstallNotifyL( status );
        }    
    iUninstallStatus = status;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaUsifUninstallStrategy::CCaUsifUninstallStrategy( RProperty& aProperty )
    {
	aProperty.Attach( KUidSystemCategory, Swi::KUidSoftwareInstallKey );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaUsifUninstallStrategy::ConstructL()
    {
    }

