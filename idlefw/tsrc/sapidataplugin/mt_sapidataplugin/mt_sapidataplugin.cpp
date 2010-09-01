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

//  CLASS HEADER
#include "mt_sapidataplugin.h"

//  EXTERNAL INCLUDES
#include <digia/eunit/eunitmacros.h>
#include <hspluginsettings.h>
#include <aipluginsettings.h>
#include <aiutility.h>
//using namespace HSPluginSettingsIf;
#include <sapidataplugin.h>
#include <sapidatapluginuids.hrh>

//  INTERNAL INCLUDES
#include "observer.h"
#include "aicpsexecuteparam.h"
#include "sapidataplugin.h"
#include "caicpscommandbuffer_stub.h"
#include <mcspluginuids.hrh>

_LIT( KPubData, "publisherData" );
_LIT( KText, "Hello test!" );
_LIT( KPublisher, "publisher" );
_LIT( KWRTPublisher, "wrt_publisher");
_LIT( KTemplateWidget,"ai3templatedwidget");
_LIT( KContentIdValue, "com.accuweather.widget.touchNG" );
_LIT8( KContentType, "content_type" );
_LIT8( KContentId, "content_id" );
_LIT8( KAction1, "active_1" );
_LIT8( KAction2, "active_2" );
_LIT8( KPublisherId, "publisherId" );

// ---------------------------------------------------------------------------
// CleanupResetAndDestroy()
// ---------------------------------------------------------------------------
//
static void CleanupResetAndDestroy( TAny* aAny )
    {
    static_cast< RAiSettingsItemArray* >( aAny )->ResetAndDestroy();
    }

// CONSTRUCTION

