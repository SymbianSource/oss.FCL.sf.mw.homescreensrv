/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef __MCSRUNNINGAPPSWGMONITOR_H__
#define __MCSRUNNINGAPPSWGMONITOR_H__

#include <w32std.h>
#include <widgetregistryconstants.h>

#include "menuengoperation.h"


//Forward declatations
class CMenuEng;
class MMcsRunningAppsHandler;
class CApaWindowGroupName;

/*
 * Windows Group change monitor. 
 * It cooperates with CMcsRunningAppsHandler. If any application was 
 * launched or closed it passes an array with currently running applications 
 * UIDs to CMcsRunningAppsHandler
 */
NONSHARABLE_CLASS( CMcsRunningAppsWgMonitor ): public CActive, MMenuEngOperation
    {
    public:
	/**
	*   Factory method.
	*/
    static CMcsRunningAppsWgMonitor* NewL( 
        CMenuEng& aEng, MMcsRunningAppsHandler* aRunningAppsHandler,
        TInt aPriority = EPriorityStandard );

    /**
    *   Destructor.
    */
    ~CMcsRunningAppsWgMonitor();

        
private:
	/**
	*   Constructor.
	*/
	CMcsRunningAppsWgMonitor( 
	    CMenuEng& aEng, MMcsRunningAppsHandler* aRunningAppsHandler,
	    TInt aPriority );

	/**
	 * 2nd phase constructor
	 */
	void ConstructL();

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
    
private:    
    /**
    *   It is called if an event is received from the windowserver.
    */
    void RunL();
    
    /**
    * Error handling: ignore error.
    * @param aErr Error code.
    */
    TInt RunError( TInt aError );

    /**
     *   Provides Active object safe cancellation
    */
    void DoCancel();
    
    /**
    *   It activates the object, requests an event from the windowserver.
    */
    void Start();
    
    /*
     * Fills iRunningApps array with UIDs of currently running applications
     * and passes this array to CMcsRunningAppsHandler
    */
    void UpdateRunningApplicationsL();

    /**
     *   Updates the list of the running widgets.
     */
    void UpdateWidgetTaskListL( );
    
private://data
    /*
    * engine. not own
    */
    CMenuEng& iEng ; ///< Engine.

    /**
    *   The events can be received via this session.
    */
    RWsSession iWsSession;
    
    /**
    *   Seesion to widget registry. own
    */
    RWidgetRegistryClientSession iWidgetSession;

    /*
     * own 
     */
    RArray<TUid> iRunningApps;
    
    /*
     * not own
     */
    MMcsRunningAppsHandler* iRunningAppsHandler;
    
    /*
     * own 
     */
    RWindowGroup iWg;
    
    /*
     * own 
     */    
    CApaWindowGroupName* iWgName;
    
    /*
     * 
     */
    TBool iOperationInProgress;
    };
    
 
	
#endif // __MCSRUNNINGAPPSWGMONITOR_H__
