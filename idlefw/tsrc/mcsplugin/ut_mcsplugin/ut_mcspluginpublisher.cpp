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
* Description: unit test for the mcsplugin publisher
*
*/

#include "ut_mcspluginpublisher.h"

#define protected public
#define private public

#include "mcsplugin.h"
#include "mcspluginengine.h"
#include "mcsplugindata.h"

#undef private
#undef protected

#include <e32cmn.h>
#include <digia/eunit/eunitmacros.h>
#include <mcspluginuids.hrh>
#include <mcsdef.h>

#include <hscontentpublisher.h>
#include <hspluginsettings.h>


using namespace HSPluginSettingsIf;

_LIT( KMcsPluginName, "mcsplugin" );
_LIT8( KNameSpace, "3" );

_LIT( KMenuIconFile, "aimcsplugin.mif" );
_LIT( KMenuBookmarkIconId, "16386" );
_LIT( KMenuBookmarkMaskId, "16387" );
_LIT( KMenuMailboxIconId, "16388" );
_LIT( KMenuMailboxMaskId, "16389" );
_LIT( KMenuTypeMailbox, "menu:mailbox" );

_LIT8( KProperNameType, "type" );
_LIT8( KProperNameUid, "uid" );
_LIT8( KProperValueBookmark, "bookmark" );
_LIT8( KProperValueFolder, "folder" );
_LIT8( KProperValueMailbox, "mailbox" );
_LIT8( KMenuAttrUndefUid, "0x99999991" );

_LIT( KTestName, "test:name" );
_LIT( KTestIconAttr, "icon_attributes" );
_LIT( KTestIconValue, "skin(270501603 9361):mif(icon.mif 16384 16385)" );



// ----------------------------------------------------------------------------
// CleanupResetAndDestroy()
// ----------------------------------------------------------------------------
//
template<class T>
static void CleanupResetAndDestroy( TAny* aObj )
    {
    if( aObj )
        {
        static_cast<T*>( aObj )->ResetAndDestroy();
        }
    }

// ----------------------------------------------------------------------------
// CleanupResetAndDestroyPushL
// ----------------------------------------------------------------------------
//
template<class T>
static void CleanupResetAndDestroyPushL(T& aArray)
    {
    CleanupStack::PushL( TCleanupItem( &CleanupResetAndDestroy<T>, &aArray ) );
    }


