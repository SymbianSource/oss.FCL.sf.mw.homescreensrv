/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Entry point of Active Idle process.
*
*/


#include "ecom/ecom.h"
#include "aifw.h"
#include "debug.h"

GLDEF_C TInt E32Main()
    {
    __TICK("FW: Entry point");
    __UHEAP_MARK;
#ifdef _DEBUG
    // Check heap and handle count balance in debug builds
    TInt initPhc, initThc;
    RThread().HandleCount(initPhc, initThc);
#endif

    // Create cleaup stack
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (!cleanupStack)
        {
        return KErrNoMemory;
        }
        
    // Create and run the Active Idle Framework
    TInt aiFwResult = KErrNone;
    TRAP( aiFwResult, 
        CAiFw* fw = CAiFw::NewLC();
        fw->RunL();
        CleanupStack::PopAndDestroy( fw );
        );

    // Clean up before exit
    delete cleanupStack;
    REComSession::FinalClose();

#ifdef _DEBUG
    // Check heap and handle count balance in debug builds
    TInt exitPhc, exitThc;
    RThread().HandleCount(exitPhc, exitThc);
    if ( exitThc != initThc )
        {
        __PRINT(__DBG_FORMAT("FW: Handle count mismatch %d at exit"), exitThc - initThc);
        }
#endif

    __UHEAP_MARKEND;
    __PRINT_IF_ERROR(aiFwResult, "FW: Framework exit with error %d");
    return aiFwResult;
    }

