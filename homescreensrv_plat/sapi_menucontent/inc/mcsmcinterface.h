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


#ifndef __MCSMCINTERFACE_H
#define __MCSMCINTERFACE_H

#include <liwcommon.h>
#include <liwserviceifbase.h>
#include <liwbufferextension.h>

class CMCSService;
class TMenuItem;
/**
 * This class provides the MCS SAPI interface to LIW Framework 
*/
class CMCSMCInterface : public CBase, public MLiwInterface
	{
		
	public:

	    /**
	     * Two-phased constructor.
	     * @return CMCSMCInterface* 
	    */
		static CMCSMCInterface* NewL(const TDesC& aContent);

	    /**
	    * Destructor.
	    */
		virtual ~CMCSMCInterface();

	private:

		/**
	     * Constructor
	    */
		CMCSMCInterface();

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
	 * Same functionality required outside the CMCSMCInterface class in case of async
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
         * Issues GetRunningApplications
         * @param aInParamList Input param list
         * @param aCallback Callback object pointer
         * @return void
        */
        void GetRunningAppsL (const CLiwGenericParamList& aInParamList, 
                MLiwNotifyCallback* aCallback );

		/**
	     * Issues ExecuteAction
	     * @param aInParamList Input param list
         * @param aCallback Callback object pointer
	     * @return void
	    */
		void ExecuteActionL( const CLiwGenericParamList& aInParamList, 
				MLiwNotifyCallback* aCallback );

		/**
	     * Issues Organize
	     * @param aInParamList Input param list
	     * @param aOutParamList Output param list
         * @param aCallback Callback object pointer
	     * @return void
	    */
		void OrganizeL( const CLiwGenericParamList& aInParamList,
		        CLiwGenericParamList& aOutParamList,
		        MLiwNotifyCallback* aCallback );		
		
		/**
	     * Issues GetGenericParam
	     * @param aInputMap input map
	     * @param aParamName name of parameter
         * @param aOutputList output parameter
	     * @return TBool return true if exist at least one element in list
	    */
		TBool GetParamFromMapL( const CLiwMap* aInputMap,
				const TDesC8& aParamName, RArray<TInt>& aOutputList);

		/**
	     * Issues GetGenericParam
	     * @param aInputMap Input map
	     * @param aParamName name of parameter
         * @param aOutputValue output parameter
	     * @return TBool return true if exist expcected param
	    */
		TBool GetParamFromMapL( const CLiwMap* aInputMap,
				const TDesC8& aParamName, TInt& aOutputValue);
		
	private:

	  	/**
	     * CMCSService class pointer
	    */		
		CMCSService* iMCSService;

	};


#endif // __MCSMCINTERFACE_H

// End of file
