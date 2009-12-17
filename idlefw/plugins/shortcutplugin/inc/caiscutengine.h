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
* Description:  Shortcut plug-in engine class.
*
*/


#ifndef CAISCUTENGINE_H
#define CAISCUTENGINE_H

#include <apgnotif.h>                   // For MApaAppListServObserver
#include <msvapi.h>                     // For MMsvSessionObserver
#include <FavouritesDb.h>               // For RFavouritesDb
#include <FavouritesDbObserver.h>       // For MFavouritesDbObserver
#include <ConeResLoader.h>
#ifdef __WEB_WIDGETS
#include <WidgetRegistryClient.h>
#endif

#include <aiutility.h>

#include "aiscutdefs.h"
#include "taiscutparser.h"
#include "aicontentpublisher.h"

class CCoeEnv;
class CVwsSessionWrapper;
class CActiveFavouritesDbNotifier;
class CAiScutPlugin;
class CAiScutShortcut;
class CRepository;
class CAiScutRepositoryWatcher;
class CAiScutTargetShutter;
class CKeyLockPolicyApi;

/**
 * Defines the different scenarios of shortcut access checking.
 */
enum TScutAccessCheckType
{
    EScutCheckMailbox,
    EScutCheckApp,
    EScutCheckBkm,
    EScutCheckAll
};

/**
 * Defines which default can be used, when daulting is needed (uninstall/mem card removed)
 */
enum TScutDefault
{
    EScutUserDefined = 0,
    EScutDefaultToPlatform,
    EScutDefaultToTheme
};

enum TScutLockKey
    {
    EScutFirstLockKey,
    EScutSecondLockKey
};

/**
 *  Shortcut plug-in engine class.
 *
 *  Handles most of plug-in activity.
 *
 *  @since S60 v3.2
 */
