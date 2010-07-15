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
* Description: EUnit unit test class for THsPublisherInfo
*
*/


#ifndef _UT_HSPUBLISHERINFO_H
#define _UT_HSPUBLISHERINFO_H

// System includes
#include <e32base.h>
#include <digia/eunit/ceunittestsuiteclass.h>

// User includes 

// Forward declarations

// Clas declaration
NONSHARABLE_CLASS( UT_HsPublisherInfo ) : public CEUnitTestSuiteClass    
    {
public:
    // Constructors and destructors

    /**
     * Two phase construction
     */
    static UT_HsPublisherInfo* NewL();

    /**
     * Two phase construction
     */    
    static UT_HsPublisherInfo* NewLC();
    
    /**
     * Destructor
     */
    ~UT_HsPublisherInfo();

private: 
    // Constructors and destructors

    UT_HsPublisherInfo();
    
    void ConstructL();

private:    
    // New methods

    void SetupL();        
    void Teardown();
    
    void TestConstructionL();
    void TestAssigmentOperatorL();
    void TestEqualsOperatorL();
    void TestGetUidL();
    void TestGetNameL();
    void TestGetNamespaceL();
                        
private:    
    // data

    EUNIT_DECLARE_TEST_TABLE;    
    };

#endif //  _UT_HSPUBLISHERINFO_H

// End of file
