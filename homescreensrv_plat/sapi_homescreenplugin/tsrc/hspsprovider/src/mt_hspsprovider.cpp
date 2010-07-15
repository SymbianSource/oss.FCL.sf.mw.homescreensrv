/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of MT_CHSPSProvider
*
*/


#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/eunitdecorators.h>

#include "mt_hspsprovider.h"
#include "mt_hspsinstallationservice.h"

_LIT8( KInvalidProvider, "Service.Invalid" );
_LIT8( KHSPSConfigurationService, "IConfiguration" );
_LIT8( KHSPSInvalidInterface, "IInvalid" );

// ======== LOCAL FUNCTIONS ====================================================


// ======== MEMBER FUNCTIONS ===================================================

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
MT_CHSPSProvider* MT_CHSPSProvider::NewL()
    {
    MT_CHSPSProvider* self = new( ELeave ) MT_CHSPSProvider();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//    
MT_CHSPSProvider::~MT_CHSPSProvider()
    {
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
MT_CHSPSProvider::MT_CHSPSProvider()
    {
    }

// -----------------------------------------------------------------------------
// Symbian Constructor
// -----------------------------------------------------------------------------
//    
void MT_CHSPSProvider::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// -----------------------------------------------------------------------------
// Initialize test case
// -----------------------------------------------------------------------------
//    
void MT_CHSPSProvider::SetupL()
    {
    MT_CHspsInstallationService* installationService = MT_CHspsInstallationService::NewL();
        CleanupStack::PushL( installationService );

    // Uninstall possible test configurations
    installationService->UninstallConfigurationL( KHSPSMTAppUid, KHSPSActiveConfMinimal );
    installationService->UninstallConfigurationL( KHSPSMTAppUid, KHSPSActiveConfTypical );
    installationService->UninstallConfigurationL( KHSPSMTAppUid, KHSPSActiveConfOperator );

    // Install test configurations
    installationService->InstallConfigurationL( KHSPSInstallMinimalRootConf );
    installationService->InstallConfigurationL( KHSPSInstallTypicalRootConf );
    installationService->InstallConfigurationL( KHSPSInstallOperatorRootConf );

    CleanupStack::PopAndDestroy( installationService );
    ResetResources();
    }

// -----------------------------------------------------------------------------
// Deinitialize test case
// -----------------------------------------------------------------------------
//    
void MT_CHSPSProvider::Teardown(  )
    {
    if ( iInterface )
    	{
        iInterface->Close();
        iInterface = NULL;
        }
    if ( iServiceHandler && iService )
    	{
        RCriteriaArray interestList;
        interestList.AppendL( iService );
        iServiceHandler->DetachL( interestList );   
        interestList.Reset();
    	}
    if ( iServiceHandler )
        {
        delete iServiceHandler;
        iServiceHandler = NULL;
        }
    if ( iService )
        {
        delete iService;
        iService = NULL;
        }    
    }

// -----------------------------------------------------------------------------
// Test case 1: Attach
// -----------------------------------------------------------------------------
//    
void MT_CHSPSProvider::TestCase_AttachL(  )
    {
    /* Attach to HSPS Configuration interface */
    AttachServiceL( KHSPS, KHSPSConfigurationService, KHSPSTestAppUid, ETrue );
    /* Detach from HSPS */
    DetachServiceL();
    }

// -----------------------------------------------------------------------------
// Test case 2: Attach - Invalid provider
// -----------------------------------------------------------------------------
//    
void MT_CHSPSProvider::TestCase_AttachInvalidProviderL(  )
    {
    /* Attach to Invalid provider */
    TRAPD( err, AttachServiceL( KInvalidProvider, KHSPSConfigurationService, KHSPSTestAppUid ) );
    EUNIT_ASSERT_DESC( err == KErrNotFound, "Attach invalid provider");
    }

// -----------------------------------------------------------------------------
// Test case 3: Attach - Invalid interface
// -----------------------------------------------------------------------------
//    
void MT_CHSPSProvider::TestCase_AttachInvalidInterfaceL(  )
    {
    /* Attach to HSPS */
    TRAPD( err, AttachServiceL( KHSPS, KHSPSInvalidInterface, KHSPSTestAppUid ) );
    EUNIT_ASSERT_DESC( err == KErrNotFound, "Attach invalid interface");
    }

// -----------------------------------------------------------------------------
// Test case table
// -----------------------------------------------------------------------------
//    
EUNIT_BEGIN_TEST_TABLE(
    MT_CHSPSProvider,
    "This is a test suite for HSPS provider.",
    "UNIT" )

EUNIT_TEST(
    "Attach",
    "HSPS provider",
    "TestCase_AttachL",
    "FUNCTIONALITY",
    SetupL, TestCase_AttachL, Teardown )

EUNIT_TEST(
    "Attach - Invalid provider",
    "HSPS provider",
    "TestCase_AttachInvalidProviderL",
    "FUNCTIONALITY",
    SetupL, TestCase_AttachInvalidProviderL, Teardown )

EUNIT_TEST(
    "Attach - Invalid interface",
    "HSPS provider",
    "TestCase_AttachInvalidInterfaceL",
    "FUNCTIONALITY",
    SetupL, TestCase_AttachInvalidInterfaceL, Teardown )

EUNIT_END_TEST_TABLE

//  END OF FILE
