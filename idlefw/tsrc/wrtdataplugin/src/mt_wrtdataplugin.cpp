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
* Description:  
*
*/

#include "mt_wrtdataplugin.h"
#include "wrtdatapluginobserver.h"
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/eunitdecorators.h>
#include <hscontentpublisher.h>
#include <wrtdatapluginuids.hrh>

_LIT( KWRTPublisherName,"WRTWidget" );
_LIT8( KNameSpace, "5" );


// - Construction -----------------------------------------------------------

MT_wrtdataplugin* MT_wrtdataplugin::NewL()
    {
    MT_wrtdataplugin* self = MT_wrtdataplugin::NewLC();
    CleanupStack::Pop();
    return self;
    }

MT_wrtdataplugin* MT_wrtdataplugin::NewLC()
    {
    MT_wrtdataplugin* self = new( ELeave ) MT_wrtdataplugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

MT_wrtdataplugin::~MT_wrtdataplugin()
    {
       if ( iPlugin )
        {
        delete iPlugin;
        }
    if ( iContentObserver )
        {
        delete iContentObserver;
        }

    }

MT_wrtdataplugin::MT_wrtdataplugin()
    {
    }

void MT_wrtdataplugin::ConstructL()
    {
    CEUnitTestSuiteClass::ConstructL();
    }

// - Test methods -----------------------------------------------------------



void MT_wrtdataplugin::SetupL(  )
    {

    
    TUid uid = { WRTDP_UID_ECOM_IMPLEMENTATION_CONTENTPUBLISHER_DATAPLUGIN };    
    THsPublisherInfo pubInfo( uid, KWRTPublisherName, KNameSpace );
    iPlugin = CHsContentPublisher::NewL( pubInfo );
    iContentObserver = CWrtDataPluginObserver::NewL();
    
    EUNIT_ASSERT( iPlugin );
    EUNIT_ASSERT( iContentObserver );
    __UHEAP_MARK;    

    }
    

void MT_wrtdataplugin::Teardown(  )
    {
    __UHEAP_MARKEND;
    if ( iPlugin )
        {
        delete iPlugin;
        iPlugin = NULL;
        }
    if ( iContentObserver ) 
        {
        delete iContentObserver;
        iPlugin = NULL;
        }
       

    }

    
void MT_wrtdataplugin::TestStartL()
    {
    TInt loop = 10000;

    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Start(CHsContentPublisher::ESystemStartup);
        iPlugin->Stop(CHsContentPublisher::ESystemShutdown);
        }
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Start(CHsContentPublisher::EPageStartup);
        iPlugin->Stop(CHsContentPublisher::EPageShutdown);
        }
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Start(CHsContentPublisher::EPluginStartup);
        iPlugin->Stop(CHsContentPublisher::EPluginShutdown);
        }

    EUNIT_ASSERT( ETrue );
    }


void MT_wrtdataplugin::TestStopL()
    {
    TInt loop = 10000;

    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Start(CHsContentPublisher::ESystemStartup);
        iPlugin->Stop(CHsContentPublisher::ESystemShutdown);
        }
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Start(CHsContentPublisher::EPageStartup);
        iPlugin->Stop(CHsContentPublisher::EPageShutdown);
        }
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Start(CHsContentPublisher::EPluginStartup);
        iPlugin->Stop(CHsContentPublisher::EPluginShutdown);
        }
        
    EUNIT_ASSERT( ETrue );
    }

    
