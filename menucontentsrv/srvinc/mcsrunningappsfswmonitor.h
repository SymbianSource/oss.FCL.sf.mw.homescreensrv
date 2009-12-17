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


#ifndef __MCSRUNNINGAPPSFSWMONITOR_H__
#define __MCSRUNNINGAPPSFSWMONITOR_H__

#include <e32property.h>

#include "menuengoperation.h"


//Forward declatations
class CMenuEng;
class MMcsRunningAppsHandler;   

/*
 * FSW hidden monitor. 
 * It cooperates with CMcsRunningAppsHandler. It updates FSW status for 
 * all applications and passes an array with FSW hidden applications 
 * UIDs to CMcsRunningAppsHandler
 */
NONSHARABLE_CLASS( CMcsRunningAppsFswMonitor ): public CActive, MMenuEngOperation
    {
public:
	/**
	*   Factory method.
	*/
    static CMcsRunningAppsFswMonitor* NewL( 
        CMenuEng& aEng, MMcsRunningAppsHandler* aRunningAppsHandler,
        TInt aPriority = EPriorityStandard);

    /**
    *   Destructor.
    */
    ~CMcsRunningAppsFswMonitor();
   
    /**
    * Checks if aUid is present in iFswHiddenApps array
    * @param aUid uid of an application
    * @return FSW hidden value
    */
    TBool IsFswHidden( TUid aUid );
    
private:
	/**
	*   Constructor.
	*/
	CMcsRunningAppsFswMonitor( 
	    CMenuEng& aEng, MMcsRunningAppsHandler* aRunningAppsHandler,
	    TInt aPriority );

	/**
	 * 2nd phase constructor
	 */
	void ConstructL();

private:    // from MMenuEngOperation

    /**
    * Execute engine operation:
    * Do the scan 
    */
    void RunMenuEngOperationL();

    /**
    * Error handling: ignore error.
    * @param aErr Error code.
    */
    void CompletedMenuEngOperation( TInt aErr );
    
private:    //from CActive    
    
    /**
    *   It is called if FSW status property changes
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
    
private:
    /**
    *   It activates the object, requests an event from the windowserver.
    */
    void Start();  

    
    /**
     * Fills iFswHiddenApps array with UIDs of FSW hidden applications
     * and passes this array to CMcsRunningAppsHandler
    */
    void UpdateFswHiddenApplicationsL();

    
private://data
    /*
    * engine. not own
    */
    CMenuEng& iEng ; ///< Engine.

    /*
     * own 
     */
    RArray<TUid> iFswHiddenApps;

    /**
    * FSW status change listener property
    */
    RProperty iFSWStatusProperty;    

    /*
     * not own
     */
    MMcsRunningAppsHandler* iRunningAppsHandler;
    
    };
    
 
	
#endif // __MCSRUNNINGAPPSFSWMONITOR_H__
