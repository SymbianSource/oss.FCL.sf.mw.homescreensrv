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
* Description: EUnit test for homescreen settings API, main suite
*
*/

// External includes
#include <digia/eunit/ceunittestsuite.h>

// Internal includes
#include <MT_Map.h>
#include "mt_pluginmap.h"
#include "mt_hspsconfiguration.h"
#include <MT_HsPluginSettings.h>
#include "mt_objectmap.h"
#include "mt_plugininfo.h"


/**
 * Test suite factory function.
 */
EXPORT_C MEUnitTest* CreateTestSuiteL()
    {
    _LIT(KMainSuiteName, "MT_HsSettings");
    CEUnitTestSuite* mainSuite = CEUnitTestSuite::NewLC(KMainSuiteName);

    MEUnitTest* subSuite = MT_Map::NewLC();
    mainSuite->AddL( subSuite );
    CleanupStack::Pop(); // subSuite

    subSuite = MT_HsPluginSettings::NewLC();
    mainSuite->AddL( subSuite );
    CleanupStack::Pop(); // subSuite

    subSuite = MT_PluginMap::NewLC();
    mainSuite->AddL( subSuite );
    CleanupStack::Pop(); // subSuite

    subSuite = MT_HspsConfiguration::NewLC();
    mainSuite->AddL( subSuite );
    CleanupStack::Pop(); // subSuite

    subSuite = MT_ObjectMap::NewLC();
    mainSuite->AddL( subSuite );
    CleanupStack::Pop(); // subSuite

    subSuite = MT_PluginInfo::NewLC();
    mainSuite->AddL( subSuite );
    CleanupStack::Pop(); // subSuite

    CleanupStack::Pop(mainSuite);
    return mainSuite;
    }

//  END OF FILE
