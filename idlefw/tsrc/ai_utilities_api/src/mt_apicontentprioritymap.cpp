/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

// CLASS UNDER TEST
#include <contentprioritymap.h>

#include <mt_apicontentprioritymap.h>
#include <digia/eunit/eunitmacros.h>

using namespace AiUtility;

namespace
    {
    _LIT8( KTestId, "TestElementId" );
    
    const TInt KDefaultPriority( 0 );
    
    const TInt KHighPriority( 1 );
    }


// CONSTRUCTION
MT_ApiContentPriorityMap* MT_ApiContentPriorityMap::NewL()
    {
    MT_ApiContentPriorityMap* self = MT_ApiContentPriorityMap::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

MT_ApiContentPriorityMap* MT_ApiContentPriorityMap::NewLC()
    {
    MT_ApiContentPriorityMap* self = new( ELeave ) MT_ApiContentPriorityMap();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
MT_ApiContentPriorityMap::~MT_ApiContentPriorityMap()
    {
#if _BullseyeCoverage
    cov_write();
#endif
    }

// Default constructor
MT_ApiContentPriorityMap::MT_ApiContentPriorityMap()
    {
    }

// Second phase construct
void MT_ApiContentPriorityMap::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

void MT_ApiContentPriorityMap::SetupL(  )
    {
    iContentPriorityMap = CContentPriorityMap::NewL();
    }

void MT_ApiContentPriorityMap::Teardown(  )
    {
    delete iContentPriorityMap;
    iContentPriorityMap = NULL;
    }

void MT_ApiContentPriorityMap::TestEmptyMapL(  )
    {
    // Test that map is empty
    EUNIT_ASSERT_EQUALS( iContentPriorityMap->CurrentPriority( KTestId ),
                         KErrNotFound );
                         
    // Test initial value setting
    EUNIT_ASSERT_EQUALS( iContentPriorityMap->SetCurrentPriority( KTestId, KDefaultPriority ),
                         KErrNone );
    
    // Test current priority
    EUNIT_ASSERT_EQUALS( iContentPriorityMap->CurrentPriority( KTestId ),
                         KDefaultPriority );

    }

void MT_ApiContentPriorityMap::TestClearL(  )
    {
    // Clear empty map (for coverage )
    iContentPriorityMap->ClearPriority( KTestId ); // 
 
    // Set default priority
    EUNIT_ASSERT_EQUALS( iContentPriorityMap->SetCurrentPriority( KTestId, KDefaultPriority ),
                         KErrNone );
   
    // Clear priority
    iContentPriorityMap->ClearPriority( KTestId );
   
    // Test that priority is cleared
    EUNIT_ASSERT_EQUALS( iContentPriorityMap->CurrentPriority( KTestId ),
                         KErrNotFound );
    }

void MT_ApiContentPriorityMap::TestUpdateL(  )
    {
    // Set default priority
    EUNIT_ASSERT_EQUALS( iContentPriorityMap->SetCurrentPriority( KTestId, KDefaultPriority ),
                         KErrNone );
    
    // Update priority
    EUNIT_ASSERT_EQUALS( iContentPriorityMap->SetCurrentPriority( KTestId, KHighPriority ),
                         KErrNone );
    
    // Test updated priority
    EUNIT_ASSERT_EQUALS( iContentPriorityMap->CurrentPriority( KTestId ),
                         KHighPriority );
    }

void MT_ApiContentPriorityMap::TestOverrideL()
    {
    // Test empty map
    EUNIT_ASSERT( iContentPriorityMap->OverrideContent( KTestId, KDefaultPriority ) );
    
    // Set default
    EUNIT_ASSERT_EQUALS( iContentPriorityMap->SetCurrentPriority( KTestId, KDefaultPriority ),
                         KErrNone );
    
    // Test again with default priority
    EUNIT_ASSERT( iContentPriorityMap->OverrideContent( KTestId, KDefaultPriority ) );                    
    
    // Test high priority
    EUNIT_ASSERT( iContentPriorityMap->OverrideContent( KTestId, KHighPriority ) );
    
    // Set high priority
    EUNIT_ASSERT_EQUALS( iContentPriorityMap->SetCurrentPriority( KTestId, KHighPriority ),
                         KErrNone );
    
    // Test default priority
    EUNIT_ASSERT( !iContentPriorityMap->OverrideContent( KTestId, KDefaultPriority ) );

    }

void MT_ApiContentPriorityMap::TestResetL()
    {
    iContentPriorityMap->Reset();
     }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    MT_ApiContentPriorityMap,
    "Unit test suite for AiUtility::CContentPriorityMap",
    "AiUtility::CContentPriorityMap" )

EUNIT_TEST(
    "Test empty map",
    "AiUtility::CContentPriorityMap",
    "CurrentPriority, SetCurrentPriority",
    "FUNCTIONALITY",
    SetupL, TestEmptyMapL, Teardown)

EUNIT_TEST(
    "Test clear functionality",
    "AiUtility::CContentPriorityMap",
    "All",
    "FUNCTIONALITY",
    SetupL, TestClearL, Teardown)

EUNIT_TEST(
    "Test update functionality",
    "AiUtility::CContentPriorityMap",
    "All",
    "FUNCTIONALITY",
    SetupL, TestUpdateL, Teardown)

EUNIT_TEST(
    "Test override functionality",
    "AiUtility::CContentPriorityMap",
    "OverrideContent, SetCurrentPriority",
    "FUNCTIONALITY",
    SetupL, TestOverrideL, Teardown)

EUNIT_TEST(
    "Test reset functionality",
    "AiUtility::CContentPriorityMap",
    "Reset, Reset",
    "FUNCTIONALITY",
    SetupL, TestResetL, Teardown)
    
    
EUNIT_END_TEST_TABLE

//  END OF FILE
