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
* Description: EUnit unit test class for CAiEventHandler
*
*/


#ifndef _UT_AIEVENTHANDLER_H
#define _UT_AIEVENTHANDLER_H

// System includes
#include <e32base.h>
#include <digia/eunit/ceunittestsuiteclass.h>

// User includes 

// Forward declarations
class CAiUiControllerManager;
class CAiPluginFactory;
class CAiEventHandler;
class CActiveSchedulerWait;
class CAiStateManager;
// Clas declaration
NONSHARABLE_CLASS( UT_AiEventHandler ) : public CEUnitTestSuiteClass    
    {
public:
    // Constructors and destructors

    /**
     * Two phase construction
     */
    static UT_AiEventHandler* NewL();

    /**
     * Two phase construction
     */    
    static UT_AiEventHandler* NewLC();
    
    /**
     * Destructor
     */
    ~UT_AiEventHandler();

private: 
    // Constructors and destructors

    UT_AiEventHandler();
    
    void ConstructL();

       
private:    
    // New methods

    void SetupL();        
    void Teardown();
    
    void TestHandlePluginEventL();
    void TestHasMenuItemL();
    void TestRefreshContentL();
    void TestSuspendContentL();
    
    static void CallBack( TAny* aPtr, TInt aResult );
private:    
    // data

    EUNIT_DECLARE_TEST_TABLE; 
    
    CAiUiControllerManager* iUiMgr;
    
    CAiPluginFactory* iFactory;   
    
    CAiEventHandler* iHandler;
    
    CAiStateManager* iStateManager;
            
    CActiveSchedulerWait* iWait;
    
    TInt iResult;
    };

#endif //  _UT_AIEVENTHANDLER_H

// End of file
