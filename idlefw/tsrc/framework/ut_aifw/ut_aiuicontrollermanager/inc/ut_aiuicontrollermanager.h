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
* Description: EUnit unit test class for CAiUiControllerManager
*
*/


#ifndef _UT_AIUICONTROLLERMANAGER_H
#define _UT_AIUICONTROLLERMANAGER_H

// System includes
#include <e32base.h>
#include <digia/eunit/ceunittestsuiteclass.h>

// User includes 

// Forward declarations
class CAiUiControllerManager;
class CAiFw;

// Clas declaration
NONSHARABLE_CLASS( UT_AiUiControllerManager ) : public CEUnitTestSuiteClass    
    {
public:
    // Constructors and destructors

    /**
     * Two phase construction
     */
    static UT_AiUiControllerManager* NewL();

    /**
     * Two phase construction
     */    
    static UT_AiUiControllerManager* NewLC();
    
    /**
     * Destructor
     */
    ~UT_AiUiControllerManager();

private: 
    // Constructors and destructors

    UT_AiUiControllerManager();
    
    void ConstructL();

private:    
    // New methods

    void SetupL();        
    void Teardown();
    
    void TestGetUiControllersL();
    void TestActivateUIL();
    void TestGetMainUiControllerL();
    void TestIsMainUiControllerL();
    void TestRunApplicationL();
    void TestLoadUIDefinitionL();
    void TestGetCoeEnvL();
    void TestDestroySecondaryUiControllersL();
    void TestExitMainControllerL();
    void TestSetStateHandlerL();
                    
private:    
    // data

    EUNIT_DECLARE_TEST_TABLE;
    
    CAiFw* iAiFw;
    CAiUiControllerManager* iUiMgr;
    };

#endif //  _UT_AIUICONTROLLERMANAGER_H

// End of file