void MT_wrtdataplugin::TestResumeL()
    {
    TInt loop = 10000;
    
    iPlugin->Start(CHsContentPublisher::ESystemStartup);
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Suspend(CHsContentPublisher::EBackground);
        iPlugin->Resume(CHsContentPublisher::EForeground);
        }
    iPlugin->Stop(CHsContentPublisher::ESystemShutdown);

    iPlugin->Start(CHsContentPublisher::EPageStartup);
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Suspend(CHsContentPublisher::EBackground);
        iPlugin->Resume(CHsContentPublisher::EForeground);
        }
    iPlugin->Stop(CHsContentPublisher::EPageShutdown);

    iPlugin->Start(CHsContentPublisher::EPluginStartup);
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Suspend(CHsContentPublisher::EBackground);
        iPlugin->Resume(CHsContentPublisher::EForeground);
        }
    iPlugin->Stop(CHsContentPublisher::EPluginShutdown);
    
    

    // Suspend - Resume (EBackupRestore)
    
    iPlugin->Start(CHsContentPublisher::ESystemStartup);
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Suspend(CHsContentPublisher::EBackupRestore);
        iPlugin->Resume(CHsContentPublisher::EForeground);
        }
    iPlugin->Stop(CHsContentPublisher::ESystemShutdown);
    
    iPlugin->Start(CHsContentPublisher::EPageStartup);
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Suspend(CHsContentPublisher::EBackupRestore);
        iPlugin->Resume(CHsContentPublisher::EForeground);
        }
    iPlugin->Stop(CHsContentPublisher::EPageShutdown);
    
    iPlugin->Start(CHsContentPublisher::EPluginStartup);
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Suspend(CHsContentPublisher::EBackupRestore);
        iPlugin->Resume(CHsContentPublisher::EForeground);
        }
    iPlugin->Stop(CHsContentPublisher::EPluginShutdown);
    

    // Suspend - Resume (EGeneralThemeChange)
    
    iPlugin->Start(CHsContentPublisher::ESystemStartup);
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Suspend(CHsContentPublisher::EGeneralThemeChange);
        iPlugin->Resume(CHsContentPublisher::EForeground);
        }
    iPlugin->Stop(CHsContentPublisher::ESystemShutdown);
    
    iPlugin->Start(CHsContentPublisher::EPageStartup);
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Suspend(CHsContentPublisher::EGeneralThemeChange);
        iPlugin->Resume(CHsContentPublisher::EForeground);
        }
    iPlugin->Stop(CHsContentPublisher::EPageShutdown);
    
    iPlugin->Start(CHsContentPublisher::EPluginStartup);
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Suspend(CHsContentPublisher::EGeneralThemeChange);
        iPlugin->Resume(CHsContentPublisher::EForeground);
        }
    iPlugin->Stop(CHsContentPublisher::EPluginShutdown);
    
    EUNIT_ASSERT( ETrue );
    }


