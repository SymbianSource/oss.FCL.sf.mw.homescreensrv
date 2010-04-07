/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Container for Psln Active Idle settings view.
*
*/

#ifndef C_PSLNACTIVEIDLEPLUGINENGINE_H
#define C_PSLNACTIVEIDLEPLUGINENGINE_H

#include <pslnfwbasecontainer.h>
#include <mpslnfwappthemeobserver.h>

// UID of Active Idle.
// changed from 0x102750F0 to 0xEDFF45C4 for standalone app
const TUid KUidActiveIdle = { 0xEDFF45C4 };


class CPslnFWAppThemeHandler;
class CXnODT;
class CPslnActiveIdlePlugin; // for notifying MSK chech need
class CPeriodic;


/**
 *  CPslnActiveIdlePluginEngine container class
 *
 *  @since S60 3.2
 */
class CPslnActiveIdlePluginEngine :
    public CBase,
    public MPslnFWAppThemeObserver
{
public: // Constructors and destructor

    void ConstructL(const TRect& aRect);

    CPslnActiveIdlePluginEngine(CPslnActiveIdlePlugin* aPlugin);

    ~CPslnActiveIdlePluginEngine();

// new methods

    /**
     * Activates currently highlighted theme.
     */
    void ActivateThemeL();
    
// from MPslnFWAppThemeObserver

    void HandleMessage(TXnServiceCompletedMessage aMessage);

    void HandleMessage(
        TXnServiceCompletedMessage aMessage,
        CArrayPtrFlat<CXnODT>& aAppThemeList);
        
    TInt CurrentUIController()
        {
        return iCurrentUIController;
        };
        
    void AddThemesToContainerL();

    void ManualGetSkinsRestartL(TBool aForceStart = EFalse);

private: // new methods

    /**
     * Adds new theme(s) to the listbox. Multiple themes are
     * only added when list updation is required (icons need
     * to be updated). Otherwise latest theme info is inquired
     * from XUIKON.
     */
    void AddNewThemeL(TXnServiceCompletedMessage aMessage);
    TInt GetExtStandbyScreenState(TInt& aValue);

    /**
     * Add HS items to list.
     */
    void AddHomeScreenThemesL();

    void CheckCurrentUIControllerL();
    
    void CheckCurrentAi1PluginL();
    
    void UpdateUIControllersInCenrepL(TInt aControllerId);
    
    void UpdateStatusPaneVisibilityCenrepL(TBool aPaneHidden);
    
    void UpdateAi1PluginLoadCenrepL(TInt aNewUid);
    
    TInt RestartAifw();

    TBool IsAiRunning();

    void UiTimerRestart();
        
private:

    static TInt RestartTimerCallback( TAny* aSelf );

    static TInt UiRefreshTimerCallback( TAny* aSelf );
    
private: // data

    /**
     * Application specific skin handler.
     * Own.
     */
    CPslnFWAppThemeHandler* iPslnFWThemeHandler;

    /**
     * List of themes. Each theme is described within an ODT.
     * Own.
     */
    CArrayPtrFlat<CXnODT>*  iMySkinList;

    /**
     * Indicates that skin list should be updated.
     */
    TBool                   iThemeListUpdated;

    /**
     * HomeScreen items start index in iMySkinList.
     */
    TInt                    iHSStartIndex;

    /**
     * HomeScreen items end index in iMySkinList.
     */
    TInt                    iHSEndIndex;

    /**
     * Flag to indicate if we are avaiting the first restart event.
     */
    TBool                   iFirstUpdateRound;

    /**
     * Flag to indicate if we have injected first round data.
     */
    TBool                   iFirstUpdateRoundInjected;

    /**
     * Current AI2 UI Controller id.
     */
    TInt                    iCurrentUIController;
    
    /**
     * Current AI1 plugin id.
     */
    TInt                    iCurrentAi1Plugin;

    /**
     * Flag to indicate if we have injected first round data.
     * Not owned.
     */
    CPslnActiveIdlePlugin*  iPlugin;

    /**
     * Restart timer. To restart AI2.
     * Own.
     */
    CPeriodic*              iRestartTimer;

    /**
     * UI Refresh timer.
     * Own.
     */
    CPeriodic*              iUiRefreshTimer;

    /**
     * Restart timer. To restart AI2.
     * Own.
     */
    TInt                    iRestartRetryCount;

    /**
     * Kepp track what has been added to list.
     * Own.
     */
    TInt                    iAddIndex;
    
};

#endif // C_PSLNACTIVEIDLEPLUGINENGINE_H

// End of File.
