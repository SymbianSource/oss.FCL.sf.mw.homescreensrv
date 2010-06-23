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
* Description: casrvappscanner.cpp
*
*/

#include <e32property.h>
#include <bautils.h>
#include <swi/sisregistrysession.h>
#include <swi/sisregistryentry.h>
#include <swi/sisregistrypackage.h>
#include <usif/scr/screntries.h>

#include "cadef.h"
#include "casrvappscanner.h"
#include "cautils.h"
#include "casrvmmchistory.h"
#include "pathinfo.h"
#include "casrvengutils.h"
#include "cainnerquery.h"
#include "cainnerentry.h"
#include "caarraycleanup.inl"
#include "castorageproxy.h"

#include "caarraycleanup.inl"

using namespace Usif;

// ==================== LOCAL FUNCTIONS ====================

/**
 * Identity function to search in an array of CCaInnerEntry.
 * Identity is the ID.
 * @param aLeft Search term.
 * @param aRight Array item.
 * @return ETrue if ID-s match.
 */
LOCAL_C TBool IdMatch( const CCaInnerEntry& aLeft,
        const CCaInnerEntry& aRight )
    {
    return aLeft.GetId() == aRight.GetId();
    }

/**
 * Identity function to search in an array of CCaInnerEntry.
 * Identity is the ID.
 * @param aLeft Search term.
 * @param aRight Array item.
 * @return ETrue if ID-s match.
 */
LOCAL_C TBool UidMatch( const CCaInnerEntry& aLeft,
        const CCaInnerEntry& aRight )
    {
    return aLeft.GetUid() == aRight.GetUid();
    }

