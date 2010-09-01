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


#include <e32base.h>
#include <e32def.h>
#include "mcsdef.h"
#include "mcsmenufilter.h"
#include "mcsmenuitem.h"

#include "serviceerrno.h"
#include "mcsservice.h"
#include "mcsmcinterface.h"
#include "mcscallback.h"
#include "mcsconstants.h"

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CMCSMCInterface* CMCSMCInterface::NewL(const TDesC& aContent)
	{
	CMCSMCInterface* self = new (ELeave) CMCSMCInterface();
	CleanupStack::PushL(self);
	self->ConstructL(aContent);
	CleanupStack::Pop(self);
	return self;
	}
// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//	
CMCSMCInterface::~CMCSMCInterface()
	{
	delete iMCSService;
	}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CMCSMCInterface::CMCSMCInterface()
	{
	}

// ---------------------------------------------------------------------------
// Symbian Constructor
// ---------------------------------------------------------------------------
//	
void CMCSMCInterface::ConstructL(const TDesC& aContent)
	{
	iMCSService = CMCSService::NewL(aContent);
	}

// ---------------------------------------------------------------------------
//  Executes the SAPI as per params
// ---------------------------------------------------------------------------
//
void CMCSMCInterface::ExecuteCmdL(const TDesC8& aCmdName,
		const CLiwGenericParamList& aInParamList,
		CLiwGenericParamList& aOutParamList, TUint aCmdOptions,
		MLiwNotifyCallback* aCallback)
	{
	TInt errcode = KErrNotSupported;
	TInt32 transactionId = -1;

	aOutParamList.AppendL(TLiwGenericParam(KErrorCode,
			TLiwVariant(ErrCodeConversion(KErrNone))));
	
	if ( !aCallback )
		{
		// Callback is a null pointer
		errcode = KErrArgument;
		}
	else
		{
		if (aCmdName.CompareF(KCmdExecuteAction) == 0)
			{
			TRAP(errcode, ExecuteActionL( aInParamList, aCallback ));
			}
		else if (aCmdName.CompareF(KcmdRunningApps) == 0)
	        {
			TRAP(errcode, GetRunningAppsL( aInParamList, aCallback ));
			}
		else if (aCmdName.CompareF(KCmdOrganize) == 0)
			{
			aOutParamList.Reset();
			TRAP(errcode, OrganizeL( aInParamList, aOutParamList, aCallback ));
			}
		
		// Append transaction id in case of asynchronous requests 
		if ( (KLiwOptASyncronous & aCmdOptions ) && (errcode
				== KErrNone ) && (transactionId != -1 ))
			{
			aOutParamList.AppendL(TLiwGenericParam(KTransactionID,
					TLiwVariant(TInt32(transactionId))));
			}
		}

	if (errcode != KErrNone)
		{
		aOutParamList.Reset();
		aOutParamList.AppendL(TLiwGenericParam(KErrorCode,
				TLiwVariant(ErrCodeConversion(errcode))));
		}
	}

// ---------------------------------------------------------------------------
// Closes the interface
// ---------------------------------------------------------------------------
//
void CMCSMCInterface::Close()
	{
	delete this;
	}

// ---------------------------------------------------------------------------
// Closes the interface
// ---------------------------------------------------------------------------
//
void CMCSMCInterface::GetRunningAppsL(const CLiwGenericParamList& aInParamList,
		MLiwNotifyCallback* aCallback)
	{
	CMCSCallback* cb = CMCSCallback::NewL(aCallback, aInParamList,
			aCallback->GetTransactionID(), *iMCSService,
			CMCSCallback::EGetRunningApps);
	CleanupStack::PushL(cb);
	iMCSService->GetRunningAppsL(cb);
	CleanupStack::Pop(cb);

	}
// ---------------------------------------------------------------------------
// Execute actions 
// ---------------------------------------------------------------------------
//
void CMCSMCInterface::ExecuteActionL(const CLiwGenericParamList& aInParamList,
		MLiwNotifyCallback* aCallback)
	{
	TLiwVariant param;
	TInt32 itemId = -1;
	TInt pos = 0;
	TPtrC8 action;
	TBuf8<KMenuMaxAttrNameLen> buf8;

	const TLiwGenericParam* inParam = aInParamList.FindFirst(pos, KInData);

	if (inParam)
		{
		const CLiwMap* inputMap = inParam->Value().AsMap();
		if (inputMap)
			{
			// Item Id
			buf8.Copy(KMenuAttrId());
			if (inputMap->FindL(buf8, param))
				{
				itemId = param.AsTInt32();
				}
			param.Reset();
			// Action 
			if (inputMap->FindL(KAction, param))
				{
				action.Set(param.AsData());
				if (param == KActionOpen() )
					{
					action.Set(KMenuCmdOpen());
					}
				else
					if (param == KActionRemove() )
						{
						action.Set(KMenuCmdRemove());
						}
					else
						{
						itemId = -1;
						}
				}
			else
				{
				itemId = -1;
				}
			param.Reset();
			}
		}

	if (itemId == -1)
		{
		User::Leave(KErrArgument);
		}
	else
		{
		CMCSCallback* cb = CMCSCallback::NewL(aCallback, aInParamList,
				aCallback->GetTransactionID(), *iMCSService,
				CMCSCallback::EExecuteAction);
		CleanupStack::PushL(cb);
		iMCSService->ExecuteActionL(cb, itemId, action);
		CleanupStack::Pop(cb);
		}
	}

