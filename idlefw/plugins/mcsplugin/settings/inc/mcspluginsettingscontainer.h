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
* Description:  MCS settings plug-in container.
*
*/


#ifndef CMCSPLUGINSETTINGSCONTAINER_H
#define CMCSPLUGINSETTINGSCONTAINER_H

// External includes
#include <gsbasecontainer.h>
#include <mcsmenu.h>
#include <mcsmenunotifier.h>
#include <favouritesdbobserver.h>   // For MFavouritesDbObserver
#include <favouritesdb.h>           // For RFavouritesDb
#include <msvapi.h>                 // For MMsvSessionObserver

// Internal includes
#include "mcspluginwatcher.h"

// Forward declaration
class CAknRadioButtonSettingPage;
class CMCSPluginSettingsModel;
class CActiveFavouritesDbNotifier;
class CMCSPluginWatcher;

/**
 *  @ingroup group_mcsplugin
 *
 * Shortcut settings plug-in container
 *
 * @lib mcspluginsettings.lib
 * @since S60 v3.2
 */
class CMCSPluginSettingsContainer : public CGSBaseContainer, 
    public MFavouritesDbObserver, public MMsvSessionObserver,
    public MMCSPluginWatcherObserver
{

public:
    /**
     * Public constructor
     */
    CMCSPluginSettingsContainer();

    /*
     * Descructor
     */
    virtual ~CMCSPluginSettingsContainer();

    /**
     * Sets pointer to settings plug-in model.
     *
     * @since S60 v3.2
     * @param aModel Pointer to settings plug-in model
     */
    void SetModel(CMCSPluginSettingsModel* aModel);

    /**
     * Handles a setting change command
     *
     * @since S60 v3.2
     */
    void HandleChangeCommandL();

    /**
     * Handles a help command
     *
     * @since S60 v9.1
     */
    void HandleHelpCommandL();

    /**
     * Close change dialog
     *
     * @since S60 v3.2
     */
    void CloseChangeDialog();

    /**
     * Reset current list
     *
     * @since S60 v3.2
     */
    void ResetCurrentListL(TInt aIndex);

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

    // from base class MMCSPluginWatcherObserver

    /**
     * Hanle notify
     */
    void HandleNotifyL();
    
    /**
     * Sets currently selected index
     */
    void SetCurrentItemIndex(TInt aIdx);
    
private:

    /*
     * Leaving constructor
     * 
     * @param aRect
     */
    void ConstructL(const TRect& aRect);

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

    // from base class CGSBaseContainer

    /**
     * From CGSBaseContainer
     * Constructs the settings listbox
     *
     * @since S60 v3.2
     * @param aResLbxId Listbox resource id
     */
    void ConstructListBoxL(TInt aResLbxId);

    /**
     * Chandles a setting change command to select application from a list
     *
     * @since S60 v3.2
     * @param aItem Applist item to modify
     * @param aNew ETrue if selecting an application for first time, 
     *             EFalse if not
     * @return ETrue if shortcut was changed. EFalse if not
     */
    TBool HandleAppListChangeCommandL( const TInt& aIndex,
        const TInt& aSettingIndex );

    /**
     * Chandles a setting change command to select bookmark from a list
     *
     * @since S60 v3.2
     * @param aItem Applist item to modify
     * @param aNew ETrue if selecting an application for first time, 
     *             EFalse if not
     * @return ETrue if shortcut was changed. EFalse if not
     */
    TBool HandleBookmarkChangeCommandL( const TInt& aIndex,
        const TInt& aSettingIndex );

    /**
     * Creates Help Context for launching Shortcuts Help page.
     */
    TCoeHelpContext GetHelpContext() const;

    /**
     * Offer key event
     * 
     * @param aKeyEvent
     * @param aType
     */
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

    /**
     * Checks if there is a need to update the middle softkey label.
     */
    void CheckMiddleSoftkeyLabel();

    // From MFavouritesDbObserver
    /**
     * Handles database event.
     * @param aEvent Database event.
     */
    void HandleFavouritesDbEventL(RDbNotifier::TEvent aEvent);

private:  // data

    /**
     * Settings plug-in model.
     * Not own.
     */
    CMCSPluginSettingsModel*       iModel;

    /**
     * Application selection list page.
     * Own.
     */
    CAknRadioButtonSettingPage* iAppListDialog;

    /**
     * Bookmark selection list page.
     * Own.
     */
    CAknRadioButtonSettingPage* iBkmListDialog;

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

    // MCS resource
    RMenu iMenu;

    // MCS change notifier
    RMenuNotifier iNotifier;

    /* MCS change notifier watcher
     * Own
     */
    CMCSPluginWatcher* iNotifyWatcher;
};

#endif // CMCSPLUGINSETTINGSCONTAINER_H

// End of File.
