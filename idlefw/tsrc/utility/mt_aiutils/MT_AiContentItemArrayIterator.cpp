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
#include <aiutility.h>
#include <aicontentmodel.h>

#include "MT_AiContentItemArrayIterator.h"
#include <digia/eunit/eunitmacros.h>

const TInt KNonExistentItemId = -1;
_LIT(KNonExistentItemCid, "NonExistentCid");



// CONSTRUCTION
MT_AiContentItemArrayIterator* MT_AiContentItemArrayIterator::NewL()
    {
    MT_AiContentItemArrayIterator* self = MT_AiContentItemArrayIterator::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

MT_AiContentItemArrayIterator* MT_AiContentItemArrayIterator::NewLC()
    {
    MT_AiContentItemArrayIterator* self = new( ELeave ) MT_AiContentItemArrayIterator();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
MT_AiContentItemArrayIterator::~MT_AiContentItemArrayIterator()
    {
#if _BullseyeCoverage
    cov_write();
#endif
    }

// Default constructor
MT_AiContentItemArrayIterator::MT_AiContentItemArrayIterator()
    {
    }

// Second phase construct
void MT_AiContentItemArrayIterator::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS



void MT_AiContentItemArrayIterator::SetupL(  )
    {
    }

void MT_AiContentItemArrayIterator::Teardown(  )
    {
    }

void MT_AiContentItemArrayIterator::TestEmptyL(  )
    {
    MAiContentItemIterator* iter = NULL;
    // Test that Release(NULL) does not crash
    Release(iter);
    
    // Test that the iterator works correctly with an empty (NULL) array
    iter = AiUtility::CreateContentItemArrayIteratorL(NULL, 0);
    CleanupReleasePushL(iter);

    // Make two passes through the iterator to test also Reset
    for (TInt round = 1; round <= 2; ++round)
        {
        EUNIT_ASSERT( !iter->HasNext() );
        EUNIT_ASSERT_SPECIFIC_LEAVE( iter->NextL(), KErrOverflow );
        EUNIT_ASSERT_SPECIFIC_LEAVE( iter->ItemL(0), KErrNotFound );
        EUNIT_ASSERT_SPECIFIC_LEAVE( iter->ItemL(KNullDesC), KErrNotFound );
        iter->Reset();
        }

    CleanupStack::PopAndDestroy(iter);
    
    // Array with negative count should work as an empty array
    iter = AiUtility::CreateContentItemArrayIteratorL(NULL, -1);
    CleanupReleasePushL(iter);

    // Make two passes through the iterator to test also Reset
    for (TInt round = 1; round <= 2; ++round)
        {
        EUNIT_ASSERT( !iter->HasNext() );
        EUNIT_ASSERT_SPECIFIC_LEAVE( iter->NextL(), KErrOverflow );
        EUNIT_ASSERT_SPECIFIC_LEAVE( iter->ItemL(0), KErrNotFound );
        EUNIT_ASSERT_SPECIFIC_LEAVE( iter->ItemL(KNullDesC), KErrNotFound );
        iter->Reset();
        }

    CleanupStack::PopAndDestroy(iter);
    }

TBool operator==(const TAiContentItem& aLhs, const TAiContentItem& aRhs)
    {
    // Use pointer comparison as CAiContentItemArrayIterator iterates through
    // the original items
    return (&aLhs == &aRhs);
    }

void DoTestOneL
        ( const TAiContentItem& aRefItem, MAiContentItemIterator& aIter )
    {
    // Make two passes through the iterator to test also Reset
    for (TInt pass = 1; pass <= 2; ++pass)
        {
        EUNIT_ASSERT( aIter.HasNext() );
        EUNIT_ASSERT_EQUALS( aRefItem, aIter.NextL() );
        EUNIT_ASSERT( !aIter.HasNext() );

        EUNIT_ASSERT_SPECIFIC_LEAVE( aIter.NextL(), KErrOverflow );
        EUNIT_ASSERT_SPECIFIC_LEAVE( aIter.ItemL(KNonExistentItemId), KErrNotFound );
        EUNIT_ASSERT_SPECIFIC_LEAVE( aIter.ItemL(KNonExistentItemCid), KErrNotFound );

        EUNIT_ASSERT_EQUALS( aRefItem, aIter.ItemL(aRefItem.id) );
        EUNIT_ASSERT_EQUALS( aRefItem, aIter.ItemL(ContentCid(aRefItem)) );

        EUNIT_ASSERT_SPECIFIC_LEAVE( aIter.ItemL(KNonExistentItemId), KErrNotFound );
        EUNIT_ASSERT_SPECIFIC_LEAVE( aIter.ItemL(KNonExistentItemCid), KErrNotFound );

        aIter.Reset();
        }
    }

void MT_AiContentItemArrayIterator::TestOneL(  )
    {
    const TInt KTestId = 42;
    static const wchar_t KTestCid[] = L"testcid";
    static const char KTestType[] = "testtype";
    static const TAiContentItem KOneItem[] = { { KTestId, KTestCid, KTestType } };
    
    // Test iterator created with NewL
    MAiContentItemIterator* iter =
        AiUtility::CreateContentItemArrayIteratorL(KOneItem, 1);
    CleanupReleasePushL(iter);
    DoTestOneL( KOneItem[0], *iter );
    CleanupStack::PopAndDestroy(iter);
    
    // Test iterator created with create template helper
    iter = AiUtility::CreateContentItemArrayIteratorL(KOneItem);
    CleanupReleasePushL(iter);
    DoTestOneL( KOneItem[0], *iter );
    CleanupStack::PopAndDestroy(iter);
    }

void DoTestManyL
        ( const TAiContentItem& aRefItem, MAiContentItemIterator& aIter, TInt aCount )
    {
    // Make two passes through the iterator to test also Reset
    for (TInt pass = 1; pass <= 2; ++pass)
        {
        TBool refItemIndex = -1;
        for ( TInt count = 0; count < aCount; ++count )
            {
            EUNIT_ASSERT( aIter.HasNext() );
            if (aIter.NextL() == aRefItem)
                {
                refItemIndex = count;
                }
            }
        EUNIT_ASSERT( refItemIndex >= 0 );
        EUNIT_ASSERT( !aIter.HasNext() );
        
        EUNIT_ASSERT_SPECIFIC_LEAVE( aIter.NextL(), KErrOverflow );
        EUNIT_ASSERT_SPECIFIC_LEAVE( aIter.ItemL(KNonExistentItemId), KErrNotFound );
        EUNIT_ASSERT_SPECIFIC_LEAVE( aIter.ItemL(KNonExistentItemCid), KErrNotFound );
        
        EUNIT_ASSERT_EQUALS( aRefItem, aIter.ItemL(aRefItem.id) );
        EUNIT_ASSERT_EQUALS( aRefItem, aIter.ItemL(ContentCid(aRefItem)) );

        EUNIT_ASSERT_SPECIFIC_LEAVE( aIter.ItemL(KNonExistentItemId), KErrNotFound );
        EUNIT_ASSERT_SPECIFIC_LEAVE( aIter.ItemL(KNonExistentItemCid), KErrNotFound );

        aIter.Reset();
        }
    }

void MT_AiContentItemArrayIterator::TestManyL(  )
    {
    const TInt KTestId1 = 42;
    static const wchar_t KTestCid1[] = L"testcid1";
    static const char KTestType1[] = "testtype1";
    const TInt KTestId2 = 53;
    static const wchar_t KTestCid2[] = L"testcid2";
    static const char KTestType2[] = "testtype2";
    static const TAiContentItem KTwoItems[] = 
        { 
        { KTestId1, KTestCid1, KTestType1 },
        { KTestId2, KTestCid2, KTestType2 } 
        };
    
    // Test iterator created with NewL
    MAiContentItemIterator* iter =
        AiUtility::CreateContentItemArrayIteratorL(KTwoItems, 2);
    CleanupReleasePushL(iter);
    DoTestManyL( KTwoItems[0], *iter, 2 );
    DoTestManyL( KTwoItems[1], *iter, 2 );
    CleanupStack::PopAndDestroy(iter);
    
    // Test iterator created with template helper
    iter = AiUtility::CreateContentItemArrayIteratorL(KTwoItems);
    CleanupReleasePushL(iter);
    DoTestManyL( KTwoItems[0], *iter, 2 );
    DoTestManyL( KTwoItems[1], *iter, 2 );
    CleanupStack::PopAndDestroy(iter);
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    MT_AiContentItemArrayIterator,
    "Unit test suite for MAiContentItemIterator implementation returned from AiUtility::CreateContentItemArrayIteratorL",
    "CAiContentItemArrayIterator" )

EUNIT_TEST(
    "Test empty (NULL) array",
    "MAiContentItemIterator",
    "All",
    "FUNCTIONALITY",
    SetupL, TestEmptyL, Teardown)

EUNIT_TEST(
    "Test array with one item",
    "MAiContentItemIterator",
    "All",
    "FUNCTIONALITY",
    SetupL, TestOneL, Teardown)

EUNIT_TEST(
    "Test array with multiple items",
    "MAiContentItemIteratore",
    "All",
    "FUNCTIONALITY",
    SetupL, TestManyL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE
