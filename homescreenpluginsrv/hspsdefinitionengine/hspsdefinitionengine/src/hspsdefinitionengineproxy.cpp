/*
* Copyright (c) 2005,2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ECom plugins implementation proxy
*
*/


// INCLUDE FILES
#include    "hspsdefinitionengine.h"
#include    "hspsdefinitionengine.rh"
#include	<e32base.h>
#include 	<ecom/implementationproxy.h>
#include    <ecom/ecom.h>

// Provides a key value pair table, this is used to identify
// the correct construction function for the requested interface.
const TImplementationProxy ImplementationTable[] =
	{
#ifdef __EABI__ 
	IMPLEMENTATION_PROXY_ENTRY( KhspsDefinitionEngineImplUid, ChspsDefinitionEngine::NewL) 
#else
    {KhspsDefinitionEngineImplUid, ChspsDefinitionEngine::NewL}
#endif
	};

// Function used to return an instance of the proxy table.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
	
//  End of File  
