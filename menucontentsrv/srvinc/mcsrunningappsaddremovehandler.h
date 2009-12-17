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


#ifndef __MCSRUNNINGAPPSADDREMOVEHANDLER_H__
#define __MCSRUNNINGAPPSADDREMOVEHANDLER_H__


#include "menuengoperation.h"


//Forward declatations
class CMenuEng;
class MMcsRunningAppsHandler;   

/*
 * Supports AttributeChanged notification handling
 * if running application was moved from one folder to another 
 */

NONSHARABLE_CLASS( CMcsRunningAppsAddRemoveHandler ): public CBase, 
	MMenuEngOperation
    {
public:
	/**
	*   Factory method.
	*/
    static CMcsRunningAppsAddRemoveHandler* NewL( 
        CMenuEng& aEng, MMcsRunningAppsHandler* aRunningAppsHandler );

    /**
    *   Destructor.
    */
    ~CMcsRunningAppsAddRemoveHandler();
   
    /*
	 * Appends a folder to iFolders array and queue to menueng operation
	 * @param aFolder of folder for which the method was invoked
	 */
    void HandleAddRemoveL( TInt aFolder );
    
private:
	/**
	*   Constructor.
	*/
	CMcsRunningAppsAddRemoveHandler( 
	    CMenuEng& aEng, MMcsRunningAppsHandler* aRunningAppsHandler );

	/**
	 * 2nd phase constructor
	 */
	void ConstructL();

private:    // from MMenuEngOperation

    /**
    * Execute engine operation
    * Handles AttributeChanged notification and queue next operation
    * if iFolders array is not empty 
    */
    void RunMenuEngOperationL();

    /**
    * Error handling: ignore error.
    * @param aErr Error code.
    */
    void CompletedMenuEngOperation( TInt aErr );
    
private://data
    /*
    * engine. not own
    */
    CMenuEng& iEng ; ///< Engine.

    /*
     * not own
     */
    MMcsRunningAppsHandler* iRunningAppsHandler;
    
    /*
     * Flag storing operation status information 
     */
    TBool iOperationInProgress;
    
    /*
     * own. 
     */
    RArray<TInt> iFolders;
    };
    
 
	
#endif // __MCSRUNNINGAPPSADDREMOVEHANDLER_H__
