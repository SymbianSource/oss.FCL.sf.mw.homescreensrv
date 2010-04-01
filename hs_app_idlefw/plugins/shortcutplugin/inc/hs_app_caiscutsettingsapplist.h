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
* Description:  Application list for settings listbox.
*
*/


#ifndef CAISCUTSETTINGSAPPLIST_H
#define CAISCUTSETTINGSAPPLIST_H

#include <e32base.h>
#include <bamdesca.h>   // For MDesCArray
#include <apgcli.h>     // For RApaLsSession
#include <msvapi.h>     // For MMsvSessionObserver
#include <apgnotif.h>   // For MApaAppListServObserver
#include "hs_app_aiscutsettings.hrh" // For TAiScutSettingType

class CAiScutSettingsItem;
class CCoeEnv;

/**
 * Shortcut definition parameter for theme change shortcut.
 */
_LIT( KScutParamChangeTheme,     "view=0x102750A7" );

/**
 * Shortcut definition parameter for messaging. New message
 */
_LIT( KScutParamNewMsg,          "new=msg" );

/**
 * Shortcut definition parameter for messaging. New email
 */
_LIT( KScutParamNewEmail,        "new=email" );

/**
 * Shortcut definition parameter for messaging. New syncml mail
 */
_LIT( KScutParamNewSyncMLMail,   "new=syncmlmail" );

/**
 * Shortcut definition parameter for messaging. New postcard
 */
_LIT( KScutParamNewPostcard,     "new=postcard" );

/**
 * Shortcut definition parameter for messaging. New audio message
 */
_LIT( KScutParamNewAudioMsg,     "new=audiomsg" );

/**
 * Shortcut definition parameter for messaging. Select message type
 */
_LIT( KScutParamSelectMsgType,   "new" );

/**
 * Shortcut definition parameter for messaging. Remote mailbox
 */
_LIT( KScutParamMailbox,         "mailbox=" );

/**
 * Shortcut definition parameter for general settings. Connectivity view
 */
_LIT( KScutParamConnectivityView,     "view=0x10207250" );

/**
 * Shortcut definition parameter for general settings. Installations view
 */
_LIT( KScutParamInstallationsView,     "view=0x10283321" );


/**
 * Observer interface for application list events
 *
 * @since S60 v3.2
 */
class MAiScutListObserver
{
public:

    /**
     * AppList event codes
     */
    enum TScutListEvent
    {
        EAppListReady,
        EAppListUpdated,
        EBkmListUpdated
    };

    /**
     * Callback for application list events
     *
     * @since S60 v3.2
     * @param aEvent AppList event code
     * @param aAdded ETrue if applications were added, EFalse if removed
     */
    virtual void HandleScutListEventL( TScutListEvent aEvent, TBool aAdded ) = 0;

    virtual TBool IsHidden(const TUid& aAppUid) const = 0;

};


/**
 * Application list for settings listbox
 *
 * @since S60 v3.2
 */
