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


#ifndef _UT_MODIFIERKEYTRACKER_H
#define _UT_MODIFIERKEYTRACKER_H

// System includes
#include <e32base.h>
#include <digia/eunit/ceunittestsuiteclass.h>

// User includes
#include "modifierkeytracker.h"

// Forward declarations

// Class declaration
NONSHARABLE_CLASS( UT_MofifierKeyTracker ) : public CEUnitTestSuiteClass    
    {
public:     
    // Constructors and destructors

    /**
     * Two phase construction
     */
    static UT_MofifierKeyTracker* NewL();
    static UT_MofifierKeyTracker* NewLC();
    /**
     * Destructor
     */
    ~UT_MofifierKeyTracker();

private:    
    // Constructors and destructors

    UT_MofifierKeyTracker();
    void ConstructL();

private:    
    // New methods

     void SetupL();
    
     void Teardown();
    
    void T_TModifierKeyTracker_ConstructL( );
    
    void T_TModifierKeyTracker_NonModiferUpdateL( );
    
    void T_TModifierKeyTracker_TestModifierKeyDownL
            ( TInt aScanCode, TUint aExcpectedStatus  );
            
    void T_TModifierKeyTracker_TestModifierKeyUpL
            ( TInt aScanCode, TUint aExcpectedStatus  );
            
    void T_TModifierKeyTracker_SingleModiferUpdateL
            ( TInt aScanCode, TUint aExcpectedStatus  );
            
    void T_TModifierKeyTracker_SingleModiferUpdateL( );
    
    void T_TModifierKeyTracker_MultiModiferUpdateL( );
    
    void T_TModifierKeyTracker_MixedModiferNonModiferUpdateL( );

private:    
    // Data
        
    EUNIT_DECLARE_TEST_TABLE;
    AiWsPlugin::TModifierKeyTracker* iTModifierKeyTracker;
    };

#endif      //  _UT_MODIFIERKEYTRACKER_H

// End of file
