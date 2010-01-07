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


#ifndef CMCSPLUGINENGINE_H
#define CMCSPLUGINENGINE_H

// INCLUDE FILES
#include <e32base.h>
#include <mcsmenu.h>
#include <mcsmenunotifier.h>
#include <favouritesdbobserver.h>   // For MFavouritesDbObserver
#include <favouritesdb.h>           // For RFavouritesDb
#include <msvapi.h>                 // For MMsvSessionObserver

#include "mcspluginwatcher.h"

class CGulIcon;
class CMCSPluginData;
class TMCSData;
class CMCSPlugin;
class CActiveFavouritesDbNotifier;

/**
 *  @ingroup group_mcsplugin
 *
 *  MCSPluginEngine class
 *
 *  @since S60 9.1
 */
class CMCSPluginEngine : public CBase, public MMCSPluginWatcherObserver,
    public MFavouritesDbObserver, public MMsvSessionObserver
{

public:  // Constructors and destructor

    /**
     * Constructor to use in the object creation. Initializes the necessary 
     * data.
     *
     * @param aPlugin
     * @param aInstanceUid
     */
    CMCSPluginEngine( CMCSPlugin& aPlugin, const TDesC8& aInstanceUid );

    /**
    * Part of the two phased constuction
    *
    * @param aPlugin
    * @param aInstanceUid
    * @return Pointer to the created CMCSPluginEngine object
    */
    static CMCSPluginEngine* NewL( CMCSPlugin& aPlugin, 
        const TDesC8& aInstanceUid );

    /**
    * Destructor
    *
    * @param none
    * @return none
    */
    ~CMCSPluginEngine();

    /**
     * Gets the menu data.
     * 
     * @param aIndex
     * @return TMCSData&
     */
    TMCSData& MenuDataL( const TInt& aIndex );

    /** Gets the menu item count
     * 
     * @return TInt
     */
    TInt MenuItemCount();

    /**
     * Finds the menu item header for given filter.
     * 
     * @param aFilter
     * @return TMenuItem
     */
    TMenuItem FindMenuItemL( CMenuFilter& aFilter );

    /**
     * Retrieves the menu item object from MCS.
     * 
     * @param aMenuItem
     * @return CMenuItem*
     */
    CMenuItem* FetchMenuItemL( const TMenuItem& aMenuItem );

    /**
     * Returns icon for given menu item and given attribute
     * 
     * @param aMenuItem
     * @param aAttr
     * @return CGulIcon*
     */
    CGulIcon* ItemIconL( CMenuItem& aMenuItem, const TDesC& aAttr );

    /**
     * Returns text for given menu item and given attribute
     * 
     * @param aMenuItem
     * @param aAttr
     */
    TPtrC ItemTextL( CMenuItem& aMenuItem, const TDesC& aAttr );

    /** 
     * Launches menu item
     * 
     * @param aIndex
     */
    void LaunchItemL( const TInt& aIndex );

    /**
    * Resumes the engine
    *
    * @param void
    * @return void
    */
    void ResumeL();

    /**
    * Suspends the engine
    *
    * @param void
    * @return void
    */
    void Suspend();

    /**
     * ShowSettingsL
     */
    void ShowSettingsL();

    /**
     * From MMCSPluginWatcherObserver
     */
    void HandleNotifyL();

    // From MFavouritesDbObserver
    /**
     * Handles database event.
     * @param aEvent Database event.
     */
    void HandleFavouritesDbEventL( RDbNotifier::TEvent aEvent );

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
    void HandleSessionEventL( TMsvSessionEvent aEvent, TAny* aArg1,
        TAny* aArg2, TAny* aArg3 );

    /**
    * Called during plugin desctruction
    * Decrements reference counters of all run-time generated items
    * and deletes those which have reference counter == 0
    */
    void CleanMCSItemsL();

private:
    /*
     * Part of the two phased construction
     */
    void ConstructL();

    /**
     * InitL
     */
    void InitL();

    /**
     * Tells the settings container to start observing for changes in favorites
     * database and mailbox db.
     *
     */
    void StartObservingL();

    /**
     * Tells the settings container to stop observing for changes in favorites
     * database and mailbox db.
     *
     */
    void StopObserving();

    /**
     * ConstructMenuItemForIconL
     * 
     * @param aPath
     * @param aMenuItem
     * @return TBool
     */
    TBool ConstructMenuItemForIconL( const TDesC& aPath, CMenuItem& aMenuItem );

    /**
    * Helper method. Adds a given constant to a value of reference counter
    *
    * @param aItem A Menu Item to update
    * @param aValueToAdd A constant to add 
    * @return The actual value of updated reference count
    */
    TInt UpdateMenuItemsRefCountL( CMenuItem* aItem, const TInt aValueToAdd );

protected:

private:

    /* Plugin data
    * Own
    */
    CMCSPluginData* iPluginData;

    // MCS resource
    RMenu iMenu;

    // MCS change notifier
    RMenuNotifier iNotifier;

    // MCS asynchronous operation watcher
    CMCSPluginWatcher* iWatcher;

    // MCS change notifier watcher
    CMCSPluginWatcher* iNotifyWatcher;

    //
    CMCSPlugin& iPlugin;

    // Reference to plugin owned instanceUid
    const TDesC8& iInstanceUid;
    // Indicating that backup is in progress
    TBool iSuspend;
    //Offset of resource file.
    TInt iResourceOffset;

    /**
     * Bookmark database change observer.
     * Own.
     */
    CActiveFavouritesDbNotifier* iBookmarkDbObserver;

    /**
     * Bookmark database.
     */
    RFavouritesDb iBookmarkDb;

    /**
     * Bookmark database session.
     */
    RFavouritesSession iBookmarkSession;

    /**
     * Message server session
     * Own.
     */
    CMsvSession* iMsvSession;
};

#endif // CMCSPLUGINENGINE_H


