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
// CCaSrvAppScanner::NewL
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
// CCaSrvAppScanner::~CCaSrvAppScanner
// ---------------------------------------------------------
//
CCaSrvAppScanner::~CCaSrvAppScanner()
    {
    Cancel();
    iApaLsSession.Close();
    iInstalledPackages.Close();
    delete iInstallNotifier;
    delete iNotifier;
    delete iMmcHistory;
    iFs.Close();
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::CCaSrvAppScanner
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
// CCaSrvAppScanner::ConstructL
// ---------------------------------------------------------
//
void CCaSrvAppScanner::ConstructL()
    {
    User::LeaveIfError( iFs.Connect() );
    TFileName path;
    User::LeaveIfError( iFs.PrivatePath( path ) );
    TUint attribute;
    if( iFs.Att( path, attribute) == KErrNotFound )
        {
        TInt mdRes = iFs.MkDirAll( path );
        if ( mdRes != KErrNone )
            {
            User::Leave( mdRes );
            }
        }
    iMmcHistory = new (ELeave) CCaSrvMmcHistory();
    iMmcHistory->LoadL( iFs, KCaMmcHistoryFname() );
    // The notifier has its own session to apparc, instead of taking
    // it as argument... :(
    iNotifier = CApaAppListNotifier::NewL(
            this, CActive::EPriorityStandard );
    iInstallNotifier = CCaInstallNotifier::NewL(
            *this, CCaInstallNotifier::ESisInstallNotification );

    User::LeaveIfError( iApaLsSession.Connect() );
    User::LeaveIfError( iApaLsSession.GetAllApps() ); // This is async.
    iApaLsSession.RegisterListPopulationCompleteObserver( iStatus );
    iCollectionDownloadId = 0;
    SetActive();
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::RunL
// ---------------------------------------------------------
//
void CCaSrvAppScanner::RunL()
    {
    User::LeaveIfError( iStatus.Int() ); // Handle errors in RunL.
    // AppArc app scan complete, we have the app list.
    UpdateApplicationEntriesL();
    InstallationNotifyL();
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::DoCancel
// ---------------------------------------------------------
//
void CCaSrvAppScanner::DoCancel()
    {
    iApaLsSession.CancelListPopulationCompleteObserver();
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::RunError
// ---------------------------------------------------------
//
TInt CCaSrvAppScanner::RunError( TInt /*aError*/)
    {
    // Ignore the error (what else could we do?).
    // When next AppArc update occurs, we will run again.
    return KErrNone;
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::UpdateApplicationItemL
// ---------------------------------------------------------
//
void CCaSrvAppScanner::UpdateApplicationEntryL(
        RPointerArray<CCaInnerEntry>& aCaEntries,
        const TCaAppAtributes& aApaItem, TUint aMmcId )
    {
    TInt appuid = aApaItem.GetUid();
    RPointerArray<CCaInnerEntry> resultArray;
    CleanupResetAndDestroyPushL( resultArray );
    GetCaAppEntriesL( appuid, resultArray );

    // This app is not in the storage, add it now.
    // We don't add hidden items, there are too many of them!
    // do not display Menu app
    if( !resultArray.Count() )
        {
        AddAppEntryL( appuid, aMmcId );
        }//if

    // "removable", "missing" and "visible"  flags update
    for( TInt j = 0; j < resultArray.Count(); j++ )
        {
        //we need to handle first run of appscanner,
        //there might be some incorrect data in content xml file
        //if this will have impact on performance we may run this methods only at start up

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
// CCaSrvAppScanner::UpdateAppEntryL
// ---------------------------------------------------------
//
void CCaSrvAppScanner::UpdateAppEntryL( CCaInnerEntry* aEntry, TUint aMmcId )
    {
    TBool toUpdate = HandleMmcAttrUpdateL( aEntry, aMmcId );
    toUpdate = HandleHiddenFlagUpdateL( aEntry ) || toUpdate;
    toUpdate = HandleMissingFlagUpdate( aEntry ) || toUpdate;
    toUpdate = HandleLockDeleteFlagUpdateL( aEntry ) || toUpdate;
    toUpdate = SetApaAppInfoL( aEntry ) || toUpdate;
       
    if( iInstalledPackages.Find( aEntry->GetUid() ) != KErrNotFound )
        {
        AddEntryToDownloadedCollectionL( aEntry->GetId() );
        toUpdate = HandleUsedFlagUpdateL( aEntry ) || toUpdate;
        }
    if( toUpdate )
        {
        //update app in storage
        iCaStorageProxy.AddL( aEntry );
        RemoveFromInstalledPackages( aEntry->GetUid() );
        }
    }
// ---------------------------------------------------------
// CCaSrvAppScanner::HandleHiddenFlagUpdateL
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::HandleHiddenFlagUpdateL( CCaInnerEntry* aItem )
    {
    TBool toChange( EFalse );
    TBool itemHidden = ( 0 == ( aItem->GetFlags() & EVisible ) );
    if( itemHidden )
        {
        aItem->SetFlags( aItem->GetFlags() | EVisible );
        toChange = ETrue;
        }
    return toChange;
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::HandleUsedFlagUpdateL
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::HandleUsedFlagUpdateL( CCaInnerEntry* aItem )
    {
    if( aItem->GetFlags() & EUsed )
        {
        aItem->SetFlags( aItem->GetFlags() & ~EUsed );
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::HandleMmcAttrUpdateL
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::HandleMmcAttrUpdateL(
        CCaInnerEntry* aItem, TUint aMmcId )
    {
    TBool toUpdate( ETrue );
    if( IsInMmc( TUid::Uid( aItem->GetUid() ) ) )
        {
        //app is instaled on mmc - KCaAttrMmcId attribute update
        TBuf<KUidChars> uidString;
        uidString.Format( KHexFormat, aMmcId );
        aItem->AddAttributeL( KCaAttrMmcId, uidString );
        }
    else if ( IsInMassStorage( TUid::Uid( aItem->GetUid() ) ) )
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
// CCaSrvAppScanner::UpdateApplicationItemsL
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
// CCaSrvAppScanner::UpdateApplicationItemsL
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

    RArray<TCaAppAtributes> apaItems;
    CleanupClosePushL( apaItems );
    GetApaItemsL( apaItems );
    RemoveApp( apaItems, KSatUid.iUid );
    RemoveApp( apaItems, KHsAppUid.iUid );

    //for every item in apaAndCrItems array
    for( TInt i = 0; i < apaItems.Count(); i++ )
        {
            // if there was leave for any item we ignore it
            // and proceed to the next one
            TRAP_IGNORE(UpdateApplicationEntryL(
                            resultArray, apaItems[i], currentMmcId));
        }
    // Here the big list cwith items that refer to missing apps.
    HandleMissingItemsL( resultArray );
    CleanupStack::PopAndDestroy( &apaItems );
    CleanupStack::PopAndDestroy( &resultArray );
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::HandleLockDeleteFlagUpdateL
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::HandleLockDeleteFlagUpdateL( CCaInnerEntry* aItem )
    {
    TBool toChange( EFalse );
    TBool isVisible = ( ( aItem->GetFlags() & EVisible ) != 0 );
    if( isVisible && IsInRom( aItem->GetUid() ) )
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
// CCaSrvAppScanner::HandleMissingFlagUpdateL
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::HandleMissingFlagUpdate( CCaInnerEntry* aItem )
    {
    if( aItem->GetFlags() & EMissing )
        {
        //application found so we unset "missing" flag
        aItem->SetFlags( aItem->GetFlags() & ~EMissing );
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::RemoveApp
// ---------------------------------------------------------
//
void CCaSrvAppScanner::RemoveApp( RArray<TCaAppAtributes>& aArray, TInt32 aUid )
    {
    TCaAppAtributes app( aUid );
    TInt id = aArray.Find( app, TCaAppAtributes::MatchItems );
    if( id != KErrNotFound )
        {
        aArray.Remove( id );
        }
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::RemoveSatApp
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
// CCaSrvAppScanner::HandleHsAppEntryL
// ---------------------------------------------------------
//
void CCaSrvAppScanner::HandleHsAppEntryL( RPointerArray<CCaInnerEntry>& aArray )
    {
    CCaInnerEntry* appEntry = CCaInnerEntry::NewLC();
    appEntry->SetUid( KHsAppUid.iUid );
    TInt index = aArray.Find(
            appEntry, TIdentityRelation<CCaInnerEntry>( UidMatch ) );
    
    if ( index != KErrNotFound )
        { // hs app already in storage - ensure it is hidden and remove from resultArray
        if ( ( aArray[index]->GetFlags() & EVisible ) != 0 )
            {
            aArray[index]->SetFlags( aArray[index]->GetFlags() & ~EVisible);
            iCaStorageProxy.AddL( aArray[index] );
            }        
        delete aArray[index];
        aArray.Remove( index );        
        }
    else
        { // if not found add as not visible to the storage
        appEntry->SetEntryTypeNameL( KCaTypeApp );
        appEntry->SetFlags( 0 ); 
        appEntry->SetRole( EItemEntryRole );
        SetApaAppInfoL( appEntry );
        iCaStorageProxy.AddL( appEntry );
        }
    CleanupStack::PopAndDestroy( appEntry );
    }


// ---------------------------------------------------------
// CCaSrvAppScanner::GetApaItemsL
// ---------------------------------------------------------
//
void CCaSrvAppScanner::GetApaItemsL( RArray<TCaAppAtributes>& aArray )
    {
    TApaAppInfo appInfo;
    TApaAppCapabilityBuf appCap;

    User::LeaveIfError( iApaLsSession.GetAllApps( 0 ) );
    // for every application get uid, hidden and missing attribute
    // and add to aArray.
    while( KErrNone == iApaLsSession.GetNextApp( appInfo ) )
        {
        User::LeaveIfError( iApaLsSession.GetAppCapability(
                appCap, appInfo.iUid ) );
        // "Hidden" status according to AppArc.
        if( !appCap().iAppIsHidden )
            {
            TCaAppAtributes appAtributes( appInfo.iUid.iUid );
            aArray.AppendL( appAtributes );
            }
        }
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::GetMcsItemsL
// ---------------------------------------------------------
//
void CCaSrvAppScanner::GetCaAppEntriesL(
        RPointerArray<CCaInnerEntry>& aArray )
    {
    CCaInnerQuery* allAppQuery = CCaInnerQuery::NewLC();
    CDesC16ArrayFlat* appType = new ( ELeave ) CDesC16ArrayFlat( 1 );
    CleanupStack::PushL( appType );
    appType->AppendL( KCaTypeApp );
    allAppQuery->SetEntryTypeNames( appType );
    CleanupStack::Pop( appType );
    iCaStorageProxy.GetEntriesL( allAppQuery, aArray );
    CleanupStack::PopAndDestroy( allAppQuery );
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::GetMcsItemsL
// ---------------------------------------------------------
//
TInt CCaSrvAppScanner::GetCollectionDownloadIdL()
    {
    if( iCollectionDownloadId == 0 )
        {
        RPointerArray<CCaInnerEntry> resultArray;
        CleanupResetAndDestroyPushL( resultArray );
        CCaInnerQuery* allAppQuery = CCaInnerQuery::NewLC();
        CDesC16ArrayFlat* appType = new ( ELeave ) CDesC16ArrayFlat( 1 );
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
// CCaSrvAppScanner::GetMcsItemsL
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
// CCaSrvAppScanner::HandleAppListEvent
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
// CCaSrvAppScanner::HandleInstallNotifyL
// ---------------------------------------------------------
//
void CCaSrvAppScanner::HandleInstallNotifyL( TInt aUid )
    {
    NotifyL( aUid );
    RArray<TUid> uids;
    CleanupClosePushL( uids );

    Swi::RSisRegistrySession iSisRegSession;
    User::LeaveIfError( iSisRegSession.Connect() );
    CleanupClosePushL( iSisRegSession );

    // Open sis package entry related to aUid
    Swi::RSisRegistryEntry packageEntry;
    if( KErrNone
            == packageEntry.Open( iSisRegSession, TUid::Uid( aUid ) ) )
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
                HandleInstallNotifyL( embedded[i]->Uid().iUid );
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
    CleanupStack::PopAndDestroy( &uids );
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::NotifyL
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
// CCaSrvAppScanner::NotifyL
// ---------------------------------------------------------
//
void CCaSrvAppScanner::NotifyL( TInt aAppUid )
    {
    // Get entries for given aAppUid
    RPointerArray<CCaInnerEntry> resultArray;
    CleanupResetAndDestroyPushL( resultArray );
    GetCaAppEntriesL( aAppUid, resultArray );
    if( resultArray.Count() )
        {
        iCaStorageProxy.AddL( resultArray[0] );
        }
    CleanupStack::PopAndDestroy( &resultArray );
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::AddEntryToDownloadedCollectionL
// ---------------------------------------------------------
//
void CCaSrvAppScanner::AddEntryToDownloadedCollectionL( TInt aEntryId )
    {
    TCaOperationParams params = { TCaOperationParams::EPrepend, GetCollectionDownloadIdL(), 0 // not used
    		};

    RArray<TInt> entryIds;
    CleanupClosePushL( entryIds );
    entryIds.AppendL( aEntryId );
    iCaStorageProxy.OrganizeL( entryIds, params );

    CleanupStack::PopAndDestroy( &entryIds );
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::RemoveToDownloadedCollectionL
// ---------------------------------------------------------
//
void CCaSrvAppScanner::RemoveEntryFromDownloadedL( TInt aEntryId )
    {
    TCaOperationParams params = { TCaOperationParams::ERemove, GetCollectionDownloadIdL(), 0 // not used
    		};

    RArray<TInt> entryIds;
    CleanupClosePushL( entryIds );
    entryIds.AppendL( aEntryId );
    iCaStorageProxy.OrganizeL( entryIds, params );

    CleanupStack::PopAndDestroy( &entryIds );
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::ScheduleScan
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
// CCaSrvAppScanner::AddAppItemL
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
    HandleLockDeleteFlagUpdateL( appEntry );
    HandleMmcAttrUpdateL( appEntry, aCurrentMmcId );

    iCaStorageProxy.AddL( appEntry );
    if( iInstalledPackages.Find( aUid ) != KErrNotFound )
        {
        AddEntryToDownloadedCollectionL( appEntry->GetId() );
        }
    RemoveFromInstalledPackages( aUid );

    CleanupStack::PopAndDestroy( appEntry );
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::RemoveFromInstalledPackages
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
// CCaSrvAppScanner::IsMidletL
// Checks if given app is midlet by reading the apptype uid (2nd uid)
// ---------------------------------------------------------------------------
//
TBool CCaSrvAppScanner::SetApaAppInfoL( CCaInnerEntry* aEntry )
    {
    TBool changed( EFalse );
    TApaAppInfo info;
    if( KErrNone == iSrvEngUtils.GetAppInfo( *aEntry, info ) )
        {
        RBuf attrVal;
        attrVal.CleanupClosePushL();
        attrVal.CreateL( KCaMaxAttrValueLen );
        aEntry->FindAttribute( KCaAttrLongName, attrVal );
        if( attrVal.Compare( info.iCaption ) != KErrNone
                || aEntry->GetText().Compare( info.iCaption )
                        != KErrNone )
            {
            aEntry->SetTextL( info.iCaption );
            aEntry->AddAttributeL( KCaAttrLongName, info.iCaption );
            changed = ETrue;
            }
        CleanupStack::PopAndDestroy( &attrVal );
        }
    return changed;
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::HandleMissingItemsL
// ---------------------------------------------------------
//
void CCaSrvAppScanner::HandleMissingItemsL(
        RPointerArray<CCaInnerEntry>& aCaEntries )
    {
    for( TInt i = 0; i < aCaEntries.Count(); i++ )
        {
        const TInt id = aCaEntries[i]->GetId();
        TUint mmcId = 0;
        RBuf attrVal;
        attrVal.CleanupClosePushL();
        attrVal.CreateL( KCaMaxAttrValueLen );
        if( aCaEntries[i]->FindAttribute( KCaAttrMmcId(), attrVal ) )
            {
            MenuUtils::GetTUint( attrVal, mmcId );
            if( mmcId && KErrNotFound != iMmcHistory->Find( mmcId )
                    && mmcId != CurrentMmcId() )
                {
                // This item is on an MMC which is currently in the MMC history.
                // Set it "missing" but keep it.
                AddObjectFlagL( aCaEntries[i], EMissing );
                }
            else if ( attrVal == KCaMassStorage()
                    && IsDriveInUse( DriveInfo::EDefaultMassStorage ) )
                {
                AddObjectFlagL( aCaEntries[i], EMissing );
                }
            else
                {
                RemoveAppL( aCaEntries[i] );
                }
            }
        else
            {
            RemoveAppL( aCaEntries[i] );
            }
        CleanupStack::PopAndDestroy( &attrVal );
        }
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::RemoveAppL
// ---------------------------------------------------------
//
void CCaSrvAppScanner::RemoveAppL( CCaInnerEntry* aAppEntry )
    {
    RArray<TInt> idsToRemove;
    CleanupClosePushL( idsToRemove );
    idsToRemove.AppendL( aAppEntry->GetId() );
    iCaStorageProxy.RemoveL( idsToRemove );
    CleanupStack::PopAndDestroy( &idsToRemove );
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::AddObjectFlagL
// ---------------------------------------------------------
//
void CCaSrvAppScanner::AddObjectFlagL(
        CCaInnerEntry* aEntry, const TInt& aFlags )
    {
    TBool itemFlagPresent = ( 0 != ( aEntry->GetFlags() & aFlags ) );
    if( !itemFlagPresent )
        {
        aEntry->SetFlags( aEntry->GetFlags() | aFlags );
        iCaStorageProxy.AddL( aEntry );
        }
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::UpdateMmcHistoryL
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
// CCaSrvAppScanner::CurrentMmcId
// ---------------------------------------------------------
//
TUint CCaSrvAppScanner::CurrentMmcId() const
    {
    // Get mmc id. Errors are ignored.
    TUint mmcId = 0;
    TInt mmcDrive;
    TInt err;
    err = DriveInfo::GetDefaultDrive(
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
// CCaSrvAppScanner::IsFileInDrive
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::IsFileInDrive(
        const TDesC& aFileName,
        const DriveInfo::TDefaultDrives& aDefaultDrive ) const
        {
        if ( aFileName.Length() )
            {
            TInt mmcDrive;
        TInt err = DriveInfo::GetDefaultDrive( aDefaultDrive, mmcDrive );
        if( !err )
            {
            TInt fileDrive;
            err = RFs::CharToDrive( aFileName[0], fileDrive );
            if( !err && fileDrive == mmcDrive )
                {
                return ETrue;
                }
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::IsAppInDrive
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::IsAppInDrive(
        const TUid aUid,
        const DriveInfo::TDefaultDrives& aDefaultDrive ) const
    {
    TBool ret( EFalse );
    TApaAppInfo info;
    TInt err = iApaLsSession.GetAppInfo( info, aUid );
    if( !err && IsFileInDrive( info.iFullName, aDefaultDrive ) )
        {
        ret = ETrue;
        }
    return ret;
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::IsInMmc
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::IsInMmc( const TUid aUid ) const
    {
    return IsAppInDrive( aUid, DriveInfo::EDefaultRemovableMassStorage );
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::IsInMassStorage
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::IsInMassStorage( const TUid aUid ) const
    {
    return IsAppInDrive( aUid, DriveInfo::EDefaultMassStorage );
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::IsInRom
// ---------------------------------------------------------
//
TBool CCaSrvAppScanner::IsInRom( TInt aUid )
    {
    return IsAppInDrive( TUid::Uid( aUid ), DriveInfo::EDefaultRom );
    }

// ---------------------------------------------------------
// CCaSrvAppScanner::IsDriveInUse
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

// ==================== MEMBER FUNCTIONS ====================

// ---------------------------------------------------------
// TAppAtributes::TAppAtributes
// ---------------------------------------------------------
//
TCaAppAtributes::TCaAppAtributes( TUint aUid )
    {
    iUid = aUid;
    }

// ---------------------------------------------------------
// TAppAtributes::GetUid
// ---------------------------------------------------------
//
TUint TCaAppAtributes::GetUid() const
    {
    return iUid;
    }

// ---------------------------------------------------------
// TAppAtributes::MatchItems
// ---------------------------------------------------------
//
TBool TCaAppAtributes::MatchItems( const TCaAppAtributes& item1,
        const TCaAppAtributes& item2 )
    {
    return item1.GetUid() == item2.GetUid();
    }

