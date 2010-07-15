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

// System includes
#include <e32base.h>
#include <e32cmn.h> 
#include <digia/eunit/eunitmacros.h>
#include <digia/eunit/teunitassertioninfo.h>

// User includes
#include <hspublisherinfo.h>

#include "ut_hspublisherinfo.h"

// Constants
_LIT( KName, "testname" );
_LIT8( KNamespace, "testnamespace" );

const TInt KUid( 0xBABE2BED );
    

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS =======
// ----------------------------------------------------------------------------
// UT_HsPublisherInfo::NewL()
//
// ----------------------------------------------------------------------------
//
UT_HsPublisherInfo* UT_HsPublisherInfo::NewL()
    {
    UT_HsPublisherInfo* self = UT_HsPublisherInfo::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// UT_HsPublisherInfo::NewLC()
//
// ----------------------------------------------------------------------------
//
UT_HsPublisherInfo* UT_HsPublisherInfo::NewLC()
    {
    UT_HsPublisherInfo* self = new ( ELeave ) UT_HsPublisherInfo();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// UT_HsPublisherInfo::~UT_HsPublisherInfo()
//
// ----------------------------------------------------------------------------
//
UT_HsPublisherInfo::~UT_HsPublisherInfo()
    {    
    }

// ----------------------------------------------------------------------------
// UT_HsPublisherInfo::UT_HsPublisherInfo()
//
// ----------------------------------------------------------------------------
//
UT_HsPublisherInfo::UT_HsPublisherInfo()
    {
    }

// ----------------------------------------------------------------------------
// UT_HsPublisherInfo::ConstructL()
//
// ----------------------------------------------------------------------------
//
void UT_HsPublisherInfo::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

// ----------------------------------------------------------------------------
// UT_HsPublisherInfo::SetupL()
//
// ----------------------------------------------------------------------------
//
void UT_HsPublisherInfo::SetupL()
    {
    }

// ----------------------------------------------------------------------------
// UT_HsPublisherInfo::Teardown()
//
// ----------------------------------------------------------------------------
//
void UT_HsPublisherInfo::Teardown()
    {
    }

// ----------------------------------------------------------------------------
// UT_HsPublisherInfo::TestConstructionL()
//
// ----------------------------------------------------------------------------
//
void UT_HsPublisherInfo::TestConstructionL()
    {
    TUid uid( TUid::Uid( KUid ) );
    
    THsPublisherInfo info;
    
    EUNIT_ASSERT_EQUALS( info.iUid == TUid::KNullUid, ETrue );
    EUNIT_ASSERT_EQUALS( info.iName == KNullDesC, ETrue );
    EUNIT_ASSERT_EQUALS( info.iNamespace == KNullDesC8, ETrue );
        
    THsPublisherInfo info2( uid, KName, KNamespace );

    EUNIT_ASSERT_EQUALS( info2.iUid == uid, ETrue );
    EUNIT_ASSERT_EQUALS( info2.iName == KName, ETrue );
    EUNIT_ASSERT_EQUALS( info2.iNamespace == KNamespace, ETrue );    
    }

// ----------------------------------------------------------------------------
// UT_HsPublisherInfo::TestAssigmentOperatorL()
//
// ----------------------------------------------------------------------------
//
void UT_HsPublisherInfo::TestAssigmentOperatorL()
    {
    TUid uid( TUid::Uid( KUid ) );
    
    THsPublisherInfo info;
    
    THsPublisherInfo info2( uid, KName, KNamespace );

    info = info2;

    EUNIT_ASSERT_EQUALS( info.iUid == uid, ETrue );
    EUNIT_ASSERT_EQUALS( info.iName == KName, ETrue );
    EUNIT_ASSERT_EQUALS( info.iNamespace == KNamespace, ETrue );          
    }

// ----------------------------------------------------------------------------
// UT_HsPublisherInfo::TestEqualsOperatorL()
//
// ----------------------------------------------------------------------------
//
void UT_HsPublisherInfo::TestEqualsOperatorL()
    {
    TUid uid( TUid::Uid( KUid ) );
    
    THsPublisherInfo info( uid, KName, KNamespace ); 
    THsPublisherInfo info2 = info;
             
    EUNIT_ASSERT_EQUALS( info == info2, ETrue );
    
    THsPublisherInfo info3;
        
    EUNIT_ASSERT_EQUALS( !( info == info3 ), ETrue );
    }

// ----------------------------------------------------------------------------
// UT_HsPublisherInfo::TestGetUidL()
//
// ----------------------------------------------------------------------------
//
void UT_HsPublisherInfo::TestGetUidL()
    {
    TUid uid( TUid::Uid( KUid ) );
    
    THsPublisherInfo info( uid, KName, KNamespace );
    
    EUNIT_ASSERT_EQUALS( info.Uid() == uid, ETrue );
    }

// ----------------------------------------------------------------------------
// UT_HsPublisherInfo::TestGetNameL()
//
// ----------------------------------------------------------------------------
//
void UT_HsPublisherInfo::TestGetNameL()
    {
    TUid uid( TUid::Uid( KUid ) );
    
    THsPublisherInfo info( uid, KName, KNamespace );

    EUNIT_ASSERT_EQUALS( info.Name() == KName, ETrue );    
    }

// ----------------------------------------------------------------------------
// UT_HsPublisherInfo::TestGetNamespaceL()
//
// ----------------------------------------------------------------------------
//
void UT_HsPublisherInfo::TestGetNamespaceL()
    {
    TUid uid( TUid::Uid( KUid ) );
    
    THsPublisherInfo info( uid, KName, KNamespace );

    EUNIT_ASSERT_EQUALS( info.Namespace() == KNamespace, ETrue );        
    }

//------------------------------------------------------------------------------
// Test case table
//
//------------------------------------------------------------------------------
//
EUNIT_BEGIN_TEST_TABLE(
    UT_HsPublisherInfo,
    "Unit test suite for THsPublisherInfo",
    "UNIT" )

    EUNIT_TEST(
        "Construction",
        "THsPublisherInfo",
        "",
        "FUNCTIONALITY",
        SetupL, TestConstructionL, Teardown )

    EUNIT_TEST(
        "AssigmentOperator",
        "THsPublisherInfo",
        "",
        "FUNCTIONALITY",
        SetupL, TestAssigmentOperatorL, Teardown )

    EUNIT_TEST(
        "EqualsOperator",
        "THsPublisherInfo",
        "",
        "FUNCTIONALITY",
        SetupL, TestEqualsOperatorL, Teardown )
    
    EUNIT_TEST(
        "GetUid",
        "THsPublisherInfo",
        "",
        "FUNCTIONALITY",
        SetupL, TestGetUidL, Teardown )

    EUNIT_TEST(
        "GetName",
        "THsPublisherInfo",
        "",
        "FUNCTIONALITY",
        SetupL, TestGetNameL, Teardown )

    EUNIT_TEST(
        "GetNamespace",
        "THsPublisherInfo",
        "",
        "FUNCTIONALITY",
        SetupL, TestGetNamespaceL, Teardown )
        
EUNIT_END_TEST_TABLE

// End of file
