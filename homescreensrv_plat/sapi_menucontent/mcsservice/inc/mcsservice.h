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


#ifndef __MCSSERVICE_H
#define __MCSSERVICE_H

#include <e32base.h>
#include <liwcommon.h>
#include <liwserviceifbase.h>
#include <liwbufferextension.h>
#include "mcsmenusapi.h"
#include "mcsmenuitem.h"
#include "mcsoperationmanager.h"

class CMCSReqNotification;
/**
 *  MCS callback interface, its pure interface class, application need to 
 *  Write there own callback derriving from this class.
 */
 
 class MMCSCallback 
    {
    /**
     * Handles notifications caused by an asynchronous calls.
     *
     * @return Success/Error code for the callback.
     */
    public:
        
    virtual void NotifyResultL(TInt aErrCode, TAny* aResult = NULL ) = 0;
    };

/**
 * MCS Service class.
*/
class CMCSService : public CBase
	{
	public:

		/**
		 * Two Phase Constructor
		*/ 
		IMPORT_C static CMCSService* NewL(const TDesC& aContent);

		/**
		 * Destructor
		*/ 
		virtual ~CMCSService();

	private:

		/**
		 * Constructor
		*/ 
		CMCSService();

		/**
		 * Constructor
		*/ 
		void ConstructL(const TDesC& aContent);
	
		/**
         * @param aInputMap map containing attributes to add or modify
         * @param aMenuitem menuitem object 
         * @param aFlag flag enum
         * @return void
        */
		void SetFlagInMenuitemL( CLiwMap& aInputMap, 
                            CMenuItem& aMenuitem, TMenuItem::TFlags aFlag);
		
    
	
	public:
	    
	    /**
        * Add / Changes an item and its attribute. 
        * @param aCallBack CallBack for asynchronous requests
        * @param aType type of item
        * @param aParentId parent id of the item
        * @param aBeforeId item id after which the new item will be added 
        * @param aInputMap attributes to add or modify
        * @return void
        */ 
	    IMPORT_C void UpdateItemL( MMCSCallback* aCallBack, TDesC& aType, 
	    		TInt aId, TInt aParentId, TInt aBeforeId, CLiwMap& aInputMap );

	    /**
   	    * Gets the list of items from xml 
   	    * @param aInParamList list containing input parameters for getlist
		* @param aCallBack CallBack for asynchronous requests
	    * @return void
		*/ 
		IMPORT_C void GetListL( const CLiwGenericParamList& aInParamList, 
				MMCSCallback* aCallBack );


        /**
         * Gets the Uid of running applications 
         * @param aCallback CallBack for asynchronous requests
         * @return void
        */
        IMPORT_C  void GetRunningAppsL( MMCSCallback* aCallback );
        
		/**
         * Execute a action (open/remove) 
         * @param aCallback CallBack for asynchronous requests
         * @param aItemId item id
         * @param aAction action to execute
         * @return void
        */
        IMPORT_C void ExecuteActionL( MMCSCallback* aCallback,
                TInt aItemId, TDesC8& aAction );

        /**
        * Start notifying the changes 
        * @param aCallback CallBack for asynchronous requests, Ownership is passed
        * @param aFolderId folder id to observe the events notification.
        * @param aEvents events to observe
        * @return void
        */ 
        IMPORT_C void RegisterObserverL( TInt aFolderId,TInt aEvents,MMCSCallback* aCallBack  );
        
        /**
        * Stop notifying the changes 
        */ 
        IMPORT_C void UnRegisterObserverL();
        
        /**
        * Delete an Item
        * @param aCallback CallBack for asynchronous requests, Ownership is passed
        * @param aItemId item id to delete 
        * @return void
        */ 
        IMPORT_C void DeleteL( MMCSCallback* aCallBack, TInt aItemId);
    	
    	/**
         * Set Get running applications output to output parameter
         * @param aOutputArray GetList array
         * @param aOutParamList Output parammeter
         * @return void
         */
    	IMPORT_C void SetRunningAppsOutputL( RArray<TUid>& aOutputArray,
    	        CLiwGenericParamList& aOutParamList );

        /**
         * Organize
         * @param aCallback CallBack for asynchronous requests, Ownership is passed
         * @return void
         */
    	IMPORT_C void OrganizeL(MMCSCallback* aCallBack, 
    			TInt aItemId, TInt aBeforeItemId);

        /**
         * Organize
         * @param aCallback CallBack for asynchronous requests, Ownership is passed
         * @return void
         */
    	IMPORT_C void OrganizeL(MMCSCallback* aCallBack, 
    			const RArray<TInt>& aList, TInt aFolderId, TInt aBeforeItemId);
    	
    	/**
         * Set the events and folder to notify
         * @param aFolderId folder id which modified
         * @param aEvent events occured
         * @param aOutParamList Output parammeter
         * @return void
         */
        IMPORT_C void SetReqNotificationOutputL( TInt aFolderId, 
                TInt aEvent, CLiwGenericParamList& aOutParamList );

	private:
	    
		/** Menu */
	    RMenuSapi iMCS;
	    /** Result Type*/
	    TBool iFlatResult;
        /** Notification events registered status */
	    TBool iIsRegistered;
	    /** Event notifier */
	    CMCSReqNotification *iReqNotifier;
	    /* own */
	    RMcsOperationManager iOperationManager;
        /**
         * CLiwGenericParamList class pointer
         * Not Own.
        */
        CLiwGenericParamList* iAttributes;

        /**
         * CDesC16ArrayFlat class pointer
         * Own.
        */
        CDesC16Array* iIgnoredAttributes;
        
        /**
         * CDesC16ArrayFlat class pointer
         * Own.
        */
        CDesC16Array* iRequiredAttributes;
	    
	};


#endif /* __MCSSERVICE_H */
