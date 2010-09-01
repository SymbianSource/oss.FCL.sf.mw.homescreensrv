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
#include <ecom/ecom.h>
#include <Stiftestinterface.h>
#include "menusatinterfacetest.h"
#include "waitactive.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CClientServiceTester::CClientServiceTester
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//


// -----------------------------------------------------------------------------
// CClientServiceTester::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMenuSatInterfaceTest::CMenuSatInterfaceTest( 
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
void CMenuSatInterfaceTest::ConstructL()
    {   
    ///save preconditions
    RProperty::Get( KCRUidMenu, KMenuSatUIIconId, iIconId );
    //save preconditions
    }

// -----------------------------------------------------------------------------
// CClientServiceTester::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMenuSatInterfaceTest* CMenuSatInterfaceTest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CMenuSatInterfaceTest* self = new (ELeave) CMenuSatInterfaceTest( aTestModuleIf );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// Destructor
CMenuSatInterfaceTest::~CMenuSatInterfaceTest()
    { 
    ///load preconditions
    RProperty::Set( KCRUidMenu, KMenuSatUIIconId, iIconId );
    //load preconditions
	WaitL(20000);
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
    
    return ( CScriptBase* ) CMenuSatInterfaceTest::NewL( aTestModuleIf );
        
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


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//


void CMenuSatInterfaceTest::WaitL(TInt aMicroSec)
	{
	CWaitActive* wait = CWaitActive::NewL();
    wait->Wait(aMicroSec);
    delete wait;
	}

