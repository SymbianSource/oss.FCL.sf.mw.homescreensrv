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

//  CLASS HEADER
#include <mt_apiaicontentitemarrayiterator.h>
#include <mt_apiaipspropertyobserver.h>
#include <mt_apicontentprioritymap.h>
#include <mt_apiaistrparser.h>
#include <mt_apiaiplugintool.h>
#include <mt_apiaipluginsettings.h>

//  EXTERNAL INCLUDES
#include <digia/eunit/ceunittestsuite.h>

/**
 * Test suite factory function.
 */
EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    _LIT(KMainSuiteName, "MT_ApiAiUtils");
    CEUnitTestSuite* mainSuite = CEUnitTestSuite::NewLC(KMainSuiteName);
    
    MEUnitTest* subSuite = MT_ApiAiContentItemArrayIterator::NewLC();
    mainSuite->AddL(subSuite);
    CleanupStack::Pop();  // subSuite

    subSuite = MT_ApiAiPSPropertyObserver::NewLC();
    mainSuite->AddL(subSuite);
    CleanupStack::Pop();  // subSuite
    
    subSuite = MT_ApiContentPriorityMap::NewLC();
    mainSuite->AddL(subSuite);
    CleanupStack::Pop();    // subSuite
    
    subSuite = MT_ApiAiStrParser::NewLC();
    mainSuite->AddL(subSuite);
    CleanupStack::Pop();    // subSuite
    
    subSuite = MT_ApiAiplugintool::NewLC();
    mainSuite->AddL(subSuite);
    CleanupStack::Pop();    // subSuite

    subSuite = MT_ApiAiPluginSettings::NewLC();
    mainSuite->AddL(subSuite);
    CleanupStack::Pop();    // subSuite
    
    CleanupStack::Pop(mainSuite);
    return mainSuite;
    }



//  END OF FILE






