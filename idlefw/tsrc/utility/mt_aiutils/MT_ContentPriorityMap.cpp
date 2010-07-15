/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "contentprioritymap.h"

#include "MT_ContentPriorityMap.h"
#include <digia/eunit/eunitmacros.h>

using namespace AiUtility;

namespace
    {
    _LIT8( KTestId, "TestElementId" );
    
    const TInt KDefaultPriority( 0 );
    
    const TInt KHighPriority( 1 );
    }


// CONSTRUCTION
MT_ContentPriorityMap* MT_ContentPriorityMap::NewL()
    {
    MT_ContentPriorityMap* self = MT_ContentPriorityMap::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

MT_ContentPriorityMap* MT_ContentPriorityMap::NewLC()
    {
    MT_ContentPriorityMap* self = new( ELeave ) MT_ContentPriorityMap();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
MT_ContentPriorityMap::~MT_ContentPriorityMap()
    {
#if _BullseyeCoverage
    cov_write();
#endif
    }

// Default constructor
MT_ContentPriorityMap::MT_ContentPriorityMap()
    {
    }

// Second phase construct
void MT_ContentPriorityMap::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS

void MT_ContentPriorityMap::SetupL(  )
    {
    iContentPriorityMap = CContentPriorityMap::NewL();
    }

void MT_ContentPriorityMap::Teardown(  )
    {
    delete iContentPriorityMap;
    iContentPriorityMap = NULL;
    }

void MT_ContentPriorityMap::TestEmptyMapL(  )
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

void MT_ContentPriorityMap::TestClearL(  )
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

void MT_ContentPriorityMap::TestUpdateL(  )
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

void MT_ContentPriorityMap::TestOverrideL()
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

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    MT_ContentPriorityMap,
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

EUNIT_END_TEST_TABLE

//  END OF FILE
