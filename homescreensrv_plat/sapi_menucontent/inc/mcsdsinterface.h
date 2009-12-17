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
* Description:  Defination of class CMCSInterface
*
*/


#ifndef __MCSDSINTERFACE_H
#define __MCSDSINTERFACE_H

#include <liwcommon.h>
#include <liwserviceifbase.h>
#include <liwbufferextension.h>

class CMCSService;
class CMenuFilter;
class TMenuItem;
/**
 * This class provides the MCS SAPI interface to LIW Framework 
*/
class CMCSDSInterface : public CBase, public MLiwInterface
	{
		
	public:

	    /**
	     * Two-phased constructor.
	     * @return CMCSDSInterface* 
	    */
		static CMCSDSInterface* NewL(const TDesC& aContent);

	    /**
	    * Destructor.
	    */
		virtual ~CMCSDSInterface();

	private:

		/**
	     * Constructor
	    */
		CMCSDSInterface();

		/**
	     * Symbian Constructor
	    */
		void ConstructL(const TDesC& aContent);

	// MLiwInterface member functions
	public:

		/**
	     * Executes the SAPI as per params
	     * @param aCmdName Command name defining which operation is to be performed
	     * @param aInParamList  Input param list
	     * @param aOutParamList Output list
	     * @param aCmdOptions Command option
	     * @param aCallback Callback object pointer
	    */
		virtual void ExecuteCmdL( const TDesC8& aCmdName,
		                       const CLiwGenericParamList& aInParamList,
		                       CLiwGenericParamList& aOutParamList,
		                       TUint aCmdOptions = 0,
		                       MLiwNotifyCallback* aCallback = 0 );
		
		/**
	     * Closes the interface 
	    */
		virtual void Close();
		
	/**
     * Static Functions
	 * 
	 * Same functionality required outside the CMCSDSInterface class in case of async
	 * operation. Those set of functions can be given as static.
    */
	public:
 
		/**
	     * Converts symbian errcode to equivalent SAPI error code
	     * @param aErrorCode Symbian error code
		 *
	     * @return mapped SAPI error code
	    */
 		static TInt32 ErrCodeConversion(TInt aErrorCode);

	private:
	
	    /**
         * Issues GetList
         * @param aInParamList Input param list
         * @param aCallback Callback object pointer
         * @return void
        */
        void AddItemL(const CLiwGenericParamList& aInParamList, 
                      MLiwNotifyCallback* aCallback );
		/**
	     * Issues GetList
	     * @param aInParamList Input param list
	     * @param aCallback Callback object pointer
	     * @return void
	    */
		void GetListL(const CLiwGenericParamList& aInParamList, 
   					  MLiwNotifyCallback* aCallback );

		/**
		*
	    */
		void CheckGetListInputListL( const CLiwGenericParamList&
					aInParamList );
		
		/**
	     * Issues Change Notification
	     * @param aInParamList Input param list
	     * @param aCallback Callback object pointer
	     * @return void
	    */
		void RequestNotificationL( const CLiwGenericParamList& aInParamList, 
				                       			MLiwNotifyCallback* aCallback );
		/**
         * Issues Delete an item 
         * @param aInParamList Input param list
         * @param aCallback Callback object pointer
         * @return void
        */
        void DeleteL(const CLiwGenericParamList& aInParamList, 
                      MLiwNotifyCallback* aCallback );

         
	private:

	  	/**
	     * CMCSService class pointer
	    */		
		CMCSService* iMCSService;

	};


#endif // __MCSDSINTERFACE_H

// End of file
