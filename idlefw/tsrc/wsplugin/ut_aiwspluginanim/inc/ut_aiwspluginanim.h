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
* Description: EUnit unit test class for AiWsPlugin
*
*/


#ifndef _UT_CAIWSPLUGINANIM_H
#define _UT_CAIWSPLUGINANIM_H

// System includes
#include <e32base.h>
#include <digia/eunit/ceunittestsuiteclass.h>

// User includes

// Forward declarations
class CAiWsPluginAnim;
class CAnimGeneralFunctionsStub;
class UT_AiWsPluginAnimTls;

// Clas declaration
NONSHARABLE_CLASS( UT_AiWsPluginAnim ) : public CEUnitTestSuiteClass	
    {
public:     
    // Constructors and destructors

    /**
     * Two phase construction
     */
    static UT_AiWsPluginAnim* NewL();
    static UT_AiWsPluginAnim* NewLC();
    /**
     * Destructor
     */
    ~UT_AiWsPluginAnim();

private:    
    // Constructors and destructors

    UT_AiWsPluginAnim();
    void ConstructL();

private:    
    // New methods
    
    void EmptySetup();
    void SetupTlsL();
    void SetupDependenciesL();
    void SetupTestObjL(TBool aHasFocus);
    void SetupNoSliderL( TBool aHasFocus );
    void SetupNoSliderL();
    void SetupL(TBool aHasFocus);
    void SetupL();
    void Teardown();
     
    void T_ConstructL(TBool aHasFocus);
    void T_ConstructL();
    void T_CAiWsPluginAnim_EmptyFunctionsL();
    void T_CAiWsPluginAnim_FocusChangedL( TBool aFocus );
    void T_CAiWsPluginAnim_FocusChangedL();
    void T_CAiWsPluginAnim_OfferRawEventL();
    
private:    
    // Data
    
    EUNIT_DECLARE_TEST_TABLE; 
    
    CAiWsPluginAnim* iCAiWsPluginAnim;
    CAnimGeneralFunctionsStub* iAnimGeneralFunctions;
    UT_AiWsPluginAnimTls* iTls;
    TInt iFeatMgrCount;    
    };

#endif      //  _UT_CAIWSPLUGINANIM_H

// End of file
