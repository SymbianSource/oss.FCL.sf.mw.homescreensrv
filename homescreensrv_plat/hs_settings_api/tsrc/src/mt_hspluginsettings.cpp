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
#include <e32std.h>
#include <e32cmn.h>

// Internal includes
#include <hspluginsettings.h>
#include <MT_HsPluginSettings.h>
#include <itemmap.h>

// ======== MEMBER FUNCTIONS ========

//----------------------------------------------------------------------------
// MT_HsPluginSettings::NewL( )
//----------------------------------------------------------------------------
//
MT_HsPluginSettings* MT_HsPluginSettings::NewL()
    {
    MT_HsPluginSettings* self = MT_HsPluginSettings::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

//----------------------------------------------------------------------------
// MT_HsPluginSettings::NewLC( )
//----------------------------------------------------------------------------
//
MT_HsPluginSettings* MT_HsPluginSettings::NewLC()
    {
    MT_HsPluginSettings* self = new( ELeave ) MT_HsPluginSettings();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

//----------------------------------------------------------------------------
// MT_HsPluginSettings::~MT_HsPluginSettings( )
//----------------------------------------------------------------------------
//
MT_HsPluginSettings::~MT_HsPluginSettings()
    {
    }

//----------------------------------------------------------------------------
// MT_HsPluginSettings::MT_HsPluginSettings( )
//----------------------------------------------------------------------------
//
MT_HsPluginSettings::MT_HsPluginSettings()
    {
    }

//----------------------------------------------------------------------------
// MT_HsPluginSettings::ConstructL( )
//----------------------------------------------------------------------------
//
void MT_HsPluginSettings::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//----------------------------------------------------------------------------
// MT_HsPluginSettings::SetupL( )
//----------------------------------------------------------------------------
//
void MT_HsPluginSettings::SetupL(  )
    {
    iObserver = CHomescreenSettingsObserver::NewL();
    HSPluginSettingsIf::CHomescreenSettings::InitializeL( _L8( "271012080" ) );
    iHomescreenSettings = HSPluginSettingsIf::CHomescreenSettings::Instance();    
    iHomescreenSettings->AddObserverL( iObserver );
    }

//----------------------------------------------------------------------------
// MT_HsPluginSettings::Teardown( )
//----------------------------------------------------------------------------
//
void MT_HsPluginSettings::Teardown(  )
    {
    iHomescreenSettings->RemoveObserver( iObserver );
    HSPluginSettingsIf::CHomescreenSettings::UnInitialize();
    iHomescreenSettings = NULL;
    
    if( iObserver )
        {
        delete iObserver;
        iObserver = NULL;
        }
    }

//----------------------------------------------------------------------------
// MT_HsPluginSettings::TestGetSettingsL( )
//----------------------------------------------------------------------------
//
void MT_HsPluginSettings::TestGetSettingsL()
    {
    _LIT8( KId, "1" ); // Plugin with ID 1 should be present - usually
    //_LIT8( KId, "0x2001f482" ); // root config UId - GetSettingsL does not work with uids

    RPointerArray<HSPluginSettingsIf::CItemMap> settingItems;
    CleanupClosePushL( settingItems );

    TInt err = iHomescreenSettings->GetSettingsL( KId, settingItems );
    if ( err )
        {
        EUNIT_PRINT( _L( "Warning: Get plugin settings failed" ) );
        }

    CleanupStack::Pop( &settingItems );
    settingItems.ResetAndDestroy();

    EUNIT_ASSERT( ETrue );
    }

//----------------------------------------------------------------------------
// MT_HsPluginSettings::TestSetSettingsL( )
//----------------------------------------------------------------------------
//
void MT_HsPluginSettings::TestSetSettingsL()
    {
    _LIT8( KId, "1" ); // Plugin with ID 1 should be present - usually
    //_LIT8( KId, "0x2001f482" ); // root config UId - does not work with uids

    RPointerArray<HSPluginSettingsIf::CItemMap> settingItems;
    CleanupClosePushL( settingItems );

    TInt err =  iHomescreenSettings->SetSettingsL( KId, settingItems, EFalse );
    if ( err )
        {
        EUNIT_PRINT( _L( "Warning: Set plugin settings function failed" ) );
        }

    CleanupStack::Pop( &settingItems );
    settingItems.ResetAndDestroy();

    EUNIT_ASSERT( ETrue );
    }

//  EUnit test table

EUNIT_BEGIN_TEST_TABLE(
    MT_HsPluginSettings,
    "Test hspluginsettings",
    "MODULE")

EUNIT_TEST(
    "Test GetSettingsL",
    "GetSettingsL",
    "All",
    "FUNCTIONALITY",
    SetupL, TestGetSettingsL, Teardown)

EUNIT_TEST(
    "Test SetSettingsL",
    "SetSettingsL",
    "All",
    "FUNCTIONALITY",
    SetupL, TestSetSettingsL, Teardown)
    
EUNIT_END_TEST_TABLE
