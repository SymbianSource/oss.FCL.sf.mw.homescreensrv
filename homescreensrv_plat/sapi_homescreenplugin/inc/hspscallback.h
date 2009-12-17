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
* Description:  Defination of class CHspsCallback
*
*/


#ifndef __HSPSCALLBACK_H
#define __HSPSCALLBACK_H

#include <LiwCommon.h>
#include <LiwServiceIfBase.h>
#include <LiwBufferExtension.h>

#include "hspsconfigurationservice.h"


/**
 *  @ingroup group_hsps_sapi
 *  HSPS callback interface, its pure interface class, application need to 
 *  Write there own callback derriving from this class.
 */
 
 class MHspsCallback 
    {
    /**
     * Handles notifications caused by an asynchronous calls.
     *
     * @return Success/Error code for the callback.
     */
    public:
        
    virtual void NotifyResultL( TInt aErrCode, ChspsRequestNotificationParams& aParams, RArray<TInt>& aIds ) = 0;
    };

/**
* @ingroup group_hsps_sapi 
* This class provides asynchronous notifications to HSPS SAPI user
*/
class CHspsReqNotifCallback : public CBase, public MHspsCallback
	{
	  
	public:

	  	/**
	     * Two-phase Constructor
	     * @return   CHspsReqNotifCallback* 
	    */		
		static CHspsReqNotifCallback* NewL( MLiwNotifyCallback *aPtrNotifyCallback, 
		        const CLiwGenericParamList &aPtrInParamList, TInt32 aTransactionId,
		        CHspsConfigurationService& aHspsService );
        
		
		/**
		 * Destructor.
		 */
       virtual ~CHspsReqNotifCallback();

	  	/**
	     * Gives the result of asynchronous SAPI command Request Notification 
	     * @param aErrCode errcode
	     * @param aResult Result
	     * @return   void
	    */		
		void NotifyResultL( TInt aErrCode, ChspsRequestNotificationParams& aParams, RArray<TInt>& aIds );
	private:

	  	/**
	     * Constructor
	    */		
		CHspsReqNotifCallback( MLiwNotifyCallback *aPtrNotifyCallback, 
		        const CLiwGenericParamList &aPtrInParamList, TInt32 aTransactionId,
		        CHspsConfigurationService& aMcsService );
       
		const TDesC8& ConvertEvent2LiwData( TInt aEvent );
	private:
	
	    MLiwNotifyCallback* iPtrNotifyCallback;
        const CLiwGenericParamList* iPtrInParamList;
        TInt iTransactionId;
        CHspsConfigurationService* iHspsConfigurationService;
	};


#endif __HSPSCALLBACK_H