// CONSTRUCTION

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_MCSPluginPublisher* UT_MCSPluginPublisher::NewL()
{
    UT_MCSPluginPublisher* self = UT_MCSPluginPublisher::NewLC();
    CleanupStack::Pop();

    return self;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_MCSPluginPublisher* UT_MCSPluginPublisher::NewLC()
{
    UT_MCSPluginPublisher* self = new(ELeave) UT_MCSPluginPublisher();
    CleanupStack::PushL(self);

    self->ConstructL();

    return self;
}

// ---------------------------------------------------------------------------
// Destructor (virtual by CBase)
// ---------------------------------------------------------------------------
//
UT_MCSPluginPublisher::~UT_MCSPluginPublisher()
{
}

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
UT_MCSPluginPublisher::UT_MCSPluginPublisher()
{
}

// ---------------------------------------------------------------------------
// Second phase construct
// ---------------------------------------------------------------------------
//
void UT_MCSPluginPublisher::ConstructL()
{
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
}

// ---------------------------------------------------------------------------
// void UT_McsPluginPublisher::HandleSessionEventL
// ---------------------------------------------------------------------------
//
void UT_MCSPluginPublisher::HandleSessionEventL(
          TMsvSessionEvent /*aEvent*/, 
          TAny* /*aArg1*/, 
          TAny* /*aArg2*/,
          TAny* /*aArg3*/ )
{
// No event handling
}


// ---------------------------------------------------------------------------
// void UT_McsPluginPublisher::SetupL()
// ---------------------------------------------------------------------------
//
void UT_MCSPluginPublisher::SetupL()
{
    _LIT8( KAppUid, "271012080" );
    CHomescreenSettings::InitializeL( KAppUid );

    TUid uid = { AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_MCSPLUGIN };    

    THsPublisherInfo pubInfo( uid, KMcsPluginName, KNameSpace );

    iMCSPlugin = static_cast<CMCSPlugin*> 
        (CHsContentPublisher::NewL( pubInfo ));
    
    EUNIT_ASSERT( iMCSPlugin );
    
    RAiSettingsItemArray settings;
    iMCSPlugin->ConfigureL( settings );
    

    // MCS plugin settings
    iPluginSettings = CHomescreenSettings::Instance();
    if( iPluginSettings == NULL )
        {
        User::Leave( KErrUnknown );
        }

    User::LeaveIfError( iPluginSettings->GetSettingsL( KNameSpace, iSettings ));
    if ( iSettings.Count() <= 0 )
        {
        User::Leave( KErrNotFound );
        }
}

// ---------------------------------------------------------------------------
// void UT_McsPluginPublisher::Teardown()
// ---------------------------------------------------------------------------
//
void UT_MCSPluginPublisher::Teardown()
{
    User::LeaveIfError( iPluginSettings->SetSettingsL( KNameSpace,
        iSettings, EFalse ));

    iSettings.ResetAndDestroy();

    CHomescreenSettings::UnInitialize();

    if( iMCSPlugin )
    {
        delete iMCSPlugin;
        iMCSPlugin = NULL;
    }
}

// ---------------------------------------------------------------------------
// Test CMCSData class
// ---------------------------------------------------------------------------
//
void UT_MCSPluginPublisher::TestMCSDataL()
{
    _LIT( KTestName, "test:name" );
    _LIT( KTestValue, "test:value" );

    CMCSData* data = new ( ELeave ) CMCSData();
    CleanupStack::PushL( data );

    data->SetNameL( KTestName );
    data->SetValueL( KTestValue );

    EUNIT_ASSERT_EQUALS( *data->Name(), KTestName );
    EUNIT_ASSERT_EQUALS( *data->Value(), KTestValue );

    CleanupStack::PopAndDestroy( data );
}

// ---------------------------------------------------------------------------
// Test CMCSPluginData::RemoveDataL
// ---------------------------------------------------------------------------
//
void UT_MCSPluginPublisher::TestMCSPluginData_RemoveDataL()
{
    CMCSData& data = iMCSPlugin->iEngine->iPluginData->DataItemL( 0 );

    data.MenuItem().SetType( KMenuTypeMailbox );
    data.MenuItem().SetId( 0 );

    iMCSPlugin->iEngine->iPluginData->RemoveDataL( 0 );

    EUNIT_ASSERT_EQUALS( data.MenuItem().Id(), KErrNotFound );
    EUNIT_ASSERT_EQUALS( data.IsDirty(), ETrue );
}

// ---------------------------------------------------------------------------
// Test CMCSPluginData::GetBkmDataL
// ---------------------------------------------------------------------------
//
void UT_MCSPluginPublisher::TestMCSPluginData_GetBkmDataL()
{
    RPointerArray<CItemMap> settings;
    CleanupResetAndDestroyPushL( settings );

    User::LeaveIfError( iPluginSettings->GetSettingsL( KNameSpace , settings ));

    CItemMap* itemMap = settings[0];
    RPointerArray<HSPluginSettingsIf::CPropertyMap>&
        properties = itemMap->Properties();

    for ( TInt i = 0; i < properties.Count(); i++ )
    {
        if ( properties[i]->Name() == KProperNameType )
            properties[i]->SetValueL( KProperValueBookmark );
        if ( properties[i]->Name() == KProperNameUid )
            properties[i]->SetValueL( KMenuAttrUndefUid );
    }

    User::LeaveIfError( iPluginSettings->SetSettingsL( KNameSpace, settings,
        EFalse ));
    iMCSPlugin->iEngine->iPluginData->UpdateDataL();

    CMCSData& itemData =  iMCSPlugin->iEngine->iPluginData->DataItemL( 0 );
    EUNIT_ASSERT_EQUALS(itemData.MenuItem().Type(), KMenuTypeUrl );

    CleanupStack::PopAndDestroy();
}

// ---------------------------------------------------------------------------
// Test CMCSPluginData::GetFolderData
// ---------------------------------------------------------------------------
//
void UT_MCSPluginPublisher::TestMCSPluginData_GetFolderDataL()
{
    RPointerArray<CItemMap> settings;
    CleanupResetAndDestroyPushL( settings );

    User::LeaveIfError( iPluginSettings->GetSettingsL( KNameSpace , settings ));

    CItemMap* itemMap = settings[0];
    RPointerArray<HSPluginSettingsIf::CPropertyMap>&
        properties = itemMap->Properties();

    for ( TInt i = 0; i < properties.Count(); i++ )
    {    
        if ( properties[i]->Name() == KProperNameType )
            properties[i]->SetValueL( KProperValueFolder );
    }

    User::LeaveIfError( iPluginSettings->SetSettingsL( KNameSpace, settings,
        EFalse ));
    iMCSPlugin->iEngine->iPluginData->UpdateDataL();

    CMCSData& itemData =  iMCSPlugin->iEngine->iPluginData->DataItemL( 0 );
    EUNIT_ASSERT_EQUALS(itemData.MenuItem().Type(), KMenuTypeFolder );

    CleanupStack::PopAndDestroy();
}

// ---------------------------------------------------------------------------
// Test CMCSPluginData::GetMailboxDataL
// ---------------------------------------------------------------------------
//
void UT_MCSPluginPublisher::TestMCSPluginData_GetMailboxDataL()
{
    RPointerArray<CItemMap> settings;
    CleanupResetAndDestroyPushL( settings );

    User::LeaveIfError( iPluginSettings->GetSettingsL( KNameSpace , settings ));

    CItemMap* itemMap = settings[ 0 ];
    RPointerArray<HSPluginSettingsIf::CPropertyMap>&
        properties = itemMap->Properties();

    for ( TInt i = 0; i < properties.Count(); i++ )
    {
        if ( properties[i]->Name() == KProperNameType )
            properties[i]->SetValueL( KProperValueMailbox );
    }

    User::LeaveIfError( iPluginSettings->SetSettingsL( KNameSpace, settings,
        EFalse ));
    iMCSPlugin->iEngine->iPluginData->UpdateDataL();

    CMCSData& itemData =  iMCSPlugin->iEngine->iPluginData->DataItemL(0);
    EUNIT_ASSERT_EQUALS( itemData.MenuItem().Type(), KMenuTypeMailbox );

    CleanupStack::PopAndDestroy();
}

// ---------------------------------------------------------------------------
// CMCSPluginEngine::CreateBkmItemL
// ---------------------------------------------------------------------------
//
void UT_MCSPluginPublisher::TestMCSPluginEngine_CreateBkmItemL()
{
    CMCSData* data = new ( ELeave ) CMCSData();
    CleanupStack::PushL( data );

    data->MenuItem().SetType( KMenuTypeUrl );
    data->MenuItem().SetId( 0 );
    data->SetNameL( KTestName );

    CMenuItem *menuItem = iMCSPlugin->iEngine->FetchMenuItemL( *data );
    CleanupStack::PushL( menuItem );

    TBool attrExists = ETrue;

    EUNIT_ASSERT_EQUALS( menuItem->GetAttributeL( KMenuAttrLongName,
        attrExists ), KTestName );
    EUNIT_ASSERT_EQUALS( menuItem->GetAttributeL( KMenuAttrIconFile,
        attrExists ), KMenuIconFile );
    EUNIT_ASSERT_EQUALS( menuItem->GetAttributeL( KMenuAttrIconId,
        attrExists ), KMenuBookmarkIconId );
    EUNIT_ASSERT_EQUALS( menuItem->GetAttributeL( KMenuAttrMaskId,
        attrExists ), KMenuBookmarkMaskId );

    CleanupStack::PopAndDestroy( menuItem );
    CleanupStack::PopAndDestroy( data );
}

// ---------------------------------------------------------------------------
// CMCSPluginEngine::CreateMailboxItemL
// ---------------------------------------------------------------------------
//
void UT_MCSPluginPublisher::TestMCSPluginEngine_CreateMailboxItemL()
{
    CMCSData* data = new ( ELeave ) CMCSData();
    CleanupStack::PushL( data );

    data->MenuItem().SetType( KMenuTypeMailbox );
    data->MenuItem().SetId( 0 );
    data->SetNameL( KTestName );

    CMenuItem *menuItem = iMCSPlugin->iEngine->FetchMenuItemL( *data );
    CleanupStack::PushL( menuItem );

    TBool attrExists = ETrue;

    EUNIT_ASSERT_EQUALS( menuItem->GetAttributeL( KMenuAttrLongName,
        attrExists ), KTestName );
    EUNIT_ASSERT_EQUALS( menuItem->GetAttributeL( KMenuAttrIconFile,
        attrExists ), KMenuIconFile );
    EUNIT_ASSERT_EQUALS( menuItem->GetAttributeL( KMenuAttrIconId,
        attrExists ), KMenuMailboxIconId );
    EUNIT_ASSERT_EQUALS( menuItem->GetAttributeL( KMenuAttrMaskId,
        attrExists ), KMenuMailboxMaskId );

    CleanupStack::PopAndDestroy( menuItem );
    CleanupStack::PopAndDestroy( data );
}

// ---------------------------------------------------------------------------
// CMCSPluginEngine::ItemIconL
// ---------------------------------------------------------------------------
//
void UT_MCSPluginPublisher::TestMCSPluginEngine_ItemIconL()
{
    CMCSData* data = new ( ELeave ) CMCSData();
    CleanupStack::PushL( data );

    data->MenuItem().SetType( KMenuTypeUrl );
    data->MenuItem().SetId( 0 );
    data->SetNameL( KTestName );

    CMenuItem *menuItem = iMCSPlugin->iEngine->FetchMenuItemL( *data );
    CleanupStack::PushL( menuItem );

    menuItem->SetAttributeL( KTestIconAttr, KTestIconValue );

    CGulIcon* icon = iMCSPlugin->iEngine->ItemIconL( menuItem, KTestIconAttr );
    CleanupStack::PushL( icon );

    EUNIT_ASSERT( icon );

    CleanupStack::PopAndDestroy( icon );
    CleanupStack::PopAndDestroy( menuItem );
    CleanupStack::PopAndDestroy( data );
}


//  TEST TABLE
EUNIT_BEGIN_TEST_TABLE
    (
    UT_MCSPluginPublisher,
    "MCS plugin publisher tests",
    "UNIT"
    )

EUNIT_TEST
    (
    "Test MCS data",
    "CMCSData",
    "VariablesSettings",
    "FUNCTIONALITY",
    SetupL, TestMCSDataL, Teardown )

EUNIT_TEST
    (
    "Test remove data",
    "CMCSPluginData",
    "RemoveDataL",
    "FUNCTIONALITY",
    SetupL, TestMCSPluginData_RemoveDataL, Teardown )
   
EUNIT_TEST
    (
    "Test create bookmark data item",
    "CMCSPluginData",
    "GetBkmDataL",
    "FUNCTIONALITY",
    SetupL, TestMCSPluginData_GetBkmDataL, Teardown )
    
EUNIT_TEST
    (
    "Test create folder data item",
    "CMCSPluginData",
    "GetFolderData",
    "FUNCTIONALITY",
    SetupL, TestMCSPluginData_GetFolderDataL, Teardown )

EUNIT_TEST
    (
    "Test create mailbox data item",
    "CMCSPluginData",
    "GetMailboxDataL",
    "FUNCTIONALITY",
    SetupL, TestMCSPluginData_GetMailboxDataL, Teardown )

EUNIT_TEST
    (
    "Test create bookmark item ",
    "CMCSPluginEngine",
    "CreateBkmItemL",
    "FUNCTIONALITY",
    SetupL, TestMCSPluginEngine_CreateBkmItemL, Teardown )

EUNIT_TEST
    (
    "Test create mailbox item ",
    "CMCSPluginEngine",
    "CreateMailboxItemL",
    "FUNCTIONALITY",
    SetupL, TestMCSPluginEngine_CreateMailboxItemL, Teardown )

EUNIT_TEST
    (
    "Test icon for given menu item ",
    "CMCSPluginEngine",
    "ItemIconL",
    "FUNCTIONALITY",
    SetupL, TestMCSPluginEngine_ItemIconL, Teardown )


EUNIT_END_TEST_TABLE

//  END OF FILE