// ---------------------------------------------------------------------------
// GetParamFromMap
// ---------------------------------------------------------------------------
//
TBool CMCSMCInterface::GetParamFromMapL( const CLiwMap* aInputMap,
		const TDesC8& aParamName, RArray<TInt>& aOutputList )
	{
	TBool status = EFalse;
	TLiwVariant param;
	param.PushL();
	if ( aInputMap->FindL ( aParamName, param ) )
		{
		const CLiwList* paramList = param.AsList();
		if(paramList)
			{
			for (TInt i = 0; i < paramList->Count(); i++)
				{
				TLiwVariant variant;
				variant.PushL();
				if (paramList->AtL(i, variant) )
					{
					TInt value = 0;
					if (variant.Get(value) )
						{
						aOutputList.AppendL( value );
						}
					}
				CleanupStack::PopAndDestroy( &variant );
				}
			}
		}
	CleanupStack::PopAndDestroy( &param );
	
	if( aOutputList.Count() > 0 )
		{
		status = ETrue;
		}
		
	return status;
	}

// ---------------------------------------------------------------------------
// GetParamFromMap
// ---------------------------------------------------------------------------
//
TBool CMCSMCInterface::GetParamFromMapL( const CLiwMap* aInputMap,
		const TDesC8& aParamName, TInt& aOutputValue )
	{
	TBool status = EFalse;
	TLiwVariant param;
	param.PushL();
	if ( aInputMap->FindL( aParamName, param ) )
		{
		param.Get( aOutputValue );
		status = ETrue;
		}
	CleanupStack::PopAndDestroy( &param );
	return status;
	}

// ---------------------------------------------------------------------------
// Organize 
// ---------------------------------------------------------------------------
//
void CMCSMCInterface::OrganizeL( const CLiwGenericParamList& aInParamList,
		                         CLiwGenericParamList& aOutParamList, 
		                         MLiwNotifyCallback* aCallback )
	{
	TInt errcode = KErrArgument;
	TBuf8<KMenuMaxAttrNameLen> attrname;
	TBuf16<KMenuMaxAttrValueLen> paramType;
	TInt pos = 0;
	
	const TLiwGenericParam* inParam = aInParamList.FindFirst(pos, KInData);
	if (inParam)
		{
		const CLiwMap* inputMap = (CLiwMap*)inParam->Value().AsMap();
		if (inputMap)
			{			
			RArray<TInt> list;
			TInt beforeItemId = 0;
            TInt folderId = 0;
			CleanupClosePushL( list );
			if( GetParamFromMapL( inputMap, KItemIds, list ) )
				{
				GetParamFromMapL( inputMap, KBeforeItemId, beforeItemId );
				
				CMCSCallback* cb = CMCSCallback::NewL( aCallback, aInParamList,
					aCallback->GetTransactionID(), *iMCSService,
					CMCSCallback::EOrganize );

				CleanupStack::PushL(cb);
				if( GetParamFromMapL( inputMap, KFolderId, folderId ) )
					{
					iMCSService->OrganizeL( cb, list, folderId, beforeItemId );
					}
				else
					{
					iMCSService->OrganizeL( cb, list[0], beforeItemId );
					}
				CleanupStack::Pop(cb);
				errcode = KErrNone; 
				}

			CleanupStack::PopAndDestroy( &list );
			}
		}

	aOutParamList.AppendL( TLiwGenericParam( KErrorCode,
    		TLiwVariant( ErrCodeConversion( errcode ) ) ) );
	}

// ---------------------------------------------------------------------------
// ErrCode Conversion
// ---------------------------------------------------------------------------
//
TInt32 CMCSMCInterface::ErrCodeConversion(TInt code)
	{
	TInt32 err;
	switch (code)
		{
		case KErrCancel:
			// Returning KErrNone incase of KErrCancel
		case KErrNone:
			err= SErrNone;
			break;

		case KErrNotFound:
			err= SErrNotFound;
			break;

		case KErrNoMemory:
			err = SErrNoMemory;
			break;

		case KErrInUse:
			err = SErrServiceInUse;
			break;

		case KErrNotSupported:
			err = SErrServiceNotSupported;
			break;

		case KErrBadName:
			err = SErrBadArgumentType;
			break;

		case KErrArgument:
			err = SErrInvalidServiceArgument;
			break;

		default:
			err = SErrGeneralError;
			break;
		}

	return err;
	}

// End of file

