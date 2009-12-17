/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <ecom/implementationproxy.h>
#include "menuapphandler.h"
#include "menuurlhandler.h"
#include "menulinkhandler.h"

// ============================ LOCAL FUNCTIONS ===============================

LOCAL_C CMenuHandlerPlugin* AppHandlerNewL( RMenu *aMenu )
    {
    return CMenuAppHandler::NewL( *aMenu );
    }

LOCAL_C CMenuHandlerPlugin* UrlHandlerNewL( RMenu *aMenu )
    {
    return CMenuUrlHandler::NewL( *aMenu );
    }

LOCAL_C CMenuHandlerPlugin* LinkHandlerNewL( RMenu *aMenu )
    {
    return CMenuLinkHandler::NewL( *aMenu );
    }

// ============================ CONSTANTS ===============================

const TImplementationProxy ImplementationTable[] = 
	{
    IMPLEMENTATION_PROXY_ENTRY( 0x200113E0, AppHandlerNewL ),
    IMPLEMENTATION_PROXY_ENTRY( 0x200113E1, UrlHandlerNewL ),
    IMPLEMENTATION_PROXY_ENTRY( 0x20019595, LinkHandlerNewL ),
	};

// ============================ GLOBAL FUNCTIONS ===============================

EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

