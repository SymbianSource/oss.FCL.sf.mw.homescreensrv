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
* Description: EUnit unit test class for CAiStateManager
*
*/


#ifndef _UT_AISTATEMANAGER_H
#define _UT_AISTATEMANAGER_H

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
NONSHARABLE_CLASS( UT_AiStateManager ) : public CEUnitTestSuiteClass
    {
public:
    // Constructors and destructors

    /**
     * Two phase construction
     */
    static UT_AiStateManager* NewL();

    /**
     * Two phase construction
     */    
    static UT_AiStateManager* NewLC();
    
    /**
     * Destructor
     */
    ~UT_AiStateManager();

private: 
    // Constructors and destructors

    UT_AiStateManager();
    
    void ConstructL();

private:    
    // New methods

    void SetupL();        
    void Teardown();
        
    void TestStateChangeL();
    void TestLoadAndDestroyPluginL();
    void TestUpgradePluginL();
    void NotifyReleasePluginsL();
    
    static void CallBack( TAny* aPtr, TInt aResult );
    static TInt TimerCallBack( TAny* aPtr );
    
private:    
    // data

    EUNIT_DECLARE_TEST_TABLE; 
    
    CAiUiControllerManager* iUiMgr;
    CAiPluginFactory* iFactory;
    CAiStateManager* iMgr;
    CAiStateManager* iStateManager;
    CActiveSchedulerWait* iWait;
    TInt iResult;
    CPeriodic* iPeriodic;
    };

#endif //  _UT_AISTATEMANAGER_H

// End of file
