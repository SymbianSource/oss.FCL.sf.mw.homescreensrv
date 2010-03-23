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
* Description: casrvappscanner.h
*
*/

#ifndef CASRVAPPSCANNER_H_
#define CASRVAPPSCANNER_H_

#include <e32base.h>
#include <apgcli.h>
#include <apgnotif.h>
#include <driveinfo.h>
#include "cainstallnotifier.h"
#include "castorage_global.h"

class TCaAppAtributes;
class CCaSrvMmcHistory;
class CCaInnerEntry;
class CCaStorageProxy;
class CCaSrvEngUtils;
namespace Swi
    {
    class RSisRegistryEntry;
    }

/**
 * Application scanner.
 * Using AppArc application information, keeps menu application
 * items up-to-date.
 *
 * Operation is asynchronous (because AppArc startup is asynchronous):
 * 1. ConstructL(): invokes iApaLsSession to build app list (async).
 * 2. RunL(): App list is complete - appscanner update entries with application.
 * 4. HandleAppListEvent(): change in app list, go back to step 2.
 *
 * App scanning does the following:
 * - Applications present in the system but not appearing in the menu are added
 *   to the appropriate folder.
 * - Hide hidden applications, show non-hidden applications (flag "hidden").
 * - Hide / show MMC-related items, according to whether MMC is present or not
 *   (flag "missing"). Keep an MMC history, purge old MMC-based menu items
 *   (== missing menu items whose MMC card has not been seen for long).
 * - Observing AppArc (and MMC, etc etc) to keep the menu data up
 *   to date continuously.
 */

