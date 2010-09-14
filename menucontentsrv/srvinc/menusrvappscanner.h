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

#ifndef __MENUSRVAPPSCANNER_H__
#define __MENUSRVAPPSCANNER_H__

#include <e32base.h>
#include <apgcli.h>
#include <apgnotif.h>
#include <driveinfo.h>
#include <cenrepnotifyhandler.h>
#include "menuengoperation.h"
#include "mcssatnotifier.h"
#include "menuengfilter.h"
#include "mcsfreespaceobserver.h"
#include "mcsmenuitem.h"
#include "mcsmenunotifier.h"
#include "mcsinstallnotifier.h"


// FORWARD DECLARATION

class CMenuEng;
class CMenuSrvAppAttributes;
class CMenuSrvMmcHistory;
class CRepository;
class CCenRepNotifyHandler;
class CMenuSrvEngUtils;

/**
* Application scanner.
* Using AppArc application information, keeps menu application
* items ("menu:application") up-to-date.
*
* Operation is asynchronous (because AppArc startup is asynchronous):
* 1. ConstructL(): invokes iApaLsSession to build app list (async).
* 2. RunL(): App list is complete, this object schedules itself to Engine.
* 3. RunMenuEngOperationL(): The operation is executed by the engine.
* 4. HandleAppListEvent(): change in app list, go back to step 2.
*
* App scanning does the following:
* - Applications present in the system but not appearing in the menu are added
*   to the appropriate folder.
* - Java apps should appear / disappear. TODO not impemented yet!
* - Hide hidden applications, show non-hidden applications (flag "hidden").
* - Hide / show MMC-related items, according to whether MMC is present or not
*   (flag "missing"). Keep an MMC history, purge old MMC-based menu items
*   (== missing menu items whose MMC card has not been seen for long).
* - Observing AppArc (and MMC, and Java, etc etc) to keep the menu data up
*   to date continuously.
*/
NONSHARABLE_CLASS( CMenuSrvAppScanner )
: public CActive, public MMenuEngOperation, public MApaAppListServObserver,
    public MCenRepNotifyHandlerCallback, public MMcsSATNotifierCallback,
    MMcsFreeSpaceObserver, MMcsInstallListener
    {

public:     // Constructor

    /**
    * Destructor.
    */
    virtual ~CMenuSrvAppScanner();

    /**
    * Two-phased constructor.
    * @param aEng Engine.
    */
    static CMenuSrvAppScanner* NewL(
            CMenuEng& aEng,
            CMenuSrvEngUtils& aSrvEngUtils );

public:
    /**
     * Schedule appscanner run.
     * Self complete active object.
     */
    void ScheduleScan();

private:    // Constructors and destructor

    /**
    * Constructor.
    * @param aEng Engine.
    */
    CMenuSrvAppScanner( CMenuEng& aEng, CMenuSrvEngUtils& aSrvEngUtils );

    /**
    * Second phased constructor.
    */
    void ConstructL();

private:    // from CActive

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

private:    // from MMcsFreeSpaceObserver
    /**
     * Handle Free Space Event.
     */
    void HandleFreeSpaceEventL();

private:    // from MMenuEngOperation

    /**
    * Execute engine operation:
    * Do the application scan and update menu accordingly.
    */
    void RunMenuEngOperationL();

    /**
    * Error handling: ignore error.
    * @param aErr Error code.
    */
    void CompletedMenuEngOperation( TInt aErr );

private:    // from MApaAppListServObserver

    /**
    * Application list event.
    * @param aEvent only one type event exists.
    */
    void HandleAppListEvent( TInt aEvent );

private:    // from MCenRepNotifyHandlerCallback

    /**
    * Called if one of CR keys has changed
    * @param aKey Id of the key that has changed.
    * @param aNewValue The new value of the key.
    */
    void HandleNotifyString( TUint32 aKey, const TDesC16& aNewValue );

private:    // from MMcsSATNotifierCallback

    /**
    * Inherited from MNotifierMMcsSATNotifierCallbackCallback
    *
    * @since Series 60 3.1
    * @return void
    */
    void SATChangeL();

private:    // from MMcsInstallListener

    /**
    * Called application is installed.
    * @param aUid uid of an application.
    * @param aOperation operation type.
    */
    void HandleInstallNotifyL( TUid aUid,
    		CMcsInstallNotifier::TNotificationType aNotificationType  );

private:    // new methods

    /**
    */
    void HandleLockDeleteFlagUpdateL(
            const TMenuItem& aItem, const CMenuSrvAppAttributes& aApaItem );

    /**
    * Updates application's missing attribute.
    * Add new menu item referring to this application.
    * @param aItem menu item.
    */
    void HandleMissingFlagUpdateL(
            const TMenuItem& aItem, const CMenuSrvAppAttributes& aApaItem );

    /**
    * Notifies engine abaut application installation.
    */
    void InstallationNotifyL();

    /**
    * Removes uid from installed packages array.
    * @param aUid application uid.
    */
    void RemoveFromInstalledPackages( TUid aUid );

    /**
    * Removes uid from installed packages array.
    * @param aUid application uid.
    * @param aId Item ID.
    * @param aEvent event for engine to append.
    */
    CMenuEngObject& ModifiableObjectL( TUid aUid, TInt aId,
                TInt aEvent = RMenuNotifier::EItemAttributeChanged );

    /**
    * Updates hidden flag.
    * @param aItem menu item.
    * @param aUid Application's uid.
    * @param aApaItemHidden Application's hidden status from ApaLsSession.
    */
    void HandleHiddenFlagUpdateL(
            const TMenuItem & aItem,
            const CMenuSrvAppAttributes& aApaItem );
    /**
    * Updates native attribute.
    * @param aItem menu item.
    * @param aUid Application's uid.
    */
    void HandleNativeAttrUpdateL(
            const TMenuItem& aItem,
            const CMenuSrvAppAttributes& aApaItem );

    /**
    * Updates mmc attribute.
    * @param aItem menu item.
    * @param aUid Application's uid.
    * @param aMmcId MMC ID of currently inserted MMC, or 0.
    */
    void HandleMmcAttrUpdateL(
            const TMenuItem& aItem,
            const CMenuSrvAppAttributes& aApaItem,
            TUint aMmcId );

    /**
    */
    void GetCrItemsL( RPointerArray<CMenuSrvAppAttributes>& aArray );


    /**
    */
    void ParseUidsL( const TDesC& aHiddenApplications,
                                        RPointerArray<CMenuSrvAppAttributes>& aArray );

    /**
    */
    void SetHidden( const TDesC& aHiddenAppUid,
                                RPointerArray<CMenuSrvAppAttributes>& aArray );

    /**
    */
    void GetMcsAppItemsL( RArray<TMenuItem>& aArray);

    /**
    */
    void GetMcsAppItemsL( TBool aLegacy, const TUint aUid,
                                        RArray<TMenuItem>& aArray);


    /**
    * New application is found, no menu item refers to it.
    * Add new menu item referring to this application.
    * @param aUid Application capability.
    * @param aCurrentMmcId MMC ID of currently inserted MMC, or 0.
    */
    void AddAppItemL(
            const CMenuSrvAppAttributes& aApaItem,
            TUint aCurrentMmcId );

    TInt CreateInstallFolderL( const CMenuSrvAppAttributes& aApaItem );


    /**
    * Find menu folder which has the specified "applicationgroup_name"
    * attribute. If no such folder exists, create.
    * @param aAppGroupName value of the "applicationgroup_name" attribute
    * @param aFolderId Folder ID.
    */
    void AppGroupFolderL( const TDesC& aAppGroupName, TInt& aFolderId );

    /**
    * Find the default folder (folder having attribute 'default="1"').
    * If not found, return 0.
    * @param aFolderId Dedault folder or 0.
    */
    void DefaultFolderL( TInt& aFolderId );

    /**
    * Make sure that folder is writable (remove "lock_moveinto").
    * @param aFolder Folder id.
    */
    void EnsureFolderWritableL( TInt aFolder );

    /**
    * Handle items are in the menu but no corresponding app is present:
    * - Items that are on a recently used MMC are get "missing" flag but kept.
    * - Items that are not on MMC or the MMC is not recently used, are
    *   removed form the menu.
    * @param aItems Missing items.
    */
    void HandleMissingItemsL( const RArray<TMenuItem>& aItems );

    /**
    * Returns proper flag to be set for missing or hidden item.
    * @param aItem menu item.
    * @return item proper flags for hidding.
    */
    TMenuItem::TFlags GetProperFlagL( const TMenuItem& aItem );

    /**
    * Updates objects flags to aFlagPresent if needed.
    * @param aFlagValue value for flag.
    * @param aItem menu item.
    * @param aFlag menu item's flag.
    * @param aEvent menu item's event for engine.
    */
    void SetObjectFlagsL(
            TBool aFlagValue,
            const TMenuItem& aItem,
            const TMenuItem::TFlags& aFlag,
            const RMenuNotifier::TEvent& aEvent = RMenuNotifier::EItemAttributeChanged );

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
    TBool IsFileInDrive(
            const TDesC& aFileName,
            const DriveInfo::TDefaultDrives& aDefaultDrive ) const;

    /**
    * Check if application is installed on given drive type.
    * @param aUid app uid.
    * @param aDefaultDrive drive type.
    * @return ETrue if app is installed on given drive type.
    */
    TBool IsAppInDrive(
            const CMenuSrvAppAttributes& aApaItem,
            const DriveInfo::TDefaultDrives& aDefaultDrive ) const;

    /**
    * Check if application is installed on MMC.
    * @param aUid app uid.
    * @return ETrue if app is installed on MMC.
    */
    TBool IsInMmc( const CMenuSrvAppAttributes& aApaItem ) const;

    /**
    * Check if application is installed on mass storage.
    * @param aUid app uid.
    * @return ETrue if app is installed on mass storage.
    */
    TBool IsInMassStorage( const CMenuSrvAppAttributes& aApaItem ) const;

    /**
    * Check if application is installed on ROM.
    * @param aUid app uid.
    * @return ETrue if app is installed on ROM.
    */
    TBool IsInRomL( const CMenuSrvAppAttributes& aApaItem ) const;

    /**
    * Check if drive's status is EDriveInUse.
    * @param aDefaultDrive default drive type.
    * @return ETrue if drive is in use.
    */
    TBool IsDriveInUse(
    		const DriveInfo::TDefaultDrives& aDefaultDrive );

    /**
    TODO
    */
    void HideItemIfPresentL( TInt aFolder, TInt aId, const TDesC& aUid, TBool aWasHidden );

    /**
    TODO
    */
    void GetHiddenFlagAndUidL( TInt aId, TBool& aHidden, TPtrC& aUidStr );

    /**
    TODO
    */
    void SetHiddenFlagL( TInt aId, TBool aWasHidden, TBool aHide );

    /**
    TODO
    */
    void ValidateLinkUidL( TInt aFolder, TInt aId, const TDesC& aUidStr );

    /**
    TODO
    */
    void UpdateLinkL( TInt aId, TBool aExists, TBool aShowItem, TBool aMissingItem );

    /**
    TODO
    */
    void ValidateLinkRefIdL( TInt aId, const TDesC& refIdStr );

    /**
    TODO
    */
    void ValidateLinkL( TInt aFolder, TInt aId );

    /**
    TODO
    */
    void UpdateApplicationItemL(
    		RArray<TMenuItem>& aMcsItems,
            const CMenuSrvAppAttributes& aApaItem,
            TUint aMmcId,
            TBool isLegacy );

    /**
    TODO
    */
    void UpdateApplicationItemsL();

    /**
    TODO
    */
    void UpdateApplicationWithHideIfInstalledItemsL();

    /**
    TODO
    */
    void UpdateLinkItemsL();


private:    // data

    CMenuEng& iEng; ///< Engine.
    CMenuSrvEngUtils& iSrvEngUtils; ///< Server engine utils.
    CApaAppListNotifier* iNotifier; ///< Change notifier. Own.
    CMenuSrvMmcHistory* iMmcHistory; ///< MMC history. Own.
    RFs iFs; ///< File Server Session. Own.

    /*
    * Sat change notifier. Own
    */
    CMcsSatNotifier* iMcsSatNotifier;

    /*
    * Central repository session
    */
    CRepository* iCenRepSession;

    /*
    * Central repository change handler for Hidden Apps
    */
    CCenRepNotifyHandler* iCenRepNotifyHandlerHiddenApps;

    /*
    *Central repository change handler for Control Panel Hidden Apps
    */
    CCenRepNotifyHandler* iCenRepNotifyHandlerCPHiddenApps;

    /*
    *Central repository change handler for Control Panel Hidden Folders
    */
    CCenRepNotifyHandler* iCenRepNotifyHandlerCPHiddenFolders;

    /*
    * Free space observer. Own.
    */
    CMcsFreeSpaceObserver* iFreeSpaceObserver;

    TBool iOpStatus;

    RArray<TUid> iInstalledPackages; ///< Own.

    CMcsInstallNotifier* iSisInstallNotifier; ///< Native app notifier. Own.
    CMcsInstallNotifier* iJavaInstallNotifier; ///< Java app notifier. Own.

    };

#endif // __MENUSRVAPPSCANNER_H__