class CAiScutEngine : public CBase
                    , public MMsvSessionObserver
                    , public MApaAppListServObserver
                    , public MFavouritesDbObserver
{

public:

    /**
     * First phase contructor.
     *
     * @since S60 v3.2
     * @param aPlugin Reference to the main plug-in class
     */
    static CAiScutEngine* NewL(CAiScutPlugin& aPlugin);

    virtual ~CAiScutEngine();

    // from base class MMsvSessionObserver

    /**
     * Handles an event from the message server
     *
     * @since S60 v3.2
     * @param aEvent Indicates the event type.
     * @param aArg1 Event type-specific argument value
     * @param aArg2 Event type-specific argument value
     * @param aArg3 Event type-specific argument value
     */
    void HandleSessionEventL(
        TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

    // from base class MApaAppListServObserver

    /**
     * From MApaAppListServObserver.
     * Handles a change in the application list
     *
     * @since S60 v3.2
     * @param aEvent Event id
     */
    void HandleAppListEvent(TInt aEvent);

    /**
     * Resumes the engine
     *
     * @since S60 v3.2
     */
    virtual void ResumeL(TBool aPublishAll, TAiTransitionReason aReason);

    /**
     * Suspends the engine
     *
     * @since S60 v3.2
     */
    void Suspend();

    /**
     *
     */
    static TInt HandleShortcutsChanged(TAny* aPtr);

    /**
     *
     */
    static TInt HandleHiddenAppsChanged(TAny* aPtr);

    /**
     *
     */
    TBool IsHidden(const TUid& aAppUid) const;

    /**
     * Handles an event sent by the AI framework
     *
     * @since S60 v3.2
     * @param aEvent Event (service) id
     * @param aParam Event parameter(s)
     */
    virtual void HandleAiEventL(TInt aEvent, const TDesC& aParam);

    /**
     * Called by the timer. Retries the access check for shortcuts.
     *
     * @since S60 v3.2
     */
    void RetryAccessCheck();

    /**
     * Checks if application is midlet or widget.
     * @param aUid Application uid
     * @since S60 v3.2
     */
    TBool IsNonNative(const TUid& aUid);
    
    TBool IsHiddenFromFSW( const TUid& aAppUid );
#ifdef __WEB_WIDGETS
    /**
     * Ask client session if given UID is widget.
     */
    TBool IsWidget(const TUid& aUid);
#endif

    /**
     * Handles call state changes (non-static version).
     *
     * @since S60 v3.2
     */
    void HandleCallStateChange();

    /**
     * Merges the user defined shortcuts with the defaults.
     * @param aType  shortcut type
     * @param aRecreateAll forces the recreation of shortcuts
     * @since S60 v3.2
     */
    void MergeShortcuts(TShortcutType aType, TBool aRecreateAll);

    /**
     *Get Application title
     */
    TBool GetAppTitle(
        const TUid&         aAppUid,
        const TUid&         aViewId,
        TDes&               aTitle,
        TAiScutAppTitleType aType);

    /**
     * Returns the application architecture server session owned by the engine
     *
     * @since S60 v3.2
     * @return Reference to RApaLsSession
     */
    inline RApaLsSession& ApaSession();

    /**
     * Returns reference to the bookmark database owned by the engine
     *
     * @since S60 v3.2
     * @return Reference to RFavouritesDb
     */
    inline RFavouritesDb& FavouritesDb();

    /**
     * Returns the message server session owned by the engine
     *
     * @since S60 v3.2
     * @return Pointer to CMsvSession
     */
    inline CMsvSession* MsvSession();

    /**
     * Returns a pointer to view server session wrapper.
     *
     * @since S60 v3.2
     * @return Pointer to view server session wrapper
     */
    inline CVwsSessionWrapper* VwsSession();

    /**
     * Returns a pointer to the coe environment
     *
     * @since S60 v3.2
     * @return Pointer to coe environment
     */
    inline CCoeEnv* Env();

    /**
     * Creates the shortcuts merging the defaults with user defined ones.
     *
     * @since S60 v3.2
     */
    void CreateShortcutsL();

protected:


    /**
     * C++ constructor
     *
     * @since S60 v3.2
     * @param aPlugin Reference to the main plug-in class
     */
    CAiScutEngine(CAiScutPlugin& aPlugin);

    void ConstructL();

protected:

    /**
     * Merges the user defined shortcuts with the defaults.
     * Leaving version
     * @param aType  shortcut type
     * @param aRecreateAll forces the recreation of shortcuts
     * @since S60 v3.2
     */
    void MergeShortcutsL(TShortcutType aType, TBool aRecreateAll);

    /**
     * Checks shortcut accessibility. If the AppArc server or message server
     * respond that they are not ready, a timer is launched to check later.
     *
     * @since S60 v3.2
     * @param aCheckType Specifies which kind of shortcuts should check its access
     * @param aPublishAll All shortcuts are published
     */
    void CheckAccessAndPublish(TInt aCheckType, TBool aPublishAll);

    /**
     * Tells each shortcut to check whether or not its target is accessible.
     * Shortcuts will revert to the default setting if the user setting is
     * not accessible.
     *
     * @since S60 v3.2
     * @param aCheckType Specifies which kind of shortcuts should check its access
     * @param aPublishAll All shortcuts are published
     */
    void CheckAccessAndPublishL(TInt aCheckType, TBool aPublishAll);

    /**
     * Finds the shortcut object with the given id
     *
     * @since S60 v3.2
     * @param aId Shortcut id
     * @return Shortcut index or KErrNotFound
     */
    TInt FindShortcutIndex(TInt32 aId);

    /**
     * Handles the shortcut launch by index.
     *
     * @since S60 v3.2
     * @param aParam Index of the shortcut to launch
     */
    void HandleLaunchByIndexL(const TDesC& aParam);

    /**
     * Handles the special launching of a shortcut. The 
     * launching can be done based on the location of 
     * the shortcut (sk, toolbar item etc) or based on the
     * application it points to. Currently when this function is
     * called to a shortcut that points to appshell the fastswap
     * window is opened
     * 
     * @since s60 v5.0
     * @param aParam Index of the shortcut to perform the special launch
     */
    void HandleLaunchByIndexAlternateL(const TDesC& aParam);
    /**
     * Handles the shortcut launch by value
     *
     * @since S60 v3.2
     * @param aParam Value of the shortcut to launch
     */
    void HandleLaunchByValueL(const TDesC& aParam);

    /**
     * Shows the plug-in settings dialog.
     *
     * @since S60 v3.2
     */
    void ShowSettingsL(const TDesC& aParam);

    /**
     * Shows the plug-in setting.
     *
     * @since S60 v5.0
     */
    void ShowSettingL(const TDesC& aParam);
    
    /**
     * Opens the fast swap window
     * 
     * @since S60 v5.0
     */
    void OpenFastSwap();
    /**
     * Handles PS commands from WSPlugin.
     *
     * @since S60 v3.2
     */
    static TInt HandlePSCommand(TAny* aAny);

    /**
     * Handles call state changes.
     *
     * @since S60 v3.2
     */
    static TInt CallStateChangeCallback(TAny* aPtr);

    /**
     * Callback for delay timer
     */
    static TInt DelayTimerCallBack(TAny *aSelf );
        
    
    /**
     * Get soft key uid
     *
     * @since S60 v3.2
     * @param aSoftkeyId shortcut index
     */
    TUid SoftkeyUid(TUint32 aSoftkeyId);

    /**
     *Check softkey delay is required
     *
     * @since S60 v3.2
     * @param AppUid application uid     
     */
    TBool IsDelayRequired(TUid aAppUid);

    /**
     * Handles PS commands from WSPlugin (leaving version).
     *
     * @since S60 v3.2
     */
    void DoHandlePSCommandL();

    /**
     * Activates phone/video call application
     *
     * @since S60 v3.2
     */
    void ActivateTopMostApp();

    /**
     * Check MsvSession is Needed
     */
    TBool IsMsvSessionNeeded();

    /**
     * Check Bookmark Observer is Needed
     */
    TBool IsBookmarkObserverNeeded();

    /**
     * Load application titles list
     */
    void LoadAppTitleListL();

    /**
     * Get hidden applications
     */
    void GetHiddenAppsL();
    
    void CheckForThemeDefaultReinstalledL();

    TBool IsLockKey( TInt aScanCode, TScutLockKey aLockKey ) const;

    TInt AddOverrideIcon( TAiScutIcon &aIcon );

    /**
     * Starts a delay timer that calls HandleLaunchByIndexL() after
     * a short delay. Delay is read from cenrep with the key KAIKeyLockTimeout
     * 
     * @param aParam The parameter to pass to HandleLaunchByIndexL()
     * 
     */
    void DelayedLaunchByIndexL( const TDesC &aParam );
private: // From MFavouritesDbObserver

    /**
     * Handles database event.
     * @param aEvent Database event.
     */
    void HandleFavouritesDbEventL(RDbNotifier::TEvent aEvent);

protected:  // data

    /**
     * Shortcut objects.
     * Own.
     */
    RAiShortcutArray                iShortcuts;

    /**
     * Shortcut objects for storing theme default scuts.
     * Own.
     */
    RAiShortcutInfoArray            iThemeShortcuts;
    
    /**
     * Flags for shortcuts to indicate whether platform or theme default is used.
     */
    RArray<TScutDefault>            iDefaultUsed;

    /**
     * Central Repository session.
     * Own.
     */
    CRepository*                    iRepository;

    /**
     * Notifier for changes in the application list
     * (installations / uninstallations).
     * Own.
     */
    CApaAppListNotifier*            iAppNotifier;

    /**
     * Notifier for changes in the shortcut settings.
     * Own.
     */
    CAiScutRepositoryWatcher*       iSettingsNotifier;

    /**
     * Message server session.
     * Own.
     */
    CMsvSession*                    iMsvSession;

    /**
     * Timer that performs the availability check after a given delay.
     * Own
     */
    CPeriodic*                      iTimer;

    /**
     * Reference to the plug-in.
     * Not own.
     */
    CAiScutPlugin&                  iPlugin;

    /**
     * Application architecture server session.
     */
    RApaLsSession                   iApaSession;

    /**
     * Bookmark database session.
     * Own.
     */
    RFavouritesSession              iBookmarkSess;

    /**
     * Bookmark database.
     * Own.
     */
    RFavouritesDb                   iBookmarkDb;

    /**
     * Bookmark database change observer.
     * Own.
     */
    CActiveFavouritesDbNotifier*    iBookmarkDbObserver;

    /**
     * View server session wrapper.
     * Own
     */
    CVwsSessionWrapper*             iVwsSession;

    /**
     * Flag to indicate that all shortcus are to be published. Used when resuming.
     */
    TBool                           iPublishAll;

    /**
     *Call key event observer
     * Own
     */
    MAiPSPropertyObserver*          iKeyEventObserver;

    /**
     * Call state PS observer.
     * Own.
     */
    MAiPSPropertyObserver*          iCallStateObserver;

    /**
     * Call state
     */
    TBool                           iActiveCall;

    /**
     * First keylock key
     */
    TInt                            iFirstLockKey;

    /**
     * First keylock key pressed flag to ensure first lock has been
     * pressed when skipping key lock. Enabling the correct response to
     * skip command.
     */
    TBool                           iFirstLockKeyPressed;

    /**
     * Second keylock key
     */
    TInt                            iSecondLockKey;

    /**
     * Resource loader for common text resources.
     */
    RConeResourceLoader             iResourceLoaderTexts;

    /**
     * Resource loader for sendui text resources.
     */
    RConeResourceLoader             iResourceLoaderSendUi;

    /**
     * Coe environment.
     * Not own.
     */
    CCoeEnv*                        iEnv;

    /**
     * Array of UIDs which are hidden from the TARM
     */
    RArray<TUid>                    iHiddenApps;

    /**
     * Central repository object for Hidden Apps
     * Own
     */
    CRepository*                    iHiddenAppsRepository;

    /**
     * Central repository change handler for Hidden Apps
     * Own
     */
    CAiScutRepositoryWatcher*       iHiddenAppsNotifier;

    /**
     * Application titles list
     */
    RArray<TAiScutAppTitleEntry>    iAppTitleList;

    /**
     * Own
     */
    CAiScutTargetShutter*           iScutShutter;

    /**
     *
     */
    TBool                           iSoftkeyAppRunning;
    
#ifdef __WEB_WIDGETS
    /**
     * Widget registry client session to check if UID means widget.
     */
    RWidgetRegistryClientSession    iWidgetRegistry;
    TBool                           iWidgetRegistryConnected;
#endif
    /**
    * for parsing the icon overrides and softkeys
    */
    TAiScutParser                   iParser;

    /**
     * Override icons and softkeyicons are held here
     */
    RArray <TAiScutIcon>    iIcons;

    /**
     * For checking keylock buttons
     * Own
     */
    CKeyLockPolicyApi *iKeylockApi;
    /**
     * Timer user to delay the launching
     * of specific shortcuts.
     * Own
     */
    CPeriodic *iDelayTimer;
    
    /**
     * Command passed to HandleLaunchByIndexL()
     * when delayed launching is used
     * Own
     */
    HBufC    *iDelayedLaunchCmd;
    
    /**
     * Delayed launching delay. Read from cenrep
     */
    TInt iDelayTimerDelay;
};

#include "caiscutengine.inl"

#endif // CAISCUTENGINE_H

// End of File.