NONSHARABLE_CLASS( CCaSrvAppScanner ):
    public CActive,
    public MCaInstallListener,
    public MApaAppListServObserver
    {

public:
    // Constructor

    /**
     * Destructor.
     */
    virtual ~CCaSrvAppScanner();

    /**
     * Two-phased constructor.
     * @param aCaStorageProxy storage.
     * @param aUtils storage utils.
     */
    static CCaSrvAppScanner* NewL( CCaStorageProxy& aCaStorageProxy,
            CCaSrvEngUtils& aUtils );

private:
    // Constructors and destructor

    /**
     * Constructor.
     * @param aCaStorageProxy storage.
     * @param aUtils storage utils.
     */
    CCaSrvAppScanner( CCaStorageProxy& aCaStorageProxy,
            CCaSrvEngUtils& aUtils );

    /**
     * Second phased constructor.
     */
    void ConstructL();

public:
    // from MCaInstallListener

    /**
     * Handle Install Event.
     * @since S60 v5.0
     * @param aUid installed uid.
     */
    void HandleInstallNotifyL( TInt aUid );

private:

    /**
     * Notifies storage abaut change for aAppUid.
     * @param aAppUid application uid.
     */
    void NotifyL( TInt aAppUid );

    /**
     * Notifies storage abaut change for aPackageEntry related app.
     * @param aPackageEntry application package entry.
     */
    void NotifyL( Swi::RSisRegistryEntry & aPackageEntry );

private:
    // from CActive

    /**
     * Request completed, schedule engine operation.
     */
    void RunL();

    /**
     * Cancel request.
     */
    void DoCancel();

    /**
     * Error handling: ignore error.
     * @param aErr Error code.
     */
    TInt RunError( TInt aError );

private:
    // from MApaAppListServObserver

    void HandleAppListEvent( TInt aEvent );

public:
    /**
     * Schedule appscanner run.
     * Self complete active object.
     */
    void ScheduleScan();

private:
    // new methods

    /**
     * Gets collectionId of download collection
     * @result id of download collection
     */
    TInt GetCollectionDownloadIdL();

    /**
     * Adds application entry to downloaded collection
     * @param aEntryId application entry id.
     */
    void AddEntryToDownloadedCollectionL( TInt aEntryId );

    /**
     * Removes application entry from downloaded collection
     * @param aEntryId application entry id.
     */
    void RemoveEntryFromDownloadedL( TInt aEntryId );

    /**
     * Handles removal of stub items from download collection
     * @param aEntryId application entry id.
     */
    void HandleStubUpdatesL ();

    TBool HandleLockDeleteFlagUpdateL( CCaInnerEntry* aItem );

    /**
     * Updates application's missing attribute.
     * Add new menu item referring to this application.
     * @param aItem menu item.
     * @return ETrue if missing flag was updated
     */
    TBool HandleMissingFlagUpdate( CCaInnerEntry* aItem );

    /**
     * Updates hidden flag.
     * @param aItem menu item.
     * @param aUid Application's uid.
     * @return ETrue if hidden flag was updated
     */
    TBool HandleHiddenFlagUpdateL( CCaInnerEntry* aItem );

    /**
     * Updates used flag.
     * @param aItem menu item.
     * @param aUid Application's uid.
     * @return ETrue if used flag was updated
     */
    TBool HandleUsedFlagUpdateL( CCaInnerEntry* aItem );
    /**
     * Updates mmc attribute.
     * @param aItem menu item.
     * @param aUid Application's uid.
     * @param aMmcId MMC ID of currently inserted MMC, or 0.
     * @return ETrue if mmc attribute was updated
     */
    TBool HandleMmcAttrUpdateL( CCaInnerEntry* aItem, TUint aMmcId );

    /**
     * Check if application is in rom
     * Add new menu item referring to this application.
     * @param aUid uid applications
     * @return ETrue if app is in rom
     */
    TBool IsInRom( TInt aUid );

    /**
     * Get applications from AppArc
     * @param aArray RArray with applications
     */
    void GetApaItemsL( RArray<TCaAppAtributes>& aArray );

    /**
     * Remove sat applications from array(found in AppArc)
     * @param aArray RArray with application
     */
    void RemoveSatApp( RArray<TCaAppAtributes>& aArray );

    /**
     * Get applications from CaStorage
     * @param aArray RPointerArray with CCaInnerEntries
     * contains applications
     */
    void GetCaAppEntriesL( RPointerArray<CCaInnerEntry>& aArray );

    /**
     * Remove sat application from array(found in CaStorage)
     * @param aArray RPointerArray with CCaInnerEntries
     * contains applications
     */
    void RemoveSatAppL( RPointerArray<CCaInnerEntry>& aArray );
    /**
     * Get application with specified uid from CaStorage
     * @param aUid RPointerArray with CCaInnerEntries
     * @param aArray RPointerArray with CCaInnerEntries
     * contains application
     */
    void GetCaAppEntriesL( TInt aUid, RPointerArray<CCaInnerEntry>& aArray );

    /**
     * New application is found, no menu item refers to it.
     * Add new menu item referring to this application.
     * @param aUid Application capability.
     * @param aCurrentMmcId MMC ID of currently inserted MMC, or 0.
     */
    void AddAppEntryL( TUint aUid, TUint aCurrentMmcId );

    /**
     * Removes given uid from iInstalledPackages.
     * Add new menu item referring to this application.
     * @param aUid Application capability.
     */
    void RemoveFromInstalledPackages( TUint aUid );

    /**
     * Set information from TApaAppInfo to entry
     * @param aEntry CCaInnerEntry with application.
     * @return ETrue if app info was updatedd
     */
    TBool SetApaAppInfoL( CCaInnerEntry* aEntry );

    /**
     * Find the default folder (folder having attribute 'default="1"').
     * If not found, return 0.
     * @param aFolderId Dedault folder or 0.
     */
    void DefaultFolderL( TInt& aFolderId );

    /**
     * Handle items are in the menu but no corresponding app is present:
     * - Items that are on a recently used MMC are get "missing" flag but kept.
     * - Items that are not on MMC or the MMC is not recently used, are
     *   removed form the menu.
     * @param aItems Missing items.
     */
    void HandleMissingItemsL( RPointerArray<CCaInnerEntry>& aCaEnties );

    /**
     * Removes app from storage.
     * @param aAppEntry app to remove.
     */
    void CCaSrvAppScanner::RemoveAppL( CCaInnerEntry* aAppEntry );

    /**
     * Adds flag objects(only one flag at time).
     * @param aEntry entry.
     * @param aFlag menu item's flag.
     */
    void AddObjectFlagL( CCaInnerEntry* aEntry, const TInt& aFlag );

    /**
     * Check currently inserted MMC card, update and save MMC history.
     * @return Current MMC id, or 0 if no MMC is inserted.
     */
    TUint UpdateMmcHistoryL();

    /**
     * Get current MMC id. 0 if no MMC is inserted, or in case of any errors.
     * @return Current MMC id, or 0.
     */
    TUint CurrentMmcId() const;

    /**
     * Check if file is on given default drive type.
     * @param aFileName File name.
     * @return ETrue if aFileName is on given default drive type.
     */
    TBool IsFileInDrive( const TDesC& aFileName,
            const DriveInfo::TDefaultDrives& aDefaultDrive ) const;

    /**
     * Check if application is installed on given drive type.
     * @param aUid app uid.
     * @param aDefaultDrive drive type.
     * @return ETrue if app is installed on given drive type.
     */
    TBool IsAppInDrive( const TUid aUid,
            const DriveInfo::TDefaultDrives& aDefaultDrive ) const;

    /**
     * Check if application is installed on MMC.
     * @param aUid app uid.
     * @return ETrue if app is installed on MMC.
     */
    TBool IsInMmc( const TUid aUid ) const;

    /**
     * Check if application is installed on mass storage.
     * @param aUid app uid.
     * @return ETrue if app is installed on mass storage.
     */
    TBool IsInMassStorage( const TUid aUid ) const;

    /**
     * Check if drive's status is EDriveInUse.
     * @param aDefaultDrive default drive type.
     * @return ETrue if drive is in use.
     */
    TBool IsDriveInUse( const DriveInfo::TDefaultDrives& aDefaultDrive );

    /**
     * Add application to storage or update entry if necessary
     * @param aCaEnties with applications from storage
     * @param aApaItem application item with uid and hidden flag on or off
     * @param aMmcId unique mmc id
     */
    void UpdateApplicationEntryL( RPointerArray<CCaInnerEntry>& aCaEnties,
            const TCaAppAtributes& aApaItem, TUint aMmcId );

    /**
     * Update entry from storage
     * @param aEntry application entry
     * @param aMmcId unique mmc id
     */
    void UpdateAppEntryL( CCaInnerEntry* aEntry, TUint aMmcId );

    /**
     * Get all application from storage and visible from apparc
     * and update if necessary
     */
    void UpdateApplicationEntriesL();

    /**
     * Notifys storage abaut updated apps
     */
    void InstallationNotifyL();

private:
    // data

    RApaLsSession iApaLsSession; ///< AppArc session. Own.
    CApaAppListNotifier* iNotifier; ///< Change notifier. Own.
    CCaInstallNotifier* iInstallNotifier;///< Install notifier. Own.
    CCaSrvMmcHistory* iMmcHistory; ///< MMC history. Own.
    RFs iFs; ///< File Server Session. Own.

    CCaStorageProxy& iCaStorageProxy;///< not own
    CCaSrvEngUtils& iSrvEngUtils;//<not own
    RArray<TInt> iInstalledPackages;

    TInt iCollectionDownloadId;
CA_STORAGE_TEST_FRIEND_CLASS    (T_casrvAppScaner)

    };

/**
 * Application atributes.
 * Object contains uid and hidden flag
 */
class TCaAppAtributes
    {

public:
    /**
     * Constructor.
     * @param aUid uid application.
     */
    TCaAppAtributes( TUint aUid );

    /*
     * Get uid
     * @return application uid
     */
    TUint GetUid() const;

    /*
     * Match application items
     * @param item1 first item to matching.
     * @param item2 second item to matching.
     */
    static TBool MatchItems( const TCaAppAtributes& item1,
            const TCaAppAtributes& item2 );

private:
    // data

    TUint iUid;///< uid application

    };

#endif /* CASRVAPPSCANNER_H_ */
