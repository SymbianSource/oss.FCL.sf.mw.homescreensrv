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
#include "caouserafter.h"
#include <aiutility.h>
#include "caipspropertyobserver.h"

#include "MT_AiPSPropertyObserver.h"
#include <digia/eunit/eunitmacros.h>

const TInt KRandTestUid( 0x017000F5 );
const TInt KRandTestKey( 0x000000FF );

void ChangeKeyValueL( TInt aValue )
    {
    RProperty::Define( TUid::Uid( KRandTestUid ),
                       KRandTestKey,
                       RProperty::EInt );
                                
    User::LeaveIfError( 
                    RProperty::Set( TUid::Uid( KRandTestUid ),
                    KRandTestKey,
                    aValue ) );
    
    }


// CONSTRUCTION
MT_AiPSPropertyObserver* MT_AiPSPropertyObserver::NewL()
    {
    MT_AiPSPropertyObserver* self = MT_AiPSPropertyObserver::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

MT_AiPSPropertyObserver* MT_AiPSPropertyObserver::NewLC()
    {
    MT_AiPSPropertyObserver* self = new( ELeave ) MT_AiPSPropertyObserver();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// Destructor (virtual by CBase)
MT_AiPSPropertyObserver::~MT_AiPSPropertyObserver()
    {
#if _BullseyeCoverage
    cov_write();
#endif
    }

// Default constructor
MT_AiPSPropertyObserver::MT_AiPSPropertyObserver()
    {
    }

// Second phase construct
void MT_AiPSPropertyObserver::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();

    ChangeKeyValueL( 5 );
    CAOUserAfter::AfterLD( 2 * 1000000 );
    }

//  METHODS

TInt MT_AiPSPropertyObserver::HandleSystemStateChangedL(TAny* aPtr)
    {
    MT_AiPSPropertyObserver* self =
        static_cast<MT_AiPSPropertyObserver*>( aPtr );
    TInt value = 0;
    
    self->iPSObserver->Get(value);
	
    if ( value == 10 )
		{
		self->iStateOk = ETrue;
		}
	
    return KErrNone;
    }

void MT_AiPSPropertyObserver::SetupL(  )
    {
    iPSObserver = AiUtility::CreatePSPropertyObserverL(
                TCallBack( HandleSystemStateChangedL, this ),
                TUid::Uid( KRandTestUid ),
                KRandTestKey );
    }

void MT_AiPSPropertyObserver::Teardown(  )
    {
    Release( iPSObserver );
    }

void MT_AiPSPropertyObserver::TestBasicFunctionalityL(  )
    {
    MAiPSPropertyObserver* obs = NULL;
    // Test that Release(NULL) does not crash
    Release(obs);
    
    // test
    ChangeKeyValueL( 10 );
    CAOUserAfter::AfterLD( 2 * 1000000 );
    if( iStateOk )
        {
        EUNIT_ASSERT( ETrue );
        return;
        }
    EUNIT_ASSERT( EFalse );
    }

void MT_AiPSPropertyObserver::TestGettersL(  )
    {
    TInt err = KErrNone;
    TBuf16<100> buf16;
    TBuf8<100> buf8;
    TInt value = 0;
    err = iPSObserver->Get(buf16);
    
    EUNIT_ASSERT( err != KErrNone );
   
    err = KErrNone;
    err = iPSObserver->Get(buf8);

    EUNIT_ASSERT( err != KErrNone );

    err = iPSObserver->Get(value);

    EUNIT_ASSERT( err == KErrNone );
    EUNIT_ASSERT( value == 5 || value == 10 );
    }

void MT_AiPSPropertyObserver::TestDoCancelL(  )
    {
    iStateOk = EFalse;
    ChangeKeyValueL( 10 );
    Release(iPSObserver);
    iPSObserver = NULL;
    CAOUserAfter::AfterLD( 2 * 1000000 );
    if( !iStateOk ) // make sure that the change was cancelled
        {
        EUNIT_ASSERT( ETrue );
        return;
        }
    EUNIT_ASSERT( EFalse );
    }

//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    MT_AiPSPropertyObserver,
    "Unit test suite for MAiContentItemIterator implementation returned from AiUtility::CreateContentItemArrayIteratorL",
    "CAiContentItemArrayIterator" )

EUNIT_TEST(
    "Test one",
    "MAiContentItemIterator",
    "All",
    "FUNCTIONALITY",
    SetupL, TestBasicFunctionalityL, Teardown)

EUNIT_TEST(
    "Test one",
    "MAiContentItemIterator",
    "All",
    "FUNCTIONALITY",
    SetupL, TestGettersL, Teardown)

EUNIT_TEST(
    "Test one",
    "MAiContentItemIterator",
    "All",
    "FUNCTIONALITY",
    SetupL, TestDoCancelL, Teardown)

EUNIT_END_TEST_TABLE

//  END OF FILE
