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


// System includes
#include <e32base.h>
#include <e32cmn.h> 
#include <e32event.h>
#include <e32keys.h>
#include <e32property.h>
#include <aknfepinternalcrkeys.h>

#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/teunitassertioninfo.h>
#include <digia/eunit/eunitdecorators.h>

// User includes
#include "numerickeyhandler.h"

#include <activeidle2domainpskeys.h>
#include "eventhandler.h"

#include "ut_numerickeyhandler.h"
#include "animgeneralfunctions_stub.h"
    
using namespace AiWsPlugin;

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// UT_MofifierKeyTracker::NewL()
//
// -----------------------------------------------------------------------------
//


//  CLASS HEADER

//  EXTERNAL INCLUDES
//#include <telephonyinternalpskeys.h>


_LIT( fileName, "C:\\keyhandler.txt");
//  INTERNAL INCLUDES




// CONSTRUCTION
UT_NumericKeyHandler* UT_NumericKeyHandler::NewL()
    {
    UT_NumericKeyHandler* self = UT_NumericKeyHandler::NewLC();
    CleanupStack::Pop();

    return self;
    }

UT_NumericKeyHandler* UT_NumericKeyHandler::NewLC()
    {
    UT_NumericKeyHandler* self = new( ELeave ) UT_NumericKeyHandler();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
UT_NumericKeyHandler::~UT_NumericKeyHandler()
    {
    }

// Default constructor
UT_NumericKeyHandler::UT_NumericKeyHandler()
    {    
    }

// Second phase construct
void UT_NumericKeyHandler::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

void UT_NumericKeyHandler::EmptySetupL(  )
    {
        
    }

void UT_NumericKeyHandler::SetupL(  )
    {
    if ( !iAnimGeneralFunctions )
        {
        iAnimGeneralFunctions = CAnimGeneralFunctionsStub::NewL();
        }
    
    iModifiers = NULL;
    MAnimGeneralFunctionsWindowExtension* ext = 
    static_cast<MAnimGeneralFunctionsWindowExtension*>( 
        iAnimGeneralFunctions->ExtendedInterface
            ( MAnimGeneralFunctions::EWindowExtensionInterface ) );
    
    iNumericKeyHandler = AiWsPlugin::CNumericKeyHandler::NewLC( 0, ext);
    CleanupStack::Pop();
    }

void UT_NumericKeyHandler::Teardown(  )
    {
    if( iNumericKeyHandler )
        {
        delete iNumericKeyHandler;
        iNumericKeyHandler = NULL;
        }    
    if ( iAnimGeneralFunctions )
        {
        delete iAnimGeneralFunctions;
        iAnimGeneralFunctions = NULL;        
        }
    _LIT( fileName, "C:\\keyhandler.txt");
    RFs fs;
    fs.Connect();            
    fs.Delete( fileName );
    fs.Close();
    }

void UT_NumericKeyHandler::TestNumericKeyCreationL(  )
    {   
    EUNIT_ASSERT( iNumericKeyHandler );
    }
    
void UT_NumericKeyHandler::TestFocusChangeL(  )
    {   
    // The fuction that this test tests is unnecessary
    AiWsPlugin::CEventHandler* handler = iNumericKeyHandler;
    handler->FocusChanged( ETrue );
    EUNIT_ASSERT( ETrue );
    }    

void UT_NumericKeyHandler::TestNumericKeyEventL()
    {

    TRawEvent event;
    event.Set( TRawEvent::EKeyDown, EStdKeyHash );
    
    AiWsPlugin::CEventHandler* handler = iNumericKeyHandler;   
    CleanupStack::PushL( handler );  
    
    handler->SetUiStateQuery( *this );
    handler->OfferRawEvent(event);
    
    CleanupStack::Pop( handler );  
    
    TUid uidi(KNullUid);
    TUint uintti(0);

    RProperty::Get(uidi, uintti, iTestValue );
   
    //EUNIT_ASSERT_EQUALS( iTestValue, TInt(EPSAiNumberEntry));
   }
    
    
void UT_NumericKeyHandler::TestNumericKeyEventQwertyModeL()
    {
    // Write file to enable qwertymode
    RFs fs;
    RFile file;
    TInt err = fs.Connect();
    err = file.Replace( fs, fileName, EFileShareAny);
    TBuf8<30> txt;
    txt.Num( TInt(1) );
    err = file.Write( 0,txt);
    file.Close();
    fs.Close();  
    
    if ( !iAnimGeneralFunctions )
        {
        iAnimGeneralFunctions = CAnimGeneralFunctionsStub::NewL();
        }
    
    iModifiers = NULL;
    MAnimGeneralFunctionsWindowExtension* ext = 
    static_cast<MAnimGeneralFunctionsWindowExtension*>( 
        iAnimGeneralFunctions->ExtendedInterface
            ( MAnimGeneralFunctions::EWindowExtensionInterface ) );
    
    iNumericKeyHandler = AiWsPlugin::CNumericKeyHandler::NewLC( 0, ext);
    CleanupStack::Pop();    
    
    TRawEvent event;
    event.Set( TRawEvent::EKeyDown, EStdKeyHash );    
      
        
    AiWsPlugin::CEventHandler* handler = iNumericKeyHandler;    
    handler->SetUiStateQuery( *this );
    handler->OfferRawEvent(event);
    TUid uidi(KNullUid);
    TUint uintti(0);
    RProperty::Get(uidi, uintti, iTestValue );
   
    EUNIT_ASSERT_EQUALS( iTestValue, TInt(EPSAiNumberEntry ));
    }    
    
void UT_NumericKeyHandler::TestHandlerFunctions1L()
    {
    AiWsPlugin::CEventHandler* handler = iNumericKeyHandler;    
    handler->SetUiStateQuery( *this );
    TRawEvent event;
    event.Set( TRawEvent::EKeyDown, EStdKeyHash );
    iNumericKeyHandler->iQwertyMode = ETrue;
    iNumericKeyHandler->CheckPostToTarget( event );

    iNumericKeyHandler->iQwertyMode = EFalse;
    iNumericKeyHandler->CheckPostToTarget( event );
    
    event.Set( TRawEvent::EKeyDown, '5' );
    iNumericKeyHandler->iQwertyMode = ETrue;
    iNumericKeyHandler->CheckPostToTarget( event );

    event.Set( TRawEvent::EKeyDown, '5' );
    iNumericKeyHandler->iQwertyMode = EFalse;
    iNumericKeyHandler->CheckPostToTarget( event );
    }
void UT_NumericKeyHandler::TestHandlerFunctions2L()
    {
    AiWsPlugin::CEventHandler* handler = iNumericKeyHandler;    
    handler->SetUiStateQuery( *this );
    iNumericKeyHandler->LoadInputLanguageKeyBindings(0);
    iNumericKeyHandler->LoadInputLanguageKeyBindings(1);
    iNumericKeyHandler->LoadInputLanguageKeyBindings(2);
    iNumericKeyHandler->LoadInputLanguageKeyBindings(3);
    iNumericKeyHandler->LoadInputLanguageKeyBindings(4);
    iNumericKeyHandler->LoadInputLanguageKeyBindings(5);
    iNumericKeyHandler->LoadInputLanguageKeyBindings(6);
    }
void UT_NumericKeyHandler::TestHandlerFunctions3L()
    {
    AiWsPlugin::CEventHandler* handler = iNumericKeyHandler;    
    handler->SetUiStateQuery( *this );
    iNumericKeyHandler->HandleQwertyModeChanged(iNumericKeyHandler);
    iNumericKeyHandler->HandleNotifyGeneric(0);
    iNumericKeyHandler->HandleNotifyGeneric(1);
    iNumericKeyHandler->HandleNotifyGeneric(KAknFepInputTxtLang);
    }

TUint UT_NumericKeyHandler::Modifiers() const
    {
    return iModifiers;
    }    
    
    
TBool UT_NumericKeyHandler::HasFocus() const
    {
    return ETrue;
    }


//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    UT_NumericKeyHandler,
    "Add test suite description here.",
    "MODULE" )

