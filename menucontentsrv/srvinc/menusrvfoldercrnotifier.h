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

#ifndef __MENUSRVFOLDERSCANNER_H__
#define __MENUSRVFOLDERSCANNER_H__

#include <e32base.h>
#include <apgcli.h>
#include <apgnotif.h>
#include <cenrepnotifyhandler.h> 
#include "menuengoperation.h"
#include "menuengfilter.h"
#include "mcsfreespaceobserver.h"


// FORWARD DECLARATION

class CMenuEng;
class TMenuItem;
class CMenuSrvMmcHistory;
class CRepository;
class CCenRepNotifyHandler;

/**
* Folder scanner.
* Using AppArc application information, keeps menu application
* items ("menu:application") up-to-date.
*
* Operation is asynchronous (because AppArc startup is asynchronous):
* 1. ConstructL(): invokes iApaLsSession to build app list (async).
* 2. RunL(): App list is complete, this object schedules itself to Engine.
* 3. RunMenuEngOperationL(): The operation is executed by the engine.
* 4. HandleAppListEvent(): change in app list, go back to step 2.
*
* App scanning does the following:
* - Applications present in the system but not appearing in the menu are added
*   to the appropriate folder.
* - Java apps should appear / disappear.
* - Hide hidden applications, show non-hidden applications (flag "hidden").
* - Hide / show MMC-related items, according to whether MMC is present or not
*   (flag "missing"). Keep an MMC history, purge old MMC-based menu items
*   (== missing menu items whose MMC card has not been seen for long).
* - Observing AppArc (and MMC, and Java, etc etc) to keep the menu data up
*   to date continuously.
*/
NONSHARABLE_CLASS( CMenuSrvFolderCrNotifier )
: public CActive, public MMenuEngOperation, public MCenRepNotifyHandlerCallback, 
MMcsFreeSpaceObserver
    {

public:     // Constructor

    /**
    * Destructor.
    */
    virtual ~CMenuSrvFolderCrNotifier();

    /**
    * Two-phased constructor.
    * @param aEng Engine.
    */
    static CMenuSrvFolderCrNotifier* NewL( CMenuEng& aEng );

private:    // Constructors and destructor

    /**
    * Constructor.
    * @param aEng Engine.
    */
    CMenuSrvFolderCrNotifier( CMenuEng& aEng );

    /**
    * Second phased constructor.
    */
    void ConstructL();

private:    // from CActive

    /**
    * Request completed, schedule engine operation.
    */
    void RunL();

    /**
    * Cancel request.
    */
    void DoCancel();

    /**
    * Error handling: ignore error.
    * @param aErr Error code.
    */
    TInt RunError( TInt aError );
    
private:    // from MMcsFreeSpaceObserver
    /**
     * Handle Free Space Event.
     */
    void HandleFreeSpaceEventL();

private:    // from MMenuEngOperation

    /**
    * Execute engine operation:
    * Do the application scan and update menu accordingly.
    */
    void RunMenuEngOperationL();

    /**
    * Error handling: ignore error.
    * @param aErr Error code.
    */
    void CompletedMenuEngOperation( TInt aErr );
    
    /**
     * Notifies engine with events
     * @param aNewHiddenFolder nome of a folder. 
     */ 
    void NotifyEngineL( const TDesC& aFolder );

    /**
     * Compares arrays and sends notification to engine abaut differences
     * @param aLeftArray array with folders names. 
     * @param aRightArray array with folders names.
     */ 
    void NotifyEngineL( const RArray<TPtrC>& aLeftArray, 
    		const RArray<TPtrC>& aRightArray );
    
    /**
     */ 
    void HandleNewCrHiddenFoldersL( const TDesC& aNewHiddenFolder );
    


private:    // from MCenRepNotifyHandlerCallback 

    /**
    * Called if one of CR keys has changed
    * @param aId  Id of the key that has changed.  
    * @param aNewValue  The new value of the key.  
    */
    void HandleNotifyString( TUint32 aKey, const TDesC16& aNewValue );


private:    // new methods

    /**
     */ 
    void ParseHiddenFoldersL(  const TDesC& aHiddenFolders, 
                                RArray<TPtrC>& aHiddenFoldersArray );

private:    // data
    
    CMenuEng& iEng ; ///< Engine.
    RFs iFs; ///< File Server Session. Own.

    /*
    * Central repository session
    */
	CRepository* iCenRepSession;

    /*
    *Central repository change handler for Control Panel Hidden Folders
    */
    CCenRepNotifyHandler* iCenRepNotifyHandlerCPHiddenFolders;	

    TBool iOpStatus;

    /*
     * Free Space observer.
     */
    CMcsFreeSpaceObserver* iFreeSpaceObserver;

    /*
    * Old value from central repository.
    */
    RBuf iHiddenCRFolders;
    
    /*
    * New value from central repository.
    */
    RBuf iNewHiddenCRFolders;
    
    };



#endif // __MENUSRVFOLDERSCANNER_H__
