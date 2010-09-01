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


#ifndef _UT_NUMERICKEYHANDLER_H
#define _UT_NUMERICKEYHANDLER_H

// System includes
#include <e32base.h>
#include <w32adll.h>
#include <e32def.h>
#include <digia/eunit/ceunittestsuiteclass.h>

// User includes
#include "uistate.h"
#include "aiwspluginanimdef.h"

// Forward declarations
class CAnimGeneralFunctionsStub;

typedef MAnimGeneralFunctionsWindowExtension::TWindowGroupInfo TIdPlgWindowGroupInfo;

namespace AiWsPlugin {

class CNumericKeyHandler;

// Class declaration
NONSHARABLE_CLASS( UT_NumericKeyHandler ) : public CEUnitTestSuiteClass,
   public MUiState
   {
public:     
    // Constructors and destructors

    /**
     * Two phase construction
     */
    static UT_NumericKeyHandler* NewL();
    static UT_NumericKeyHandler* NewLC();
    /**
     * Destructor
     */
    ~UT_NumericKeyHandler();

private:    
    // Constructors and destructors

    UT_NumericKeyHandler();
    void ConstructL();


public: 
    // From base class MUiState
    
    TUint Modifiers() const;
    TBool HasFocus() const;

private:    
    // New methods

     void SetupL();
     
     void EmptySetupL();
    
     void Teardown();
    
     void TestNumericKeyCreationL();
     
     void TestNumericKeyEventL();
     
     void TestFocusChangeL();
     
     void TestNumericKeyEventQwertyModeL();

     void TestHandlerFunctions1L();

     void TestHandlerFunctions2L();

     void TestHandlerFunctions3L();
     
private:   
    // Data

    EUNIT_DECLARE_TEST_TABLE; 
    
    CNumericKeyHandler* iNumericKeyHandler;
    
    CAnimGeneralFunctionsStub* iAnimGeneralFunctions;
      
    TInt iTestValue;
    
    TUint iModifiers;    
    };
}

#endif      //  _UT_NUMERICKEYHANDLER_H

// End of file
