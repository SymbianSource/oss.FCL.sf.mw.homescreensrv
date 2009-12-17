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
// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "cpspluginapitest.h"

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CClientServiceTester::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCPSPluginApiTest::CCPSPluginApiTest( 
    CTestModuleIf& aTestModuleIf ):
        CScriptBase( aTestModuleIf )
    {
    TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksMem );
    }

// -----------------------------------------------------------------------------
// CClientServiceTester::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCPSPluginApiTest::ConstructL()
    {   
    }

// -----------------------------------------------------------------------------
// CClientServiceTester::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCPSPluginApiTest* CCPSPluginApiTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CCPSPluginApiTest* self = new (ELeave) CCPSPluginApiTest( aTestModuleIf );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// Destructor
CCPSPluginApiTest::~CCPSPluginApiTest()
    { 
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point.
// Returns: CScriptBase: New CScriptBase derived object
// -----------------------------------------------------------------------------
//
EXPORT_C CScriptBase* LibEntryL( 
    CTestModuleIf& aTestModuleIf ) // Backpointer to STIF Test Framework
    {
    
    return ( CScriptBase* ) CCPSPluginApiTest::NewL( aTestModuleIf );
        
    }

// -----------------------------------------------------------------------------
// E32Dll is a DLL entry point function.
// Returns: KErrNone
// -----------------------------------------------------------------------------
//
#ifndef EKA2 // Hide Dll entry point to EKA2
GLDEF_C TInt E32Dll(
    TDllReason /*aReason*/) // Reason code
    {
    return(KErrNone);
    
    }
#endif // EKA2    
// -----------------------------------------------------------------------------
// CMCSAPITest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CCPSPluginApiTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {
   
    static TStifFunctionInfo const KFunctions[] =
        {          		

		ENTRY( "Test_CContentHarvesterPluginUpdateL",CCPSPluginApiTest::Test_CContentHarvesterPluginUpdateL ),
		};

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }    

//  End of File