// ==================== MEMBER FUNCTIONS ====================

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
CCaSrvAppScanner* CCaSrvAppScanner::NewL( CCaStorageProxy& aCaStorageProxy,
        CCaSrvEngUtils& aUtils )
    {
    CCaSrvAppScanner* scanner = new ( ELeave ) CCaSrvAppScanner(
            aCaStorageProxy, aUtils );
    CleanupStack::PushL( scanner );
    scanner->ConstructL();
    CleanupStack::Pop( scanner );
    return scanner;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
CCaSrvAppScanner::~CCaSrvAppScanner()
    {
    Cancel();
    iSoftwareRegistry.Close();
    iApaLsSession.Close();
    iInstalledPackages.Close();
    delete iInstallNotifier;
    delete iNotifier;
    delete iMmcHistory;
    iFs.Close();
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
CCaSrvAppScanner::CCaSrvAppScanner( CCaStorageProxy& aCaStorageProxy,
        CCaSrvEngUtils& aUtils ) :
    CActive( CActive::EPriorityStandard ),
    iCaStorageProxy( aCaStorageProxy ), iSrvEngUtils( aUtils )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    TFileName path;
    User::LeaveIfError( iFs.PrivatePath( path ) );
    TUint attribute;
    if( iFs.Att( path, attribute ) == KErrNotFound )
        {
        TInt mdRes = iFs.MkDirAll( path );
        if ( mdRes != KErrNone )
            {
            User::Leave( mdRes );
            }
        }
    iMmcHistory = CCaSrvMmcHistory::NewL();
    iMmcHistory->LoadL( iFs, KCaMmcHistoryFname() );
    iNotifier = CApaAppListNotifier::NewL(
            this, CActive::EPriorityStandard );
    iInstallNotifier = CCaInstallNotifier::NewL(
            *this, CCaInstallNotifier::ESisInstallNotification );

    User::LeaveIfError( iApaLsSession.Connect() );
    User::LeaveIfError( iApaLsSession.GetAllApps() ); // This is async.
    User::LeaveIfError( iSoftwareRegistry.Connect() );
    iApaLsSession.RegisterListPopulationCompleteObserver( iStatus );
    iCollectionDownloadId = 0;
    iAllCollectionId = 0;
    
    //TODO temporary sollution for fake uninstall progres notification
    ClearUninstallFlagL();
    
    SetActive();
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
//TODO temporary sollution for fake uninstall progres notification
void CCaSrvAppScanner::ClearUninstallFlagL()
    {
    CCaInnerQuery* uninstallQuery = CCaInnerQuery::NewLC();
    uninstallQuery->SetFlagsOn( EUninstall );
    
    RPointerArray<CCaInnerEntry> resultArray;
    CleanupResetAndDestroyPushL( resultArray );
    iCaStorageProxy.GetEntriesL( uninstallQuery, resultArray );
    for( TInt i = 0; i < resultArray.Count(); i++ )
    {
        resultArray[i]->SetFlags( resultArray[i]->GetFlags() & ~EUninstall );
        iCaStorageProxy.AddL( resultArray[i] );
    }
    CleanupStack::PopAndDestroy( &resultArray );
    
    CleanupStack::PopAndDestroy( uninstallQuery );    
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::RunL()
    {
    User::LeaveIfError( iStatus.Int() );
    // AppArc app scan complete, we have the app list.
    UpdateApplicationEntriesL();
    InstallationNotifyL();
    MakeNotEmptyCollectionsVisibleL();
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::DoCancel()
    {
    iApaLsSession.CancelListPopulationCompleteObserver();
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TInt CCaSrvAppScanner::RunError( TInt /*aError*/)
    {
    // Ignore the error (what else could we do?).
    // When next update occurs, we will run again.
    return KErrNone;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::UpdateApplicationEntryL(
        RPointerArray<CCaInnerEntry>& aCaEntries,
        TUint aAppUid, TUint aMmcId )
    {
    RPointerArray<CCaInnerEntry> resultArray;
    CleanupResetAndDestroyPushL( resultArray );
    GetCaAppEntriesL( aAppUid, resultArray );

    // This app is not in the storage, add it now.
    // We don't add hidden items, there are too many of them!
    // do not display Menu app
    if( !resultArray.Count() )
        {
        AddAppEntryL( aAppUid, aMmcId );
        }//if

    // "removable", "missing" and "visible"  flags update
    for( TInt j = 0; j < resultArray.Count(); j++ )
        {
        //We need to handle first run of appscanner,
        //there might be some incorrect data. If this will have impact
        //on performance we may run this methods only at start up.

        UpdateAppEntryL( resultArray[j], aMmcId );
        TInt index = aCaEntries.Find( resultArray[j],
                TIdentityRelation<CCaInnerEntry>( IdMatch ) );
        if( index != KErrNotFound )
            {
            delete aCaEntries[index];
            aCaEntries.Remove( index );
            }
        }//for
    CleanupStack::PopAndDestroy( &resultArray );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::UpdateAppEntryL( CCaInnerEntry* aEntry, TUint aMmcId )
    {
    TBool toUpdate = HandleMmcAttrUpdateL( aEntry, aMmcId );
    TBool missingVisibleFlagChanged =
        HandleMissingVisibleFlagsUpdate( aEntry );
    toUpdate = missingVisibleFlagChanged || toUpdate;
    toUpdate = HandleRemovableVisibleFlagsUpdateL( aEntry ) || toUpdate;
    toUpdate = SetApaAppInfoL( aEntry ) || toUpdate;

    if( iInstalledPackages.Find( aEntry->GetUid() ) != KErrNotFound )
        {
        AddEntryToDownloadedCollectionL( aEntry->GetId() );
        toUpdate = HandleUsedFlagUpdate( aEntry ) || toUpdate;
        AddEntryToPredefinedCollectionL( aEntry, ETrue );
        }
    if( toUpdate )
        {
        //update app in storage
        TItemAppearance itemAppearanceChange = EItemAppearanceNotChanged;
        if( missingVisibleFlagChanged )
            {
            itemAppearanceChange = EItemAppeared;
            }
        iCaStorageProxy.AddL( aEntry, EFalse, itemAppearanceChange );
        RemoveFromInstalledPackages( aEntry->GetUid() );
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::HandleMmcAttrUpdateL(
        CCaInnerEntry* aItem, TUint aMmcId )
    {
    TBool toUpdate( ETrue );
    if( IsInMmcL( TUid::Uid( aItem->GetUid() ) ) )
        {
        //app is instaled on mmc - KCaAttrMmcId attribute update
        RBuf uidString;
        uidString.CleanupClosePushL();
        uidString.CreateL(KUidChars);
        uidString.Format( KHexFormat, aMmcId );
        aItem->AddAttributeL( KCaAttrMmcId, uidString );
        CleanupStack::PopAndDestroy( &uidString );
        }
    else if ( IsInMassStorageL( TUid::Uid( aItem->GetUid() ) ) )
        {
        //its app installed on mass storage, we need to leave it
        //in case of connecting usb in mass storage mode
        aItem->AddAttributeL( KCaAttrMmcId, KCaMassStorage );
        }
    else
        {
        RBuf attrVal;
        attrVal.CleanupClosePushL();
        attrVal.CreateL( KCaMaxAttrValueLen );
        if( aItem->FindAttribute( KCaAttrMmcId, attrVal ) )
            {
            aItem->RemoveAttributeL( KCaAttrMmcId );
            }
        else
            {
            toUpdate = EFalse;
            }
        CleanupStack::PopAndDestroy( &attrVal );
        //its installed on c: drive - remove attribute
        }
    return toUpdate;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::InstallationNotifyL()
    {
    for( TInt i = iInstalledPackages.Count() - 1; i >= 0; i-- )
        {
        NotifyL( iInstalledPackages[i] );
        iInstalledPackages.Remove( i );
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::UpdateApplicationEntriesL()
    {
    TUint currentMmcId = UpdateMmcHistoryL();
    // get all Content arsenal enties with type application
    RPointerArray<CCaInnerEntry> resultArray;
    CleanupResetAndDestroyPushL( resultArray );
    GetCaAppEntriesL( resultArray );
    HandleHsAppEntryL( resultArray );
    RemoveSatAppL( resultArray );

    RArray<TUint> appUids;
    CleanupClosePushL( appUids );
    GetApaItemsL( appUids );
    RemoveApp( appUids, KSatUid.iUid );
    RemoveApp( appUids, KHsAppUid.iUid );

    //for every item in apaAndCrItems array
    for( TInt i = 0; i < appUids.Count(); i++ )
        {
        // if there was leave for any item we ignore it
        // and proceed to the next one
        TRAP_IGNORE(UpdateApplicationEntryL(
                resultArray, appUids[i], currentMmcId));
        }
    // Here the big list with items that refer to missing apps.
    HandleMissingItemsL( resultArray );
    CleanupStack::PopAndDestroy( &appUids );
    CleanupStack::PopAndDestroy( &resultArray );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::HandleRemovableVisibleFlagsUpdateL(
        CCaInnerEntry* aItem )
    {
    TBool toChange( EFalse );
    if( ( aItem->GetFlags() & EVisible ) && IsInRomL( aItem->GetUid() ) )
        {
        if( ( aItem->GetFlags() & ERemovable ) != 0 )
            {
            aItem->SetFlags( aItem->GetFlags() & ~ERemovable );
            toChange = ETrue;
            }
        }
    else
        {
        if( ( aItem->GetFlags() & ERemovable ) == 0 )
            {
            aItem->SetFlags( aItem->GetFlags() | ERemovable );
            toChange = ETrue;
            }
        }
    return toChange;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::HandleMissingVisibleFlagsUpdate(
        CCaInnerEntry* aItem )
    {
    TBool ret( EFalse );
    if( aItem->GetFlags() & EMissing ||
        !( aItem->GetFlags() & EVisible ) )
        {
        //application found so we unset "missing" and set "visible" flags
        aItem->SetFlags( aItem->GetFlags() & ~EMissing | EVisible );
        ret = ETrue;
        }
    return ret;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::HandleVisibleFlagUpdate( CCaInnerEntry* aItem )
    {
    TBool toChange( EFalse );
    if( !( aItem->GetFlags() & EVisible ) )
        {
        aItem->SetFlags( aItem->GetFlags() | EVisible );
        toChange = ETrue;
        }
    return toChange;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::HandleUsedFlagUpdate( CCaInnerEntry* aItem )
    {
    TBool changed( EFalse );
    if( aItem->GetFlags() & EUsed )
        {
        aItem->SetFlags( aItem->GetFlags() & ~EUsed );
        changed = ETrue;
        }
    return changed;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::RemoveApp( RArray<TUint>& aArray, TInt32 aUid )
    {
    TInt id = aArray.Find( aUid );
    if( id != KErrNotFound )
        {
        aArray.Remove( id );
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::RemoveSatAppL( RPointerArray<CCaInnerEntry>& aArray )
    {
    CCaInnerEntry* sat = CCaInnerEntry::NewL();
    sat->SetUid( KSatUid.iUid );
    TInt index = aArray.Find(
            sat, TIdentityRelation<CCaInnerEntry>( UidMatch ) );
    if ( index != KErrNotFound )
        {
        delete aArray[index];
        aArray.Remove( index );
        }
    delete sat;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::HandleHsAppEntryL(
        RPointerArray<CCaInnerEntry>& aArray )
    {
    CCaInnerEntry* appEntry = CCaInnerEntry::NewLC();
    appEntry->SetUid( KHsAppUid.iUid );
    TInt index = aArray.Find(
            appEntry, TIdentityRelation<CCaInnerEntry>( UidMatch ) );

    if ( index != KErrNotFound )
        {
        // hs app already in storage - ensure it is hidden
        // and remove from resultArray
        if ( aArray[index]->GetFlags() & EVisible )
            {
            aArray[index]->SetFlags( aArray[index]->GetFlags() & ~EVisible);
            iCaStorageProxy.AddL( aArray[index], EFalse, EItemDisappeared );
            }
        delete aArray[index];
        aArray.Remove( index );
        }
    else
        {
        // if not found add as not visible to the storage
        appEntry->SetEntryTypeNameL( KCaTypeApp );
        appEntry->SetFlags( 0 );
        appEntry->SetRole( EItemEntryRole );
        SetApaAppInfoL( appEntry );
        iCaStorageProxy.AddL( appEntry );
        }
    CleanupStack::PopAndDestroy( appEntry );
    }


// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::GetApaItemsL( RArray<TUint>& aArray )
    {
    TApaAppInfo* appInfo = new ( ELeave ) TApaAppInfo();
    CleanupStack::PushL(appInfo);
    TApaAppCapabilityBuf appCap;

    User::LeaveIfError( iApaLsSession.GetAllApps( 0 ) );
    // for every application get uid, hidden and missing attribute
    // and add to aArray.
    while( KErrNone == iApaLsSession.GetNextApp( *appInfo ) )
        {
        User::LeaveIfError( iApaLsSession.GetAppCapability(
                appCap, appInfo->iUid ) );
        // "Hidden" status according to AppArc.
        if( !appCap().iAppIsHidden )
            {
            aArray.AppendL( appInfo->iUid.iUid );
            }
        }
    CleanupStack::PopAndDestroy(appInfo);
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::GetCaAppEntriesL(
        RPointerArray<CCaInnerEntry>& aArray )
    {
    CCaInnerQuery* allAppQuery = CCaInnerQuery::NewLC();
    CDesC16ArrayFlat* appType =
            new ( ELeave ) CDesC16ArrayFlat( KGranularityOne );
    CleanupStack::PushL( appType );
    appType->AppendL( KCaTypeApp );
    allAppQuery->SetEntryTypeNames( appType );
    CleanupStack::Pop( appType );
    iCaStorageProxy.GetEntriesL( allAppQuery, aArray );
    CleanupStack::PopAndDestroy( allAppQuery );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TInt CCaSrvAppScanner::GetCollectionDownloadIdL()
    {
    if( iCollectionDownloadId == 0 )
        {
        RPointerArray<CCaInnerEntry> resultArray;
        CleanupResetAndDestroyPushL( resultArray );
        CCaInnerQuery* allAppQuery = CCaInnerQuery::NewLC();
        CDesC16ArrayFlat* appType =
                new ( ELeave ) CDesC16ArrayFlat( KGranularityOne );
        CleanupStack::PushL( appType );
        appType->AppendL( KCaTypeCollectionDownload );
        allAppQuery->SetEntryTypeNames( appType );
        CleanupStack::Pop( appType );
        iCaStorageProxy.GetEntriesL( allAppQuery, resultArray );
        CleanupStack::PopAndDestroy( allAppQuery );
        if( resultArray.Count() )
            {
            iCollectionDownloadId = resultArray[0]->GetId();
            }
        CleanupStack::PopAndDestroy( &resultArray );
        }
    return iCollectionDownloadId;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TInt CCaSrvAppScanner::GetAllCollectionIdL()
    {
    if( iAllCollectionId == 0 )
        {
        CCaInnerQuery *getAllCollectionIdQuery = CCaInnerQuery::NewLC();
        CDesC16ArrayFlat *typenameArray = new(ELeave) CDesC16ArrayFlat(
                KGranularityOne );
        CleanupStack::PushL( typenameArray );
        typenameArray->AppendL( KCaTypeMenuCollections );
        getAllCollectionIdQuery->SetEntryTypeNames( typenameArray );
        CleanupStack::Pop( typenameArray );

        RArray<TInt> idArray;
        CleanupClosePushL( idArray );
        iCaStorageProxy.GetEntriesIdsL( getAllCollectionIdQuery,
                idArray );
        if( idArray.Count() )
            {
            iAllCollectionId = idArray[0];
            }
        CleanupStack::PopAndDestroy( &idArray );
        CleanupStack::PopAndDestroy( getAllCollectionIdQuery );
        }
    return iAllCollectionId;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::GetCaAppEntriesL( TInt aUid,
        RPointerArray<CCaInnerEntry>& aArray )
    {
    CCaInnerQuery* allAppQuery = CCaInnerQuery::NewLC();
    allAppQuery->SetUid( aUid );
    iCaStorageProxy.GetEntriesL( allAppQuery, aArray );
    CleanupStack::PopAndDestroy( allAppQuery );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::HandleAppListEvent( TInt /*aEvent*/)
    {
    // We only have one event, EAppListChanged.
    // Call back RunL async, to requeue and initiate rescan.
    if( !IsActive() )
        {
        ScheduleScan();
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::HandleInstallNotifyL( TInt aUid )
    {
    NotifyL( aUid );
    RArray<TUid> uids;
    CleanupClosePushL( uids );

    Swi::RSisRegistrySession iSisRegSession;
    CleanupClosePushL( iSisRegSession );
    User::LeaveIfError( iSisRegSession.Connect() );

    // Open sis package entry related to aUid
    Swi::RSisRegistryEntry packageEntry;
    if( KErrNone
            == packageEntry.Open( iSisRegSession, TUid::Uid( aUid ) ) )
        {
        CleanupClosePushL( packageEntry );

        // Get packageEntry's embedded sis'
        RPointerArray<Swi::CSisRegistryPackage> embedded;
        CleanupResetAndDestroyPushL( embedded );
        packageEntry.EmbeddedPackagesL( embedded );
        if( embedded.Count() )
            {
            // For each embadded sis we notify storage - recursive call
            for( TInt i = 0; i < embedded.Count(); ++i )
                {
                HandleInstallNotifyL( embedded[i]->Uid().iUid );
                }
            }
        else
            {
            // There are no embaddes sis', so we can notify storage
            // of changes in apps included in packageEntry
            NotifyL( packageEntry );
            }
        CleanupStack::PopAndDestroy( &embedded );
        CleanupStack::PopAndDestroy( &packageEntry );
        }
    CleanupStack::PopAndDestroy( &iSisRegSession );
    CleanupStack::PopAndDestroy( &uids );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::NotifyL( Swi::RSisRegistryEntry & aPackageEntry )
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
            iInstalledPackages.Append( sids[i].iUid );
            }
        }
    CleanupStack::PopAndDestroy( &sids );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::NotifyL( TInt aAppUid )
    {
    // Get entries for given aAppUid
    RPointerArray<CCaInnerEntry> resultArray;
    CleanupResetAndDestroyPushL( resultArray );
    GetCaAppEntriesL( aAppUid, resultArray );
    if( resultArray.Count() && ( resultArray[0]->GetFlags() & EVisible ) )
        {//TODO: this only for icons. This functionality should be change
        iCaStorageProxy.AddL( resultArray[0] );
        }
    CleanupStack::PopAndDestroy( &resultArray );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::AddEntryToDownloadedCollectionL( TInt aEntryId )
    {
    TCaOperationParams params;
    params.iOperationType = TCaOperationParams::EPrepend;
    params.iGroupId = GetCollectionDownloadIdL();
    params.iBeforeEntryId = 0; // Not used.

    RArray<TInt> entryIds;
    CleanupClosePushL( entryIds );
    entryIds.AppendL( aEntryId );
    iCaStorageProxy.OrganizeL( entryIds, params );

    CleanupStack::PopAndDestroy( &entryIds );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::AddEntryToPredefinedCollectionL(
        CCaInnerEntry* aEntry, TBool aUpdate )
    {
    TApaAppCapabilityBuf capability;
    User::LeaveIfError( iApaLsSession.GetAppCapability( capability,
            TUid::Uid( aEntry->GetUid() ) ) );

    if( capability().iGroupName.Length() )
        {
        // appgroup_name is defined for this app. Find or create folder.
        CCaInnerQuery *innerQuery = CCaInnerQuery::NewLC();
        innerQuery->SetRole( CCaInnerQuery::Group );
        innerQuery->AddAttributeL( KCaAppGroupName,
                capability().iGroupName );

        // get entries by attributes
        RPointerArray<CCaInnerEntry> resultArrayItems;
        CleanupResetAndDestroyPushL( resultArrayItems );
        iCaStorageProxy.GetEntriesL( innerQuery, resultArrayItems );

        RArray<TInt> entryIds;
        CleanupClosePushL( entryIds );
        entryIds.AppendL( aEntry->GetId() );
        TCaOperationParams organizeParams;
        organizeParams.iBeforeEntryId = 0;
        organizeParams.iOperationType = TCaOperationParams::EAppend;

        if( resultArrayItems.Count() )
            {
            // collection with appgroup_name exist - add entry
            // to this collection
            organizeParams.iGroupId = resultArrayItems[0]->GetId();
            }
        else
            {
            // create new collection
            TInt predefinedCollectionId = CreatePredefinedCollectionL(
                    capability().iGroupName );

            organizeParams.iGroupId = predefinedCollectionId;

            // add new collection to all collection
            AddCollectionToAllCollectionL( predefinedCollectionId );

            if( aUpdate )
                {
                iCaStorageProxy.OrganizeL( entryIds, organizeParams );
                }
            }

        if( !aUpdate )
            {
            iCaStorageProxy.OrganizeL( entryIds, organizeParams );
            }

        CleanupStack::PopAndDestroy( &entryIds );
        CleanupStack::PopAndDestroy( &resultArrayItems );
        CleanupStack::PopAndDestroy( innerQuery );
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TInt CCaSrvAppScanner::CreatePredefinedCollectionL( const TDesC& aGroupName )
    {
    CCaInnerEntry *innerEntry = CCaInnerEntry::NewLC();
    innerEntry->SetTextL( aGroupName );
    innerEntry->SetEntryTypeNameL( KCaTypeCollection );
    innerEntry->SetRole( CCaInnerQuery::Group );
    innerEntry->AddAttributeL( KCaAppGroupName, aGroupName );
    innerEntry->SetFlags( EVisible | ERemovable );
    innerEntry->SetIconDataL( KCollectionIconFileName,
            KNullDesC, KNullDesC );
    iCaStorageProxy.AddL( innerEntry );
    // Get new collection Id
    TInt newCollectionId = innerEntry->GetId();
    CleanupStack::PopAndDestroy( innerEntry );

    return newCollectionId;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::AddCollectionToAllCollectionL( TInt aCollectionId )
    {
    RArray<TInt> entryIds;
    CleanupClosePushL( entryIds );
    entryIds.AppendL( aCollectionId );

    TCaOperationParams organizeParams;
    organizeParams.iBeforeEntryId = 0;
    organizeParams.iOperationType = TCaOperationParams::EAppend;
    organizeParams.iGroupId = GetAllCollectionIdL();
    iCaStorageProxy.OrganizeL( entryIds, organizeParams );
    CleanupStack::PopAndDestroy( &entryIds );
    }



// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::RemoveEntryFromDownloadedL( TInt aEntryId )
    {
    TCaOperationParams params;
    params.iOperationType = TCaOperationParams::ERemove;
    params.iGroupId = GetCollectionDownloadIdL();
    params.iBeforeEntryId = 0; // Not Used

    RArray<TInt> entryIds;
    CleanupClosePushL( entryIds );
    entryIds.AppendL( aEntryId );
    iCaStorageProxy.OrganizeL( entryIds, params );

    CleanupStack::PopAndDestroy( &entryIds );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::ScheduleScan()
    {
    if( !IsActive() )
        {
        TRequestStatus* ownStatus = &iStatus;
        *ownStatus = KRequestPending;
        SetActive();
        User::RequestComplete( ownStatus, KErrNone );
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::AddAppEntryL( TUint aUid, TUint aCurrentMmcId )
    {
    // Now add the app entry.
    CCaInnerEntry* appEntry = CCaInnerEntry::NewLC();

    appEntry->SetEntryTypeNameL( KCaTypeApp );
    appEntry->SetUid( aUid );
    appEntry->SetFlags( EVisible );
    appEntry->SetRole( EItemEntryRole );

    SetApaAppInfoL( appEntry );
    HandleRemovableVisibleFlagsUpdateL( appEntry );
    HandleMmcAttrUpdateL( appEntry, aCurrentMmcId );

    iCaStorageProxy.AddL( appEntry );

    AddEntryToPredefinedCollectionL( appEntry );

    if( iInstalledPackages.Find( aUid ) != KErrNotFound )
        {
        AddEntryToDownloadedCollectionL( appEntry->GetId() );
        RemoveFromInstalledPackages( aUid );
        }

    CleanupStack::PopAndDestroy( appEntry );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::RemoveFromInstalledPackages( TUint aUid )
    {
    TInt appIndex = iInstalledPackages.Find( aUid );
    if( appIndex != KErrNotFound )
        {
        iInstalledPackages.Remove( appIndex );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CCaSrvAppScanner::SetApaAppInfoL( CCaInnerEntry* aEntry )
    {
    TBool changed( EFalse );
    TApaAppInfo* info = new ( ELeave ) TApaAppInfo();
    CleanupStack::PushL(info);
    if( KErrNone == iSrvEngUtils.GetAppInfo( *aEntry, *info ) )
        {
        RBuf attrVal;
        attrVal.CleanupClosePushL();
        attrVal.CreateL( KCaMaxAttrValueLen );
        aEntry->FindAttribute( KCaAttrShortName, attrVal );
        if( attrVal.Compare( info->iShortCaption ) != KErrNone
                || aEntry->GetText().Compare( info->iCaption )
                        != KErrNone )
            {
            aEntry->SetTextL( info->iCaption );
            aEntry->AddAttributeL( KCaAttrShortName, info->iShortCaption );
            changed = ETrue;
            }
        
        
        if ( UpdateComponentIdL( *aEntry ) )
            {
            changed = ETrue;
            }
        
        CleanupStack::PopAndDestroy( &attrVal );
        
        // check if its java app and add attr for entries
        TUid appTypeUid;
        if( KErrNone == iApaLsSession.GetAppType( appTypeUid, info->iUid ) )
            {
            if( appTypeUid == KMidletApplicationTypeUid )
                {
                aEntry->AddAttributeL(
                        KCaAttrAppType,
                        KCaAttrAppTypeValueJava );
                
                TComponentId compId = 
                        iSoftwareRegistry.GetComponentIdForAppL( info->iUid );
                CPropertyEntry* propertyEntry = 
                    iSoftwareRegistry.GetComponentPropertyL( compId, KCaScrPropertyAppSettings );
                CleanupStack::PushL( propertyEntry );
                if ( propertyEntry && 
                        propertyEntry->PropertyType() == CPropertyEntry::ELocalizedProperty ) 
                    {
                    aEntry->AddAttributeL(
                            KCaAttrAppSettingsPlugin,
                            static_cast<CLocalizablePropertyEntry*>(propertyEntry)->StrValue() );
                    }
                CleanupStack::PopAndDestroy( propertyEntry );
                }
            else if ( appTypeUid == KCWRTApplicationTypeUid )
                {
                aEntry->AddAttributeL(
                        KCaAttrAppType,
                        KCaAttrAppTypeValueCWRT );
                TComponentId compId = 
                        iSoftwareRegistry.GetComponentIdForAppL( info->iUid );
                CPropertyEntry* isMiniview = 
                                        iSoftwareRegistry.GetComponentPropertyL( compId,
                                                KCaScrPropertyIsMiniviewSupported );
                CleanupStack::PushL( isMiniview );
                // check first if we support mini view
                if (isMiniview && isMiniview->PropertyType() == CPropertyEntry::EIntProperty &&
                        static_cast<CIntPropertyEntry*>(isMiniview)->IntValue())
                    {
                    CPropertyEntry* appId = 
                        iSoftwareRegistry.GetComponentPropertyL( compId, KCaScrPropertyAppId );
                    CleanupStack::PushL( appId );
                    if ( appId && 
                         appId->PropertyType() == CPropertyEntry::ELocalizedProperty ) 
                        {
                        aEntry->AddAttributeL(
                                            KCaAttrAppWidgetUri,
                                            KCaAttrAppWidgetUriCWRTValue );
                        aEntry->AddAttributeL(
                                            KCaAttrAppWidgetParamWebAppId,
                                            static_cast<CLocalizablePropertyEntry*>(appId)->StrValue() );
                        }
                    CleanupStack::PopAndDestroy( appId );
                    }                
                CleanupStack::PopAndDestroy( isMiniview );
                }
            }
        }
    CleanupStack::PopAndDestroy( info );
    return changed;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::HandleMissingItemsL(
        RPointerArray<CCaInnerEntry>& aCaEntries )
    {
    for( TInt i = 0; i < aCaEntries.Count(); i++ )
        {
        RBuf attrVal;
        attrVal.CleanupClosePushL();
        attrVal.CreateL( KCaMaxAttrValueLen );
        if( aCaEntries[i]->FindAttribute( KCaAttrMmcId(), attrVal ) )
            {
            TUint mmcId = 0;
            MenuUtils::GetTUint( attrVal, mmcId );
            if( ( mmcId && KErrNotFound != iMmcHistory->Find( mmcId )
                        && mmcId != CurrentMmcId() ) 
                    || ( attrVal == KCaMassStorage() 
                        && IsDriveInUse( DriveInfo::EDefaultMassStorage ) ) )
                {
                // This item is on an MMC which is currently
                // in the MMC history or on a mass storage in use.
                // Set it "missing" but keep it.
                SetMissingFlagL( aCaEntries[i] );
                }
            else
                {
                aCaEntries[i]->RemoveAttributeL( KCaAttrMmcId() );
                ClearVisibleFlagL( aCaEntries[i] );
                }
            }
        else
            {
            ClearVisibleFlagL( aCaEntries[i] );
            }
        CleanupStack::PopAndDestroy( &attrVal );
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::SetMissingFlagL( CCaInnerEntry* aEntry )
    {
    if( !( aEntry->GetFlags() & EMissing ) )
        {
        aEntry->SetFlags( ( aEntry->GetFlags() | EMissing ) & ~EUninstall );
        iCaStorageProxy.AddL( aEntry, EFalse, EItemDisappeared );
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::ClearVisibleFlagL( CCaInnerEntry* aEntry )
    {
    if( aEntry->GetFlags() & EVisible )
        {
        aEntry->SetFlags(
                aEntry->GetFlags() &
                ~EUninstall &
                ~EVisible &
                ~EMissing &
                ~EUsed );
        iCaStorageProxy.AddL( aEntry, EFalse, EItemDisappeared );
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TUint CCaSrvAppScanner::UpdateMmcHistoryL()
    {
    TUint mmcId = CurrentMmcId();
    if( mmcId )
        {
        iMmcHistory->InsertL( mmcId );
        iMmcHistory->SaveL( iFs, KCaMmcHistoryFname() );
        }
    return mmcId;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TUint CCaSrvAppScanner::CurrentMmcId() const
    {
    // Get mmc id. Errors are ignored.
    TUint mmcId = 0;
    TInt mmcDrive;
    TInt err = DriveInfo::GetDefaultDrive(
            DriveInfo::EDefaultRemovableMassStorage, mmcDrive );
    if( !err )
        {
        TVolumeInfo volumeInfo;
        err = iFs.Volume( volumeInfo, mmcDrive );
        if( !err )
            {
            mmcId = volumeInfo.iUniqueID;
            }
        }
    return mmcId;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::IsFileInDrive(
    const TDesC& aFileName,
    const DriveInfo::TDefaultDrives& aDefaultDrive ) const
    {
    TBool retVal( EFalse );
    if ( aFileName.Length()>0 )
        {
        TInt mmcDrive;
        TInt err = DriveInfo::GetDefaultDrive( aDefaultDrive, mmcDrive );
        if( !err )
            {
            TInt fileDrive;
            err = RFs::CharToDrive( aFileName[0], fileDrive );
            if( !err && fileDrive == mmcDrive )
                {
                retVal = ETrue;
                }
            }
        }
    return retVal;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::IsAppInDriveL(
        const TUid aUid,
        const DriveInfo::TDefaultDrives& aDefaultDrive ) const
    {
    TBool ret( EFalse );
    TApaAppInfo* appInfo = new ( ELeave ) TApaAppInfo();
    TInt err = iApaLsSession.GetAppInfo( *appInfo, aUid );
    if( !err && IsFileInDrive( appInfo->iFullName, aDefaultDrive ) )
        {
        ret = ETrue;
        }
    delete appInfo;
    return ret;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::IsInMmcL( const TUid aUid ) const
    {
    return IsAppInDriveL( aUid, DriveInfo::EDefaultRemovableMassStorage );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::IsInMassStorageL( const TUid aUid ) const
    {
    return IsAppInDriveL( aUid, DriveInfo::EDefaultMassStorage );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::IsInRomL( TInt aUid )
    {
    return IsAppInDriveL( TUid::Uid( aUid ), DriveInfo::EDefaultRom );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::IsDriveInUse(
        const DriveInfo::TDefaultDrives& aDefaultDrive )
    {
    TBool inUse( EFalse );
    TInt drive;

    TInt err = DriveInfo::GetDefaultDrive( aDefaultDrive, drive );
    if( err == KErrNone )
        {
        TUint status;
        err = DriveInfo::GetDriveStatus( iFs, drive, status );
        if( err == KErrNone && ( status & DriveInfo::EDriveInUse ) )
            {
            inUse = ETrue;
            }
        }

    return inUse;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::MakeNotEmptyCollectionsVisibleL()
    {
    RPointerArray<CCaInnerEntry> resultArray;
    CleanupResetAndDestroyPushL( resultArray );
    CCaInnerQuery* hiddenCollectionsQuery = CCaInnerQuery::NewLC();
    CDesC16ArrayFlat* entryType = new ( ELeave ) CDesC16ArrayFlat(
            KGranularityOne );
    CleanupStack::PushL( entryType );
    entryType->AppendL( KCaTypeCollection );
    hiddenCollectionsQuery->SetEntryTypeNames( entryType );
    hiddenCollectionsQuery->SetFlagsOff( EVisible );
    iCaStorageProxy.GetEntriesL( hiddenCollectionsQuery, resultArray );
    CleanupStack::Pop( entryType );
    CleanupStack::PopAndDestroy( hiddenCollectionsQuery );
    if( resultArray.Count() )
        {
        for( TInt i=0; i<resultArray.Count(); i++ )
            {
            // for any not visible collection
            MakeCollectionVisibleIfHasVisibleEntryL( resultArray[i] );
            }
        }
    CleanupStack::PopAndDestroy( &resultArray );
   }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaSrvAppScanner::MakeCollectionVisibleIfHasVisibleEntryL(
        CCaInnerEntry* aEntry )
    {
    RPointerArray<CCaInnerEntry> resultEntriesArray;
    CleanupResetAndDestroyPushL( resultEntriesArray );
    CCaInnerQuery* visibleEntriesQuery = CCaInnerQuery::NewLC();
    visibleEntriesQuery->SetParentId( aEntry->GetId() );
    visibleEntriesQuery->SetFlagsOn( EVisible );
    visibleEntriesQuery->SetFlagsOff( EMissing );
    iCaStorageProxy.GetEntriesL( visibleEntriesQuery, resultEntriesArray );
    if( resultEntriesArray.Count() )
        {
        // set collection visible if hidden
        if( HandleVisibleFlagUpdate( aEntry ) )
            {
            // update here this collection
            iCaStorageProxy.AddL( aEntry );
            }
        }
    CleanupStack::PopAndDestroy( visibleEntriesQuery );
    CleanupStack::PopAndDestroy( &resultEntriesArray );
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------//
TBool CCaSrvAppScanner::UpdateComponentIdL( CCaInnerEntry& aEntry ) const
    {
    TBool updated( EFalse );
    
    TUid uid;
    TInt err(KErrNone);
    TComponentId componentId( 0 );
    TRAP(err, componentId = 
            iSoftwareRegistry.GetComponentIdForAppL(
            uid.Uid( aEntry.GetUid() ) ) )
    
    if ( componentId > 0 && err == KErrNone )
        {
        RBuf newComponentId;
        newComponentId.CleanupClosePushL();
        newComponentId.CreateL( sizeof(TComponentId) + 1 );
        newComponentId.AppendNum( componentId );
        
        RBuf oldComponentId;
        oldComponentId.CleanupClosePushL();
        oldComponentId.CreateL( KCaMaxAttrValueLen );
        
        const TBool componentIdAttributeFound = 
            aEntry.FindAttribute( KCaComponentId, oldComponentId );
              
        if ( !componentIdAttributeFound 
            || oldComponentId.Compare( newComponentId ) != 0 )
            {
            // 'add' or 'update' the component id attribute value
            aEntry.AddAttributeL( KCaComponentId, 
                newComponentId );
            
            updated = ETrue;
            }
        
        CleanupStack::PopAndDestroy( &oldComponentId );
        CleanupStack::PopAndDestroy( &newComponentId );
        }
    return updated;
    }
