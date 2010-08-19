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
* Description: EUnit test for homescreen settings API, plugin map tests
*
*/

// External includes
#include <digia/eunit/eunitmacros.h>

// Internal includes
#include "pluginmap.h"
#include "mt_pluginmap.h"

// ======== MEMBER FUNCTIONS ========

//----------------------------------------------------------------------------
// MT_PluginMap::NewL( )
//----------------------------------------------------------------------------
//
MT_PluginMap* MT_PluginMap::NewL()
    {
    MT_PluginMap* self = MT_PluginMap::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

//----------------------------------------------------------------------------
// MT_PluginMap::NewLC( )
//----------------------------------------------------------------------------
//
MT_PluginMap* MT_PluginMap::NewLC()
    {
    MT_PluginMap* self = new( ELeave ) MT_PluginMap();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

//----------------------------------------------------------------------------
// MT_PluginMap::~MT_PluginMap( )
//----------------------------------------------------------------------------
//
MT_PluginMap::~MT_PluginMap()
    {
    }

//----------------------------------------------------------------------------
// MT_PluginMap::MT_PluginMap( )
//----------------------------------------------------------------------------
//
MT_PluginMap::MT_PluginMap()
    {
    }

//----------------------------------------------------------------------------
// MT_PluginMap::ConstructL( )
//----------------------------------------------------------------------------
//
void MT_PluginMap::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//----------------------------------------------------------------------------
// MT_PluginMap::SetupL( )
//----------------------------------------------------------------------------
//
void MT_PluginMap::SetupL(  )
    {
    iPluginMap = HSPluginSettingsIf::CPluginMap::NewL();
    }

//----------------------------------------------------------------------------
// MT_PluginMap::Teardown( )
//----------------------------------------------------------------------------
//
void MT_PluginMap::Teardown(  )
    {
    if ( iPluginMap )
        {
        delete iPluginMap;
        iPluginMap = NULL;
        }
    }

//----------------------------------------------------------------------------
// MT_PluginMap::PluginIdTestL( )
//----------------------------------------------------------------------------
//
void MT_PluginMap::PluginIdTestL(  )
    {
    // First try to read Id which has not been set yet
    TBufC8<10> id ( iPluginMap->PluginId() );
    EUNIT_ASSERT( ( id.Compare( KNullDesC8 ) == 0 ) );
    
    // Set new Id and try to read it
    _LIT8( KId, "10" );
    iPluginMap->SetPluginIdL( KId );
    TBufC8<10> id2 ( iPluginMap->PluginId() );
    EUNIT_ASSERT( ( id2.Compare( KId ) == 0 ) );
    }

//----------------------------------------------------------------------------
// MT_PluginMap::PluginUidTestL( )
//----------------------------------------------------------------------------
//
void MT_PluginMap::PluginUidTestL(  )
    {
    // First try to read Uid which has not been set yet
    TBufC8<10> uid( iPluginMap->PluginUid() );
    EUNIT_ASSERT( ( uid.Compare( KNullDesC8 ) == 0 ) );
    
    // Set new Uid and try to read it
    _LIT8( KUid, "10" );
    iPluginMap->SetPluginUidL( KUid );
    TBufC8<10> uid2( iPluginMap->PluginUid() );
    EUNIT_ASSERT( ( uid2.Compare( KUid ) == 0 ) );
    }

//  EUnit test table

EUNIT_BEGIN_TEST_TABLE(
    MT_PluginMap,
    "Test for the CPluginMap basic functionality",
    "MODULE")


EUNIT_TEST(
    "Test SetId, GetId",
    "CPluginMap",
    "SetPluginIdL, PluginId",
    "FUNCTIONALITY",
    SetupL, PluginIdTestL, Teardown)

EUNIT_TEST(
    "Test SetUid, GetUid",
    "CPluginMap",
    "SetPluginUidL, PluginUid",
    "FUNCTIONALITY",
    SetupL, PluginUidTestL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE
