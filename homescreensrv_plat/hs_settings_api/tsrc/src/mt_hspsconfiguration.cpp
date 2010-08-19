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
* Description: EUnit test for homescreen settings API, hsps configuration tests
*
*/

// External includes
#include <digia/eunit/eunitmacros.h>

// Internal includes
#include "plugininfo.h"
#include "pluginmap.h"
#include "itemmap.h"
#include "objectmap.h"
#include "mt_hspsconfiguration.h"

// ======== MEMBER FUNCTIONS ========

//----------------------------------------------------------------------------
// MT_HspsConfiguration::NewL( )
//----------------------------------------------------------------------------
//
MT_HspsConfiguration* MT_HspsConfiguration::NewL()
    {
    MT_HspsConfiguration* self = MT_HspsConfiguration::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

//----------------------------------------------------------------------------
// MT_HspsConfiguration::NewLC( )
//----------------------------------------------------------------------------
//
MT_HspsConfiguration* MT_HspsConfiguration::NewLC()
    {
    MT_HspsConfiguration* self = new( ELeave ) MT_HspsConfiguration();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

//----------------------------------------------------------------------------
// MT_HspsConfiguration::~MT_HspsConfiguration( )
//----------------------------------------------------------------------------
//
MT_HspsConfiguration::~MT_HspsConfiguration()
    {
    }

//----------------------------------------------------------------------------
// MT_HspsConfiguration::MT_HspsConfiguration( )
//----------------------------------------------------------------------------
//
MT_HspsConfiguration::MT_HspsConfiguration()
    {
    }

//----------------------------------------------------------------------------
// MT_HspsConfiguration::ConstructL( )
//----------------------------------------------------------------------------
//
void MT_HspsConfiguration::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//----------------------------------------------------------------------------
// MT_HspsConfiguration::SetupL( )
//----------------------------------------------------------------------------
//
void MT_HspsConfiguration::SetupL(  )
    {
    iConfiguration = HSPluginSettingsIf::CHspsConfiguration::NewL();
    }

//----------------------------------------------------------------------------
// MT_HspsConfiguration::Teardown( )
//----------------------------------------------------------------------------
//
void MT_HspsConfiguration::Teardown(  )
    {
    if ( iConfiguration )
        {
        delete iConfiguration;
        iConfiguration = NULL;
        }
    }

//----------------------------------------------------------------------------
// MT_HspsConfiguration::ConfigIdTestL( )
//----------------------------------------------------------------------------
//
void MT_HspsConfiguration::ConfigIdTestL( )
    {
    // First try to read Id which has not been set yet
    TBufC8<10> id ( iConfiguration->ConfId() );
    EUNIT_ASSERT( id.Compare( KNullDesC8 ) == 0 );

    // Set new Id and try to read it
    _LIT8( KId, "10" );
    EUNIT_ASSERT_NO_LEAVE( iConfiguration->SetConfIdL( KId ) );
    TBufC8<10> id2 ( iConfiguration->ConfId() );
    EUNIT_ASSERT( id2.Compare( KId ) == 0 );
    }

//----------------------------------------------------------------------------
// MT_HspsConfiguration::PluginInfoTestL( )
//----------------------------------------------------------------------------
//
void MT_HspsConfiguration::PluginInfoTestL( )
    {
    // HspsConfiguration creates plugininfo during construction, so 
    // test its existence
    EUNIT_ASSERT( ( & iConfiguration->PluginInfo() ) != NULL );
    HSPluginSettingsIf::CPluginInfo& info = iConfiguration->PluginInfo();

    // Fill info with something, in teardown no memory leak should occur
    _LIT8( KName, "My name is Nobody (Il mio nome e Nessuno)" );
    EUNIT_ASSERT_NO_LEAVE( info.SetNameL( KName ) );
    }

//----------------------------------------------------------------------------
// MT_HspsConfiguration::PluginsTestL( )
//----------------------------------------------------------------------------
//
void MT_HspsConfiguration::PluginsTestL( )
    {
    // First try to read map which has not been set yet
    EUNIT_ASSERT(  iConfiguration->PluginMaps().Count() == 0 );
    RPointerArray<HSPluginSettingsIf::CPluginMap>& maps = iConfiguration->PluginMaps();
    
    // Fill map with something, in teardown no memory leak should occur
    TInt itemCount = 10;
    for ( TInt i = 0; i < itemCount; i++ )
        {
        // Should be deleted automatically in the iConfiguration destructor, 
        // otherwise a memory leak occurs (can be considered as the last test)
        iConfiguration->AddPluginMapL( HSPluginSettingsIf::CPluginMap::NewL() );
        }

    // Test if the items were added
    EUNIT_ASSERT( maps.Count() == itemCount );
    }

//----------------------------------------------------------------------------
// MT_HspsConfiguration::ItemsTestL( )
//----------------------------------------------------------------------------
//
void MT_HspsConfiguration::ItemsTestL( )
    {
    // First try to read map which has not been set yet
    EUNIT_ASSERT( iConfiguration->Settings().Count() == 0 );
    RPointerArray<HSPluginSettingsIf::CItemMap>& maps = iConfiguration->Settings();
    
    // Fill map with something, in teardown no memory leak should occur
    TInt itemCount = 10;
    for ( TInt i = 0; i < itemCount; i++ )
        {
        // Should be deleted automatically in the iConfiguration destructor, 
        // otherwise a memory leak occurs (can be considered as the last test)
        iConfiguration->AddItemMapL( HSPluginSettingsIf::CItemMap::NewL() );
        }

    // Test if the items were added
    EUNIT_ASSERT( maps.Count() == itemCount );
    }

//----------------------------------------------------------------------------
// MT_HspsConfiguration::ObjectsTestL( )
//----------------------------------------------------------------------------
//
void MT_HspsConfiguration::ObjectsTestL( )
    {
    // First try to read map which has not been set yet
    EUNIT_ASSERT( iConfiguration->Resources().Count() == 0 );
    RPointerArray<HSPluginSettingsIf::CObjectMap>& maps = iConfiguration->Resources();
    
    // Fill map with something, in teardown no memory leak should occur
    TInt itemCount = 10;
    for ( TInt i = 0; i < itemCount; i++ )
        {
        // Should be deleted automatically in the iConfiguration destructor, 
        // otherwise a memory leak occurs (can be considered as the last test)
        iConfiguration->AddObjectMapL( HSPluginSettingsIf::CObjectMap::NewL() );
        }

    // Test if the items were added
    EUNIT_ASSERT( maps.Count() == itemCount );
    }

//  EUnit test table
EUNIT_BEGIN_TEST_TABLE(
    MT_HspsConfiguration,
    "Test for the CPluginMap basic functionality",
    "MODULE")


EUNIT_TEST(
    "Test ConfigId Set Get functionality",
    "CHSPSConfiguration",
    "SetConfIdL, ConfId",
    "FUNCTIONALITY",
    SetupL, ConfigIdTestL, Teardown)

EUNIT_TEST(
    "Test PluginInfo",
    "CHSPSConfiguration",
    "PluginInfo",
    "FUNCTIONALITY",
    SetupL, PluginInfoTestL, Teardown)

EUNIT_TEST(
    "Test Plugin maps",
    "CHSPSConfiguration",
    "PluginMaps, AddPluginMapL",
    "FUNCTIONALITY",
    SetupL, PluginsTestL, Teardown)

EUNIT_TEST(
    "Test Item maps",
    "CHSPSConfiguration",
    "Settings, AddItemMapL",
    "FUNCTIONALITY",
    SetupL, ItemsTestL, Teardown)

EUNIT_TEST(
    "Test Object maps",
    "CHSPSConfiguration",
    "Resources, AddObjectMapL",
    "FUNCTIONALITY",
    SetupL, ObjectsTestL, Teardown)

EUNIT_END_TEST_TABLE