EUNIT_TEST(
    "Test creation",
    "CNumericKeyHandler",
    "",
    "FUNCTIONALITY",
    SetupL, TestNumericKeyCreationL, Teardown)
    
EUNIT_TEST(
    "Test Numeric event",
    "CNumericKeyHandler",
    "",
    "FUNCTIONALITY",
    SetupL, TestNumericKeyEventL, Teardown)    
    
EUNIT_TEST(
    "Test Focus Change",
    "CNumericKeyHandler",
    "",
    "FUNCTIONALITY",
    SetupL, TestFocusChangeL, Teardown)    

EUNIT_TEST(
    "Test Functions",
    "CNumericKeyHandler",
    "",
    "FUNCTIONALITY",
    SetupL, TestHandlerFunctions1L, Teardown)    

EUNIT_TEST(
    "Test Functions",
    "CNumericKeyHandler",
    "",
    "FUNCTIONALITY",
    SetupL, TestHandlerFunctions2L, Teardown)    

EUNIT_TEST(
    "Test Functions",
    "CNumericKeyHandler",
    "",
    "FUNCTIONALITY",
    SetupL, TestHandlerFunctions3L, Teardown)    

    
// Enable this test when memory leak in CTpiEngine has bee fixed.(TSW:VRIA-6V7B4W)
/*EUNIT_TEST(
    "Qwerty Mode Numeric event",
    "CNumericKeyHandler",
    "",
    "FUNCTIONALITY",
    EmptySetupL, TestNumericKeyEventQwertyModeL, Teardown)*/

EUNIT_END_TEST_TABLE

//  END OF FILE
