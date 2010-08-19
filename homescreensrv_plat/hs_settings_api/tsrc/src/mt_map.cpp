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
#include <digia/eunit/eunitmacros.h>

// Internal includes
#include <itemmap.h>
#include <propertymap.h>
#include <MT_Map.h>

// ======== MEMBER FUNCTIONS ========

//----------------------------------------------------------------------------
// MT_Map::NewL( )
//----------------------------------------------------------------------------
//
MT_Map* MT_Map::NewL()
    {
    MT_Map* self = MT_Map::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

//----------------------------------------------------------------------------
// MT_Map::NewLC( )
//----------------------------------------------------------------------------
//
MT_Map* MT_Map::NewLC()
    {
    MT_Map* self = new( ELeave ) MT_Map();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

//----------------------------------------------------------------------------
// MT_Map::~MT_Map( )
//----------------------------------------------------------------------------
//
MT_Map::~MT_Map()
    {
    }

//----------------------------------------------------------------------------
// MT_Map::MT_Map( )
//----------------------------------------------------------------------------
//
MT_Map::MT_Map()
    {
    }

//----------------------------------------------------------------------------
// MT_Map::ConstructL( )
//----------------------------------------------------------------------------
//
void MT_Map::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//----------------------------------------------------------------------------
// MT_Map::SetupL( )
//----------------------------------------------------------------------------
//
void MT_Map::SetupL(  )
    {
    iItemMap = HSPluginSettingsIf::CItemMap::NewL();
    iPropertyMap = HSPluginSettingsIf::CPropertyMap::NewL();
    }

//----------------------------------------------------------------------------
// MT_Map::Teardown( )
//----------------------------------------------------------------------------
//
void MT_Map::Teardown(  )
    {
    if( iItemMap )
        {
        delete iItemMap;
        iItemMap = NULL;        
        }
    
    if( iPropertyMap )
        {
        delete iPropertyMap;
        iPropertyMap = NULL;
        }
    }

//----------------------------------------------------------------------------
// MT_Map::TeardownPropertyMap( )
//----------------------------------------------------------------------------
//
void MT_Map::TeardownPropertyMap(  )
    {
    if( iItemMap )
        {
        delete iItemMap;
        iItemMap = NULL;        
        }
    }

//----------------------------------------------------------------------------
// MT_Map::TestItemIdL( )
//----------------------------------------------------------------------------
//
void MT_Map::TestItemIdL()
    {
    iItemMap->SetItemIdL( _L8( "12" ) );
    const TDesC8& itemIdVal = iItemMap->ItemId();
    EUNIT_ASSERT( itemIdVal == _L8( "12" ) );
    }

//----------------------------------------------------------------------------
// MT_Map::TestItemNameL( )
//----------------------------------------------------------------------------
//
void MT_Map::TestItemNameL()
    {
    iItemMap->SetItemNameL( _L8( "item name" ) );
    const TDesC8& itemNameVal = iItemMap->ItemName();
    EUNIT_ASSERT( itemNameVal == _L8( "item name" ) );
    }

//----------------------------------------------------------------------------
// MT_Map::TestPropertyNameL( )
//----------------------------------------------------------------------------
//
void MT_Map::TestPropertyNameL()
    {
    iPropertyMap->SetNameL( _L8( "property name" ) );
    const TDesC8& propertyNameVal = iPropertyMap->Name();
    EUNIT_ASSERT( propertyNameVal == _L8( "property name" ) );
    }

//----------------------------------------------------------------------------
// MT_Map::TestPropertyValueL( )
//----------------------------------------------------------------------------
//
void MT_Map::TestPropertyValueL()
    {
    iPropertyMap->SetValueL( _L8( "property value" ) );
    const TDesC8& propertyValueVal = iPropertyMap->Value();
    EUNIT_ASSERT( propertyValueVal == _L8( "property value" ) );
    }

//----------------------------------------------------------------------------
// MT_Map::TestPropertyMapL( )
//----------------------------------------------------------------------------
//
void MT_Map::TestPropertyMapL()
    {
    iPropertyMap->SetNameL( _L8( "property name" ) );
    const TDesC8& propertyNameVal = iPropertyMap->Name();
    EUNIT_ASSERT( propertyNameVal == _L8( "property name" ) );

    iPropertyMap->SetValueL( _L8( "property value" ) );
    const TDesC8& propertyValueVal = iPropertyMap->Value();
    EUNIT_ASSERT( propertyValueVal == _L8( "property value" ) );

    RPointerArray< HSPluginSettingsIf::CPropertyMap >& iProperties = iItemMap->Properties();
    EUNIT_ASSERT( iProperties.Count() == 0 );
    iItemMap->AddPropertyMapL( iPropertyMap );
    EUNIT_ASSERT( iProperties.Count() == 1 );
    }

//  EUnit test table

EUNIT_BEGIN_TEST_TABLE(
    MT_Map,
    "Test MAiPluginTool implementation returned from AiUtility::CreatePluginToolL",
    "MODULE")

EUNIT_TEST(
    "Test SetItemIdL, ItemId",
    "SetItemIdL, ItemId",
    "All",
    "FUNCTIONALITY",
    SetupL, TestItemIdL, Teardown)

EUNIT_TEST(
    "Test SetItemNameL, ItemName",
    "SetItemNameL, ItemName",
    "All",
    "FUNCTIONALITY",
    SetupL, TestItemNameL, Teardown)

EUNIT_TEST(
    "Test SetNameL, Name",
    "SetNameL, Name",
    "All",
    "FUNCTIONALITY",
    SetupL, TestPropertyNameL, Teardown)

EUNIT_TEST(
    "Test SetValueL, Value",
    "SetValueL, Value",
    "All",
    "FUNCTIONALITY",
    SetupL, TestPropertyValueL, Teardown)
    
EUNIT_TEST(
    "Test AddPropertyMapL, Properties",
    "AddPropertyMapL, Properties",
    "All",
    "FUNCTIONALITY",
    SetupL, TestPropertyMapL, TeardownPropertyMap)

EUNIT_END_TEST_TABLE
