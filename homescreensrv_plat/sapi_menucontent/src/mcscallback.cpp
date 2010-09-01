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

#include "mcscallback.h"
#include "mcsdsinterface.h"
#include "mcsconstants.h"

// ---------------------------------------------------------------------------
// Two-phase Constructor
// ---------------------------------------------------------------------------
//
CMCSCallback* CMCSCallback::NewL( MLiwNotifyCallback *aPtrNotifyCallback, 
								  const CLiwGenericParamList& aPtrInParamList, 
								  TInt32 aTransactionId, CMCSService& aMcsService,TCmds aCmd  )
	{
	return new (ELeave)	CMCSCallback( aPtrNotifyCallback, aPtrInParamList, aTransactionId,
	    aMcsService, aCmd);
	}
	
// ---------------------------------------------------------------------------
//  Constructor
// ---------------------------------------------------------------------------
//	
CMCSCallback::CMCSCallback( MLiwNotifyCallback *aPtrNotifyCallback, 
							const CLiwGenericParamList& aPtrInParamList, 
							TInt32 aTransactionId, CMCSService& aMcsService,
							TCmds aCmd )
	{
	iPtrNotifyCallback = aPtrNotifyCallback; 
	iPtrInParamList = &aPtrInParamList;
	iTransactionId = aTransactionId;
	iMcsService = &aMcsService;
	iCmd = aCmd;
	}
	
// ---------------------------------------------------------------------------
// Gives the result of asynchronous SAPI calls
// ---------------------------------------------------------------------------
//
void CMCSCallback::NotifyResultL( TInt aErrCode, TAny* aResult )
	{
	CLiwGenericParamList* outParams= NULL;

    if(iCmd != ERequestNotification || 
	        (iCmd == ERequestNotification  && aErrCode == KErrCancel))
	    {
		CleanupStack::PushL( this);
		outParams = CLiwGenericParamList::NewLC();
		outParams->AppendL(TLiwGenericParam(KErrorCode,
				TLiwVariant(CMCSDSInterface::ErrCodeConversion(aErrCode))));
		}
	else
		{
		outParams = CLiwGenericParamList::NewLC();
		}

	HandleComandL( aErrCode, aResult, *outParams );

    
	TInt event = KLiwEventInProgress;
	if (aErrCode == KErrCancel)
		{
		event = KLiwEventCanceled;
		}
	else
		if (aErrCode != KErrNone)
			{
			event = KLiwEventStopped;
			}

	((MLiwNotifyCallback*)iPtrNotifyCallback)->HandleNotifyL(
			iTransactionId, event, *outParams,
			*((CLiwGenericParamList*)iPtrInParamList));

	CleanupStack::PopAndDestroy(outParams);
	if (iCmd != ERequestNotification || (iCmd == ERequestNotification
			&& aErrCode == KErrCancel))
		{
		CleanupStack::PopAndDestroy(this);
		}
	}


// ---------------------------------------------------------------------------
// CMCSCallback::HandleComandL
// ---------------------------------------------------------------------------
//
void CMCSCallback::HandleComandL( TInt &aErrCode, TAny* aResult,
		CLiwGenericParamList& aParamList)
	{

	switch (iCmd)
		{
		case EGetList:
			{
			if (aResult && aErrCode == KErrNone)
				{
				aParamList.AppendL(*static_cast<CLiwGenericParamList*>(aResult));
				}
			}
			break;
		case EExecuteAction:
			{
			// Do Nothing.
			}
			break;
		case ERequestNotification:
			{
			if (aResult)
				{
				TInt &folderId = *((TInt*)aResult);
				TInt event = aErrCode;
				if (event)
					{
					aErrCode = KErrNone;
					}
				aParamList.AppendL(TLiwGenericParam( KErrorCode,
						TLiwVariant(CMCSDSInterface::ErrCodeConversion(aErrCode))));
				iMcsService->SetReqNotificationOutputL(folderId, event, aParamList );
					
				}
			}
			break;
		case EAddORChange:
			{
			if (aErrCode== KErrNone)
				{
				TBuf8<KMenuMaxAttrNameLen> attrname;
				TInt &itemId = *((TInt*)aResult);
				attrname.Copy(KMenuAttrId());
				aParamList.AppendL(TLiwGenericParam(attrname, TLiwVariant(  (TInt32) itemId ) ) );
				}
			// Do Nothing.
			}
			break;
		case EDelete:
			{
			// Do Nothing	    	
			}
			break;
		case EOrganize:
			{
			// Do Nothing	    	
			}
			break;
		case EGetRunningApps:
			{
			if (aResult)
				{
				RArray<TUid>& outputArray = *((RArray<TUid>*)aResult);
				iMcsService->SetRunningAppsOutputL(outputArray, aParamList);
				}
			}
			break;
		default:
			{
			// Do Nothing	    	
			}
			break;
		}
	}

// End of file
