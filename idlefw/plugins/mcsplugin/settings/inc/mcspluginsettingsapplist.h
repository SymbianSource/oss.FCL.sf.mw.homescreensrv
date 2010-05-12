/*
* Copyright (c) 2009 - 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <msvapi.h>     // For MMsvSessionObserver
#include <mcsmenu.h>    // For MenuContentService
#include <propertymap.h>// For HSPS settings property map

struct TSettingItem;
class CMenuItem;

/**
 *  @ingroup group_mcsplugin
 *
 * MCSPluginSettingsAppList class
 *
 * @since S60 v9.1
 */
class CMCSPluginSettingsAppList : public CBase,
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
     * Starts the asynchronous application list initialization
     *
     * @since S60 v3.2
     */
    void StartL();

    /**
     * Finds item from item array based on property values.
     *
     * @since S60 
     * @param aProperties
     * @return TSettingItem
     */
    TSettingItem FindItemL( RPointerArray<HSPluginSettingsIf::CPropertyMap>& 
        aProperties );

    /**
     * Returns menu item from list, based on given index
     *
     * @since S60 
     * @param aIndex List index
     * @return
     */
    CMenuItem* ItemL(const TInt& aIndex );
    
    /**
     * Returns title for undefined item
     */
    TPtrC UndefinedText() { return *iUndefinedText; };

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
     * iMenu
     */
    RMenu iMenu;

    /**
     * Name of "Undefined" application, own
     */
    HBufC* iUndefinedText;
    
    /*
     * Undefined MCS item, own
     */
    CMenuItem* iUndefinedItem;
};

#endif // CMCSPLUGINSETTINGSAPPLIST_H

// End of File.
