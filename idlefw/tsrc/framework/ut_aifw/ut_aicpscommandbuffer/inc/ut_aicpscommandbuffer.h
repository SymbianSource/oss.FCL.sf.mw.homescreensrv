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


#ifndef _UT_AICPSCOMMANDBUFFER_H
#define _UT_AICPSCOMMANDBUFFER_H

// System includes
#include <e32base.h>
#include <digia/eunit/ceunittestsuiteclass.h>

// User includes 
#include "caicpscommandbuffer.h"

// Forward declarations
class CAiCpsCommandBuffer;

// Clas declaration
NONSHARABLE_CLASS( UT_AiCpsCommandBuffer ) : public CEUnitTestSuiteClass
    {
public:
    // Constructors and destructors

    /**
     * Two phase construction
     */
    static UT_AiCpsCommandBuffer* NewL();

    /**
     * Two phase construction
     */    
    static UT_AiCpsCommandBuffer* NewLC();
    
    /**
     * Destructor
     */
    ~UT_AiCpsCommandBuffer();

private: 
    // Constructors and destructors

    UT_AiCpsCommandBuffer();
    
    void ConstructL();
        
private:    
    // New methods

    void SetupL();        
    void Teardown();
    
    void GetCPsServiceInsterfaceL();
    void AddCommandAndFlushL();
    void CreateAndDelete();
private:    
    // data
    
    CAiCpsCommandBuffer* iCommandBuffer;
    
    EUNIT_DECLARE_TEST_TABLE;
    };

#endif //  _UT_AICPSCOMMANDBUFFER_H

// End of file
