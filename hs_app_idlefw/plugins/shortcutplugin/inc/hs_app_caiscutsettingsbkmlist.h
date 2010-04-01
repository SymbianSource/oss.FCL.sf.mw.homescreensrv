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
* Description:  Bookmark list for settings listbox.
*
*/


#ifndef CAISCUTSETTINGSBKMLIST_H
#define CAISCUTSETTINGSBKMLIST_H

#include <e32base.h>
#include <bamdesca.h>               // For MDesCArray
#include <favouritesdbobserver.h>   // For MFavouritesDbObserver

class CCoeEnv;
class CActiveFavouritesDbNotifier;
class CAiScutSettingsItem;
class CAiScutSettingsModel;

/**
 * Bookmark list for settings listbox
 *
 * @since S60 v3.2
 */
class CAiScutSettingsBkmList : public CBase
                             , public MDesCArray
                             , public MFavouritesDbObserver
{

public:

    static CAiScutSettingsBkmList* NewL(
        CCoeEnv* aEnv,
        CAiScutSettingsModel* aModel,
        MAiScutListObserver& aObserver);

    virtual ~CAiScutSettingsBkmList();

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
     * @param aIndex The position of the descriptor element within a descriptor array.
     * @return A 16 bit non-modifiable pointer descriptor representing the descriptor
     *         element located at position aIndex within a descriptor array.
     */
    TPtrC MdcaPoint(TInt aIndex) const;

    /**
     * Gets the bookmark list.
     *
     * @since S60 v3.2
     */
    void GetBookmarkListL();

    /**
     * Tells the bookmark list to start or stop observing for changes
     *
     * @since S60 v3.2
     * @param aObserving ETrue if changes should be observed, EFalse if not
     */
    void SetObservingL(TBool aObserving);

    /**
     * Finds the index of the given settings item in the bookmark list
     *
     * @since S60 v3.2
     * @param aItem Setting item to find
     * @return Index of the setting item in the bookmark list
     */
    TInt FindIndex(CAiScutSettingsItem& aItem);

    /**
     * Returns target bookmark data from the given index
     *
     * @since S60 v3.2
     * @param aIndex Index of the bookmark in the list
     * @param aParams On return, the bookmark parameters
     * @param aCaption On return, the bookmark caption
     * @return KErrNotFound if the bookmark cannot be found, KErrNone otherwise
     */
    TInt GetDataByIndex(TInt aIndex, TPtrC& aParams, TPtrC& aCaption) const;

protected:

private:

    CAiScutSettingsBkmList(
        CCoeEnv* aEnv,
        CAiScutSettingsModel* aModel,
        MAiScutListObserver& aObserver);

    void ConstructL();

    /**
     * Adds an bookmark to the list
     *
     * @since S60 v3.2
     * @param aBkmInfo Bookmark information object
     */
    void AddBookmarkL(TUid aUid, const TDesC& aCaption);

    /**
     * Updates the bookmark list
     *
     * @since S60 v3.2
     */
    void UpdateBkmListL();

    // From MFavouritesDbObserver
    /**
     * Handles database event.
     * @param aEvent Database event.
     */
    void HandleFavouritesDbEventL(RDbNotifier::TEvent aEvent);

private:

    /**
     * Nested class to store individual bookmark list items
     *
     * @since S60 v3.2
     */
    class CBkmListItem : public CBase
    {
    public:

        static CBkmListItem* NewLC(TUid aUid, const TDesC& aCaption);

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
        TUid Uid() const;

        /**
         * Returns the item target bookmark caption
         *
         * @since S60 v3.2
         * @return Target bookmark caption
         */
        TPtrC Caption() const;

        /**
         * Returns the possible parameters for item target
         *
         * @since S60 v3.2
         * @return Item target parameters
         */
        TPtrC Params() const;

        /**
         * Sets the parameters for the item target
         *
         * @since S60 v3.2
         * @param aParams Parameters for item target
         */
        void SetParamsL(const TDesC& aParams);

    private:

        CBkmListItem(TUid aUid);

        void ConstructL(const TDesC& aCaption);

    private:  // data

        /**
         * Target bookmark uid
         */
        TUid iUid;

        /**
         * Target bookmark caption
         */
        HBufC* iCaption;

        /**
         * Target bookmark parameters
         */
        HBufC* iParams;

    };

private:  // data

    /**
     * List of bookmark items
     * Own.
     */
    RPointerArray<CBkmListItem> iListItems;

    /**
     * Amount of all bookmarks in the device
     */
    TInt                        iTotalAppCount;

    /**
     * Pointer to the control environment
     * Not own.
     */
    CCoeEnv*                    iEnv;

    /**
     * Settings plug-in model.
     * Not own.
     */
    CAiScutSettingsModel*       iModel;

    /**
     * Registered observer for list events
     */
    MAiScutListObserver&        iObserver;

    /**
     * A flag indicating if the bookmark list should observe changes
     */
    TBool                       iObserving;

    /**
     * Bookmark database change observer.
     * Own.
     */
    CActiveFavouritesDbNotifier*    iBookmarkDbObserver;

};

#endif // CAISCUTSETTINGSBKMLIST_H

// End of File.
