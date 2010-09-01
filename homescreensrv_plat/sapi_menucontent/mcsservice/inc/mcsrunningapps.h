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


#ifndef MCSRUNNINGAPPS_H_
#define MCSRUNNINGAPPS_H_

#include <e32base.h>
#include "mcsmenunotifier.h"

class MMCSCallback;
class RMenu;
class TMenuItem;
class RMcsOperationManager;

/**
 * This class implements MCS SAPI core logic for Getrunningapplications async requests.
 * 
 */
class CMCSRunningApps: public CActive
    {
    public:
    
        /**
        * Two-phased constructor.
        * @param aCallback Callback Object for Change notification.
        * @return CMCSRunningApps reference.
        */
        static CMCSRunningApps* NewL( RMenu& aMCS, MMCSCallback* aCallback, 
                         RMcsOperationManager& aOperationManager );
        
        /**
         * Destructor.
        */
        virtual ~CMCSRunningApps();
        
    private:

         /** 
         * Default constructor.
         * @param aCallback It is a callback notification.
         */
         CMCSRunningApps( RMenu& aMCS, MMCSCallback* aCallback,
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

    public : 
    
        /**
        * GetRunningApplicationL for asynchronous  calls.
        */
        void GetRunningApplicationL();
         
    public: // from MMCSMenuOpObserver
    
        /**
         * RunningAppsChangedL operation complete call back.
         */
        void RunningAppsChangedL();
    
    private:
    
        /** iMCS reference. Not owned. */
        RMenu iMCS;
        /** Callback Object. Not owned. */ 
        MMCSCallback* iCallback;
        /** UID Array. Owned */
        RArray<TUid> iUidArray;
        /** item id **/
        TInt iItemId;
        /** Action to Execute **/
        TPtrC8 iAction;
        /*not own*/
        RMcsOperationManager& iOperationManager;

        
    };

#endif /*MCSRUNNINGAPPS_H_*/
