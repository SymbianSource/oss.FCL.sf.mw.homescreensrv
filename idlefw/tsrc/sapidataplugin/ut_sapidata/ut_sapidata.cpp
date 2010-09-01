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
#include "ut_sapidata.h"


//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <e32cmn.h>
#include "aiutility.h"
#include "aipluginsettings.h"

//  INTERNAL INCLUDES
#define private public
#include "sapidata.h"
#undef private
#include "sapidataplugin.h"
#include "observer.h"

// Implementation of the MTest interface
void MTest::GetMenuItemsL()
	{
	
	}

// CONSTRUCTION

// ---------------------------------------------------------------------------
// UT_SapiData* UT_SapiData::NewL()
// ---------------------------------------------------------------------------
//
UT_SapiData* UT_SapiData::NewL()
    {
    UT_SapiData* self = UT_SapiData::NewLC();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
// UT_SapiData* UT_SapiData::NewLC()
// ---------------------------------------------------------------------------
//
UT_SapiData* UT_SapiData::NewLC()
    {
    UT_SapiData* self = new(ELeave) UT_SapiData();
    CleanupStack::PushL(self);

    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor (virtual by CBase)
// ---------------------------------------------------------------------------
//
UT_SapiData::~UT_SapiData()
    {
    }

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
UT_SapiData::UT_SapiData()
    {
    }

// ---------------------------------------------------------------------------
// Second phase construct
// ---------------------------------------------------------------------------
//
void UT_SapiData::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// ---------------------------------------------------------------------------
// void UT_SapiDataPlugin::SetupL()
// ---------------------------------------------------------------------------
//
void UT_SapiData::SetupL()
    {
    iContentObserver = Observer::NewL();    
    TUid uid = { SAPIDP_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_DATAPLUGIN };

    iPlugin = CSapiDataPlugin::NewL();

    // Array owned by the plugin
    RAiSettingsItemArray settings;
    settings.ReserveL( 3 );
    
    MAiPluginSettings* configuration( AiUtility::CreatePluginSettingsL() );
    CleanupDeletePushL( configuration );
       
    MAiPluginConfigurationItem& confItem = configuration->AiPluginConfigurationItem();
    confItem.SetOwnerL( KPlugin );
    confItem.SetNameL( KPublisher );
    confItem.SetValueL( KMtSapiDataPlugin );

    settings.AppendL( configuration );
    CleanupStack::Pop( configuration );
                
    // No publisher there, set empty
    iPlugin->ConfigureL( settings );
    iPlugin->SubscribeL( *iContentObserver );
    iPlugin->Start( CHsContentPublisher::ESystemStartup );
    iPlugin->Resume( CHsContentPublisher::EForeground );
    
    CleanupStack::PushL( iPlugin );
    iData = CSapiData::NewL( iPlugin );
    CleanupStack::Pop( iPlugin );
    iData->SetContentIdL( KId );
    iData->ConfigureL( settings );
    }

// ---------------------------------------------------------------------------
// void UT_SapiDataPlugin::Teardown()
// ---------------------------------------------------------------------------
//
void UT_SapiData::Teardown()
    {
    if ( iContentObserver )
        {
        delete iContentObserver;
        iContentObserver = NULL;
        }
    if ( iData )
        {
        delete iData;
        iData = NULL;
        }
    if ( iPlugin )
        {
        delete iPlugin;
        iPlugin = NULL;
        }
    }

// ---------------------------------------------------------------------------
// void UT_SapiData::TestConfigureL()
// ---------------------------------------------------------------------------
//
void UT_SapiData::TestConfigureL()
	{
    RAiSettingsItemArray configurationItemsArr;
    configurationItemsArr.ReserveL( 1 );
    
    MAiPluginSettings* settings( AiUtility::CreatePluginSettingsL() );
    CleanupDeletePushL( settings );
    
    MAiPluginSettingsItem& item( settings->AiPluginSettingsItem() ); 
    item.SetPublisherId( TUid::Uid( 3 ) );

    _LIT( KPlugin, "plugin" );
    _LIT( KPublisher16,  "publisher" );
    MAiPluginConfigurationItem& confItem = settings->AiPluginConfigurationItem();
    confItem.SetOwnerL( KPlugin );
    confItem.SetNameL( KPublisher16 );
    
    TInt32 key( 0 );
    
    item.SetKey( key );
    _LIT( KValue, "value" );
    item.SetValueL( KValue, EFalse );
    
    configurationItemsArr.Append( settings );            
    CleanupStack::Pop( settings );
    
    
    EUNIT_ASSERT_NO_LEAVE( iData->ConfigureL( configurationItemsArr ) );
    
    //delete settings;
    configurationItemsArr.ResetAndDestroy();
	}

// ---------------------------------------------------------------------------
// void UT_SapiData::TestSetContentIdL()
// ---------------------------------------------------------------------------
//
void UT_SapiData::TestSetContentIdL()
	{
	_LIT8( KId, "11" );
	EUNIT_ASSERT_NO_LEAVE( iData->SetContentIdL( KId ) );
	}

// ---------------------------------------------------------------------------
// void UT_SapiData::TestSetStartupReasonL()
// ---------------------------------------------------------------------------
//
void UT_SapiData::TestSetStartupReasonL()
	{
	_LIT8( KStartupReason, "startup_reason" );
	EUNIT_ASSERT_LEAVE( iData->SetStartupReasonL( KStartupReason ) );
	}

// ---------------------------------------------------------------------------
// void UT_SapiData::TestIsPluginActive()
// ---------------------------------------------------------------------------
//
void UT_SapiData::TestIsPluginActive()
	{
	TBool res = iData->IsPluginActive();
	EUNIT_ASSERT( res );
	}

// ---------------------------------------------------------------------------
// void UT_SapiData::TestChangePublisherStatusL()
// ---------------------------------------------------------------------------
//
void UT_SapiData::TestChangePublisherStatusL()
	{
	_LIT8( KStatus, "new_status" );
	
	iData->SetUpdateNeeded( ETrue );
	EUNIT_ASSERT_LEAVE( iData->ChangePublisherStatusL( KStatus ) );

	iData->SetUpdateNeeded( EFalse );
	EUNIT_ASSERT_LEAVE( iData->ChangePublisherStatusL( KStatus ) );
	}

// ---------------------------------------------------------------------------
// void UT_SapiData::TestTriggerActiveL()
// ---------------------------------------------------------------------------
//
void UT_SapiData::TestTriggerActiveL()
	{
	EUNIT_ASSERT_LEAVE( iData->TriggerActiveL() );
	}

// ---------------------------------------------------------------------------
// void UT_SapiData::TestUpdatePublisherStatusL()
// ---------------------------------------------------------------------------
//
void UT_SapiData::TestUpdatePublisherStatusL()
	{
	TBuf<10> publisher;
	publisher.Append( _L("publisher") );
	EUNIT_ASSERT_NO_LEAVE( iData->UpdatePublisherStatusL( publisher) );
	}

// ---------------------------------------------------------------------------
// void UT_SapiData::TestCanUpdate()
// ---------------------------------------------------------------------------
//
void UT_SapiData::TestCanUpdate()
	{
	TBuf<20> KId_EU;
	KId_EU.Append( _L("4"));

	TBuf<20> KAll_EU;
	KAll_EU.Append( _L("all"));

	TBufC<30> KPublisher_EU ( KMtSapiDataPlugin );

	
	EUNIT_ASSERT( iData->CanUpdate( KPublisher_EU, KAll_EU, KId_EU ) );
	}

// ---------------------------------------------------------------------------
// void UT_SapiData::TestGetMenuItemsL()
// ---------------------------------------------------------------------------
//
void UT_SapiData::TestGetMenuItemsL()
	{
	MTest* test;
	
	test = reinterpret_cast<MTest*> ( iData );
	
	EUNIT_ASSERT_NO_LEAVE( test->GetMenuItemsL() );	
	}

// ---------------------------------------------------------------------------
// void UT_SapiData::TestHasMenuItem()
// ---------------------------------------------------------------------------
//
void UT_SapiData::TestHasMenuItem()
	{
	_LIT16( KParam, "hello world" );
	EUNIT_ASSERT( !iData->HasMenuItem(KParam) );
	}

// ---------------------------------------------------------------------------
// void UT_SapiData::TestRemoveL()
// ---------------------------------------------------------------------------
//
void UT_SapiData::TestRemoveL()
	{
	TBuf<10> KAll;
	KAll.Append( _L("all") );	

	Observer* observer = Observer::NewL();
	
	EUNIT_ASSERT_NO_LEAVE( iData->RemoveL(observer, KAll) );	
	
	delete observer;
	}

// ---------------------------------------------------------------------------
// void UT_SapiData::TestRefreshL()
// ---------------------------------------------------------------------------
//
void UT_SapiData::TestRefreshL()
	{   
	TBuf<20> publisher;
	publisher.Append( _L("mt_sapi_plugin") );
	TBuf<10> contentType;
	contentType.Append( _L("all") );
	TBuf<10> contentId;
	contentId.Append( _L("4") );
	TBuf<10> operation;
	operation.Append( _L("operation") );
    CLiwDefaultMap* dataMap;
    
    iData->CanUpdate( publisher, contentType, contentId );
    iData->RefreshL( publisher, contentType, contentId, operation, dataMap );
    
    TInt id = 4;
    iPlugin->Clean(iContentObserver, id );
	}

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE
    (
    UT_SapiData,
    "Sapidata plugin tests",
    "UNIT"
    )

EUNIT_TEST
	(
	"Test configure",
	"CSapiData",
	"ConfigureL",
	"FUNCTIONALITY",
	SetupL, TestConfigureL, Teardown
    )
    
EUNIT_TEST
	(
	"Test refresh",
	"CSapiData",
	"RefreshL",
	"FUNCTIONALITY",
	SetupL, TestRefreshL, Teardown
    )

EUNIT_TEST
    (
    "Test set content ID",
    "CSapiData",
    "SetContentIdL",
    "FUNCTIONALITY",
    SetupL, TestSetContentIdL, Teardown
    )

EUNIT_TEST
    (
    "Test set startup reason",
    "CSapiData",
    "SetStartupReasonL",
    "FUNCTIONALITY",
    SetupL, TestSetStartupReasonL, Teardown
    )
    
EUNIT_TEST
    (
    "Test is plugin active",
    "CSapiData",
    "IsPluginActive",
    "FUNCTIONALITY",
    SetupL, TestIsPluginActive, Teardown
    )
    
EUNIT_TEST
    (
    "Test change publisher status",
    "CSapiData",
    "ChangePublisherStatusL",
    "FUNCTIONALITY",
    SetupL, TestChangePublisherStatusL, Teardown
    )

EUNIT_TEST
    (
    "Test trigger activate",
    "CSapiData",
    "TriggerActivateL",
    "FUNCTIONALITY",
    SetupL, TestTriggerActiveL, Teardown
    )

EUNIT_TEST
    (
    "Test update publisher status",
    "CSapiData",
    "UpdatePublisherStatusL",
    "FUNCTIONALITY",
    SetupL, TestUpdatePublisherStatusL, Teardown
    )
    
EUNIT_TEST
    (
    "Test can update",
    "CSapiData",
    "CanUpdate",
    "FUNCTIONALITY",
    SetupL, TestCanUpdate, Teardown
    )
    
EUNIT_TEST
    (
    "Test get menu items",
    "CSapiData",
    "GetMenuItemsL",
    "FUNCTIONALITY",
    SetupL, TestGetMenuItemsL, Teardown
    )
    
EUNIT_TEST
    (
    "Test has menu item",
    "CSapiData",
    "HasMenuItem",
    "FUNCTIONALITY",
    SetupL, TestHasMenuItem, Teardown
    )
    
EUNIT_TEST
    (
    "Test remove",
    "CSapiData",
    "RemoveL",
    "FUNCTIONALITY",
    SetupL, TestRemoveL, Teardown
    )

EUNIT_END_TEST_TABLE

//  END OF FILE