class CAiScutSettingsAppList : public CActive
                             , public MDesCArray
                             , public MMsvSessionObserver
                             , public MApaAppListServObserver
{

public:

    static CAiScutSettingsAppList* NewL( CCoeEnv* aEnv, MAiScutListObserver& aObserver );

    virtual ~CAiScutSettingsAppList();

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
    TPtrC MdcaPoint( TInt aIndex ) const;

    // from base class MMsvSessionObserver

    /**
     * Handles an event from the message server.
     * Not used, but must be defined to be able to use the messaging server.
     *
     * @since S60 v3.2
     * @param aEvent Indicates the event type.
     * @param aArg1 Event type-specific argument value
     * @param aArg2 Event type-specific argument value
     * @param aArg3 Event type-specific argument value
     */
    void HandleSessionEventL( TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3 );

    // from base class MApaAppListServObserver

    /**
     * From MApaAppListServObserver
     * Handles a change in the application list
     *
     * @since S60 v3.2
     * @param aEvent Event id
     */
    void HandleAppListEvent( TInt aEvent );

    /**
     * Starts the asynchronous appliation list initialization
     *
     * @since S60 v3.2
     */
    void StartL();

    /**
     * Checks if the application list is ready to be shown
     *
     * @since S60 v3.2
     * @return ETrue if ready, EFalse if not
     */
    TBool IsReady() const;

    /**
     * Tells the application list to start or stop observing for changes
     *
     * @since S60 v3.2
     * @param aObserving ETrue if changes should be observed, EFalse if not
     */
    void SetObservingL( TBool aObserving );

    /**
     * Finds the index of the given settings item in the application list
     *
     * @since S60 v3.2
     * @param aItem Setting item to find
     * @return Index of the setting item in the application list
     */
    TInt FindIndex( CAiScutSettingsItem& aItem );

    /**
     * Returns target application data from the given index
     *
     * @since S60 v3.2
     * @param aIndex Index of the application in the list
     * @param aUid On return, the application uid
     * @param aParams On return, the application parameters
     * @param aCaption On return, the application caption
     * @param aType On return, the application type (app or url)
     * @return KErrNotFound if the application cannot be found, KErrNone otherwise
     */
    TInt GetDataByIndex( TInt aIndex, TUid& aUid, TPtrC& aParams, TPtrC& aCaption, TAiScutSettingType& aType ) const;

    /**
     * Adds extra list items to the application list.
     * For example "No Effect" item is added here.
     *
     * @since S60 v3.2
     */
    void AddExtraItemsL();

    /**
     * Removes extra list items from the application list.
     *
     * @since S60 v3.2
     */
    void RemoveExtraItemsL();

protected:

private:

    CAiScutSettingsAppList( CCoeEnv* aEnv, MAiScutListObserver& aObserver );

    void ConstructL();

    // from base class CActive

    /**
     * From CActive
     * Implements cancellation of an outstanding request.
     *
     * @since S60 v3.2
     */
    void DoCancel();

    /**
     * From CActive
     * Performs one step of the app list initialization
     *
     * @since S60 v3.2
     */
    void RunL();

    /**
     * From CActive
     * Handles an error situation
     *
     * @since S60 v3.2
     * @param aError Error code received from system
     * @return Error code after error handling
     */
    TInt RunError( TInt aError );

    /**
     * Completes own request status to make sure active scheduler
     * calls RunL again.
     *
     * @since S60 v3.2
     */
    void CompleteSelf();

    /**
     * Adds an application to the list
     *
     * @since S60 v3.2
     * @param aAppInfo Application information object
     */
    void AddApplicationL( TApaAppInfo& aAppInfo );

    /**
     * Adds the static list items to the application list.
     * For example "New Message" and "New Email" items are added here.
     *
     * @since S60 v3.2
     */
    void AddStaticItemsL();

    /**
     * Finds all mailboxes defined in the device
     */
    CMsvEntry* GetRootEntryL();

    /**
     * Adds remote mailboxes to the application list.
     *
     * @since S60 v3.2
     */
    void AddMailboxesL();

    /**
     * Adds a mailbox to the list
     *
     * @since S60 v3.2
     * @param aMailbox Name of the mailbox to add
     */
    void AddMailboxL( const TDesC& aMailbox, const TDesC& aMailboxId );

    /**
     * Removes a mailbox from the list
     *
     * @since S60 v3.2
     * @param aMailboxParam Parameter string of the mailbox to remove
     */
    void RemoveMailboxL( const TDesC& aMailboxParam );

    /**
     * Adds extra application to applist from central repository
     * 
     * @since S60 v3.2
     */
    void AddExtraItemsFromCRL();

    TBool IsBannedUid(TUid aUid);

    /**
     * Gets the next valid application from application architecture server.
     * Valid applications are non-hidden ones
     *
     * @since S60 v3.2
     * @param aAppInfo Application information of the next valid application
     * @return Error code from RApaLsSession or one of system wide errors
     */
    TInt GetNextValidApp( TApaAppInfo& aAppInfo );

    /**
     * Updates the application list
     *
     * @since S60 v3.2
     */
    void UpdateAppListL();

    /**
     * Figures out which application is missing from the list and adds it
     *
     * @since S60 v3.2
     * @param aCount Amount of applications to add
     */
    void AddInstalledAppToListL( TInt aCount );

    /**
     * Figures out which application should not be in the list and removes it
     *
     * @since S60 v3.2
     * @param aCount Amount of applications to remove
     */
    void RemoveUninstalledAppFromListL( TInt aCount );
    
    /**
     * Checks if application is midlet.
     * @param aUid Application uid
     * @since S60 v3.2
     */
    TBool IsNonNative(const TUid& aUid);
    
    /**
     * Checks if application is located in ROM.
     * @param aUid Application uid
     * @since S60 v5.0 
     */
    TBool IsInRom( const TUid& aUid );


private:

    /**
     * Nested class to store individual application list items
     *
     * @since S60 v3.2
     */
    class CAppListItem : public CBase
    {
    public:

        static CAppListItem* NewLC( TUid aUid, const TDesC& aCaption );

        ~CAppListItem();

        /**
         * Compare method used to add the items to the list in sorted order
         *
         * @since S60 v3.2
         * @param aFirst The first item to be compared
         * @param aSecond The second item to be compared
         * @return Negative if first comes before second, zero if equal and
         *         positive if first comes after second
         */
        static TInt CompareCaption( const CAppListItem& aFirst,
            const CAppListItem& aSecond );

        /**
         * Returns the item target application uid
         *
         * @since S60 v3.2
         * @return Target application uid
         */
        TUid Uid() const;

        /**
         * Returns the item target application caption
         *
         * @since S60 v3.2
         * @return Target application caption
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
        void SetParamsL( const TDesC& aParams );

        /**
         * Sets the type for the item target
         * 
         * @since S60 v3.2
         * @param aType The item target type
         */
        void SetType( TAiScutSettingType aType );
        
        /**
         * Returns the type of this item target
         * 
         * @since S60 v3.2
         * @return Item type
         */
        TAiScutSettingType Type() const;
    private:

        CAppListItem( TUid aUid );

        void ConstructL( const TDesC& aCaption );

    private:  // data

        /**
         * Target application uid
         */
        TUid iUid;

        /**
         * Target application caption
         */
        HBufC* iCaption;

        /**
         * Target application parameters
         */
        HBufC* iParams;
        /**
         * Is this setting a application or URL. 
         * Needed for URLs added from cenrep
         */
        TAiScutSettingType iType;

    };

private:  // data

    /**
     * List of application items
     * Own.
     */
    RPointerArray<CAppListItem> iListItems;

    /**
     * Application architecture server session
     * Own.
     */
    RApaLsSession               iApaSession;

    /**
     * Message server session
     * Own.
     */
    CMsvSession*                iMsvSession;

    /**
     * Notifier for changes in the application list
     * ( installations / uninstallations )
     * Own.
     */
    CApaAppListNotifier*        iAppNotifier;

    /**
     * Amount of all applications in the device
     */
    TInt                        iTotalAppCount;

    /**
     * List of mailbox items. Points to iListItems.
     * DO NOT DELETE THROUGH THIS!
     * Own.
     */
    RPointerArray<CAppListItem> iMailboxes;

    /**
     * Flag indicating if the asynchronous application list generation is ready
     */
    TBool                       iReady;

    /**
     * Pointer to the control environment
     * Not own.
     */
    CCoeEnv*                    iEnv;

    /**
     * Registered observer for application list events
     */
    MAiScutListObserver&        iObserver;

    /**
     * A flag indicating if the app list should observe changes
     */
    TBool                       iObserving;

};

#endif // CAISCUTSETTINGSAPPLIST_H

// End of File.
