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
* Description:   Entry point to Homescreen content control server 
*                module test dll
*
*/

#include <digia/eunit/ceunittestSuite.h>

#include "mthsccapiclient.h"

// ======== LOCAL FUNCTIONS ====================================================


// ======== MEMBER FUNCTIONS ===================================================


// -----------------------------------------------------------------------------
// Creates test suite instance
// -----------------------------------------------------------------------------
//
EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    return CMTHsCcApiClient::NewL();
    }

//  END OF FILE