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
* Description:  Bookmark list for settings listbox.
*
*/


#ifndef CMCSPLUGINSETTINGSBKMLIST_H
#define CMCSPLUGINSETTINGSBKMLIST_H

#include <e32base.h>
#include <bamdesca.h>               // For MDesCArray
#include <FavouritesDbObserver.h>   // For MFavouritesDbObserver
#include <FavouritesDb.h>           // For RFavouritesDb
#include <FavouritesItemList.h>     // For CFavouritesItemList
#include <mcsmenu.h>                // For MenuContentService

class CActiveFavouritesDbNotifier;
class CAiScutSettingsItem;
class CMCSPluginSettingsModel;
class CMCSPluginWatcher;
struct TSettingItem;
/**
 *  @ingroup group_mcsplugin
 *
 * Bookmark list for settings listbox
 *
 * @since S60 v3.2
 */
class CMCSPluginSettingsBkmList : public CBase, public MDesCArray
{

    /**
     * TBookmarkType
     */
    enum TBookmarkType
        {
        EFavBookmark,
        EMCSBookmark
        };

public:

    /**
     * Part of the two phased constuction
     */
    static CMCSPluginSettingsBkmList* NewL();

    /**
     * Destructor
     */
    virtual ~CMCSPluginSettingsBkmList();

    // from base class MDesCArray

    /**
     * From MDesCArray
     * Returns the number of descriptor elements in a descriptor array.
     *
     * @since S60 v3.2
     * @return The number of descriptor elements in a descriptor array.
     */
    TInt MdcaCount() const;

    /**
     * From MDesCArray
     * Indexes into a descriptor array.
     *
     * @since S60 v3.2
     * @param aIndex The position of the descriptor element within a descriptor 
     *               array.
     * @return A 16 bit non-modifiable pointer descriptor representing the 
     *         descriptor element located at position aIndex within a descriptor 
     *         array.
     */
    TPtrC MdcaPoint(TInt aIndex) const;

    /**
     * Gets the bookmark list.
     *
     * @since S60 v3.2
     */
    void GetBookmarkListL();

    /**
     * 
     *
     * @since S60 
     * @param aIndex Setting item to find
     * @return MCS menu item
     */
    CMenuItem& ItemL( TInt aIndex );

    /**
     * Returns target bookmark data from the given index
     *
     * @since S60 v3.2
     * @param aIndex Index of the bookmark in the list
     * @param aParams On return, the bookmark parameters
     * @param aCaption On return, the bookmark caption
     * @return KErrNotFound if the bookmark cannot be found, KErrNone otherwise
     */
    //TInt GetDataByIndex(TInt aIndex, TPtrC& aParams, TPtrC& aCaption) const;
    
    TSettingItem FindItemL( 
        RPointerArray<HSPluginSettingsIf::CPropertyMap>& aProperties );
    
    /**
     * Remove menu item
     * 
     * @param aIndex 
     */
    void RemoveMenuItemL( TInt aIndex );

protected:

private:

    /**
     * Constructor
     */
    CMCSPluginSettingsBkmList();
    
    /**
     * Second-phase constructor.
     */
    void ConstructL();

    /**
     * Adds an bookmark to the list
     *
     * @since S60 v3.2
     * @param aBkmInfo Bookmark information object
     */
    void AddBookmarkL( const TDesC&  aUid, const TDesC& aCaption, 
        const TDesC& aUrl, TBookmarkType aType );

    /**
     * Updates the bookmark list
     *
     * @since S60 v3.2
     */
    void UpdateBkmListL();

    /**
     * Get bookmarks from favourites
     */
    void GetBookmarksFromFavouritesL();

    /**
     * Get bookmarks from MCS
     */
    void GetBookmarksFromMCSL();

    /**
     * Create menu item
     * 
     * @param aUid
     * @param aName
     * @param aUrl
     * @return pointer to the created CMenuItem object 
     */
    CMenuItem* CreateMenuItemL( const TDesC& aUid,const TDesC& aName,
        const TDesC& aUrl );

    /**
     * MCSMenuItemL
     * 
     * @param aUid
     * @param aName
     * @param aUrl
     * @return pointer to the CMenuItem object
     */
    CMenuItem* MCSMenuItemL( const TDesC& aUid,const TDesC& aName,
        const TDesC& aUrl );

    /**
     * GetMCSPluginFolderIdL
     * 
     * @return TInt
     */
    TInt GetMCSPluginFolderIdL();

    /**
     * UpdateMenuItemsRefCountL
     * 
     * @param aItem
     * @param aValueToAdd
     * @return TInt
     */
    TInt UpdateMenuItemsRefCountL( CMenuItem* aItem, TInt aValueToAdd );


private:

    /**
     * Nested class to store individual bookmark list items
     *
     * @since S60 v3.2
     */
    class CBkmListItem : public CBase
    {
    public:

        /**
         * Part of the two phased constuction
         * 
         * @param aUid
         * @param aCaption
         */
        static CBkmListItem* NewLC( const TDesC&  aUid, const TDesC& aCaption);

        /**
         * Destructor
         */
        ~CBkmListItem();

        /**
         * Compare method used to add the items to the list in sorted order
         *
         * @since S60 v3.2
         * @param aFirst The first item to be compared
         * @param aSecond The second item to be compared
         * @return Negative if first comes before second, zero if equal and
         *         positive if first comes after second
         */
        static TInt CompareCaption(const CBkmListItem& aFirst,
            const CBkmListItem& aSecond);

        /**
         * Returns the item target bookmark uid
         *
         * @since S60 v3.2
         * @return Target bookmark uid
         */
        //TUid Uid() const;

        /**
         * Returns the item target bookmark caption
         *
         * @since S60 v3.2
         * @return Target bookmark caption
         */
        TPtrC Caption() const;

        /**
         * iType
         */
        TBookmarkType iType;

        /**
         * Target bookmark uid
         */
        HBufC* iUid;

        /**
         * Target bookmark caption
         */
        HBufC* iCaption;

        /**
         * Target bookmark parameters
         */
        HBufC* iUrl;

    private:

        /**
         * Constructor
         */
        CBkmListItem();

        /**
         * Second-phase constructor.
         * 
         * @param aUid
         * @param aCaption
         */
        void ConstructL( const TDesC&  aUid, const TDesC& aCaption);

    private:  // data


    };

private:  // data

    /**
     * List of bookmark items
     * Own.
     */
    RPointerArray<CBkmListItem> iListItems;

    // Runtime created CMenuItems must be stored, because those are refered 
    RPointerArray<CMenuItem> iMenuItems;

    /**
     * A flag indicating if the bookmark list should observe changes
     */
    TBool iObserving;

    /**
     * Bookmark database session.
     * Own.
     */
    RFavouritesSession iBookmarkSess;

    /**
     * Bookmark database.
     * Own.
     */
    RFavouritesDb iBookmarkDb;

    /**
     * iMenu
     */
    RMenu iMenu;

    /**
     * Save watcher
     */

    CMCSPluginWatcher* iSaveWatcher;

    /**
     * Update watcher
     */
    CMCSPluginWatcher* iUpdateWatcher;

    /**
     * Remove watcher
     */
    CMCSPluginWatcher* iRemoveWatcher;

    /**
     * MCS plugin folder ID
     */
    TInt iMCSPluginFolderId;

};

#endif // CMCSPLUGINSETTINGSBKMLIST_H

// End of File.
