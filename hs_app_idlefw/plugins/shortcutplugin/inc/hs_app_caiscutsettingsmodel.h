/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Shrortcut settings plug-in model.
*
*/


#ifndef CAISCUTSETTINGSMODEL_H
#define CAISCUTSETTINGSMODEL_H

#include <e32base.h>
#include <bamdesca.h>               // For MDesCArray
#include <apgcli.h>                 // For RApaLsSession
#include <favouritesdb.h>           // For RFavouritesDb
#include <favouritesitemlist.h>     // For CFavouritesItemList

#include "hs_app_caiscutsettingsapplist.h"
#include "hs_app_caiscutsettingsbkmlist.h"

class CCoeEnv;
class CRepository;
class CAiScutSettingsItem;
class CAiScutSettingsContainer;
class CAiScutRepositoryWatcher;
class CAiScutSettings;

/**
 * Shrortcut settings plug-in model
 *
 * @lib hs_app_aiscutsettings.lib
 * @since S60 v3.2
 */
class CAiScutSettingsModel : public CBase
                           , public MDesCArray
                           , public MAiScutListObserver
{

public:

    /**
     *
     */
    static CAiScutSettingsModel* NewL(CAiScutSettings& aPlugin, CCoeEnv* aEnv);

    /**
     *
     */
    virtual ~CAiScutSettingsModel();

public: // from MDesCArray

    /**
     * Returns the number of descriptor elements in a descriptor array.
     *
     * @since S60 v3.2
     * @return The number of descriptor elements in a descriptor array.
     */
    TInt MdcaCount() const;

    /**
     * Indexes into a descriptor array.
     *
     * @since S60 v3.2
     * @param aIndex The position of the descriptor element within a descriptor array.
     * @return A 16 bit non-modifiable pointer descriptor representing the descriptor
     *         element located at position aIndex within a descriptor array.
     */
    TPtrC MdcaPoint(TInt aIndex) const;

    /**
     * Handle Shortcuts changed
     */
    static TInt HandleShortcutsChanged(TAny* aPtr);

    /**
     * Handle hidden Applications changed
     */
    static TInt HandleHiddenAppsChanged(TAny* aPtr);

public: // from MAiScutListObserver

    /**
     * From MAiScutListObserver
     * Callback for application list events
     *
     * @since S60 v3.2
     * @param aEvent AppList event code
     * @param aAdded ETrue if applications were added, EFalse if removed
     */
    void HandleScutListEventL(TScutListEvent aEvent, TBool aAdded);

    /**
     * Find application is hidden using Uid
     */
    TBool IsHidden(const TUid& aAppUid) const;

public:

    /**
     * Tells the application list and bookmark list to start or stop observing for changes
     */
    void ActivateObservers(TBool aActivate);

    /**
     * Returns a setting item for the given index
     *
     * @since S60 v3.2
     * @param aIndex Index of the setting item
     * @return Pointer to a setting item
     */
    CAiScutSettingsItem* Item(TInt aIndex) const;

    /**
     * Utility function to retrieve an appliation caption from an uid
     *
     * @since S60 v3.2
     * @param aUid Application uid
     * @return Pointer descriptor to the application caption
     */
    TInt GetAppCaption(const TUid aUid, TDes& aCaption);

    /**
     * Utility function to retrieve a bookmark caption from an uid
     *
     * @since S60 v3.2
     * @param aUid Bookmark uid
     * @return Pointer descriptor to the bookmark caption
     */
    TInt GetBkmCaptionL(const TUid aUid, TDes& aCaption);

    /**
     * Returns a pointer to the coe environment
     *
     * @since S60 v3.2
     * @return Pointer to coe environment
     */
    CCoeEnv* Env();

    /**
     * Handles saving a setting item to central repository
     *
     * @since S60 v3.2
     * @param aItem Item to save
     */
    void SaveItemL(CAiScutSettingsItem& aItem);

    /**
     * Sets pointer to settings plug-in container.
     *
     * @since S60 v3.2
     * @param aContainer Pointer to settings plug-in container.
     */
    void SetContainer(CAiScutSettingsContainer* aContainer);

    /**
     * Read bookmarks from favourites engine.
     *
     */
    void ReadBookmarksL();

    /**
     * Return bookmark count.
     */
    TInt BookmarkCount() const;

    /**
     * Get bookmark from index aIndex.
     */
    CFavouritesItem* GetBookmark(TInt aIndex);

    /**
     * Returns reference to the bookmark database owned by the engine
     *
     * @return Reference to RFavouritesDb
     */
    RFavouritesDb& FavouritesDb();

    /**
     *
     */
    CAiScutSettingsAppList* AppList();

    /**
     *
     */
    CAiScutSettingsBkmList* BkmList();
    
    /**
     *
     */
    void SetSettingsKeys(RArray<TUint32>& aKeys);
    
    /**
     *
     */
    void UpdateSettingsL();    

    /**
     *
     */
    void UpdateSettingsContainerL();

private:

    /**
     *
     */
    CAiScutSettingsModel(CAiScutSettings& aPlugin, CCoeEnv* aEnv);

    /**
     *
     */
    void ConstructL();


    /**
     *
     */
    void GetHiddenAppsL();

private:  // data

    /**
     * Array of setting items.
     * Own.
     */
    RPointerArray<CAiScutSettingsItem>  iSettings;

    /**
     * Array of keys for settings items
     * Own.
     */    
    RArray<TUint32>                     iKeys;

    /**
     * Central repository handler.
     * Own.
     */
    CRepository*                        iRepository;

    /**
     * Application architecture server session.
     * Own.
     */
    RApaLsSession                       iApaSession;

    /**
     * Bookmark database session.
     * Own.
     */
    RFavouritesSession                  iBookmarkSess;

    /**
     * Bookmark database.
     * Own.
     */
    RFavouritesDb                       iBookmarkDb;

    /**
     * Bookmark list.
     * Own.
     */
    CFavouritesItemList*                iFavItemList;

    /**
     * Settings plugin
     * Not own.
     */    
    CAiScutSettings&                    iPlugin;     

    /**
     * Coe environment.
     * Not own.
     */
    CCoeEnv*                            iEnv;

    /**
     * Application list.
     * Own.
     */
    CAiScutSettingsAppList*             iAppList;

    /**
     * Bookmark list.
     * Own.
     */
    CAiScutSettingsBkmList*             iBkmList;

    /**
     * Notifier for changes in the shortcut settings.
     * Own.
     */
    CAiScutRepositoryWatcher*           iSettingsNotifier;

    /**
     * Settings plugin container.
     * Not own.
     */
    CAiScutSettingsContainer*           iContainer;

    // Array of UIDs which are hidden from the TARM
    RArray<TUid>                        iHiddenApps;

    // Central repository object for Hidden Apps
    CRepository*                        iHiddenAppsRepository;

    // Central repository change handler for Hidden Apps
    CAiScutRepositoryWatcher*           iHiddenAppsNotifier;

};

#endif // CAISCUTSETTINGSMODEL_H

// End of File.
