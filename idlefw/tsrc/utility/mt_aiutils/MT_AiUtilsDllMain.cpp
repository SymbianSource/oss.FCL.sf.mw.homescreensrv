/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "MT_AiContentItemArrayIterator.h"
#include "MT_AiPSPropertyObserver.h"
#include "MT_ContentPriorityMap.h"
#include "MT_AiStrParser.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuite.h>

/**
 * Test suite factory function.
 */
EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    _LIT(KMainSuiteName, "MT_AiUtils");
    CEUnitTestSuite* mainSuite = CEUnitTestSuite::NewLC(KMainSuiteName);
    
    MEUnitTest* subSuite = MT_AiContentItemArrayIterator::NewLC();
    mainSuite->AddL(subSuite);
    CleanupStack::Pop();  // subSuite

    subSuite = MT_AiPSPropertyObserver::NewLC();
    mainSuite->AddL(subSuite);
    CleanupStack::Pop();  // subSuite
    
    subSuite = MT_ContentPriorityMap::NewLC();
    mainSuite->AddL(subSuite);
    CleanupStack::Pop();    // subSuite
    
    subSuite = MT_AiStrParser::NewLC();
    mainSuite->AddL(subSuite);
    CleanupStack::Pop();    // subSuite
    
    CleanupStack::Pop(mainSuite);
    return mainSuite;
    }



//  END OF FILE






