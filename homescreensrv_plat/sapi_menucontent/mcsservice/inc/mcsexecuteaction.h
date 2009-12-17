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


#ifndef __MCSEXECUTEATCION_H
#define __MCSEXECUTEATCION_H

#include <e32base.h>
#include "mcsmenuopobserver.h"

class MMCSCallback;
class RMenu;
class TMenuItem;
class RMcsOperationManager;

/**
 * This class implements MCS SAPI core logic for ExecuteAction async requests.
 * 
 */
class CMCSExecuteAction : public CActive, public MMCSMenuOpObserver
	{
	public:
	
		/**
		* Two-phased constructor.
	    * @param aMCS MenuContentService object.
	    * @param aCallback Callback Object for Change notification.
	    * @return CMCSExecuteAction reference.
		*/
		static CMCSExecuteAction* NewL( RMenu& aMCS, MMCSCallback* aCallback,
		                            RMcsOperationManager& aOperationManager );
		
		/**
        * ExecuteActionL for asynchronous  calls.
        * @param aItemId item id.
        * @param aAction action open/remove.
        */
		void ExecuteActionL( TInt aItemId, TDesC8& aAction);
		
		/**
        * ExecuteL for synchronous calls.
        * @param aItemId item id.
        * @param aAction action open/remove.
        */
		void ExecuteL(TInt aItemId, TDesC8& aAction);
	
		/**
	     * Destructor.
    	*/
		virtual ~CMCSExecuteAction();

	public: // from MMCSMenuOpObserver
	
		/**
	     * MenuOpCompletedL menu operation complete call back.
	     * @param aStatus status of the operation 
	     */
		void MenuOpCompletedL(TInt aStatus );
	
	private:

		/** 
	    * Default constructor.
	    * @param aMCS MenuContentService object.
	    * @param aCallback It is a callback notification.
	    */
		CMCSExecuteAction( RMenu& aMCS, MMCSCallback* aCallback, 
		                        RMcsOperationManager& aOperationManager );

		/** 
        * ConstructL
        */
		void ConstructL();
		
		/**
		* Inherited from CActive class 
		*/ 
    	virtual void DoCancel();

		/**
		* Inherited from CActive class 
		*/ 
        virtual void RunL();
        
		/**
		* Notify callback in case RunL leaved;
		*/ 
        TInt RunError(TInt aError);

		/**
		* Activates the request and call SetActive() function
		*/ 
		void ActivateRequest(TInt aReason);

		/**
		* Gives the result for notification request.
		*/ 
		void NotifyRequestResult(TInt aReason);
	
	private:
	
		/** Callback Object. Not owned. */ 
		MMCSCallback* iCallback;
		/** iMCS reference. Not owned. */
		RMenu iMCS;

		/** item id **/
		TInt iItemId;
		/** Action to Execute **/
		TPtrC8 iAction;
		/*not own*/
		RMcsOperationManager& iOperationManager;
	};

#endif //__MCSEXECUTEATCION_H
