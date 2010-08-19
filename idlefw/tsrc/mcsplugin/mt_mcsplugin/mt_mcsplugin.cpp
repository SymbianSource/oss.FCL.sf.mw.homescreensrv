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


//  CLASS HEADER

#include "MT_mcsplugin.h"
//  EXTERNAL INCLUDES

#include <aiutility.h>
#include <w32std.h>
#include <eikenv.h>
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <centralrepository.h>
#include <aicontentobserver.h>
#include <aipluginsettings.h>
#include <mcsplugin.h>
#include <apgtask.h>
#include <aknenv.h>
// #include <aicontentpublisher.h>
#include <ecom/implementationinformation.h>

// #include <aipropertyextension.h>
// #include <aieventhandlerextension.h>
#include <centralrepository.h>
#include <msvids.h>
#include <senduimtmuids.h>

#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/ceunitalloctestcasedecorator.h>

#include <hspluginsettings.h>

using namespace HSPluginSettingsIf;

//  INTERNAL INCLUDES

#include "observer.h"
#include "caouserafter.h"
#include <mcspluginuids.hrh>

_LIT( KEventNameLaunchByIndex,  "LaunchByIndex" );
const TInt KWait_1_secs  =  1 * 1000000;
const TInt KWait_2_secs  =  2 * 1000000;
const TInt KWait_5_secs  =  5 * 1000000;
const TInt KWait_10_secs = 10 * 1000000;
const TInt KWait_15_secs = 15 * 1000000;
const TInt KWait_20_secs = 20 * 1000000;

_LIT( KMcsPluginName, "mcsplugin" );
_LIT8( KNameSpace, "1" );

// CONSTRUCTION

/*EXPORT_C*/ MT_mcsplugin* MT_mcsplugin::NewL()
{
    MT_mcsplugin* self = MT_mcsplugin::NewLC();
    CleanupStack::Pop();

    return self;
}

/*EXPORT_C*/ MT_mcsplugin* MT_mcsplugin::NewLC()
{
    MT_mcsplugin* self = new(ELeave) MT_mcsplugin();
    CleanupStack::PushL(self);

    self->ConstructL();

    return self;
}

// ---------------------------------------------------------------------------
// Destructor (virtual by CBase)
// ---------------------------------------------------------------------------
//
MT_mcsplugin::~MT_mcsplugin()
{
}

// ---------------------------------------------------------------------------
// Default constructor
// ---------------------------------------------------------------------------
//
MT_mcsplugin::MT_mcsplugin()
{
}