void MT_wrtdataplugin::TestSuspendL()
    {
    TInt loop = 10000;
    
    iPlugin->Start(CHsContentPublisher::ESystemStartup);
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Suspend(CHsContentPublisher::EBackground);
        iPlugin->Resume(CHsContentPublisher::EForeground);
        }
    iPlugin->Stop(CHsContentPublisher::ESystemShutdown);

    iPlugin->Start(CHsContentPublisher::EPageStartup);
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Suspend(CHsContentPublisher::EBackground);
        iPlugin->Resume(CHsContentPublisher::EForeground);
        }
    iPlugin->Stop(CHsContentPublisher::EPageShutdown);

    iPlugin->Start(CHsContentPublisher::EPluginStartup);
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Suspend(CHsContentPublisher::EBackground);
        iPlugin->Resume(CHsContentPublisher::EForeground);
        }
    iPlugin->Stop(CHsContentPublisher::EPluginShutdown);
    
    

    // Suspend - Resume (EBackupRestore)
    
    iPlugin->Start(CHsContentPublisher::ESystemStartup);
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Suspend(CHsContentPublisher::EBackupRestore);
        iPlugin->Resume(CHsContentPublisher::EForeground);
        }
    iPlugin->Stop(CHsContentPublisher::ESystemShutdown);
    
    iPlugin->Start(CHsContentPublisher::EPageStartup);
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Suspend(CHsContentPublisher::EBackupRestore);
        iPlugin->Resume(CHsContentPublisher::EForeground);
        }
    iPlugin->Stop(CHsContentPublisher::EPageShutdown);
    
    iPlugin->Start(CHsContentPublisher::EPluginStartup);
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Suspend(CHsContentPublisher::EBackupRestore);
        iPlugin->Resume(CHsContentPublisher::EForeground);
        }
    iPlugin->Stop(CHsContentPublisher::EPluginShutdown);
    

    // Suspend - Resume (EGeneralThemeChange)
    
    iPlugin->Start(CHsContentPublisher::ESystemStartup);
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Suspend(CHsContentPublisher::EGeneralThemeChange);
        iPlugin->Resume(CHsContentPublisher::EForeground);
        }
    iPlugin->Stop(CHsContentPublisher::ESystemShutdown);
    
    iPlugin->Start(CHsContentPublisher::EPageStartup);
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Suspend(CHsContentPublisher::EGeneralThemeChange);
        iPlugin->Resume(CHsContentPublisher::EForeground);
        }
    iPlugin->Stop(CHsContentPublisher::EPageShutdown);
    
    iPlugin->Start(CHsContentPublisher::EPluginStartup);
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->Suspend(CHsContentPublisher::EGeneralThemeChange);
        iPlugin->Resume(CHsContentPublisher::EForeground);
        }
    iPlugin->Stop(CHsContentPublisher::EPluginShutdown);
    
    EUNIT_ASSERT( ETrue );

    }

void MT_wrtdataplugin::TestSetOnlineL()
    {
    TInt loop = 10000;
    iPlugin->Start(CHsContentPublisher::EPluginStartup);
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->SetOnline();
        iPlugin->SetOffline();
        }
    iPlugin->Stop(CHsContentPublisher::EPluginShutdown);
    EUNIT_ASSERT( ETrue );
    }

void MT_wrtdataplugin::TestSetOfflineL()
    {
    TInt loop = 10000;
    iPlugin->Start(CHsContentPublisher::EPluginStartup);
    for ( TInt i = 0; i< loop ; i++ )
        {
        iPlugin->SetOnline();
        iPlugin->SetOffline();
        }
    iPlugin->Stop(CHsContentPublisher::EPluginShutdown);
    EUNIT_ASSERT( ETrue );
    }


void MT_wrtdataplugin::TestSubscribeL()
    {
    iPlugin->SubscribeL(*iContentObserver);
    // TODO: do some operations and check observer 
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }


void MT_wrtdataplugin::TestConfigureL()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }

void MT_wrtdataplugin::TestSetPropertyL()
    {
    iPlugin->Start(CHsContentPublisher::EPluginStartup);
    iPlugin->SetProperty(CHsContentPublisher::EPublisherContent,NULL);
    TAny* any = iPlugin->GetProperty(CHsContentPublisher::EPublisherContent);
    iPlugin->Stop(CHsContentPublisher::EPluginShutdown);
    EUNIT_ASSERT( any == NULL );
    }


void MT_wrtdataplugin::TestGetPropertyL()
    {
    iPlugin->Start(CHsContentPublisher::EPluginStartup);
    iPlugin->SetProperty(CHsContentPublisher::EPublisherContent,NULL);
    TAny* any = iPlugin->GetProperty(CHsContentPublisher::EPublisherContent);
    iPlugin->Stop(CHsContentPublisher::EPluginShutdown);
    EUNIT_ASSERT( any == NULL );
    }


void MT_wrtdataplugin::TestHandleEventL()
    {
    EUNIT_ASSERT_DESC( EFalse, "Test not implemeted.");
    }



void MT_wrtdataplugin::TestHasMenuItemL()
    {
    iPlugin->Start(CHsContentPublisher::EPluginStartup);
    iPlugin->HasMenuItem(KNullDesC);
    iPlugin->Stop(CHsContentPublisher::EPluginShutdown);
    
    EUNIT_ASSERT( ETrue );
    }
    
