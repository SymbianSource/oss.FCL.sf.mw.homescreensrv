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

#include <ecom/ecom.h>
#include <Stiftestinterface.h>
#include "cpspluginapitest.h"
#include "cpstestplugin.h"

// ============================ MEMBER FUNCTIONS ===============================


TInt CCPSPluginApiTest::Test_CContentHarvesterPluginUpdateL(CStifItemParser& )
    {
    TInt result (KErrGeneral);
    CCPSTestPlugin* plugin = CCPSTestPlugin::NewLC();
    if( plugin->iInt == KErrNotFound )
        {
        plugin->UpdateL();
        result = plugin->iInt;
        }
    CleanupStack::PopAndDestroy( plugin );
    return result;
    }

