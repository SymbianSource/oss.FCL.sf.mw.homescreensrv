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
#include <swi/sisregistrysession.h>
#include <swi/sisregistryentry.h>
#include <swi/sisregistrypackage.h>
#include <javadomainpskeys.h>
#include <javaregistry.h>
#include <javaregistrypackageentry.h>
#include <javaregistryapplicationentry.h>
#include "mcsinstallstrategy.h"



// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMcsNotifierStrategy::CMcsNotifierStrategy(
            RProperty& aProperty, MMcsInstallListener& aListener ) :
    iProperty( aProperty ), iListener( aListener )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMcsSwiInstallStrategy* CMcsSwiInstallStrategy::NewL(
        RProperty& aProperty, MMcsInstallListener& aListener )
    {
    CMcsSwiInstallStrategy* self = new ( ELeave ) CMcsSwiInstallStrategy(
            aProperty, aListener );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMcsSwiInstallStrategy::NotifyListenerL()
    {
    TInt appUid;
    User::LeaveIfError( iProperty.Get( KUidSystemCategory,
            KUidSwiLatestInstallation, appUid ) );
    if( appUid )
        {
        HandleInstallNotifyL( appUid );
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMcsSwiInstallStrategy::HandleInstallNotifyL( TInt aUid )
    {
    Swi::RSisRegistrySession iSisRegSession;
    User::LeaveIfError( iSisRegSession.Connect() );
    CleanupClosePushL( iSisRegSession );

    // Open sis package entry related to aUid
    Swi::RSisRegistryEntry packageEntry;
    if( KErrNone == packageEntry.Open( iSisRegSession, TUid::Uid( aUid ) ) )
        {
        CleanupClosePushL( packageEntry );

        // Get packageEntry's embedded sis'
        RPointerArray<Swi::CSisRegistryPackage> embedded;
        CleanupClosePushL( embedded );
        packageEntry.EmbeddedPackagesL( embedded );
        if( embedded.Count() )
            {
            // For each embadded sis we notify storage - recursive call
            for( TInt i = 0; i < embedded.Count(); ++i )
                {
                iListener.HandleInstallNotifyL( embedded[i]->Uid(),
                		CMcsInstallNotifier::ESisInstallNotification );
                }
            }
        else
            {
            // There are no embaddes sis', so we can notify storage
            // of changes in apps included in packageEntry
            NotifyL( packageEntry );
            }
        embedded.ResetAndDestroy();
        CleanupStack::PopAndDestroy( &embedded );
        CleanupStack::PopAndDestroy( &packageEntry );
        }

    CleanupStack::PopAndDestroy( &iSisRegSession );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMcsSwiInstallStrategy::NotifyL( Swi::RSisRegistryEntry & aPackageEntry )
    {
    // Get sids ( == uids of exetucables included in aPackageEntry )
    RArray<TUid> sids;
    CleanupClosePushL( sids );
    aPackageEntry.SidsL( sids );
    if( sids.Count() )
        {
        // For each sid we notify storage
        for( TInt i = 0; i < sids.Count(); ++i )
            {
            iListener.HandleInstallNotifyL(
            		sids[i], CMcsInstallNotifier::ESisInstallNotification );
            }
        }
    CleanupStack::PopAndDestroy( &sids );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMcsSwiInstallStrategy::CMcsSwiInstallStrategy(
        RProperty& aProperty, MMcsInstallListener& aListener )
    : CMcsNotifierStrategy( aProperty, aListener )
    {
    iProperty.Attach( KUidSystemCategory, KUidSwiLatestInstallation );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMcsSwiInstallStrategy::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMcsJavaInstallStrategy* CMcsJavaInstallStrategy::NewL(
        RProperty& aProperty, MMcsInstallListener& aListener )
    {
    CMcsJavaInstallStrategy* self =
            new ( ELeave ) CMcsJavaInstallStrategy( aProperty, aListener );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMcsJavaInstallStrategy::NotifyListenerL()
    {
    TInt state;
    User::LeaveIfError( iProperty.Get( KUidSystemCategory,
            KPSUidJavaLatestInstallationState, state ) );
	if( ( ( state & ESASwisInstall ) || ( state & ESASwisUninstall ) )
            && ( state & ESASwisStatusSuccess ) )
        {
        TInt appUid;
        User::LeaveIfError( iProperty.Get( KUidSystemCategory,
                KPSUidJavaLatestInstallation, appUid ) );
        HandleInstallNotifyL( TUid::Uid( appUid ) );
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CMcsJavaInstallStrategy::HandleInstallNotifyL( TUid aPackageUid )
    {
    RArray<TUid> uids;
    CleanupClosePushL( uids );

    Java::CJavaRegistry* javaRegistry;
    javaRegistry = Java::CJavaRegistry::NewLC();

    Java::CJavaRegistryEntry* regEntry =
        javaRegistry->RegistryEntryL( aPackageUid );

    if( regEntry )
        {
        CleanupStack::PushL( regEntry );

        Java::TJavaRegistryEntryType entryType = regEntry->Type();

        if ( ( entryType >= Java::EGeneralPackage ) &&
                (entryType < Java::EGeneralApplication) )
            {
            //package entry
            Java::CJavaRegistryPackageEntry* regPackageEntry =
                    static_cast<Java::CJavaRegistryPackageEntry*>( regEntry );
            regPackageEntry->GetEmbeddedEntries( uids );
            }
        else
            {
            //application entry
            uids.AppendL( regEntry->Uid() );
            }

        for ( TInt i = 0; i < uids.Count(); i++ )
            {
            iListener.HandleInstallNotifyL(
            		uids[i], CMcsInstallNotifier::EJavaInstallNotification );
            }

        CleanupStack::PopAndDestroy( regEntry );
        }
    else
        {
        iListener.HandleInstallNotifyL(
        		TUid::Null(), CMcsInstallNotifier::EJavaInstallNotification );
        }
    CleanupStack::PopAndDestroy( javaRegistry );
    CleanupStack::PopAndDestroy( &uids );
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CMcsJavaInstallStrategy::CMcsJavaInstallStrategy(
        RProperty& aProperty, MMcsInstallListener& aListener  )
    : CMcsNotifierStrategy( aProperty, aListener )
    {
    iProperty.Attach( KUidSystemCategory, KPSUidJavaLatestInstallationState );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CMcsJavaInstallStrategy::ConstructL()
    {
    }

