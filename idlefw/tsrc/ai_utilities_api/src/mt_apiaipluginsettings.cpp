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

// CLASS UNDER TEST
#include <caouserafter.h>
#include <aiutility.h>
#include <aiplugintool.h>
#include <mt_apiaipluginsettings.h>

#include <aisystemuids.hrh>
#include <aipluginsettings.h>

#include <digia/eunit/eunitmacros.h>

class TStorer;

// CONSTRUCTION
MT_ApiAiPluginSettings* MT_ApiAiPluginSettings::NewL()
    {
    MT_ApiAiPluginSettings* self = MT_ApiAiPluginSettings::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

MT_ApiAiPluginSettings* MT_ApiAiPluginSettings::NewLC()
    {
    MT_ApiAiPluginSettings* self = new( ELeave ) MT_ApiAiPluginSettings();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
MT_ApiAiPluginSettings::~MT_ApiAiPluginSettings()
    {
#if _BullseyeCoverage
    cov_write();
#endif
    }

// Default constructor
MT_ApiAiPluginSettings::MT_ApiAiPluginSettings()
    {
    }

// Second phase construct
void MT_ApiAiPluginSettings::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();

    //CAOUserAfter::AfterLD( 2 * 1000000 );
    }

//  METHODS

void MT_ApiAiPluginSettings::SetupL(  )
    {
    iAiPluginSettings = AiUtility::CreatePluginSettingsL();  
    }

void MT_ApiAiPluginSettings::Teardown(  )
    {
    if( iAiPluginSettings )
    	{
    	delete iAiPluginSettings;
    	iAiPluginSettings = NULL;        
    	}

    }

void MT_ApiAiPluginSettings::TestAiPluginSettingsItemL(  )
    {
    MAiPluginSettingsItem& item = iAiPluginSettings->AiPluginSettingsItem();

    item.SetPublisherId( TUid::Uid( 0x10170FFF ) );
    item.SetValueL( _L("DesValue") );
    item.SetKey( 1 );
    
    EUNIT_ASSERT( item.PublisherId() == TUid::Uid( 0x10170FFF ) );
    EUNIT_ASSERT( item.Value().Compare( _L("DesValue")) == 0 );
    EUNIT_ASSERT( item.Key() == 1 );
    
    TInt type = iAiPluginSettings->AiPluginItemType();
    
	}

void MT_ApiAiPluginSettings::TestAiPluginContentItemL(  )
    {
    MAiPluginContentItem& contentItem = iAiPluginSettings->AiPluginContentItem();

    contentItem.SetNameL(_L("ContentItemName"));
    contentItem.SetTypeL(_L("ContentItemType"));
    
    EUNIT_ASSERT( contentItem.Name().Compare( _L("ContentItemName")) == 0 );
    EUNIT_ASSERT( contentItem.Type().Compare( _L("ContentItemType")) == 0 );
	}

void MT_ApiAiPluginSettings::TestAiPluginConfigurationItemL(  )
    {
    MAiPluginConfigurationItem& configurationItem = iAiPluginSettings->AiPluginConfigurationItem();

    configurationItem.SetOwnerL(_L("ConfigurationItemOwner"));
    configurationItem.SetNameL( _L("ConfigurationItemName") );
    configurationItem.SetValueL( _L("ConfigurationItemValue") );
    
    EUNIT_ASSERT( configurationItem.Owner().Compare( _L("ConfigurationItemOwner")) == 0 );
    EUNIT_ASSERT( configurationItem.Name().Compare( _L("ConfigurationItemName")) == 0 );
    EUNIT_ASSERT( configurationItem.Value().Compare( _L("ConfigurationItemValue")) == 0 );
	}

void MT_ApiAiPluginSettings::TestAiPluginSettingsStorerBasicL( )
    {
    //Get settings item
    MAiPluginSettingsItem& item = iAiPluginSettings->AiPluginSettingsItem();
    
    // test situation the storer is not set, value is not set
    // should do nothing
    EUNIT_ASSERT_NO_LEAVE( item.ReadFromStoreL() );
    EUNIT_ASSERT_NO_LEAVE( item.SaveToStoreL() );
    
    // test very basic storer functionality when key and value are set 
    // correctly
    TStorer storer;
    item.SetStorer( &storer ); 
    TInt32 key = 0x20A;
    TBuf<20> value( _L("VALUE") );    
    item.SetKey( key );
    item.SetValueL( value );
    
    // Store data to the storer and test storer if the result is stored
    EUNIT_ASSERT_NO_LEAVE( item.SaveToStoreL() );
    TInt32 keyRes;
    TBuf<20> valueRes;
    storer.GetKey( keyRes );
    storer.GetValue( valueRes );
    EUNIT_ASSERT( key == keyRes );
    EUNIT_ASSERT( value.Compare( valueRes ) == 0 );
      
    // Read data from the storer and test if the result is read
    TBuf<20> valueBuf;
    valueBuf.SetLength(20);
    // iValue is allocated according to length of input string, erase iValue
    item.SetValueL( valueBuf );
    EUNIT_ASSERT_NO_LEAVE( item.ReadFromStoreL() );
    EUNIT_ASSERT( value.Compare( item.Value() )==0 );
    }

// Storer class implementation
TStorer::TStorer( )
    {
    
    }

TStorer::~TStorer( )
    {
    
    }

void TStorer::SaveL( TInt32 aKey, const TDesC& aValue )
    {
        iKey = aKey;
        if ( aValue.Length()>iValue.MaxLength() )
            {
                User::Leave( KErrArgument );
            }
        iValue.SetLength( 0 );
        iValue.Copy( aValue );
    }

void TStorer::ReadL( TInt32 aKey, TDes& aValue )
    {
        if ( aKey != iKey )
            {
                User::Leave( KErrArgument );
            }
        if ( aValue.MaxLength()<iValue.Length() )
            {
                User::Leave( KErrArgument );
            }
        aValue.SetLength( 0 );
        aValue.Copy( iValue );
    }

void TStorer::SetKey( TInt32 aKey )
    {
        iKey = aKey;
    }

void TStorer::SetValue( const TDesC aValue )
    {
    if ( aValue.Length()<=iValue.MaxLength() )
        {
        iValue.SetLength( 0 );
        iValue.Copy( aValue );        
        }
    }

void TStorer::GetKey( TInt32& aKey )
    {
    aKey = iKey;
    }

void TStorer::GetValue( TDes& aValue )
    {
    if ( aValue.MaxLength()>=iValue.Length() )
        {
        aValue.SetLength( 0 );
        aValue.Copy( iValue ); 
        
        }
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    MT_ApiAiPluginSettings,
    "Test AiPluginSettingsItem implementation returned from AiUtility::CreatePluginToolL",
    "MODULE")

EUNIT_TEST(
    "Test AiPluginSettingsItem",
    "AiPluginSettingsItem",
    "All",
    "FUNCTIONALITY",
    SetupL, TestAiPluginSettingsItemL, Teardown)

EUNIT_TEST(
    "Test AiPluginContentItem",
    "AiPluginContentItem",
    "All",
    "FUNCTIONALITY",
    SetupL, TestAiPluginContentItemL, Teardown)

EUNIT_TEST(
    "Test AiPluginConfigurationItem",
    "AiPluginConfigurationItem",
    "All",
    "FUNCTIONALITY",
    SetupL, TestAiPluginConfigurationItemL, Teardown)

EUNIT_TEST(
    "Test TestAiPluginSettingsStorerBasicL",
    "TestAiPluginSettingsStorerBasicL",
    "All",
    "FUNCTIONALITY",
    SetupL, TestAiPluginSettingsStorerBasicL, Teardown)

    
EUNIT_END_TEST_TABLE

//  END OF FILE
