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
* Description: EUnit unit test class for CAiEcomObserver
*
*/


#ifndef _UT_AICPSEXECUTEPARAM_H
#define _UT_AICPSEXECUTEPARAM_H

// System includes
#include <e32base.h>
#include <digia/eunit/ceunittestsuiteclass.h>

// User includes 

// Forward declarations
class CAiCpsExecuteParam;

// Clas declaration
NONSHARABLE_CLASS( UT_AiCpsExecuteParam ) : public CEUnitTestSuiteClass
    {
public:
    // Constructors and destructors

    /**
     * Two phase construction
     */
    static UT_AiCpsExecuteParam* NewL();

    /**
     * Two phase construction
     */    
    static UT_AiCpsExecuteParam* NewLC();
    
    /**
     * Destructor
     */
    ~UT_AiCpsExecuteParam();

private: 
    // Constructors and destructors

    UT_AiCpsExecuteParam();
    
    void ConstructL();
        
private:    
    // New methods

    void SetupL();        
    void Teardown();
    
    void PluginId();
    void InParamMapL();
    
private:    
    // data
    CAiCpsExecuteParam* iExecuteParam; 
    
    EUNIT_DECLARE_TEST_TABLE;
    };

#endif //  _UT_AICPSEXECUTEPARAM_H

// End of file
