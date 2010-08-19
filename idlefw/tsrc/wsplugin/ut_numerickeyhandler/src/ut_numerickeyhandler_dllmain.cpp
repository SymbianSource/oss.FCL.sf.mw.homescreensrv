/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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


// System includes
#include <digia/eunit/ceunittestsuite.h>

// User includes
#include "ut_numerickeyhandler.h"

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// Creates test suite instance
// -----------------------------------------------------------------------------
//
EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    return AiWsPlugin::UT_NumericKeyHandler::NewL();
    }

// End of file






