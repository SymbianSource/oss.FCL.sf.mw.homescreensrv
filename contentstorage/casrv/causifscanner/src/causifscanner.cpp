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
 * Description: causifscanner.cpp
 *
 */

#include <e32property.h>
#include <f32file.h>
#include <badesca.h>

#include "causifscanner.h"
#include "cainnerentry.h"
#include "cainnerquery.h"
#include "cadef.h"
#include "castorageproxy.h"
#include "caarraycleanup.inl"

using namespace Usif;

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaUsifScanner* CCaUsifScanner::NewL( TPluginParams* aPluginParams )
    {
    CCaUsifScanner* self = new ( ELeave ) CCaUsifScanner(
            *aPluginParams->storageProxy );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaUsifScanner::CCaUsifScanner( CCaStorageProxy& aStorageProxy ) :
    iStorageProxy( aStorageProxy )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaUsifScanner::ConstructL()
    {
    iSystemInstallNotifier = CCaInstallNotifier::NewL( *this,
            CCaInstallNotifier::ESisInstallNotification );

    iUsifUninstallNotifier = CCaInstallNotifier::NewL( *this,
            CCaInstallNotifier::EUsifUninstallNotification );

    iJavaInstallNotifier = CCaInstallNotifier::NewL( *this,
                CCaInstallNotifier::EJavaInstallNotification );

    User::LeaveIfError( iFs.Connect() );
    iMmcWatcher = CCaMmcWatcher::NewL( iFs, this );

    User::LeaveIfError( iSoftwareRegistry.Connect() );

    UpdateUsifListL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CCaUsifScanner::~CCaUsifScanner()
    {
    iSoftwareRegistry.Close();
    delete iMmcWatcher;
    iFs.Close();
    delete iJavaInstallNotifier;
    delete iUsifUninstallNotifier;
    delete iSystemInstallNotifier;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
/*void CCaUsifScanner::InstallL( const TDesC& aFileName )
 {
 RSoftwareInstall sif;
 CleanupClosePushL( sif );
 User::LeaveIfError( sif.Connect());

 // Install the component
 TRequestStatus status;
 sif.Install( aFileName, status, EFalse );
 User::WaitForRequest( status );
 User::LeaveIfError(status.Int());

 // Disconnect from the SIF server
 CleanupStack::PopAndDestroy( &sif );
 }*/

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaUsifScanner::HandleInstallNotifyL( TInt /*aUid*/)
    {
    UpdateUsifListL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT
void CCaUsifScanner::MmcChangeL()
    {
    UpdateUsifListL();
    }
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaUsifScanner::UpdateUsifListL()
    {
    AddPackageL();
    UpdatePackagesL();
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaUsifScanner::CreateCaEntryFromEntryL(
        const CComponentEntry* aEntry, CCaInnerEntry* aCaEntry )
    {
    aCaEntry->SetRole( EItemEntryRole );
    aCaEntry->SetEntryTypeNameL( KCaTypePackage );
    if( !aEntry->SoftwareType().Compare( KSoftwareTypeNative ) )
        {
        aCaEntry->AddAttributeL( KCaAttrAppType, KCaAttrAppTypeValueNative );
        }
    else if( !aEntry->SoftwareType().Compare( KSoftwareTypeJava ) )
        {
        aCaEntry->AddAttributeL( KCaAttrAppType, KCaAttrAppTypeValueJava );
        }
    if( aEntry->Name().Compare( KNullDesC ) )
        {
        aCaEntry->SetTextL( aEntry->Name() );
        }
    if( !aEntry->IsHidden() )
        {
        aCaEntry->SetFlags( EVisible );
        }
    if( aEntry->IsRemovable() )
        {
        aCaEntry->SetFlags( aCaEntry->GetFlags() | ERemovable );
        }
    // entries obtained with usif should have component id.
    //it's needed for uninstalling
    RBuf compIdDesc;
    CleanupClosePushL( compIdDesc );
    compIdDesc.CreateL( KCaMaxAttrValueLen );
    compIdDesc.Num( aEntry->ComponentId() );
    aCaEntry->AddAttributeL( KCaAttrComponentId, compIdDesc );
    CleanupStack::PopAndDestroy( &compIdDesc );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaUsifScanner::AddPackageL()
    {
    RPointerArray<CCaInnerEntry> entries;
    CleanupResetAndDestroyPushL( entries );
    GetCaPackageEntriesL( entries );

    RPointerArray<CComponentEntry> resultUsifArray;
    CleanupResetAndDestroyPushL( resultUsifArray );
    GetUsifPackageEntriesL( resultUsifArray );

    for( TInt i(0); i < resultUsifArray.Count(); i++ )
        {
        if( PackageExists( entries, resultUsifArray[i] ) == KErrNotFound )
            {
            CCaInnerEntry *caEntry = CCaInnerEntry::NewLC();
            CreateCaEntryFromEntryL( resultUsifArray[i], caEntry );
            iStorageProxy.AddL( caEntry );
            CleanupStack::PopAndDestroy( caEntry );
            }
        }

    CleanupStack::PopAndDestroy( &resultUsifArray );
    CleanupStack::PopAndDestroy( &entries );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaUsifScanner::UpdatePackagesL()
    {
    RPointerArray<CCaInnerEntry> resultCaArray;
    CleanupResetAndDestroyPushL( resultCaArray );
    GetCaPackageEntriesL( resultCaArray );

    RPointerArray<CComponentEntry> resultUsifArray;
    CleanupResetAndDestroyPushL( resultUsifArray );
    GetUsifPackageEntriesL( resultUsifArray );

    FindDeletedEntriesL( resultCaArray, resultUsifArray );
    RemoveEntriesFromDbL( resultCaArray );

    CleanupStack::PopAndDestroy( &resultUsifArray );
    CleanupStack::PopAndDestroy( &resultCaArray );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaUsifScanner::GetCaPackageEntriesL(
        RPointerArray<CCaInnerEntry>& aArray )
    {
    CCaInnerQuery* allAppQuery = CCaInnerQuery::NewLC();
    CDesC16ArrayFlat* appType =
            new ( ELeave ) CDesC16ArrayFlat( KGranularityOne );
    CleanupStack::PushL( appType );
    appType->AppendL( KCaTypePackage );
    allAppQuery->SetEntryTypeNames( appType );
    CleanupStack::Pop( appType );
    iStorageProxy.GetEntriesL( allAppQuery, aArray );
    CleanupStack::PopAndDestroy( allAppQuery );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaUsifScanner::GetUsifPackageEntriesL(
        RPointerArray<CComponentEntry>& aArray )
    {
    CComponentFilter *filter = CComponentFilter::NewL();
    CleanupStack::PushL( filter );
    filter->SetRemovable( ETrue );
    filter->SetHidden( EFalse );

    // Create an SCR view
    RSoftwareComponentRegistryView scrView;
    CleanupClosePushL( scrView );
    scrView.OpenViewL( iSoftwareRegistry, filter );

    // Iterate over the matching components
    //The ownership is transferred to the calling client.
    while( CComponentEntry* entry = scrView.NextComponentL() )
        {
        CleanupStack::PushL( entry );
        if( iSoftwareRegistry.IsComponentPresentL( entry->ComponentId() ) )
            {
            aArray.AppendL( entry );
            CleanupStack::Pop( entry );
            }
        else
            {
            CleanupStack::PopAndDestroy( entry );
            }
        }
    CleanupStack::PopAndDestroy( &scrView );
    CleanupStack::PopAndDestroy( filter );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CCaUsifScanner::PackageExists( RPointerArray<CCaInnerEntry>& aArray,
        const CComponentEntry* aEntry )
    {
    TInt retVal( KErrNotFound );
    for( TInt i( 0 ); i < aArray.Count(); i++ )
        {
        TBuf<KMaxUnits> compIdDes;
        if( aArray[i]->FindAttribute( KCaAttrComponentId, compIdDes ) )
            {
            TLex lex( compIdDes );
            TUint uint( 0 );
            if( lex.Val( uint ) == KErrNone && aEntry->ComponentId() == uint )
                {
                retVal = i;
                }
            }
        }
    return retVal;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaUsifScanner::FindDeletedEntriesL(
        RPointerArray<CCaInnerEntry>& aCaArray, const RPointerArray<
                CComponentEntry>& aUsifArray )
    {
    for( TInt i( aCaArray.Count() - 1 ); i >= 0; i-- )
        {
        TBuf<KMaxUnits> compIdDes;
        if( aCaArray[i]->FindAttribute( KCaAttrComponentId, compIdDes ) )
            {
            TLex lex( compIdDes );
            TUint uint( 0 );
            User::LeaveIfError( lex.Val( uint ) );
            //for each usif entry check if entry has to be removed
            for( TInt k( 0 ); k < aUsifArray.Count(); k++ )
                {
                if( aUsifArray[k]->ComponentId() == uint
                        && iSoftwareRegistry.IsComponentPresentL( uint ) )
                    {
                    delete aCaArray[i];
                    aCaArray.Remove( i );
                    break;
                    }
                }
            }
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CCaUsifScanner::RemoveEntriesFromDbL(
        RPointerArray<CCaInnerEntry>& aCaArray )
    {
    RArray<TInt> entriesId;
    CleanupClosePushL( entriesId );
    for( TInt i( 0 ); i < aCaArray.Count(); i++ )
        {
        entriesId.Append( aCaArray[i]->GetId() );
        }
    iStorageProxy.RemoveL( entriesId );
    CleanupStack::PopAndDestroy( &entriesId );
    }
