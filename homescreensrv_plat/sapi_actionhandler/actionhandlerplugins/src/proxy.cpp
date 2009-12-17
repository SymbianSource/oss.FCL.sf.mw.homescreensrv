/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Action Handler Plugins' Implementation Proxy
 *
*/


#include <e32std.h>
#include <ecom/implementationproxy.h>
#include "ahpapplauncher.h"
#include "ahmessagehandler.h"
#include "ahtelkeyhandler.h"
#include "ahhelplauncher.h"

// Map the interface UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( 0x10282E5F, CAHAppLauncher::NewL ), 
    IMPLEMENTATION_PROXY_ENTRY( 0x10282E61, CAHTelKeyHandler::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( 0x10282E62, CAHMessageHandler::NewL ),
    IMPLEMENTATION_PROXY_ENTRY( 0x2001B285, CAHHelpLauncher::NewL ),
};

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
        TInt& aTableCount )
    {
    aTableCount =
    sizeof( ImplementationTable ) / sizeof( TImplementationProxy );

    return ImplementationTable;
    }

// end of file