// ---------------------------------------------------------------------------
// MT_SapiDataPlugin* MT_SapiDataPlugin::NewL()
// ---------------------------------------------------------------------------
//
MT_SapiDataPlugin* MT_SapiDataPlugin::NewL()
    {
    MT_SapiDataPlugin* self = MT_SapiDataPlugin::NewLC();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------------------------
// MT_SapiDataPlugin* MT_SapiDataPlugin::NewLC()
// ---------------------------------------------------------------------------
//
MT_SapiDataPlugin* MT_SapiDataPlugin::NewLC()
    {
    MT_SapiDataPlugin* self = new( ELeave ) MT_SapiDataPlugin();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// ---------------------------------------------------------------------------
// MT_SapiDataPlugin::~MT_SapiDataPlugin()
// ---------------------------------------------------------------------------
//
MT_SapiDataPlugin::~MT_SapiDataPlugin()
    {
    }

// ---------------------------------------------------------------------------
// MT_SapiDataPlugin::MT_SapiDataPlugin()
// ---------------------------------------------------------------------------
//
MT_SapiDataPlugin::MT_SapiDataPlugin()
    {
    }

// ---------------------------------------------------------------------------
// void MT_SapiDataPlugin::ConstructL()
// ---------------------------------------------------------------------------
//
void MT_SapiDataPlugin::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

// ---------------------------------------------------------------------------
// void MT_SapiDataPlugin::SetupL()
// ---------------------------------------------------------------------------
//
void MT_SapiDataPlugin::SetupL()
    {
    iContentObserver = Observer::NewL();    

    iPlugin = CSapiDataPlugin::NewL( );

    // Array owned by the plugin
    RAiSettingsItemArray settings;
    CleanupStack::PushL( TCleanupItem( CleanupResetAndDestroy, &settings ) );
    
    MAiPluginSettings* configuration( AiUtility::CreatePluginSettingsL() );
    CleanupDeletePushL( configuration );
       
    MAiPluginConfigurationItem& item( configuration->AiPluginConfigurationItem() );
    
    _LIT( KPlugin, "plugin" );
    _LIT( KPublisher,  "publisher" );
    _LIT( KMtSapiDataPlugin, "mt_sapi_plugin" );
    
    item.SetOwnerL( KPlugin() );
    item.SetNameL( KPublisher() );
    item.SetValueL( KMtSapiDataPlugin() );
    
    settings.AppendL( configuration );
    CleanupStack::Pop( configuration );
    
    iCommandBuffer = CAiCpsCommandBuffer::NewL();
    
    // Ensure interface is available
    iCommandBuffer->GetCPSInterfaceL();
                
    // No publisher there, set empty
    iPlugin->SetProperty( CHsContentPublisher::ECpsCmdBuffer, static_cast< MAiCpsCommandBuffer* >( iCommandBuffer ) ); 
    iPlugin->ConfigureL( settings );
    iPlugin->SubscribeL( *iContentObserver );
    iPlugin->Start( CHsContentPublisher::ESystemStartup );
    iPlugin->Resume( CHsContentPublisher::EForeground );
    
    CleanupStack::PopAndDestroy(); // settings
    }

// ---------------------------------------------------------------------------
// void MT_SapiDataPlugin::Teardown()
// ---------------------------------------------------------------------------
//
void MT_SapiDataPlugin::Teardown()
    {
    if ( iPlugin )
        {
        delete iPlugin;
        iPlugin = NULL;
        }
    if ( iContentObserver )
        {
        delete iContentObserver;
        iContentObserver = NULL;
        }
    }

// ---------------------------------------------------------------------------
// void MT_SapiDataPlugin::TestCommandBuffer()
// ---------------------------------------------------------------------------
//
void MT_SapiDataPlugin::TestCommandBuffer()
    {
    CLiwDefaultMap* filter = CLiwDefaultMap::NewLC();
    filter->InsertL( KPublisherId, TLiwVariant( KWRTPublisher ));
    filter->InsertL( KContentType, TLiwVariant( KTemplateWidget ));
    filter->InsertL( KContentId, TLiwVariant( KContentIdValue ));
    
    iCommandBuffer->Flush();
    
    iCommandBuffer->AddCommand( _L("1"), KPubData, filter, KAction1 );
    iCommandBuffer->AddCommand( _L("1"), KPubData, filter, KAction2 );

    iCommandBuffer->AddCommand( _L("2"), KPubData, filter, KAction1 );
    
    EUNIT_ASSERT_EQUALS( iCommandBuffer->iPlugins.Count(), 2);
    EUNIT_ASSERT_EQUALS( iCommandBuffer->iPlugins[0]->iActions.Count(), 2);    
    
    CleanupStack::PopAndDestroy( filter );    
    }

// ---------------------------------------------------------------------------
// void MT_SapiDataPlugin::TestActivity()
// ---------------------------------------------------------------------------
//
void MT_SapiDataPlugin::TestActivity()
    {
    iPlugin->Stop( CHsContentPublisher::ESystemShutdown );
    EUNIT_ASSERT(iPlugin->IsStopped());
    
    iPlugin->Resume( CHsContentPublisher::EForeground );
    EUNIT_ASSERT(!iPlugin->IsActive());
    }

// ---------------------------------------------------------------------------
// void MT_SapiDataPlugin::TestSuspendL()
// ---------------------------------------------------------------------------
//
void MT_SapiDataPlugin::TestSuspendL()
    {
    EUNIT_ASSERT( iPlugin->IsActive() );
    iPlugin->Suspend( CHsContentPublisher::EBackground );
    EUNIT_ASSERT( !iPlugin->IsActive() );
    iPlugin->Stop( CHsContentPublisher::ESystemShutdown );
    EUNIT_ASSERT( iPlugin->IsStopped() );
    iPlugin->Suspend( CHsContentPublisher::EBackground );
    EUNIT_ASSERT( iPlugin->IsStopped() );
    }

// ---------------------------------------------------------------------------
// void MT_SapiDataPlugin::TestPropertyL()
// ---------------------------------------------------------------------------
//
void MT_SapiDataPlugin::TestPropertyL()
    {
    TAny* anyPtr = NULL;
    
    anyPtr = iPlugin->GetProperty( CHsContentPublisher::EPublisherContent );
    EUNIT_ASSERT( anyPtr );

    iPlugin->Stop( CHsContentPublisher::ESystemShutdown );
    iPlugin->SetProperty( CHsContentPublisher::ECpsCmdBuffer, NULL ); 
    EUNIT_ASSERT_SPECIFIC_LEAVE( iPlugin->StartL( CHsContentPublisher::ESystemStartup ), KErrNotSupported );
    
    anyPtr = iPlugin->GetProperty( CHsContentPublisher::ECpsCmdBuffer );
    EUNIT_ASSERT( anyPtr == NULL );
    }

// ---------------------------------------------------------------------------
// void MT_SapiDataPlugin::TestFreeEngineL()
// ---------------------------------------------------------------------------
//
void MT_SapiDataPlugin::TestPublishL()
    {
    TInt cID1 = 1;
    TInt cID2 = 2;
    TBool founded = EFalse;
    TPtrC valPtr;
    valPtr.Set( KText );

    iPlugin->PublishTextL( iContentObserver, cID1, valPtr );
    
    HBufC* contentText = HBufC::NewLC(valPtr.Size());
    TPtr cDes = contentText->Des();
    cDes.Copy(valPtr);
    
    for (int i = 0; i < iPlugin->iDataArray.Count(); i++)
        if ( iPlugin->iDataArray[i] == *contentText )
            {
            founded = ETrue;
            break;
            }

    EUNIT_ASSERT(founded);

    CleanupStack::Pop( contentText );
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE
    (
    MT_SapiDataPlugin,
    "SapiDataPlugin module tests",
    "MODULE"
    )

EUNIT_TEST
    (
    "Activity",
    "SapiDataPlugin",
    "Stop, Resume",
    "FUNCTIONALITY",
    SetupL, TestActivity, Teardown
    )

EUNIT_TEST
    (
    "Suspend",
    "SapiDataPlugin",
    "Suspend",
    "FUNCTIONALITY",
    SetupL, TestSuspendL, Teardown
    )

EUNIT_TEST
    (
    "Test property",
    "SapiDataPlugin",
    "GetPropertyL, SetPropertyL",
    "FUNCTIONALITY",
    SetupL, TestPropertyL, Teardown
    )

EUNIT_TEST
    (
    "Test publish",
    "SapiDataPlugin",
    "Publish",
    "FUNCTIONALITY",
    SetupL, TestPublishL, Teardown
    )
    
EUNIT_TEST
    (
    "Test command buffer",
    "SapiDataPlugin",
    "CAiCpsCommandBuffer",
    "FUNCTIONALITY",
    SetupL, TestCommandBuffer, Teardown
    )

EUNIT_END_TEST_TABLE

//  END OF FILE
