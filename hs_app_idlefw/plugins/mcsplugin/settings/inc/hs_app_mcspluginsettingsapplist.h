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
* Description:  Application list for settings listbox.
*
*/


#ifndef CMCSPLUGINSETTINGSAPPLIST_H
#define CMCSPLUGINSETTINGSAPPLIST_H

#include <e32base.h>
#include <bamdesca.h>   // For MDesCArray
#include <apgcli.h>     // For RApaLsSession
#include <msvapi.h>     // For MMsvSessionObserver
#include <apgnotif.h>   // For MApaAppListServObserver
#include <mcsmenu.h>    // For MenuContentService
#include <propertymap.h>// For HSPS settings property map

/**
 *  @ingroup group_mcsplugin
 *
 * Observer interface for application list events
 *
 * @since S60 v3.2
 */
class MMCSPluginListObserver
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
    virtual void HandleScutListEventL( 
        TScutListEvent aEvent, TBool aAdded ) = 0;

   // virtual TBool IsHidden(const TUid& aAppUid) const = 0;

};

struct TSettingItem;
/**
 * Application list for settings listbox
 *
 * @since S60 v3.2
 */

class CMCSPluginWatcher;

/**
 *  @ingroup group_mcsplugin
 *
 * MCSPluginSettingsAppList class
 *
 * @since S60 v9.1
 */
class CMCSPluginSettingsAppList : public CBase, //public CActive
    public MDesCArray, public MMsvSessionObserver
{

public:
    /**
     * Part of the two phased constuction
     * 
     * @return CMCSPluginSettingsAppList*
     */
    static CMCSPluginSettingsAppList* NewL();

    /**
     * CompareNameL
     * 
     * @param aFirst
     * @param aSecond
     */
    static TInt CompareNameL(const CMenuItem& aFirst, const CMenuItem& aSecond);

    /**
     * Destructor
     */
    virtual ~CMCSPluginSettingsAppList();

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
    void HandleSessionEventL( TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, 
        TAny* aArg3 );

    /**
     * Starts the asynchronous appliation list initialization
     *
     * @since S60 v3.2
     */
    void StartL();

    /**
     * FindItemL
     *
     * @since S60 
     * @param aProperties
     * @return TSettingItem
     */
    TSettingItem FindItemL( RPointerArray<HSPluginSettingsIf::CPropertyMap>& 
        aProperties );

    /**
     * 
     *
     * @since S60 
     * @param 
     * @return
     */
    CMenuItem& ItemL(const TInt& aIndex );
    /**
     * RemoveMenuItemL
     * 
     * @param aIndex
     */
    void RemoveMenuItemL( TInt aIndex );

protected:

private:
    /**
     * Constructor
     */
    CMCSPluginSettingsAppList();

    /**
     * Second-phase constructor.
     */
    void ConstructL();

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
     * GetID of MCS Plugin Folder
     * 
     * @return TInt
     */
    TInt GetMCSPluginFolderIdL();

    /**
     * Update menu items
     * 
     * @return TInt
     */
    TInt UpdateMenuItemsRefCountL( CMenuItem* aItem, TInt aValueToAdd );

private:  // data

    /**
     * List of application items
     * Own.
     */
    RPointerArray<CMenuItem> iListItems;

    /**
     * Message server session
     * Own.
     */
    CMsvSession* iMsvSession;

    /**
     * Registered observer for application list events
     */
    //MMCSPluginListObserver& iObserver;

    /**
     * A flag indicating if the app list should observe changes
     */
    TBool iObserving;

    /**
     * iMenu
     */
    RMenu iMenu;

    /**
     * iSaveWatcher
     */
    CMCSPluginWatcher* iSaveWatcher;

    /**
     * iUpdateWatcher
     */
    CMCSPluginWatcher* iUpdateWatcher;

    /**
     * iRemoveWatcher
     */
    CMCSPluginWatcher* iRemoveWatcher;

    /**
     * iMCSPluginFolderId
     */
    TInt iMCSPluginFolderId;
};

#endif // CMCSPLUGINSETTINGSAPPLIST_H

// End of File.
