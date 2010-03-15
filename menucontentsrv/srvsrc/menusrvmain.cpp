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

// INCLUDE FILES
#include <mw/MemoryManager.h>
#include "menusrv.h"

// ==================== LOCAL FUNCTIONS ====================

/**
* Executable entry point
* @return Error code.
*/
GLDEF_C TInt E32Main()
    {
    RAllocator* iAllocator = MemoryManager::SwitchToFastAllocator();
    
    TInt err = RunMenuServer();
    
    MemoryManager::CloseFastAllocator(iAllocator);
    return err;
    }