void MT_wrtdataplugin::TestExtensionL()
    {
    iPlugin->Start(CHsContentPublisher::EPluginStartup);
    iPlugin->Extension(KNullUid);
    iPlugin->Stop(CHsContentPublisher::EPluginShutdown);
    
    EUNIT_ASSERT( ETrue );

    }
    
void MT_wrtdataplugin::TestPublisherInfoL()
    {
    iPlugin->Start(CHsContentPublisher::EPluginStartup);
    const THsPublisherInfo& publisherInfo = iPlugin->PublisherInfo();
    // TODO: check publisherInfo.Uid(), publisherInfo.Name(), publisherInfo.Namespace()
    iPlugin->Stop(CHsContentPublisher::EPluginShutdown);
    
    EUNIT_ASSERT( ETrue );

    }


// - EUnit test table -------------------------------------------------------

EUNIT_BEGIN_TEST_TABLE(
    MT_wrtdataplugin,
    "Add test suite description here.",
    "MODULE" )

EUNIT_TEST(
    "StartL",
    "WRTDataPlugin",
    "StartL",
    "FUNCTIONALITY",
    SetupL, TestStartL, Teardown)


EUNIT_TEST(
    "StopL",
    "WRTDataPlugin",
    "StopL",
    "FUNCTIONALITY",
    SetupL, TestStopL, Teardown)

EUNIT_TEST(
    "ResumeL",
    "WRTDataPlugin",
    "ResumeL",
    "FUNCTIONALITY",
    SetupL, TestResumeL, Teardown)

EUNIT_TEST(
    "SuspendL",
    "WRTDataPlugin",
    "SuspendL",
    "FUNCTIONALITY",
    SetupL, TestSuspendL, Teardown)

EUNIT_TEST(
    "SetOnlineL",
    "WRTDataPlugin",
    "SetOnlineL",
    "FUNCTIONALITY",
    SetupL, TestSetOnlineL, Teardown)

EUNIT_TEST(
    "SetOfflineL",
    "WRTDataPlugin",
    "SetOfflineL",
    "FUNCTIONALITY",
    SetupL, TestSetOfflineL, Teardown)

EUNIT_TEST(
    "SubscribeL",
    "WRTDataPlugin",
    "SubscribeL",
    "FUNCTIONALITY",
    SetupL, TestSubscribeL, Teardown)

EUNIT_TEST(
    "ConfigureL",
    "WRTDataPlugin",
    "ConfigureL",
    "FUNCTIONALITY",
    SetupL, TestConfigureL, Teardown)

EUNIT_TEST(
    "SetPropertyL",
    "WRTDataPlugin",
    "SetPropertyL",
    "FUNCTIONALITY",
    SetupL, TestSetPropertyL, Teardown)

EUNIT_TEST(
    "GetPropertyL",
    "WRTDataPlugin",
    "GetPropertyL",
    "FUNCTIONALITY",
    SetupL, TestGetPropertyL, Teardown)

EUNIT_TEST(
    "HandleEventL",
    "WRTDataPlugin",
    "HandleEventL",
    "FUNCTIONALITY",
    SetupL, TestHandleEventL, Teardown)

EUNIT_TEST(
    "HasMenuItem",
    "WRTDataPlugin",
    "HasMenuItem",
    "FUNCTIONALITY",
    SetupL, TestHasMenuItemL, Teardown)

EUNIT_TEST(
    "Extension",
    "WRTDataPlugin",
    "Extension",
    "FUNCTIONALITY",
    SetupL, TestExtensionL, Teardown)

EUNIT_TEST(
    "PublisherInfo",
    "WRTDataPlugin",
    "PublisherInfo",
    "FUNCTIONALITY",
    SetupL, TestPublisherInfoL, Teardown)

EUNIT_END_TEST_TABLE
