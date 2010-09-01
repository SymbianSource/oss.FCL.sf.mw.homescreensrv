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
* Description:  
*
*/


//  CLASS HEADER
#include "UT_aiprofile.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuite.h>

/**
 * Test suite factory function.
 */
EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    return UT_aiprofile::NewL();
    }

#ifndef __SECURE_API__
/**
 * Standard Symbian DLL entry point function.
 */
GLDEF_C TInt E32Dll(TDllReason)
    {
    return KErrNone;
    }
#endif

//  END OF FILE
