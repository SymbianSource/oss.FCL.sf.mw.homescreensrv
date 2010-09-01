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
* Description:  CMCSOrganize declaration
*
*/


#ifndef __MCSORGANIZEIMPL_H
#define __MCSORGANIZEIMPL_H

#include <e32base.h>
#include "mcsmenuopobserver.h"

class MMCSCallback;
class RMenu;
class TMenuItem;
class CLiwList;
class RMcsOperationManager;
/**
 * This class implements MCS SAPI core logic for GetList async requests.
 * 
 */
class CMCSOrganizeImpl : public CActive, public MMCSMenuOpObserver
    {
    public:
    
        /**
        * Two-phased constructor.
        * @param aMCS MenuContentService object.
        * @param aCallback Callback Object for Change notification.
        * @return CMCSOrganizeImpl reference.
        */
        static CMCSOrganizeImpl* NewL( RMenu& aMCS, MMCSCallback* aCallback,
                                    RMcsOperationManager& aOperationManager );
        
        /**
        * OrganizeL for asynchronous calls.
        * @param aItemId item id to move.
        * @param aBeforeItemId Before item id.
        */
        void OrganizeL( TInt aItemId, TInt aBeforeItemId );

        /**
        * OrganizeL for asynchronous calls.
        * @param aList list item ids to move.
        * @param aFolderId Folder id.
        * @param aBeforeItemId Before item id.
        */
        void OrganizeL( const RArray<TInt>& aList, 
        		TInt aFolderId, TInt aBeforeItemId );
        
        /**
        * OrganizeL for synchronous calls.
        */
        void OrganizeL();
        
        /**
         * Destructor.
        */
        virtual ~CMCSOrganizeImpl();
    
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
        CMCSOrganizeImpl( RMenu& aMCS, MMCSCallback* aCallback,
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
        /** list item ids. **/ 
        RArray<TInt> iList;
        /** item id. Not owned **/ 
        TInt iItemId;
        /** folder id **/ 
        TInt iFolderId;
        /** Before item id **/ 
        TInt iBeforeItemId;
        /** single or list **/ 
        TBool iIsSingle;
        /*not own*/
        RMcsOperationManager& iOperationManager;
    };

#endif //__MCSORGANIZEIMPL_H

// End of file
