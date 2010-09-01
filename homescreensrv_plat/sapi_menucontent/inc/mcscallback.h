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


#ifndef __MCSCALLBACK_H
#define __MCSCALLBACK_H

#include <liwcommon.h>
#include <liwserviceifbase.h>
#include <liwbufferextension.h>

#include "mcsservice.h"

/**
*
*
*/
class CMCSCallback : public CBase, public MMCSCallback
	{
	public:

    enum TCmds 
        {
        EGetList = 0, 
        EExecuteAction,
        ERequestNotification,
        EAddORChange,
        EDelete,
        EOrganize,
        EGetRunningApps
        };
        
	public:

	  	/**
	     * Two-phase Constructor
	     * @return   CMsgCallbackBase* 
	    */		
		static CMCSCallback* NewL( MLiwNotifyCallback *aPtrNotifyCallback, 
		        const CLiwGenericParamList &aPtrInParamList, TInt32 aTransactionId,
		        CMCSService& aMcsService,TCmds aCmd);

	  	/**
	     * Gives the result of asynchronous SAPI
	     * @param aErrCode errcode
	     * @param aResult Result
	     * @return   void
	    */		
		void NotifyResultL(TInt aErrCode, TAny* aResult = NULL);
		
	private:

	  	/**
	     * Constructor
	    */		
		CMCSCallback( MLiwNotifyCallback *aPtrNotifyCallback, 
		        const CLiwGenericParamList &aPtrInParamList, TInt32 aTransactionId,
		        CMCSService& aMcsService,TCmds aCmd );
	  	/**
	    */		
		void HandleComandL( TInt &aErrCode, TAny* aResult, 
				CLiwGenericParamList& aParamList );

	private:
	
	    MLiwNotifyCallback* iPtrNotifyCallback;
        const CLiwGenericParamList* iPtrInParamList;
        TInt iTransactionId;
        CMCSService* iMcsService;
        TCmds iCmd;
	};


#endif __MCSCALLBACK_H
