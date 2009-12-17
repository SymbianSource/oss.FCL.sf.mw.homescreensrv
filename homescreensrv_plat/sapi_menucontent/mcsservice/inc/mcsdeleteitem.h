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


#ifndef __MCSDELETEITEM_H_
#define __MCSDELETEITEM_H_

#include <e32base.h>
#include "mcsmenuopobserver.h"

class MMCSCallback;
class RMenu;
class RMcsOperationManager;
/**
 * This class implements MCS SAPI core logic for delete an item  async requests.
 * 
 */
class CMCSDeleteItem : public CActive, public MMCSMenuOpObserver
    {
    public:
    
        /**
        * Two-phased constructor.
        * @param aMenu MenuContentService object.
        * @param aCallback Callback Object for Change notification.
        * @return CMCSUpdateItem reference.
        */
        static CMCSDeleteItem* NewL( RMenu& aMenu, MMCSCallback* aCallback,
                                RMcsOperationManager& aOperationManager );
        
        /**
        * Delete item and its attributes
        * @param aItemId item id to delete.
        */
        void DeleteItemL(TInt aItemId);
        
        /**
         * Destructor.
        */
        virtual ~CMCSDeleteItem();
        
    public: // from MMCSMenuOpObserver
        
        /**
         * MenuOpCompletedL menu operation complete call back.
         * @param aStatus status of the operation 
         */
        void MenuOpCompletedL(TInt aStatus );
        
    private:

        /** 
        * Default constructor.
        * @param aMenu MenuContentService object.
        * @param aCallback It is a callback notification.
        */
        CMCSDeleteItem( RMenu& aMenu, MMCSCallback* aCallback,
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
        /** Item id to delete. */
        TInt iItemId;
        /*not own*/
        RMcsOperationManager& iOperationManager;
    };

#endif /*__MCSDELETEITEM_H_*/
// End of file
