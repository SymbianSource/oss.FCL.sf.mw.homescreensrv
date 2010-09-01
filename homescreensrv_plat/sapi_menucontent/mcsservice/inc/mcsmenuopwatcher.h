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
*/


#ifndef __MCSMENUOPWATCHER_H
#define __MCSMENUOPWATCHER_H

#include <e32base.h>

class MMCSMenuOpObserver;
class CMenuOperation;
class RMcsOperationManager;

/**
 * This class implements MCS SAPI menu action watcher for async calls.
 * 
 */
class CMCSMenuOpWatcher: public CActive
    {
    public:

    	/**
		* Two-phased constructor.
	    * @param iObserver Menu operation observer object.
	    * @return CMCSMenuOpWatcher reference.
		*/
		static CMCSMenuOpWatcher* NewL( MMCSMenuOpObserver& iObserver, 
		                RMcsOperationManager& aOperationManager );
		
		/**
	     * Destructor.
		*/
	    virtual ~CMCSMenuOpWatcher();

		/**
		* Watch menu operation watcher.
	    * @param iObserver Menu operation observer object.
	    * @return CMCSMenuOpWatcher reference.
		*/
	    void Watch( CMenuOperation& aOperation); 
	    
    private:
    
    	/** 
	    * Default constructor.
	    * @param iObserver Menu operation observer object.
	    */
	    CMCSMenuOpWatcher( MMCSMenuOpObserver& iObserver, 
	                        RMcsOperationManager& aOperationManager );

		/** 
		 * ConstructL
		 */
		void ConstructL();
		
		/**
         * Notify callback in case RunL leaved;
         */ 
		TInt RunError( TInt aError );
		
		/**
		* Inherited from CActive class 
		*/ 
	    void RunL();
	    
		/**
		* Inherited from CActive class 
		*/ 
	    void DoCancel();
    
	private:
		
		/** Menu operation observer */
		MMCSMenuOpObserver& iObserver;
		/** Menu operation */
	    CMenuOperation* iOperation; 
	    /*not own*/
	    RMcsOperationManager& iOperationManager;
	    
    };

#endif /*__MCSMENUOPWATCHER_H*/
