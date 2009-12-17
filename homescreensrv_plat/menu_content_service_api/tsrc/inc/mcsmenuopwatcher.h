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

class CMenuOperation;

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
		static CMCSMenuOpWatcher* NewL( TBool aStopScheduler = 0);
		
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
	    
	    TInt GetStatus();
	    
    private:
    
    	/** 
	    * Default constructor.
	    * @param iObserver Menu operation observer object.
	    */
	    CMCSMenuOpWatcher( TBool aStopScheduler );

		/** 
		 * ConstructL
		 */
		void ConstructL();
		
		/**
		* Inherited from CActive class 
		*/ 
	    void RunL();
	    
		/**
		* Inherited from CActive class 
		*/ 
	    void DoCancel();
    
	private:
		
		/** Menu operation */
	    CMenuOperation* iOperation; 
	    
	    TBool iStopScheduler;
    };

#endif /*__MCSMENUOPWATCHER_H*/
