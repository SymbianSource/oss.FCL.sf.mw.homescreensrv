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



#include <ecom/implementationproxy.h>
#include <LiwGenericParam.hrh>

#include "mcsconstants.h"
#include "mcsdsinterface.h"
#include "mcsmcinterface.h"
#include "mcsservicehandler.h"
#include "serviceerrno.h"

// ---------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------
//
CMCSServiceHandler* CMCSServiceHandler::NewL()
	{
	return new(ELeave) CMCSServiceHandler();
	}

// ---------------------------------------------------------
// Constructor.
// ---------------------------------------------------------
//
CMCSServiceHandler::CMCSServiceHandler()
	{
	}

// ---------------------------------------------------------
// Called by the LIW framework to initialise necessary information 
// from the Service Handler. This method is called when the consumer makes 
//the attach operation.
// ---------------------------------------------------------
//
void CMCSServiceHandler::InitialiseL( 
		MLiwNotifyCallback& /*aFrameworkCallback*/, 
		const RCriteriaArray& aInterest)
	{
	TInt count = aInterest.Count();
	for(TInt index = 0; index < count ; index++)
		{
		if(aInterest[index]->ContentType() == KMCSService)
			return;
		}

	User::Leave( SErrNotFound );
	}

// ---------------------------------------------------------
// Executes generic service commands included in criteria
// ---------------------------------------------------------
//
void CMCSServiceHandler::HandleServiceCmdL( 
		const TInt& aCmdId, 
		const CLiwGenericParamList& aInParamList,
		CLiwGenericParamList& aOutParamList,
		TUint /*aCmdOptions*/,
		const MLiwNotifyCallback* /*aCallback*/)
	{
	if ( aCmdId == KLiwCmdAsStr )
		{
		TInt pos = 0;
		const TLiwGenericParam* content = aInParamList.FindFirst( pos, KContentName );
		const TLiwGenericParam* cmd     = aInParamList.FindFirst( pos, KGenericParamID );
		if ( content && cmd  )
			{
			TPtrC cmdName;
			TPtrC8 cmdPtr;
			cmdName.Set(content->Value().AsDes());
			cmd->Value().Get(cmdPtr);
			
			if( cmdPtr == KMCSDataSourceInterface()) // IDataSource
			    {
			    CMCSDSInterface* mcsDSInterface = CMCSDSInterface::NewL(cmdName);
			    CleanupStack::PushL( mcsDSInterface );
			    aOutParamList.AppendL(TLiwGenericParam(KMCSDataSourceInterface,
			            TLiwVariant(mcsDSInterface)));
			    CleanupStack::Pop( mcsDSInterface );
			    }
			else if(cmdPtr == KMCSMenuContentInterface()) // IMenuContent
			    {
			    CMCSMCInterface* mcsMCInterface = CMCSMCInterface::NewL(cmdName);
                CleanupStack::PushL( mcsMCInterface );
                aOutParamList.AppendL(TLiwGenericParam(KMCSMenuContentInterface,
                        TLiwVariant(mcsMCInterface)));
                CleanupStack::Pop( mcsMCInterface );
			    }
			else
			    {
			    aOutParamList.AppendL(TLiwGenericParam(LIW::EGenericParamError, 
			                        TLiwVariant((TInt32)SErrInvalidServiceArgument)));
			    }
		    
			}
		else
		    {
		    aOutParamList.AppendL(TLiwGenericParam(LIW::EGenericParamError, 
		    		TLiwVariant((TInt32)SErrInvalidServiceArgument)));
		    }
		}
	}

// ---------------------------------------------------------
// Map the interface UIDs to implementation factory functions
// ---------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(0x2001242D, CMCSServiceHandler::NewL),
    IMPLEMENTATION_PROXY_ENTRY(0x2001242E, CMCSServiceHandler::NewL)
    
    };

// ---------------------------------------------------------
// Exported proxy for instantiation method resolution
// ---------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }
