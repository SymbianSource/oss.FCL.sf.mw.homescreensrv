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
* Description: EUnit test for homescreen settings API, object map tests
*
*/

// External includes
#include <digia/eunit/eunitmacros.h>

// Internal includes
#include "mt_plugininfo.h"
#include "plugininfo.h"


_LIT8(KConfigurationState,"ConfigurationState");
_LIT8(KName, "Name");
_LIT8(KInterface,"Interface");
_LIT8(KType,"Type");
_LIT8(KUid, "Uid");
// ======== MEMBER FUNCTIONS ========

//----------------------------------------------------------------------------
// MT_PluginInfo::NewL( )
//----------------------------------------------------------------------------
//
MT_PluginInfo* MT_PluginInfo::NewL()
    {
    MT_PluginInfo* self = MT_PluginInfo::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

//----------------------------------------------------------------------------
// MT_PluginInfo::NewLC( )
//----------------------------------------------------------------------------
//
MT_PluginInfo* MT_PluginInfo::NewLC()
    {
    MT_PluginInfo* self = new( ELeave ) MT_PluginInfo();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

//----------------------------------------------------------------------------
// MT_PluginInfo::~MT_PluginInfo( )
//----------------------------------------------------------------------------
//
MT_PluginInfo::~MT_PluginInfo()
    {
    }

//----------------------------------------------------------------------------
// MT_PluginInfo::MT_PluginInfo( )
//----------------------------------------------------------------------------
//
MT_PluginInfo::MT_PluginInfo()
    {
    }

//----------------------------------------------------------------------------
// MT_PluginInfo::ConstructL( )
//----------------------------------------------------------------------------
//
void MT_PluginInfo::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//----------------------------------------------------------------------------
// MT_PluginInfo::SetupL( )
//----------------------------------------------------------------------------
//
void MT_PluginInfo::SetupL(  )
    {
    iPluginInfo = HSPluginSettingsIf::CPluginInfo::NewL();
    }

//----------------------------------------------------------------------------
// MT_PluginInfo::Teardown( )
//----------------------------------------------------------------------------
//
void MT_PluginInfo::Teardown(  )
    {
    if ( iPluginInfo )
        {
        delete iPluginInfo;
        iPluginInfo = NULL;
        }
    }

//----------------------------------------------------------------------------
// MT_PluginInfo::TestConfigurationState( )
//----------------------------------------------------------------------------
//
void MT_PluginInfo::TestConfigurationStateL()
    {
    EUNIT_ASSERT( iPluginInfo->ConfigurationState() == KNullDesC8 ); 
    
    iPluginInfo->SetConfigurationStateL( KConfigurationState );
    const TDesC8& config = iPluginInfo->ConfigurationState();
    EUNIT_ASSERT( config == KConfigurationState );
    }

//----------------------------------------------------------------------------
// MT_PluginInfo::TestName( )
//----------------------------------------------------------------------------
//
void MT_PluginInfo::TestNameL()
    {
    EUNIT_ASSERT( iPluginInfo->Name() == KNullDesC8 );
    
    iPluginInfo->SetNameL( KName );
    const TDesC8& name = iPluginInfo->Name();
    EUNIT_ASSERT( name == KName );
    }

//----------------------------------------------------------------------------
// MT_PluginInfo::TestInterface( )
//----------------------------------------------------------------------------
//
void MT_PluginInfo::TestInterfaceL()
    {
    EUNIT_ASSERT( iPluginInfo->Interface() == KNullDesC8 );
    
    iPluginInfo->SetInterfaceL( KInterface );
    const TDesC8& interface = iPluginInfo->Interface();
    EUNIT_ASSERT( interface == KInterface );
    }

//----------------------------------------------------------------------------
// MT_PluginInfo::TestType( )
//----------------------------------------------------------------------------
//
void MT_PluginInfo::TestTypeL()
    {
    EUNIT_ASSERT( iPluginInfo->Type() == KNullDesC8 );
    
    iPluginInfo->SetTypeL( KType );
    const TDesC8& type = iPluginInfo->Type();
    EUNIT_ASSERT( type == KType );
    }

//----------------------------------------------------------------------------
// MT_PluginInfo::TestUid( )
//----------------------------------------------------------------------------
//
void MT_PluginInfo::TestUidL()
    {
    EUNIT_ASSERT( iPluginInfo->Uid() == KNullDesC8 );
    
    iPluginInfo->SetUidL( KUid );
    const TDesC8& uid = iPluginInfo->Uid();
    EUNIT_ASSERT( uid == KUid );
    }

//----------------------------------------------------------------------------


//  EUnit test table

EUNIT_BEGIN_TEST_TABLE(
    MT_PluginInfo,
    "Test CPluginInfo basic functionality",
    "MODULE")


EUNIT_TEST(
    "Test SetConfigurationStateL, TestConfigurationState",
    "CPluginInfo",
    "",
    "FUNCTIONALITY",
    SetupL, TestConfigurationStateL, Teardown)

EUNIT_TEST(
    "Test SetNameL, Name",
    "CPluginInfo",
    "",
    "FUNCTIONALITY",
    SetupL, TestNameL, Teardown)
    
EUNIT_TEST(
    "Test SetInterfaceL, Interface",
    "CPluginInfo",
    "",
    "FUNCTIONALITY",
    SetupL, TestInterfaceL, Teardown)
    
EUNIT_TEST(
    "Test SetTypeL, Type",
    "CPluginInfo",
    "",
    "FUNCTIONALITY",
    SetupL, TestTypeL, Teardown)

EUNIT_TEST(
    "Test SetUidL, Uid",
    "CPluginInfo",
    "",
    "FUNCTIONALITY",
    SetupL, TestUidL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE
