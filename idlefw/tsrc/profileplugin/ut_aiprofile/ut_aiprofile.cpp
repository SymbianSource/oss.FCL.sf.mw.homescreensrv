/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


//  CLASS HEADER
#include "UT_aiprofile.h"

//  EXTERNAL INCLUDES
#define protected public
#define private public
#include "caiprofileplugin.h"
#include "caiprofileengine.h"
#undef private
#undef protected

#include <digia/eunit/eunitmacros.h>
#include <GSPrivatePluginProviderIds.h>
#include <aknappui.h>
#include <aknviewappui.h>
#include <eikenv.h>
#include <CentralRepository.h>
#include <apgcli.h>         //RApaLsSession
#include <w32std.h>                 // For RWsSession

//  INTERNAL INCLUDES
#include <aicontentobserver.h>
#include <aiutility.h>
#include <puacodes.hrh>

#include "profileobserver.h"
#include "aiprofileplugincontentmodel.h"
#include "aiprofilepluginuids.hrh"
#include "caiprofileplugin.h"
#include "caiprofileengine.h"
#include "aipluginsettings.h"


// CONSTRUCTION

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_aiprofile* UT_aiprofile::NewL()
{
    UT_aiprofile* self = UT_aiprofile::NewLC();
    CleanupStack::Pop();

    return self;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
UT_aiprofile* UT_aiprofile::NewLC()
{
    UT_aiprofile* self = new(ELeave) UT_aiprofile();
    CleanupStack::PushL(self);

    self->ConstructL();

    return self;
}

// ---------------------------------------------------------------------------
// Destructor (virtual by CBase)
// ---------------------------------------------------------------------------
//
UT_aiprofile::~UT_aiprofile()
{
}

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
UT_aiprofile::UT_aiprofile()
{
}

// ---------------------------------------------------------------------------
// Second phase construct
// ---------------------------------------------------------------------------
//
void UT_aiprofile::ConstructL()
{
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
}

//  METHODS

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_aiprofile::SetupL()
{
    _LIT( KProfilePluginName, "profileplugin" );

    _LIT8( KNameSpace, "1" );
    TUid uid = { AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_PROFILEPLUGIN };    
    THsPublisherInfo pubInfo( uid, KProfilePluginName, KNameSpace );
    
    CHsContentPublisher* pubType = CHsContentPublisher::NewL( pubInfo );
    iProfilePlugin = static_cast<CAiProfilePlugin*>( pubType );
    
    // iProfilePlugin = CHsContentPublisher::NewL( pubInfo );
    EUNIT_ASSERT( iProfilePlugin );

    iContentObserver = ProfileObserver::NewL();

    RAiSettingsItemArray settings;
    iProfilePlugin->ConfigureL( settings );
    
    iProfilePlugin->SubscribeL( *iContentObserver );
    iProfilePlugin->Start( CHsContentPublisher::ESystemStartup );
    
    iProfilePlugin->Resume( CHsContentPublisher::EForeground );

/*
//    iGs = CGSPluginInterface::NewL(TUid::Uid(AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_PROFILEPLUGIN), NULL);
	iProfilePlugin = CAiProfilePlugin::NewL();
	EUNIT_ASSERT(iProfilePlugin);
	iContentObserver = iContentObserver->NewL();
	EUNIT_ASSERT(iContentObserver);
    iProfilePlugin->SubscribeL( *iContentObserver );
*/
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_aiprofile::Teardown()
{
    if( iProfilePlugin )
        {
        iProfilePlugin->Stop( CHsContentPublisher::ESystemShutdown );
        delete iProfilePlugin;
        iProfilePlugin = NULL;        
        }
        
    delete iContentObserver;
}


//-------------------------------------------------------------------------------------------
// Tests for CAiProfilePlugin
//-------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void UT_aiprofile::UT_resumeL()
{
	iProfilePlugin->Suspend( CHsContentPublisher::EBackground );
  iProfilePlugin->Resume( CHsContentPublisher::EForeground );
  
	iProfilePlugin->Suspend(CHsContentPublisher::EBackupRestore );
  iProfilePlugin->Resume( CHsContentPublisher::EForeground );	
  
	iProfilePlugin->Suspend(CHsContentPublisher::EGeneralThemeChange );
  iProfilePlugin->Resume( CHsContentPublisher::EForeground );
  
	iProfilePlugin->Suspend( CHsContentPublisher::EBackground );  
}

void UT_aiprofile::UT_HandleEventL()
{

    iProfilePlugin->HandleEvent(EAiProfileEventSwitchByName, KNullDesC);

    iProfilePlugin->Resume( CHsContentPublisher::EForeground );
    iProfilePlugin->HandleEvent(EAiProfileEventSwitchByName, KNullDesC);

    iProfilePlugin->HandleEvent(EAiProfileEventSwitchByName, _L("Silent"));
    iProfilePlugin->iEngine->UpdateProfileNamesL();
    EUNIT_ASSERT_EQUALS( iProfilePlugin->iEngine->ActiveProfileName(), _L("Silent") );

    //EUNIT_ASSERT_EQUALS(iProfilePlugin->iEngine->IsActiveProfileSilentL(), 1);

    //iProfilePlugin->iEngine->SetActiveProfileNameL( _L("Silent") );
    //EUNIT_ASSERT_EQUALS(iProfilePlugin->iEngine->ActiveProfileName(),_L("Silent"));

    //iProfilePlugin->iEngine->SetSwapProfileNameL(_L("General"));
    //EUNIT_ASSERT_EQUALS(iProfilePlugin->iEngine->SwapProfileName(),_L("General"));

    iProfilePlugin->HandleEvent(EAiProfileEventSwitchByIndex, _L("0"));
    //EUNIT_ASSERT_EQUALS(iProfilePlugin->iEngine->IsActiveProfileSilentL(), 0);
    //EUNIT_ASSERT_EQUALS(_L("Silent"),iProfilePlugin->iEngine->ActiveProfileName());

    iProfilePlugin->HandleEvent(EAiProfileEventSwap, _L("1"));
    //EUNIT_ASSERT_EQUALS(iProfilePlugin->iEngine->IsActiveProfileSilentL(), 1);

    iProfilePlugin->HandleEvent(EAiProfileEventSwap, _L("0"));
    //EUNIT_ASSERT_EQUALS(iProfilePlugin->iEngine->IsActiveProfileSilentL(), 0);

    iProfilePlugin->iEngine->ProfileNameByIndex( 0 );

    iProfilePlugin->iEngine->SwapProfileName();

    /*
    EUNIT_ASSERT_NO_LEAVE(iProfilePlugin->iEngine->
                    HandleActiveProfileEventL(EProfileNewActiveProfile, 0));
    */
//    iProfilePlugin->iEngine->ShowOfflineMessageL();

}

void UT_aiprofile::UT_NotifyContentUpdateL()
{
	iProfilePlugin->Resume( CHsContentPublisher::EForeground );
	
	iProfilePlugin->iEngine->PhoneSettingChanged(ESSSettingsCug,1);
	
	iProfilePlugin->iEngine->PhoneSettingChanged(ESSSettingsAls, 1);
}


void UT_aiprofile::UT_ExtensionL()
{
	/*
    iProfilePlugin->Resume( CHsContentPublisher::EForeground );
    
    TAny* anyPtr = NULL;

	EUNIT_ASSERT_NO_LEAVE(anyPtr = iProfilePlugin->Extension(KExtensionUidEventHandler));

    EUNIT_ASSERT_NO_LEAVE(anyPtr = iProfilePlugin->Extension(KExtensionUidProperty));
    
    TUid wrongUid  = { 0x3 };
    
    EUNIT_ASSERT(!iProfilePlugin->Extension(wrongUid));

    MAiPropertyExtension* propertyExtension = static_cast<MAiPropertyExtension*>(anyPtr);
    EUNIT_ASSERT(propertyExtension);

    EUNIT_ASSERT_NO_LEAVE(anyPtr = propertyExtension->GetPropertyL(EAiPublisherInfo));
    EUNIT_ASSERT(anyPtr != NULL);
	
    EUNIT_ASSERT_NO_LEAVE(anyPtr = propertyExtension->GetPropertyL(EAiPublisherContent));
    EUNIT_ASSERT(anyPtr);

    // Discover resources
    EUNIT_ASSERT_NO_LEAVE(anyPtr = propertyExtension->GetPropertyL(EAiPublisherResources));
    EUNIT_ASSERT(anyPtr);

    // Discover event iterator
    EUNIT_ASSERT_NO_LEAVE(anyPtr = propertyExtension->GetPropertyL(EAiPublisherEvents));
    EUNIT_ASSERT(anyPtr);

	EUNIT_ASSERT_NO_LEAVE(anyPtr = propertyExtension->GetPropertyL(-1));
    EUNIT_ASSERT(!anyPtr);

	//iProfilePlugin->Stop(EAiSystemStartup);
	*/
}


void UT_aiprofile::UT_ConfigureLL()
{
    RAiSettingsItemArray        defaultSettingArray;
    
    // Profiles settings view
    AddSettingL(defaultSettingArray, 0x00000001, _L("localapp:0x100058F8?view=0x02"));   
    EUNIT_ASSERT_NO_LEAVE(iProfilePlugin->ConfigureL(defaultSettingArray));
    
    iProfilePlugin->Resume( CHsContentPublisher::EForeground );
    EUNIT_ASSERT_NO_LEAVE(iProfilePlugin->PublishL());	
    iProfilePlugin->Suspend(CHsContentPublisher::EBackground );	
		
}

void UT_aiprofile::AddSettingL(RAiSettingsItemArray& aArray, const TInt32 aKey, const TDesC& aValue)
{
    MAiPluginSettings* setting = AiUtility::CreatePluginSettingsL();
    if (aArray.Append(setting) == KErrNone)
    {
        MAiPluginSettingsItem& item = setting->AiPluginSettingsItem();
        item.SetKey(aKey);
        item.SetValueL(aValue);
    }
    else
    {
        delete setting;
    }
}


//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(UT_aiprofile, "Profile plugin tests.", "UNIT")

EUNIT_TEST( "Resume",           "CAiProfilePlugin",          "resume",    
			"FUNCTIONALITY",    SetupL,        UT_resumeL,                           Teardown)
/*
EUNIT_TEST( "SetPropertyL",     "CAiProfilePlugin",          "SetPropertyL",    
			"FUNCTIONALITY",    SetupL,        UT_SetPropertyLL,                     Teardown)
*/
EUNIT_TEST( "HandleEventL",     "CAiProfilePlugin",          "HandleEventL",    
			"FUNCTIONALITY",    SetupL,        UT_HandleEventL,                     Teardown)

EUNIT_TEST( "NotifyContentUpdate",     "CAiProfilePlugin",          "NotifyContentUpdate",    
			"FUNCTIONALITY",    SetupL,        UT_NotifyContentUpdateL,                     Teardown)

EUNIT_TEST( "Extension",     "CAiProfilePlugin",          "Extension",    
			"FUNCTIONALITY",    SetupL,        UT_ExtensionL,                     Teardown)

EUNIT_TEST( "ConfigureL",     "CAiProfilePlugin",          "ConfigureL",    
			"FUNCTIONALITY",    SetupL,        UT_ConfigureLL,                     Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE
