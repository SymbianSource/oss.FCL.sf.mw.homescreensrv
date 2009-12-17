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
#include "ahpluginapitest.h"
#include "ahtestplugin.h"
#include <LiwVariant.h>
_LIT8( KTitle , "title" );
_LIT8( KMessage, "message" );
// ============================ MEMBER FUNCTIONS ===============================


TInt CAHPluginApiTest::Test_CAHPluginExecuteActionL(CStifItemParser& )
    {
    TInt result (KErrGeneral);
    CLiwDefaultMap* map = CLiwDefaultMap::NewLC( );
    map->InsertL( KTitle, _L8("title") );
    map->InsertL( KMessage, _L8("message") );
    CActionHandlerTestPlugin* plugin = CActionHandlerTestPlugin::NewLC();
    result = plugin->ExecuteActionL( map );
    CleanupStack::PopAndDestroy( plugin );
    CleanupStack::PopAndDestroy( map );
    return result;
    }