// ---------------------------------------------------------------------------
// Second phase construct
// ---------------------------------------------------------------------------
//
void MT_mcsplugin::ConstructL()
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
void MT_mcsplugin::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MT_mcsplugin::SetupL()
{
    __UHEAP_MARK;
    _LIT8( KAppUid, "271012080" );
    CHomescreenSettings::InitializeL( KAppUid );
    
    iContentObserver = Observer::NewL();
    
    TUid uid = { AI_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_MCSPLUGIN };    
    THsPublisherInfo pubInfo( uid, KMcsPluginName, KNameSpace );
    
    EUNIT_ASSERT(iContentObserver);

    iPlugin = CHsContentPublisher::NewL( pubInfo );
    EUNIT_ASSERT( iPlugin );
    
    // Array owned by the plugin
    RAiSettingsItemArray settings;
    //Content Item
    MAiPluginSettings* setting = AiUtility::CreatePluginSettingsL();
    CleanupDeletePushL( setting );
    MAiPluginContentItem& item = setting->AiPluginContentItem();
    item.SetTypeL( _L("image") );
    item.SetNameL( _L("widget/icon"));
    settings.AppendL( setting );
    CleanupStack::Pop( setting );
    
    MAiPluginSettings* setting1 = AiUtility::CreatePluginSettingsL();
    CleanupDeletePushL( setting1 );
    MAiPluginContentItem& item1 = setting1->AiPluginContentItem();
    item1.SetTypeL( _L("image") );
    item1.SetNameL( _L("widget/presenceicon"));
    settings.AppendL( setting1 );
    CleanupStack::Pop( setting1 );
    
    MAiPluginSettings* setting2 = AiUtility::CreatePluginSettingsL();
    CleanupDeletePushL( setting2 );
    MAiPluginContentItem& item2 = setting2->AiPluginContentItem();
    item2.SetTypeL( _L("text") );
    item2.SetNameL( _L("widget/longname"));
    settings.AppendL( setting2 );
    CleanupStack::Pop( setting2 );
    
    MAiPluginSettings* setting3 = AiUtility::CreatePluginSettingsL();
    CleanupDeletePushL( setting3 );
    MAiPluginContentItem& item3 = setting3->AiPluginContentItem();
    item3.SetTypeL( _L("text") );
    item3.SetNameL( _L("widget/shortname"));
    settings.AppendL( setting3 );
    CleanupStack::Pop( setting3 );
    
    iPlugin->ConfigureL( settings );
    iPlugin->SubscribeL( *iContentObserver );
    iPlugin->Start( CHsContentPublisher::ESystemStartup );

    iPlugin->Resume( CHsContentPublisher::EForeground );
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MT_mcsplugin::EmptySetupL()
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void MT_mcsplugin::Teardown()
{
    delete iPlugin;
    iPlugin = NULL;

    delete iContentObserver;
    iContentObserver = NULL;
    
    CHomescreenSettings::UnInitialize();
    __UHEAP_MARKEND;
}


// ---------------------------------------------------------------------------
// Tests Resume, plugin is resumed correctly.
// If plugin is working correctly it should be able to launch app
// ---------------------------------------------------------------------------
//
void MT_mcsplugin::TestResumeL()
{
    // Frees engine, because shortcuts needs to be recreated when theme is changed
    iPlugin->Resume( CHsContentPublisher::EForeground );

    // Free engine one more time...
    iPlugin->Stop( CHsContentPublisher::ESystemShutdown );

    // ... to test last decicions.
    iPlugin->Resume( CHsContentPublisher::EForeground );
}

// ---------------------------------------------------------------------------
// Tests Suspend, plugin is suspended.
// When plugin is suspended it shouldn't launch apps
// ---------------------------------------------------------------------------
//
void MT_mcsplugin::TestSuspendL()
{
    TRAPD(err1, iPlugin->Suspend( CHsContentPublisher::EBackground));

//    EUNIT_ASSERT_NO_LEAVE(LaunchAppFailsL(0x00000001, KScutCalculatorUid));

    iPlugin->Stop( CHsContentPublisher::ESystemShutdown );

    TRAPD(err2, iPlugin->Suspend( CHsContentPublisher::EBackground));
}

// ---------------------------------------------------------------------------
// Tests SubscribeL, adding observer.
// ---------------------------------------------------------------------------
//
void MT_mcsplugin::TestSubscribeL()
{
    EUNIT_ASSERT_NO_LEAVE(iPlugin->SubscribeL(*iContentObserver));
}

// ---------------------------------------------------------------------------
// Tests GetPropertyL
// Return values shouldn't be null
// ---------------------------------------------------------------------------
//
void MT_mcsplugin::TestGetPropertyL()
{
    TAny* anyPtr = iPlugin->GetProperty( CHsContentPublisher::EPublisherContent );
    EUNIT_ASSERT( anyPtr );
    
    MAiContentItemIterator* iter = static_cast<MAiContentItemIterator*>( anyPtr );
    EUNIT_ASSERT( iter );
}

// ---------------------------------------------------------------------------
// Free engine, ensure that plugin won't launch apps.
// ---------------------------------------------------------------------------
//
void MT_mcsplugin::TestFreeEngineL()
{
    iPlugin->Stop( CHsContentPublisher::ESystemShutdown );
    
    iPlugin->Start( CHsContentPublisher::ESystemStartup );

    iPlugin->Resume( CHsContentPublisher::EForeground );    

//    LaunchAppFailsL(0x00000001, KScutCalculatorUid);
}



// ---------------------------------------------------------------------------
// Send invalid parameters to plugin, no crash should occur
// ---------------------------------------------------------------------------
//
void MT_mcsplugin::TestHandleEventErrorHandling()
{
    iPlugin->HandleEvent(_L("invalid"), KNullDesC);
    iPlugin->HandleEvent(KEventNameLaunchByIndex, KNullDesC);
    iPlugin->HandleEvent(KEventNameLaunchByIndex, _L("0x"));
    iPlugin->HandleEvent(KEventNameLaunchByIndex, _L("99999999"));
    iPlugin->HandleEvent(KEventNameLaunchByIndex, _L("-1"));
}


//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(MT_mcsplugin, "MCSPlugin module tests", "MODULE")

EUNIT_TEST("Resume",                    "MCSPlugin",    "Resume",          "FUNCTIONALITY", SetupL,      TestResumeL,                     Teardown)
EUNIT_TEST("Suspend",                   "MCSPlugin",    "Suspend",         "FUNCTIONALITY", SetupL,      TestSuspendL,                    Teardown)
EUNIT_TEST("Subscribe",                 "MCSPlugin",    "SubscribeL",      "FUNCTIONALITY", SetupL,      TestSubscribeL,                  Teardown)
//EUNIT_TEST("Get property",              "MCSPlugin",    "GetPropertyL",    "FUNCTIONALITY", SetupL,      TestGetPropertyL,                Teardown)
//EUNIT_TEST("Free engine",               "MCSPlugin",    "FreeEngineL",     "FUNCTIONALITY", SetupL,      TestFreeEngineL,                 Teardown)
EUNIT_TEST("Test different events",     "MCSPlugin",    "HandleEvent",     "FUNCTIONALITY", SetupL,      TestHandleEventErrorHandling,     Teardown)


EUNIT_END_TEST_TABLE

//  END OF FILE
