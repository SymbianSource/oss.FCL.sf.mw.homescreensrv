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


#ifndef __MCSRUNNINGAPPSHIDDENATTRSCANNER_H__
#define __MCSRUNNINGAPPSHIDDENATTRSCANNER_H__


#include "menuengoperation.h"


//Forward declatations
class CMenuEng;
class MMcsRunningAppsHandler;   

/*
 * It scans MCS to get UIDs of applications with "hidden" attribute.
 * It cooperates with CMcsRunningAppsHandler.
 */
NONSHARABLE_CLASS( CMcsRunningAppsHiddenAttrScanner ): public CBase, MMenuEngOperation
    {
public:
	/**
	*   Factory method.
	*/
    static CMcsRunningAppsHiddenAttrScanner* NewL( 
        CMenuEng& aEng, MMcsRunningAppsHandler* aRunningAppsHandler );

    /**
    *   Destructor.
    */
    ~CMcsRunningAppsHiddenAttrScanner();
   
    /*
     * 
     */
    void ScanL();
    
    /**
    * Checks if aUid is present in iHiddenApps array
    * @param aUid UID of an application
    * @return hidden attribute of an application 
    */
    TBool IsHidden( TUid aUid );
    
private:
	/**
	*   Constructor.
	*/
	CMcsRunningAppsHiddenAttrScanner( 
	    CMenuEng& aEng, MMcsRunningAppsHandler* aRunningAppsHandler );

	/**
	 * 2nd phase constructor
	 */
	void ConstructL();

private:    // from MMenuEngOperation

    /**
    * Execute engine operation:
    * Do the application scan 
    */
    void RunMenuEngOperationL();

    /**
    * Error handling: ignore error.
    * @param aErr Error code.
    */
    void CompletedMenuEngOperation( TInt aErr );
    
private:
    /*
     * Do the scan and update iHiddenApps array with UIDs of applications
     * with "hidden" attribute. The array is subsequently passed to 
     * CMcsRunningAppsHandler
     */
    void UpdateHiddenApplicationsL();

    
private://data
    /*
    * engine. not own
    */
    CMenuEng& iEng ; ///< Engine.

    /*
     * own 
     */
    RArray<TUid> iHiddenApps;

    /*
     * not own
     */
    MMcsRunningAppsHandler* iRunningAppsHandler;
    
    /*
     * Flag storing operation status information 
     */
    TBool iOperationInProgress;
    
    };
    
 
	
#endif // __MCSRUNNINGAPPSHIDDENATTRSCANNER_H__
