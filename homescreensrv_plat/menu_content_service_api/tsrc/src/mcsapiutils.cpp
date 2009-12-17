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
#include "mcsapitest.h"
#include "waitactive.h"
#include "mcsmenuopwatcher.h"

#include <mcsmenu.h>
#include <mcsmenuitem.h>


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
CMCSAPITest::CMCSAPITest( 
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
void CMCSAPITest::ConstructL()
    {   
    iTestUtils = CTestUtils::NewL();
    iSetupSuffix = 0;
    }

// -----------------------------------------------------------------------------
// CClientServiceTester::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMCSAPITest* CMCSAPITest::NewL( 
    CTestModuleIf& aTestModuleIf )
    {
    CMCSAPITest* self = new (ELeave) CMCSAPITest( aTestModuleIf );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// Destructor
CMCSAPITest::~CMCSAPITest()
    { 
    REComSession::FinalClose();
    delete iTestUtils;
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
    
    return ( CScriptBase* ) CMCSAPITest::NewL( aTestModuleIf );
        
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

TInt CMCSAPITest::SetupMenuL(RMenu& aMenu)
	{
//	aMenu.OpenL(_L("sapitest_menudata"));
//	return KErrNone;
	_LIT(KFileName, "sapitest_menudata");
  	TBool deleteErr = iTestUtils->Delete(  );
  	TBool copyErr =  iTestUtils->Copy( KFileName , ++iSetupSuffix);
  	WaitL( 20000 );
  	TBuf<50> fileName(KFileName);
  	fileName.AppendNum(iSetupSuffix);
  	aMenu.OpenL(fileName);
  	return KErrNone;
	
	
	}

void CMCSAPITest::WaitL(TInt aMicroSec)
	{
	CWaitActive* wait = CWaitActive::NewL();
    wait->Wait(aMicroSec);
    delete wait;
	}

TInt CMCSAPITest::SaveL(CMenuItem *aItem)
	{
    TInt result (KErrGeneral);
    CMCSMenuOpWatcher* watcher =  CMCSMenuOpWatcher::NewL(ETrue);
	CleanupStack::PushL( watcher );
	
    CMenuOperation* oper = aItem->SaveL(watcher->iStatus);
    watcher->Watch( *oper );
	CActiveScheduler::Start();
	WaitL( 1000000 );
	result = watcher->GetStatus();
	CleanupStack::PopAndDestroy( watcher );  
    return result;
	}
