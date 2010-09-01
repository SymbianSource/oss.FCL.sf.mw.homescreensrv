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
* Description: EUnit unit test class for CAiPluginFactory
*
*/


#ifndef _UT_AIPLUGINFACTORY_H
#define _UT_AIPLUGINFACTORY_H

// System includes
#include <e32base.h>
#include <digia/eunit/ceunittestsuiteclass.h>

// User includes 

// Forward declarations
class CAiUiControllerManager;
class CAiPluginFactory;
class CAiStateManager;
class CActiveSchedulerWait;
class CPeriodic;

// Clas declaration
NONSHARABLE_CLASS( UT_AiPluginFactory ) : public CEUnitTestSuiteClass    
    {
public:
    // Constructors and destructors

    /**
     * Two phase construction
     */
    static UT_AiPluginFactory* NewL();

    /**
     * Two phase construction
     */    
    static UT_AiPluginFactory* NewLC();
    
    /**
     * Destructor
     */
    ~UT_AiPluginFactory();

private: 
    // Constructors and destructors

    UT_AiPluginFactory();
    
    void ConstructL();
       
    static void CallBack( TAny* aPtr, TInt aResult );
    
    static TInt TimerCallBack( TAny* aPtr );
private:    
    // New methods

    void SetupL();        
    void Teardown();
    
    void TestCreateDestroyPluginL();    
    void TestPluginByInfoL();
    void TestPluginByUidL();
    void TestPluginByNameL();
    void TestGetPublishersL();
    void TestUpgradePublishersL();
    void TestCreateDestroyAllL();
    
private:    
    // data

    EUNIT_DECLARE_TEST_TABLE; 
    
    CAiUiControllerManager* iUiMgr;
    
    CAiPluginFactory* iFactory;
    
    CAiStateManager* iStateManager;
        
    CActiveSchedulerWait* iWait;
    
    TInt iResult;
    
    CPeriodic* iPeriodic;
    };

#endif //  _UT_AIPLUGINFACTORY_H

// End of file
