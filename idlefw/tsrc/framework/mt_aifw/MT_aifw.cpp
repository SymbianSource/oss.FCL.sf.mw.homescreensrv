/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
#include "MT_aifw.h"
#include "aifw.h"
#include "AiFwTestContentPlugin.h"
#include "aicontentobserver.h"
#include "aipluginactivitypstool.h"
#include "caouserafter.h"

//  EXTERNAL INCLUDES
#include <EUnitMacros.h>
#include <CEUnitAllocTestCaseDecorator.h>
#include <bautils.h>
#include <connect/sbdefs.h>
#include <screensaverinternalpskeys.h> // this include needs to be last
#include <e32property.h>

using namespace AiTestUiController;

const TUint32 KAiFwCenrepKey( 0x2001952B );

void CopyFile1()
    {
    RFs fss;
    fss.Connect();
    TInt err = BaflUtils::CopyFile( fss,
                                    _L("z:\\system\\apps\\aifweunit\\10275102_1.cre"),
                                    _L("c:\\private\\10202be9\\persists\\10275102.cre") );
    fss.Close();
    }

void CopyFile2()
    {
    RFs fss;
    fss.Connect();
    TInt err = BaflUtils::CopyFile( fss,
                                    _L("z:\\system\\apps\\aifweunit\\10275102_2.cre"),
                                    _L("c:\\private\\10202be9\\persists\\10275102.cre") );
    fss.Close();
    }
    
void DeleteFile()
    {
    RFs fss;
    fss.Connect();
    TInt err = BaflUtils::DeleteFile( fss,
                                      _L("c:\\private\\10202be9\\persists\\10275102.cre") );
    fss.Close();
    }

void ChangeSSValueL( TInt aValue )
    {
    RProperty::Define( KPSUidScreenSaver,
                                KScreenSaverOn,
                                RProperty::EInt );
                                
    User::LeaveIfError( RProperty::Set( KPSUidScreenSaver,
                    KScreenSaverOn,
                    aValue ) );
    
    }

void ChangeBackupValueL( TInt aValue )
    {
    RProperty::Define( KUidSystemCategory,
                                conn::KUidBackupRestoreKey,
                                RProperty::EInt );
                                
    User::LeaveIfError( RProperty::Set( KUidSystemCategory,
                    conn::KUidBackupRestoreKey,
                    aValue ) );
    
    }

//  INTERNAL INCLUDES

// CONSTRUCTION
MT_aifw* MT_aifw::NewL()
    {
    MT_aifw* self = MT_aifw::NewLC();
    CleanupStack::Pop();

    return self;
    }

MT_aifw* MT_aifw::NewLC()
    {
    MT_aifw* self = new( ELeave ) MT_aifw();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
MT_aifw::~MT_aifw()
    {
    delete iFramework;
    DeleteFile();
    }

// Default constructor
MT_aifw::MT_aifw()
    {
    }

// Second phase construct
void MT_aifw::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    SetupL();
    Teardown();
    }

//  METHODS


void MT_aifw::SetupL(  )
    {
     CopyFile1();
    iFramework = CAiFw::NewLC();
    CleanupStack::Pop( iFramework );
    iControllerPlugin = AiTestUiController::CAiFwTestControllerPlugin::Instance( /**iFramework*/ );
    //iControllerPlugin->SetAppEnvReadyL();
    //iControllerPlugin->SendReadyEventL();
    CAOUserAfter::AfterLD( 1 * 1000000 );

    RAiPublisherInfoArray aPlugins;
    iControllerPlugin->GetPluginsL( aPlugins );
    iControllerPlugin->HandleLoadedPlugins( aPlugins );

    iContentPlugin = CAiFwTestContentPlugin::Instance();
    iContentPlugin->SubscribeL( *iControllerPlugin );
    iContentPlugin->Resume(EAiSystemStartup);
    }


void MT_aifw::Teardown(  )
    {
    delete iFramework;
    iFramework = NULL;
    DeleteFile();
    }
    
void MT_aifw::TestStartupL(  )
    {
    EUNIT_ASSERT( iControllerPlugin != NULL );
    EUNIT_ASSERT( iContentPlugin != NULL );
    }
    
void MT_aifw::TestSubscriptionL(  )
    {
    RPointerArray<MAiContentObserver>& observers = iContentPlugin->Observers();
    TBool match = EFalse;
    
    for( TInt i = 0; i < observers.Count(); ++i )
        {
        MAiContentObserver* obs = observers[i];
        if( obs == static_cast<MAiContentObserver*>( iControllerPlugin ) )
            {
            match = ETrue;
            break;
            }
        }
    
    EUNIT_ASSERT( match );
    }

void MT_aifw::TestStartupResumeL(  )
    {
/*    iControllerPlugin->SetForegroundL( EFalse );
    iControllerPlugin->SetForegroundL( ETrue );
    CAOUserAfter::AfterLD( 1 * 1000000 );
    iControllerPlugin->SetResourceChangedL( 0xfff );*/
    EUNIT_ASSERT( iContentPlugin->WasResumed() );
    }

void MT_aifw::TestUiActivationL(  )
    {
    //EUNIT_ASSERT( iControllerPlugin->UiActivated() );
    }
 
void MT_aifw::TestUiDefinitionLoadL(  )
    {
    //EUNIT_ASSERT( iControllerPlugin->DefinitionLoaded() );
    }

void MT_aifw::TestSettingsPassingL(  )
    {
    EUNIT_ASSERT( iContentPlugin->WasConfigured() );
    }

void MT_aifw::TestEventForwardingL(  )
    {
    iControllerPlugin->SendTestEventL();
    EUNIT_ASSERT( iContentPlugin->EventReceived());
    /*ChangeSSValueL( ETrue );
    ChangeSSValueL( EFalse );
    ChangeBackupValueL( conn::EBURNormal );
    ChangeBackupValueL( conn::KBURPartTypeMask ^ conn::EBURNormal );*/
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    MT_aifw,
    "Module test for XML AI framework.",
    "MODULE" )

EUNIT_ALLOC_TEST(
    "System startup setup test", // Test name
    "Framework", // Class
    "", // Method
    "FUNCTIONALITY",
    SetupL, TestStartupL, Teardown)

EUNIT_ALLOC_TEST(
    "Test plugin subscription",
    "FW/Plugin manager",
    "",
    "FUNCTIONALITY",
    SetupL, TestSubscriptionL, Teardown)

EUNIT_TEST(
    "Startup resume call",
    "FW/Plugin manager",
    "",
    "FUNCTIONALITY",
    SetupL, TestStartupResumeL, Teardown)

EUNIT_ALLOC_TEST(
    "Startup UI activation call",
    "FW/UI Controller manager",
    "",
    "FUNCTIONALITY",
    SetupL, TestUiActivationL, Teardown)

EUNIT_ALLOC_TEST(
    "Startup UI definition load call",
    "FW/UI Controller manager",
    "",
    "FUNCTIONALITY",
    SetupL, TestUiDefinitionLoadL, Teardown)

EUNIT_ALLOC_TEST(
    "Settings passing",
    "FW/UI Controller manager",
    "",
    "FUNCTIONALITY",
    SetupL, TestSettingsPassingL, Teardown)

EUNIT_ALLOC_TEST(
    "Plugin event forwarding",
    "FW/Plugin manager",
    "",
    "FUNCTIONALITY",
    SetupL, TestEventForwardingL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE
