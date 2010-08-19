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
* Description: unit test for the mcsplugin handler
*
*/


//  CLASS HEADER
#include "ut_sapidataplugin.h"


//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <e32cmn.h>
#include <aiutility.h>
#include <aipluginsettings.h>

//  INTERNAL INCLUDES
#include "sapidata.h"
#include "sapidataobserver.h"
#include "sapidataplugin.h"
#include "sapidatapluginconst.h"
#include "observer.h"


_LIT( KTest, "TEST" );
_LIT( KUpdate, "update" );
// CONSTRUCTION

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_SapiDataPlugin* UT_SapiDataPlugin::NewL()
    {
    UT_SapiDataPlugin* self = UT_SapiDataPlugin::NewLC();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_SapiDataPlugin* UT_SapiDataPlugin::NewLC()
    {
    UT_SapiDataPlugin* self = new(ELeave) UT_SapiDataPlugin();
    CleanupStack::PushL(self);

    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor (virtual by CBase)
// ---------------------------------------------------------------------------
//
UT_SapiDataPlugin::~UT_SapiDataPlugin()
    {

    }

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
UT_SapiDataPlugin::UT_SapiDataPlugin()
    {

    }

// ---------------------------------------------------------------------------
// Second phase construct
// ---------------------------------------------------------------------------
//
void UT_SapiDataPlugin::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// ---------------------------------------------------------------------------
// void UT_SapiDataPlugin::Teardown()
// ---------------------------------------------------------------------------
//
void UT_SapiDataPlugin::Teardown()
    {
    if ( iPlugin )
        {
        delete iPlugin;
        iPlugin = NULL;
        }
    if ( iObserver )
        {
        delete iObserver;
        iObserver = NULL;
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void UT_SapiDataPlugin::UT_SapiDataPlugin_Data()
    {
    iData = iPlugin->Data();
    EUNIT_ASSERT( iData );  
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void UT_SapiDataPlugin::SetupL()
    {
    iPlugin = CSapiDataPlugin::NewL();

    RAiSettingsItemArray settings;
    MAiPluginSettings* setting = AiUtility::CreatePluginSettingsL();

    CleanupDeletePushL( setting );
    MAiPluginContentItem& item = setting->AiPluginContentItem();
    item.SetTypeL( _L( "image" ) );
    item.SetNameL( _L( "widget/icon" ) );
    settings.AppendL( setting );
    CleanupStack::Pop( setting );

    MAiPluginSettings* setting1 = AiUtility::CreatePluginSettingsL();
    CleanupDeletePushL( setting1 );
    MAiPluginContentItem& item1 = setting1->AiPluginContentItem();
    item1.SetTypeL( _L( "image" ) );
    item1.SetNameL( _L( "widget/icon" ) );
    settings.AppendL( setting1 );
    CleanupStack::Pop( setting1 );

    MAiPluginSettings* setting2 = AiUtility::CreatePluginSettingsL();
    CleanupDeletePushL( setting2 );
    MAiPluginContentItem& item2 = setting2->AiPluginContentItem();
    item2.SetTypeL( _L( "text" ) );
    item2.SetNameL( _L( "widget/longname" ) );
    settings.AppendL( setting2 );
    CleanupStack::Pop( setting2 );

    MAiPluginSettings* setting3 = AiUtility::CreatePluginSettingsL();
    CleanupDeletePushL( setting3 );
    MAiPluginConfigurationItem& item3 = setting3->AiPluginConfigurationItem();
    item3.SetOwnerL( _L( "plugin" ) );
    item3.SetNameL( _L( "publisher" ));
    item3.SetValueL( _L( "TEST" ) );
    settings.AppendL( setting3 );
    CleanupStack::Pop( setting3 );

    TRAPD( err, iPlugin->ConfigureL( settings ) );
    EUNIT_ASSERT_EQUALS(err, KErrNone);

    iPlugin->Start( CHsContentPublisher::ESystemStartup );
    iPlugin->Resume( CHsContentPublisher::EForeground );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void UT_SapiDataPlugin::UT_SapiDataPlugin_GetTypeL()
    {
    MAiPluginSettings* setting4 = AiUtility::CreatePluginSettingsL();
    CleanupDeletePushL( setting4 );
    MAiPluginContentItem& item4 = setting4->AiPluginContentItem();
    item4.SetTypeL( _L( "text" ) );
    item4.SetNameL( _L( "widget/longname" ) );

    HBufC* objectId = item4.Name().AllocLC();
    objectId->Des().Delete(0, objectId->LocateReverse(KPluginNameSeprator) + 1);

    HBufC* type = iPlugin->GetTypeL( *objectId ).AllocLC();
    EUNIT_ASSERT_EQUALS( type != KNullDesC(), ETrue )

    CleanupStack::PopAndDestroy( type );
    CleanupStack::PopAndDestroy( objectId );
    CleanupStack::PopAndDestroy( setting4 );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void UT_SapiDataPlugin::UT_SapiDataPlugin_GetIdL()
    {
    MAiPluginSettings* setting4 = AiUtility::CreatePluginSettingsL();
    CleanupDeletePushL( setting4 );
    MAiPluginContentItem& item4 = setting4->AiPluginContentItem();
    item4.SetTypeL( _L( "text" ) );
    item4.SetNameL( _L( "widget/longname" ) );

    HBufC* objectId = item4.Name().AllocLC();
    objectId->Des().Delete(0, objectId->LocateReverse(KPluginNameSeprator) + 1);

    TInt id = iPlugin->GetIdL( *objectId );
    EUNIT_ASSERT_EQUALS( id != KErrNotFound, ETrue );

    CleanupStack::PopAndDestroy( objectId );
    CleanupStack::PopAndDestroy( setting4 );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void UT_SapiDataPlugin::UT_SapiDataPlugin_Resume()
    {
    EUNIT_ASSERT( iPlugin->IsActive() );    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void UT_SapiDataPlugin::UT_SapiDataPlugin_NetworkStatus()
    {
    iPlugin->SetOnline();    
    CSapiDataPlugin::TPluginNetworkStatus networkStatus = 
        iPlugin->NetworkStatus();
    EUNIT_ASSERT_EQUALS( networkStatus == CSapiDataPlugin::EOnline, ETrue );

    iPlugin->SetOffline();
    networkStatus = iPlugin->NetworkStatus();
    EUNIT_ASSERT_EQUALS( networkStatus == CSapiDataPlugin::EOffline, ETrue );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void UT_SapiDataPlugin::UT_SapiDataPlugin_GetProperty()
    {
    TAny* anyPtr = iPlugin->GetProperty( CSapiDataPlugin::EPublisherContent );
    MAiContentItemIterator* iter = 
        static_cast<MAiContentItemIterator*>( anyPtr);
    EUNIT_ASSERT( iter );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void UT_SapiDataPlugin::UT_SapiDataPlugin_HasMenuItem()
    {
    TBool res = iPlugin->HasMenuItem( KTest );
    EUNIT_ASSERT( res == EFalse );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
void UT_SapiDataPlugin::UT_SapiDataPlugin_Clean()
    {
    iObserver = Observer::NewL();
    TInt id = 1;

    iPlugin->SubscribeL( *iObserver );
    EUNIT_ASSERT_NO_LEAVE( iPlugin->Clean( iObserver, id ) );
    }

//  TEST TABLE


EUNIT_BEGIN_TEST_TABLE
    (
    UT_SapiDataPlugin,
    "Sapidata plugin tests",
    "UNIT"
    )

EUNIT_TEST
    (
    "Data",
    "CSapiDataPlugin",
    "Data",
    "FUNCTIONALITY",
    SetupL, UT_SapiDataPlugin_Data, Teardown
    )

EUNIT_TEST
    (
    "Resume",
    "CSapiDataPlugin",
    "Resume",
    "FUNCTIONALITY",
    SetupL, UT_SapiDataPlugin_Resume, Teardown
    )

EUNIT_TEST
    (
    "GetTypeL",
    "CSapiDataPlugin",
    "GetTypeL",
    "FUNCTIONALITY",
    SetupL, UT_SapiDataPlugin_GetTypeL, Teardown
    )

EUNIT_TEST
    (
    "GetIdL",
    "CSapiDataPlugin",
    "GetIdL",
    "FUNCTIONALITY",
    SetupL, UT_SapiDataPlugin_GetIdL, Teardown
    )

EUNIT_TEST
    (
    "NetworkStatus",
    "CSapiDataPlugin",
    "NetworkStatus",
    "FUNCTIONALITY",
    SetupL, UT_SapiDataPlugin_NetworkStatus, Teardown
    )

EUNIT_TEST
    (
    "GetProperty",
    "CSapiDataPlugin",
    "GetProperty",
    "FUNCTIONALITY",
    SetupL, UT_SapiDataPlugin_GetProperty, Teardown
    )

EUNIT_TEST
    (
    "HasMenuItem",
    "CSapiDataPlugin",
    "HasMenuItem",
    "FUNCTIONALITY",
    SetupL, UT_SapiDataPlugin_HasMenuItem, Teardown
    )
    
EUNIT_TEST
    (
    "Clean",
    "CSapiDataPlugin",
    "Clean",
    "FUNCTIONALITY",
    SetupL, UT_SapiDataPlugin_Clean, Teardown
    )   
    
EUNIT_END_TEST_TABLE

//  END OF FILE
