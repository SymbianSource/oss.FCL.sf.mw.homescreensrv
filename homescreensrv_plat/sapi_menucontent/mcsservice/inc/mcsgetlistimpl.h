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


#ifndef __MCSGETLISTIMPL_H
#define __MCSGETLISTIMPL_H

#include <e32base.h>

class MMCSCallback;
class RMenuSapi;
class TMenuItem;
class RMcsOperationManager;
class CLiwGenericParamList;
class CLiwDefaultMap;
class CLiwList;
class CLiwDefaultList;
class CAknIcon;

/**
 * This class implements MCS SAPI core logic for GetList async requests.
 * 
 */
class CMCSGetListImpl : public CActive
	{
	public:
	
		/**
		* Two-phased constructor.
	    * @param aMCS MenuContentService object.
	    * @param aCallback Callback Object for Change notification.
	    * @return CMCSGetListImpl reference.
		*/
		static CMCSGetListImpl* NewL( RMenuSapi& aMCS, MMCSCallback* aCallback, 
		                            RMcsOperationManager& aOperationManager );
		
		/**
        * GetListL for asynchronous calls.
        * @param aInParamList list containing input parameters for getlist
        */
		void GetListL( const CLiwGenericParamList& aInParamList  );
		
		/**
        * GetListL for synchronous calls.
        */
		void GetListL( );
		
		/**
	     * Destructor.
    	*/
		virtual ~CMCSGetListImpl();
	
	private:

		/** 
	    * Default constructor.
	    * @param aMCS MenuContentService object.
	    * @param aCallback It is a callback notification.
	    * @param aOperationManager for safe cleanup
	    */
		CMCSGetListImpl( RMenuSapi& aMCS, MMCSCallback* aCallback,
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
		void NotifyRequestResultL(TInt aReason);

		/*
		* Handle append icons
		*/
		void HandleAppendIconsL( CLiwGenericParamList* aParamList );

		/**
		* Appends icons to generic param list.
		*/ 
		void AppendIconsL( const CLiwList* aList, 
		    CLiwDefaultList* aDefaultList );

		/**
		* Appends icon to specific map.
		*/ 
		void AppendIconL( CLiwDefaultMap* aMap );
		void GetMaskAndBitmapL( CLiwDefaultMap& aMap, CAknIcon* aIcon );

	private:
		/*own*/
		RBuf8 iSerializedInputList;
		/*own*/
		RBuf8 iSerializedOutputList;		
		/** Callback Object. Not owned. */ 
		MMCSCallback* iCallback;
		/** iMCS reference. Not owned. */
		RMenuSapi iMCS;
		/*not own*/
        RMcsOperationManager& iOperationManager;
	};



#endif //__MCSGETLISTIMPL_H

// End of file
