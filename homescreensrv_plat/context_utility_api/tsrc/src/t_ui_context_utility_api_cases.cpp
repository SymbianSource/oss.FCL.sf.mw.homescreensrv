/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  T_ui_context_utility_api test module.
*
*/


// DEFINITION OF GLOBAL VARIABLES
#ifdef TEST_VAR_DECLARATIONS
// here You can define global objects used all over code 
CHgContextUtility* iContextUtility;
#endif

// testing code starts here
#ifdef TEST_CASES

#ifndef KNAME
#define KNAME
_LIT(KName,"James Bond");
#endif

// STIF_SETUP defines activities needed before every test case.
STIF_SETUP
    {
    iContextUtility = CHgContextUtility::NewL();
    return KErrNone;
    }

// STIF_TEARDOWN defines activities needed after every test case
STIF_TEARDOWN
    {
    delete iContextUtility;
    return KErrNone;
    }

// a Canary test to verify test environment.
STIF_TESTDEFINE( CanaryTest )
{

}

STIF_TESTDEFINE( T_MemoryOnlyTest )
{
    /*private implementation IsForeground 
    cant be covered by tests because covered by previous condition*/
    iContextUtility->AllowPublishFromBackground( ETrue );
    iContextUtility->AllowPublishFromBackground( EFalse );
    return KErrNone;
}
//Test code for testing PublishContextL function call
STIF_TESTDEFINE( T_PublishContextL )
    {
    // publish the Hg.Contact context to CF
    iContextUtility->PublishContextL( KHgCFTypeContact, KName );
    //private implementation with dalay cant be covered by test because its never called
    return KErrNone;
    }

//Test code for testing T_PublishContextDelayedL function call
STIF_TESTDEFINE( T_PublishContextDelayedL )
    {
    // publish the Hg.Contact context to CF
    iContextUtility->PublishContextDelayedL( KHgCFTypeContact, KName ,50000);
    iContextUtility->PublishContextDelayedL( KHgCFTypeContact, KName ,0);
    return KErrNone;
    }

//Test code for testing GetContextL function call
STIF_TESTDEFINE( T_GetContextL )
    {
    HBufC* icontext = iContextUtility->GetContextL( _L( "NonExistContextType" ));
    
    STIF_ASSERT_TRUE_DESC( icontext == NULL, _L( "Item found" ) );
    delete icontext;
    
    // publish the Hg.Contact context to CF
    iContextUtility->PublishContextL( KHgCFTypeContact, KName );

    icontext = iContextUtility->GetContextL( KHgCFTypeContact);

    STIF_ASSERT_TRUE_DESC( icontext != NULL, _L( "Context is not found" ) );
    delete icontext;
    
    return KErrNone;
    }

//Test code for testing GetContextL function call
STIF_TESTDEFINE( T_GetContextLWithContextSource )
    {
    HBufC* icontext = iContextUtility->GetContextL(KHgCFSource,_L( "NonExistContextType" ));
    
    STIF_ASSERT_TRUE_DESC( icontext == NULL, _L( "Item found" ) );
    delete icontext;
    
    // publish the Hg.Contact context to CF
    iContextUtility->PublishContextL( KHgCFTypeContact, KName );

    icontext = iContextUtility->GetContextL(KHgCFSource,KHgCFTypeContact);

    STIF_ASSERT_TRUE_DESC( icontext != NULL, _L( "Context is not found" ) );
    delete icontext;

    return KErrNone;
    }

//Test code for testing spliting combined context
STIF_TESTDEFINE( T_SplitCombinedStringL )
    {
    HBufC* val( NULL );
    _LIT( KDummyContext, "CtxUtilApiTest" );
    const TInt KArrayGranularity( 3 );
    CDesCArray* sourceArray = new( ELeave ) CDesC16ArrayFlat(KArrayGranularity);
    CDesCArray* resultArray = new( ELeave ) CDesC16ArrayFlat(KArrayGranularity);
    CleanupStack::PushL( sourceArray );
    CleanupStack::PushL( resultArray );
    
    iContextUtility->PublishContextL( KDummyContext, *sourceArray );
    //Can't be verified if current context is NULL, because could be already published
    
    sourceArray->AppendL( _L( "https://somedomain.com?param=parmaValue" ) );
    iContextUtility->PublishContextL( KDummyContext, *sourceArray );//there only one value. nothing to combine but returned value should be equal
    val = iContextUtility->GetContextL( KDummyContext );
    CleanupStack::PushL( val );
    STIF_ASSERT_TRUE( val && val->Length() );
    CHgContextUtility::SplitCombinedStringL( *val, *resultArray );
    CleanupStack::PopAndDestroy(val);
    STIF_ASSERT_TRUE( resultArray->Count() == sourceArray->Count() );
    for( TInt iter( 0 ); iter < sourceArray->Count(); ++iter )
        {
        STIF_ASSERT_TRUE( 0 == (*sourceArray)[iter].Compare((*resultArray)[iter]) );
        }
    CleanupStack::PopAndDestroy(resultArray);
    
    resultArray = new( ELeave ) CDesC16ArrayFlat(KArrayGranularity);
    CleanupStack::PushL( resultArray );
    sourceArray->AppendL( _L( "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\" ));
    sourceArray->AppendL( _L( "!@#$%^&&*()/\<>[]|~,." ) );
    
    iContextUtility->PublishContextL( KDummyContext, *sourceArray );
    
    val = iContextUtility->GetContextL( KDummyContext );
    STIF_ASSERT_TRUE( val && val->Length() );
    CleanupStack::PushL(val);
    CHgContextUtility::SplitCombinedStringL( *val, *resultArray );
    CleanupStack::PopAndDestroy( val );
    STIF_ASSERT_TRUE( resultArray->Count() == sourceArray->Count() );
    for( TInt iter( 0 ); iter < sourceArray->Count(); ++iter )
        {
        STIF_ASSERT_TRUE( 0 == (*sourceArray)[iter].Compare((*resultArray)[iter]) );
        }
    CleanupStack::PopAndDestroy(resultArray);
    CleanupStack::PopAndDestroy(sourceArray);
    return KErrNone;
    }

//Test code for testing PublishContextL with array argument
STIF_TESTDEFINE( T_PublishContextLWithArray )
    {
    _LIT( KDummyContext, "CtxUtilApiTest" );
    CDesCArray* arr = new ( ELeave ) CDesC16ArrayFlat( 4 );
    CleanupStack::PushL( arr );
    
    iContextUtility->PublishContextL( KDummyContext, *arr );
    
    arr->AppendL( _L( "first item" ) );
    arr->AppendL( _L( "second item" ) );
    arr->AppendL( _L( "third item" ) );
    iContextUtility->PublishContextL( KDummyContext, *arr );
    HBufC* val = iContextUtility->GetContextL( KDummyContext );
    STIF_ASSERT_TRUE( val && val->Length() );
    delete val;
    CleanupStack::PopAndDestroy( arr );
    return KErrNone;
    }

//Test code for testing PublishContextDelayedL with array argument
STIF_TESTDEFINE( T_PublishContextDelayedLWithArray )
    {
    _LIT( KDummyContext, "CtxUtilApiTest2" );
    CDesCArray* arr = new ( ELeave ) CDesC16ArrayFlat( 4 );
    CleanupStack::PushL( arr );
    arr->AppendL( _L( "first item" ) );
    arr->AppendL( _L( "second item" ) );
    arr->AppendL( _L( "third item" ) );
    iContextUtility->PublishContextDelayedL( KDummyContext, *arr, 500000 );
    iContextUtility->PublishContextDelayedL( KDummyContext, *arr, 0 );
    iContextUtility->PublishContextDelayedL( KDummyContext, *arr, 500000 );
    CWait* wait = CWait::NewL();
    wait->Start( 2000000 );
    delete wait;
    HBufC* val = iContextUtility->GetContextL( KDummyContext );
    STIF_ASSERT_TRUE( val && val->Length() );
    delete val;
    CleanupStack::PopAndDestroy( arr );
    return KErrNone;
    }

//Test code for testing RePublishWhenFgL with different arguments
//Most probably nothing will happen because there is no CCoeEnv but
//it should never crash.
STIF_TESTDEFINE( T_RePublishWhenFgL )
    {
    iContextUtility->RePublishWhenFgL( ETrue );
    iContextUtility->RePublishWhenFgL( EFalse );
    iContextUtility->RePublishWhenFgL( ETrue );
    return KErrNone;
    }

/*    IMPORT_C void PublishContactContextL( const MVPbkStoreContact& aContact,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );*/
STIF_TESTDEFINE( T_PublishContactContextL1 )
    {
    CHgCtxContactMatcher* m = CHgCtxContactMatcher::NewLC();
    m->OpenDefaultMatchStoresL();
    CVPbkContactManager& cm( m->GetContactManager() );
    MVPbkContactStoreList& sl( m->GetContactStoresL() );
	MVPbkStoreContact* newContact = sl.At( 0 ).CreateNewContactLC();
	const MVPbkFieldType* fieldType = m->FieldTypes().Find(R_VPBK_FIELD_TYPE_FIRSTNAME);
	MVPbkStoreContactField* newField = newContact->CreateFieldLC(*fieldType);
	MVPbkContactFieldTextData* textData = &(MVPbkContactFieldTextData::Cast(newField->FieldData()));
	_LIT(KName, "ctxutiltester");
	textData->SetTextL( KName );
	newContact->AddFieldL(newField);
	CleanupStack::Pop(newField);
	CWait* wait = CWait::NewLC();
    struct TDummyObs : public MVPbkContactObserver
        {
        CWait* iWait;
        TDummyObs( CWait* aWait ) : iWait( aWait ) { }
        void ContactOperationCompleted(TContactOpResult /*aResult*/) 
            { 
            iWait->Stop(); 
            }
        void ContactOperationFailed(TContactOp /*aOpCode*/, 
                TInt /*aErrorCode*/, 
                TBool /*aErrorNotified*/) { }
        } dummyObs( wait );
	newContact->CommitL( dummyObs );
    wait->Start( 2000000 );
    CleanupStack::PopAndDestroy( wait );

    iContextUtility->PublishContactContextL( *newContact, 0 );
    iContextUtility->PublishContactContextL( *newContact, 500000 );
    wait = CWait::NewL();
    wait->Start( 2000000 );
    delete wait;
    HBufC* ctx = iContextUtility->GetContextL( KHgCFTypeContact);
    STIF_ASSERT_TRUE_DESC( ctx != NULL, _L( "Context not found" ) );
    delete ctx;

    CVPbkContactLinkArray* ca = CVPbkContactLinkArray::NewLC();
    ca->AppendL( newContact->CreateLinkLC() );
    CleanupStack::Pop();
    wait = CWait::NewL();
    struct TDummyObs2 : public MVPbkBatchOperationObserver
        {
        CWait* iWait;
        TDummyObs2( CWait* aWait ) : iWait( aWait ) { }
        void StepComplete( 
                MVPbkContactOperationBase& /*aOperation*/,
                TInt /*aStepSize*/ ) { }
        TBool StepFailed(
                MVPbkContactOperationBase& /*aOperation*/,
                TInt /*aStepSize*/, TInt /*aError*/ ) 
            {
            return EFalse;
            }
        void OperationComplete( 
                MVPbkContactOperationBase& /*aOperation*/ ) 
            { 
            iWait->Stop(); 
            }
        } dummyObs2( wait );
    MVPbkContactOperationBase* op = cm.DeleteContactsL( *ca, dummyObs2 );
    CleanupDeletePushL( op );
    wait->Start( 2000000 );
    delete wait;
    CleanupStack::PopAndDestroy( 4 ); // op, ca, newContact, m
    
    return KErrNone;
    }

/*    IMPORT_C void PublishContactContextL( const MVPbkContactLink& aContactLink,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );*/
STIF_TESTDEFINE( T_PublishContactContextL2 )
    {
    CHgCtxContactMatcher* m = CHgCtxContactMatcher::NewLC();
    m->OpenDefaultMatchStoresL();
    CVPbkContactManager& cm( m->GetContactManager() );
    MVPbkContactStoreList& sl( m->GetContactStoresL() );
	MVPbkStoreContact* newContact = sl.At( 0 ).CreateNewContactLC();
	const MVPbkFieldType* fieldType = m->FieldTypes().Find(R_VPBK_FIELD_TYPE_FIRSTNAME);
	MVPbkStoreContactField* newField = newContact->CreateFieldLC(*fieldType);
	MVPbkContactFieldTextData* textData = &(MVPbkContactFieldTextData::Cast(newField->FieldData()));
	_LIT(KName, "ctxutiltester");
	textData->SetTextL( KName );
	newContact->AddFieldL(newField);
	CleanupStack::Pop(newField);
	CWait* wait = CWait::NewL();
    struct TDummyObs : public MVPbkContactObserver
        {
        CWait* iWait;
        TDummyObs( CWait* aWait ) : iWait( aWait ) { }
        void ContactOperationCompleted(TContactOpResult /*aResult*/) 
            { 
            iWait->Stop(); 
            }
        void ContactOperationFailed(TContactOp /*aOpCode*/, 
                TInt /*aErrorCode*/, 
                TBool /*aErrorNotified*/){ }
        } dummyObs( wait );
	newContact->CommitL( dummyObs );
    wait->Start( 2000000 );
    delete wait;

    CVPbkContactLinkArray* ca = CVPbkContactLinkArray::NewLC();
    ca->AppendL( newContact->CreateLinkLC() );
    CleanupStack::Pop();

    iContextUtility->PublishContactContextL( ca->At( 0 ), 0 );
    iContextUtility->PublishContactContextL( ca->At( 0 ), 500000 );
    wait = CWait::NewL();
    wait->Start( 2000000 );
    delete wait;
    HBufC* ctx = iContextUtility->GetContextL( KHgCFTypeContact);
    STIF_ASSERT_TRUE_DESC( ctx != NULL, _L( "Context not found" ) );
    delete ctx;

    wait = CWait::NewL();
    struct TDummyObs2 : public MVPbkBatchOperationObserver
        {
        CWait* iWait;
        TDummyObs2( CWait* aWait ) : iWait( aWait ) { }
        void StepComplete( 
                MVPbkContactOperationBase& /*aOperation*/,
                TInt /*aStepSize*/ ) { }
        TBool StepFailed(
                MVPbkContactOperationBase& /*aOperation*/,
                TInt /*aStepSize*/, TInt /*aError*/ ) 
            { 
            return EFalse;
            }
        void OperationComplete( 
                MVPbkContactOperationBase& /*aOperation*/ ) { iWait->Stop(); }
        } dummyObs2( wait );
    MVPbkContactOperationBase* op = cm.DeleteContactsL( *ca, dummyObs2 );
    CleanupDeletePushL( op );
    wait->Start( 2000000 );
    delete wait;
    
    // test with (surely) non-existing link
    iContextUtility->PublishContactContextL( ca->At( 0 ) );
    wait = CWait::NewL();
    wait->Start( 2000000 );
    delete wait;
    
    CleanupStack::PopAndDestroy( 4 ); // op, ca, newContact, m
    
    return KErrNone;
    }
    
/*    IMPORT_C void PublishContactContextL( const TDesC& aContactName,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );*/
STIF_TESTDEFINE( T_PublishContactContextL3 )
    {
    iContextUtility->PublishContactContextL( KName );
    HBufC* ctx = iContextUtility->GetContextL( KHgCFTypeContact);
    STIF_ASSERT_TRUE_DESC( ctx != NULL, _L( "Context not found" ) );
    delete ctx;

    iContextUtility->PublishContactContextL( KName, 500000 );
    iContextUtility->PublishContactContextL( KName, 500000 );
    iContextUtility->PublishContactContextL( KName, 500000 );
    ctx = iContextUtility->GetContextL( KHgCFTypeContact);
    STIF_ASSERT_TRUE_DESC( ctx != NULL, _L( "Context not found" ) );
    delete ctx;
    
    return KErrNone;
    }

/*    IMPORT_C void PublishContactContextL(
        const RPointerArray<MVPbkStoreContact>& aContacts,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );*/
STIF_TESTDEFINE( T_PublishContactContextL4 )
    {
    CHgCtxContactMatcher* m = CHgCtxContactMatcher::NewLC();
    m->OpenDefaultMatchStoresL();
    CVPbkContactManager& cm( m->GetContactManager() );
    MVPbkContactStoreList& sl( m->GetContactStoresL() );
	MVPbkStoreContact* newContact = sl.At( 0 ).CreateNewContactLC();
	const MVPbkFieldType* fieldType = m->FieldTypes().Find(R_VPBK_FIELD_TYPE_FIRSTNAME);
	MVPbkStoreContactField* newField = newContact->CreateFieldLC(*fieldType);
	MVPbkContactFieldTextData* textData = &(MVPbkContactFieldTextData::Cast(newField->FieldData()));
	_LIT(KName, "ctxutiltester");
	textData->SetTextL( KName );
	newContact->AddFieldL(newField);
	CleanupStack::Pop(newField);
    CWait* wait = CWait::NewL();	
    struct TDummyObs : public MVPbkContactObserver
        {
        CWait* iWait;
        TDummyObs( CWait* aWait ) : iWait( aWait ) { }
        void ContactOperationCompleted(TContactOpResult /*aResult*/) { iWait->Stop(); }
        void ContactOperationFailed(TContactOp /*aOpCode*/, TInt /*aErrorCode*/, TBool /*aErrorNotified*/) { }
        } dummyObs( wait );
	newContact->CommitL( dummyObs );
    wait->Start( 2000000 );
    delete wait;

    RPointerArray<MVPbkStoreContact> pa;
    CleanupClosePushL( pa );
    pa.AppendL( newContact );
    iContextUtility->PublishContactContextL( pa, 0 );
    iContextUtility->PublishContactContextL( pa, 500000 );
    CleanupStack::PopAndDestroy( &pa );
    wait = CWait::NewL();
    wait->Start( 2000000 );
    delete wait;
    HBufC* ctx = iContextUtility->GetContextL( KHgCFTypeContact);
    STIF_ASSERT_TRUE_DESC( ctx != NULL, _L( "Context not found" ) );
    delete ctx;

    RPointerArray<MVPbkStoreContact> pa2;
    CleanupClosePushL( pa2 );
    pa2.AppendL( newContact );
    pa2.AppendL( newContact );
    pa2.AppendL( newContact );
    pa2.AppendL( newContact );
    iContextUtility->PublishContactContextL( pa2, 500000 );
    iContextUtility->PublishContactContextL( pa2, 0 );    
    CleanupStack::PopAndDestroy( &pa2 );
    wait = CWait::NewL();
    wait->Start( 2000000 );
    delete wait;
    ctx = iContextUtility->GetContextL( KHgCFTypeContact);
    STIF_ASSERT_TRUE_DESC( ctx != NULL, _L( "Context not found" ) );
    delete ctx;

    CVPbkContactLinkArray* ca = CVPbkContactLinkArray::NewLC();
    ca->AppendL( newContact->CreateLinkLC() );
    CleanupStack::Pop();
    wait = CWait::NewL();    
    struct TDummyObs2 : public MVPbkBatchOperationObserver
        {
        CWait* iWait;
        TDummyObs2( CWait* aWait ) : iWait( aWait ) { }
        void StepComplete( 
                MVPbkContactOperationBase& /*aOperation*/,
                TInt /*aStepSize*/ ) { }
        TBool StepFailed(
                MVPbkContactOperationBase& /*aOperation*/,
                TInt /*aStepSize*/, TInt /*aError*/ ) 
            {
            return EFalse;    
            }
        void OperationComplete( 
                MVPbkContactOperationBase& /*aOperation*/ ) { iWait->Stop(); }
        } dummyObs2( wait );
    MVPbkContactOperationBase* op = cm.DeleteContactsL( *ca, dummyObs2 );
    CleanupDeletePushL( op );
    wait->Start( 2000000 );
    delete wait;
    CleanupStack::PopAndDestroy( 4 ); // op, ca, newContact, m
    
    return KErrNone;
    }

/*    IMPORT_C void PublishContactContextL(
        const RPointerArray<MVPbkContactLink>& aContactLinks,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );*/
STIF_TESTDEFINE( T_PublishContactContextL5 )
    {
    CHgCtxContactMatcher* m = CHgCtxContactMatcher::NewLC();
    m->OpenDefaultMatchStoresL();
    CVPbkContactManager& cm( m->GetContactManager() );
    MVPbkContactStoreList& sl( m->GetContactStoresL() );
	MVPbkStoreContact* newContact = sl.At( 0 ).CreateNewContactLC();
	const MVPbkFieldType* fieldType = 
        m->FieldTypes().Find(R_VPBK_FIELD_TYPE_FIRSTNAME);
	MVPbkStoreContactField* newField = newContact->CreateFieldLC(*fieldType);
	MVPbkContactFieldTextData* textData = 
        &(MVPbkContactFieldTextData::Cast(newField->FieldData()));
	_LIT(KName, "ctxutiltester");
	textData->SetTextL( KName );
	newContact->AddFieldL(newField);
	CleanupStack::Pop(newField);
    CWait* wait = CWait::NewL();	
    struct TDummyObs : public MVPbkContactObserver
        {
        CWait* iWait;
        TDummyObs( CWait* aWait ) : iWait( aWait ) { }
        void ContactOperationCompleted(TContactOpResult /*aResult*/) 
            { 
            iWait->Stop(); 
            }
        void ContactOperationFailed(TContactOp /*aOpCode*/, 
                TInt /*aErrorCode*/, 
                TBool /*aErrorNotified*/) { }
        } dummyObs( wait );
	newContact->CommitL( dummyObs );
    wait->Start( 2000000 );
    delete wait;

    CVPbkContactLinkArray* pa = CVPbkContactLinkArray::NewLC();
    pa->AppendL( newContact->CreateLinkLC() );
    CleanupStack::Pop();
    iContextUtility->PublishContactContextL( *pa, 0 );
    iContextUtility->PublishContactContextL( *pa, 500000 );
    CleanupStack::PopAndDestroy( pa );
    wait = CWait::NewL();
    wait->Start( 2000000 );
    delete wait;
    HBufC* ctx = iContextUtility->GetContextL( KHgCFTypeContact);
    STIF_ASSERT_TRUE_DESC( ctx != NULL, _L( "Context not found" ) );
    delete ctx;

    CVPbkContactLinkArray* pa2 = CVPbkContactLinkArray::NewLC();
    pa2->AppendL( newContact->CreateLinkLC() );
    CleanupStack::Pop();
    pa2->AppendL( newContact->CreateLinkLC() );
    CleanupStack::Pop();
    pa2->AppendL( newContact->CreateLinkLC() );
    CleanupStack::Pop();
    iContextUtility->PublishContactContextL( *pa2, 500000 );
    iContextUtility->PublishContactContextL( *pa2, 0 );    
    CleanupStack::PopAndDestroy( pa2 );
    wait = CWait::NewL();
    wait->Start( 2000000 );
    delete wait;
    ctx = iContextUtility->GetContextL( KHgCFTypeContact);
    STIF_ASSERT_TRUE_DESC( ctx != NULL, _L( "Context not found" ) );
    delete ctx;

    CVPbkContactLinkArray* ca = CVPbkContactLinkArray::NewLC();
    ca->AppendL( newContact->CreateLinkLC() );
    CleanupStack::Pop();
    wait = CWait::NewL();    
    struct TDummyObs2 : public MVPbkBatchOperationObserver
        {
        CWait* iWait;
        TDummyObs2( CWait* aWait ) : iWait( aWait ) { }
        void StepComplete( 
                MVPbkContactOperationBase& /*aOperation*/,
                TInt /*aStepSize*/ ) { }
        TBool StepFailed(
                MVPbkContactOperationBase& /*aOperation*/,
                TInt /*aStepSize*/, TInt /*aError*/ ) 
            { 
            return EFalse; 
            }
        void OperationComplete( 
                MVPbkContactOperationBase& /*aOperation*/ ) { iWait->Stop(); }
        } dummyObs2( wait );
    MVPbkContactOperationBase* op = cm.DeleteContactsL( *ca, dummyObs2 );
    CleanupDeletePushL( op );
    wait->Start( 2000000 );
    delete wait;
    CleanupStack::PopAndDestroy( 4 ); // op, ca, newContact, m
    
    return KErrNone;
    }
    
/*IMPORT_C void PublishContactContextL( const MDesCArray& aContactNames,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );*/
STIF_TESTDEFINE( T_PublishContactContextL6 )
    {
    CDesCArray* arr = new ( ELeave ) CDesC16ArrayFlat( 4 );
    CleanupStack::PushL( arr );
    arr->AppendL( _L( "first item" ) );
    arr->AppendL( _L( "second item" ) );
    arr->AppendL( _L( "third item" ) );
    iContextUtility->PublishContactContextL( *arr );
    iContextUtility->PublishContactContextL( *arr, 500000 );
    iContextUtility->PublishContactContextL( *arr );
    CleanupStack::PopAndDestroy( arr );
    return KErrNone;
    }

/*    IMPORT_C void PublishTextContextL( const TDesC& aText,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );*/
STIF_TESTDEFINE( T_PublishTextContextL )
    {
    iContextUtility->PublishTextContextL( KNullDesC, 100 );
    iContextUtility->PublishTextContextL( _L("fbngiuwetghorb rteogvhetui gherigth" ) );
    iContextUtility->PublishTextContextL( KNullDesC );
    iContextUtility->PublishTextContextL( _L("+35442754") );
    iContextUtility->PublishTextContextL( _L("35442754") );
    iContextUtility->PublishTextContextL( _L("abcdef@ghijk.com") );
    iContextUtility->PublishTextContextL( _L(" " ) );
    return KErrNone;
    }
            
/*    IMPORT_C void PublishUrlContextL( const TDesC& aUrl,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );*/
STIF_TESTDEFINE( T_PublishUrlContextL )
    {
    _LIT( KNokiaDomain, "http://www.nokia.com" );
    _LIT( KSomeDomain, "http://www.somedomain.com" );
    iContextUtility->PublishUrlContextL( KNokiaDomain, 1000 );
    iContextUtility->PublishUrlContextL( KSomeDomain );
    iContextUtility->PublishUrlContextL( KNokiaDomain );
    HBufC* ctx = iContextUtility->GetContextL( KHgCFTypeUrl );
    STIF_ASSERT_TRUE_DESC( ctx != NULL, _L( "Context not found" ) );
    STIF_ASSERT_TRUE( 0 == ctx->Compare( KNokiaDomain ) );
    delete ctx;
    return KErrNone;
    }
        
/*    IMPORT_C void PublishTimeContextL( const TTime& aTime,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );*/
STIF_TESTDEFINE( T_PublishTimeContextL )
    {
    TTime t( 0 );
    iContextUtility->PublishTimeContextL( t, 100 );
    iContextUtility->PublishTimeContextL( t );
    HBufC* ctx = iContextUtility->GetContextL( KHgCFTypeActiveDate );
    STIF_ASSERT_TRUE_DESC( ctx != NULL, _L( "Context not found" ) );
    delete ctx;
    return KErrNone;
    }
        
/*    IMPORT_C void PublishPhotoContextL( const TDesC& aFilename,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );*/
STIF_TESTDEFINE( T_PublishPhotoContextL )
    {
    _LIT( KJpegPath, "c:\\image.jpg" );
    _LIT( KPngPath, "c:\\image.png" );
    iContextUtility->PublishPhotoContextL( KJpegPath, 100 );
    iContextUtility->PublishPhotoContextL( KPngPath, 0 );
    iContextUtility->PublishPhotoContextL( KJpegPath );
    HBufC* ctx = iContextUtility->GetContextL( KHgCFTypePhoto );
    STIF_ASSERT_TRUE_DESC( ctx != NULL, _L( "Context not found" ) );
    STIF_ASSERT_TRUE( 0 == ctx->Compare(KJpegPath) );
    delete ctx;
    
    iContextUtility->PublishPhotoContextL( KPngPath, 100000 );
    ctx = iContextUtility->GetContextL( KHgCFTypePhoto );
    STIF_ASSERT_TRUE_DESC( ctx != NULL, _L( "Context not found" ) );
    STIF_ASSERT_TRUE( 0 == ctx->Compare(KJpegPath) );
    delete ctx;
    return KErrNone;
    }
        
/*    IMPORT_C void PublishPhotoContextL( TItemId aMdeItemId,
        CMdESession& aMdeSession,
        const TTimeIntervalMicroSeconds32& aDelay = 0 );*/
STIF_TESTDEFINE( T_PublishPhotoContextL2 )
    {
    CWait* wait = CWait::NewLC();
    struct TObs : public MMdESessionObserver {
        CWait* iWait;
        TObs( CWait* aWait ) : iWait( aWait ) { }
        void HandleSessionOpened( CMdESession& /*aSession*/, TInt /*aError*/ ) { iWait->Stop(); }
        void HandleSessionError( CMdESession& /*aSession*/, TInt /*aError*/ ) { }
        } mdeobs( wait );
    CMdESession* s = CMdESession::NewLC( mdeobs );
    wait->Start( 5000000 );
    // invalid item id
    TRAPD( err, iContextUtility->PublishPhotoContextL( 0, *s ) );
    STIF_ASSERT_TRUE( err != KErrNone ); // leave should have occured
    for ( TInt i = 0; i < 100; ++i )
        {
        TRAP_IGNORE( iContextUtility->PublishPhotoContextL( TItemId( i ), *s ) );
        }
    CleanupStack::PopAndDestroy( 2 );
    return KErrNone;
    }

/*    IMPORT_C void PublishTvContextL( const TDesC& aChannelName,
        const TDesC& aProgramName, const TDesC& aProgramDescription,
        const TDesC& aGenre ); */
STIF_TESTDEFINE( T_PublishTvContextL )
    {
    _LIT( K1, "channel A" );
    _LIT( K2, "program X" );
    _LIT( K3, "very interesting program" );
    iContextUtility->PublishTvContextL( K1, K2, K3, KNullDesC );
    iContextUtility->PublishTvContextL( K1, K2, K3, KNullDesC );
    iContextUtility->PublishTvContextL( K1, K2, KNullDesC, KNullDesC );
    HBufC* ctx = iContextUtility->GetContextL( KHgCFTypeTvProgramName );
    STIF_ASSERT_TRUE_DESC( ctx != NULL && !ctx->Compare( K2 ),
        _L( "Context not found" ) );
    delete ctx;

    iContextUtility->PublishTvContextL( KNullDesC, K2, KNullDesC, KNullDesC );
    iContextUtility->PublishTvContextL( KNullDesC, KNullDesC, KNullDesC, KNullDesC );
    iContextUtility->PublishTvContextL( KNullDesC, K2, KNullDesC, KNullDesC );
    iContextUtility->PublishTvContextL( KNullDesC, KNullDesC, KNullDesC, _L("movie") );
    ctx = iContextUtility->GetContextL( KHgCFTypeTvChannelName );
    STIF_ASSERT_TRUE_DESC( ctx != NULL && !ctx->Compare( KHgCFValueUnknownInfo ),
        _L( "Context not found" ) );
    delete ctx;
    return KErrNone;
    }

/*    IMPORT_C void PublishServiceIdL( const TDesC& aServiceId,
        const TDesC& aAccountId,
        const TTimeIntervalMicroSeconds32& aDelay = 0 ); */
STIF_TESTDEFINE( T_PublishServiceIdL )
    {
    iContextUtility->PublishServiceIdL( _L("Ovi"), _L("someid") );
    iContextUtility->PublishServiceIdL( _L("Ovi"), _L("someid"), 1000000 );
    iContextUtility->PublishServiceIdL( _L("Ovi"), _L("someid") );
    }

/*  IMPORT_C void AddMusicContextInfoL( 
        const TDesC& aKey, 
        const TDesC& aData ); */
STIF_TESTDEFINE( T_AddMusicContextInfoL )
    {
    // Fetch the pointer to hash table for testing purposes
    RPtrHashMap<TDesC, TDesC>* hash = GetImplHashTablePtr();
    _LIT( KPlaying, "Playing" );
    _LIT( KArtist, "Hans Zimmer");
    _LIT( KTitle, "Why so serious" );
    _LIT( KAlbum, "Dark Knight" );
    _LIT( KGenre, "Soundtrack" );
    
    STIF_ASSERT_TRUE_DESC( hash->Count() == 0, _L("There is stuff in the list!") );
    STIF_ASSERT_TRUE_DESC( hash->Find( KHgCFTypeMusicState ) == NULL,       _L( "State in the list!" ) );
    STIF_ASSERT_TRUE_DESC( hash->Find( KHgCFTypeMusicArtist ) == NULL,      _L( "Artist in the list!" ) );
    STIF_ASSERT_TRUE_DESC( hash->Find( KHgCFTypeMusicTitle ) == NULL,       _L( "Title in the list!" ) );
    STIF_ASSERT_TRUE_DESC( hash->Find( KHgCFTypeMusicAlbum ) == NULL,       _L( "Album in the list!" ) );
    STIF_ASSERT_TRUE_DESC( hash->Find( KHgCFTypeMusicAlbumArt ) == NULL,    _L( "Art in the list!" ) );
    STIF_ASSERT_TRUE_DESC( hash->Find( KHgCFTypeMusicUri ) == NULL,  _L( "Uri in the list!" ) );
    STIF_ASSERT_TRUE_DESC( hash->Find( KHgCFTypeMusicGenre ) == NULL,       _L( "Genre in the list!" ) );
    STIF_ASSERT_TRUE_DESC( hash->Find( KHgCFTypeMusicType ) == NULL,        _L( "Type in the list!" ) );
    
    // No room for empty keys
    TRAPD( err, iContextUtility->AddMusicContextInfoL( KNullDesC,       KPlaying ) );
    STIF_ASSERT_TRUE_DESC( err == KErrNotFound, _L( "Empty key didn't cause leave!" ) );
    STIF_ASSERT_TRUE_DESC( hash->Count() == 0, _L( "There is stuff in the list!" ) );
    
    // Fill all values and test they will be there.
    iContextUtility->AddMusicContextInfoL( KHgCFTypeMusicState,       KPlaying );
    iContextUtility->AddMusicContextInfoL( KHgCFTypeMusicArtist,      KArtist );
    iContextUtility->AddMusicContextInfoL( KHgCFTypeMusicTitle,       KTitle );
    iContextUtility->AddMusicContextInfoL( KHgCFTypeMusicAlbum,       KAlbum );
    iContextUtility->AddMusicContextInfoL( KHgCFTypeMusicAlbumArt,    KHgCFValueUnknownInfo );
    iContextUtility->AddMusicContextInfoL( KHgCFTypeMusicUri,  KNullDesC );
    iContextUtility->AddMusicContextInfoL( KHgCFTypeMusicGenre,       KGenre );
    iContextUtility->AddMusicContextInfoL( KHgCFTypeMusicType,        KHgCFValueMusicTypePlayer );
    
    STIF_ASSERT_TRUE_DESC( hash->Count() == 8, _L( "List should contain 8 items." ) );
    STIF_ASSERT_TRUE_DESC( !hash->Find( KHgCFTypeMusicState )->CompareC( KPlaying ), 
            _L( "Wrong state in the table!" ) );
    STIF_ASSERT_TRUE_DESC( !hash->Find( KHgCFTypeMusicArtist )->CompareC( KArtist ), 
            _L( "Wrong artist in the table!" ) );
    STIF_ASSERT_TRUE_DESC( !hash->Find( KHgCFTypeMusicTitle )->CompareC( KTitle ), 
            _L( "Wrong title in the table!" ) );
    STIF_ASSERT_TRUE_DESC( !hash->Find( KHgCFTypeMusicAlbum )->CompareC( KAlbum ), 
            _L( "Wrong album in the table!" ) );
    STIF_ASSERT_TRUE_DESC( !hash->Find( KHgCFTypeMusicAlbumArt )->CompareC( KHgCFValueUnknownInfo ), 
            _L( "Wrong art in the table!" ) );
    STIF_ASSERT_TRUE_DESC( !hash->Find( KHgCFTypeMusicUri )->CompareC( KHgCFValueUnknownInfo ), 
            _L( "Wrong uri in the table!" ) );
    STIF_ASSERT_TRUE_DESC( !hash->Find( KHgCFTypeMusicGenre )->CompareC( KGenre ), 
            _L( "Wrong genre in the table!" ) );
    STIF_ASSERT_TRUE_DESC( !hash->Find( KHgCFTypeMusicType )->CompareC( KHgCFValueMusicTypePlayer ), 
            _L( "Wrong type in the table!" ) );
    
    // An attempt to add same key twice, causes problems.
    TRAP( err, iContextUtility->AddMusicContextInfoL( KHgCFTypeMusicState,       KPlaying ) );
    STIF_ASSERT_TRUE_DESC( err == KErrAlreadyExists, _L( "Adding same key should cause a leave!" ) );
    STIF_ASSERT_TRUE_DESC( hash->Count() == 8, _L( "List should contain 8 items!" ) );
    
    return KErrNone;
    }

/** IMPORT_C void PublishMusicContextL( 
        const TTimeIntervalMicroSeconds32& aDelay = 0 ); */
STIF_TESTDEFINE( T_PublishMusicContextL )
    {
    RPtrHashMap<TDesC, TDesC>* hash = GetImplHashTablePtr();
    _LIT( KPlaying, "Playing" );
    _LIT( KArtist, "John Williams");
    _LIT( KTitle, "Raiders March" );
    _LIT( KAlbum, "Raiders of the Lost Ark" );

    STIF_ASSERT_TRUE_DESC( hash->Count() == 0, _L( "There is stuff in the list!" ) );
    TRAPD( err, iContextUtility->PublishMusicContextL() );
    STIF_ASSERT_TRUE_DESC( err == KErrNotReady, _L( "No point to publish anything, when list is empty!" ) );
    
    iContextUtility->AddMusicContextInfoL( KHgCFTypeMusicState,       KPlaying );
    iContextUtility->AddMusicContextInfoL( KHgCFTypeMusicArtist,      KArtist );
    iContextUtility->AddMusicContextInfoL( KHgCFTypeMusicTitle,       KTitle );
    iContextUtility->AddMusicContextInfoL( KHgCFTypeMusicAlbum,       KAlbum );
    iContextUtility->AddMusicContextInfoL( KHgCFTypeMusicAlbumArt,    KHgCFValueUnknownInfo );

    iContextUtility->PublishMusicContextL();
    STIF_ASSERT_TRUE_DESC( hash->Count() == 0, _L( "There is stuff in the list!" ) );
    
    HBufC* musicState = iContextUtility->GetContextL( KHgCFTypeMusicState );
    HBufC* musicArtist = iContextUtility->GetContextL( KHgCFTypeMusicArtist );
    HBufC* musicTitle = iContextUtility->GetContextL( KHgCFTypeMusicTitle );
    HBufC* musicAlbum = iContextUtility->GetContextL( KHgCFTypeMusicAlbum );
    HBufC* musicAlbumArt = iContextUtility->GetContextL( KHgCFTypeMusicAlbumArt );
    HBufC* musicGeneralUri = iContextUtility->GetContextL( KHgCFTypeMusicUri );
    HBufC* musicGenre = iContextUtility->GetContextL( KHgCFTypeMusicGenre );
    HBufC* musicType = iContextUtility->GetContextL( KHgCFTypeMusicType );
        
    STIF_ASSERT_TRUE_DESC( musicState != NULL       
            && !musicState->Compare( KPlaying ),    _L( "Music context not found" ) );
    STIF_ASSERT_TRUE_DESC( musicArtist != NULL      
            && !musicArtist->Compare( KArtist ),    _L( "Artist context not found" ) );
    STIF_ASSERT_TRUE_DESC( musicTitle != NULL       
            && !musicTitle->Compare( KTitle ),      _L( "Title context not found" ) );
    STIF_ASSERT_TRUE_DESC( musicAlbum != NULL       
            && !musicAlbum->Compare( KAlbum ),      _L( "Album context not found" ) );
    STIF_ASSERT_TRUE_DESC( musicAlbumArt != NULL   
            && !musicAlbumArt->Compare( KHgCFValueUnknownInfo ), _L( "Art context not found" ) );
    STIF_ASSERT_TRUE_DESC( musicGeneralUri != NULL  
            && !musicGeneralUri->Compare( KHgCFValueUnknownInfo ), _L( "Uri context not found" ) );
    STIF_ASSERT_TRUE_DESC( musicGenre != NULL       
            && !musicGenre->Compare( KHgCFValueUnknownInfo ),   _L( "Genre context not found" ) );
    STIF_ASSERT_TRUE_DESC( musicType != NULL        
            && !musicType->Compare( KHgCFValueUnknownInfo ),    _L( "Type context not found" ) );

    delete musicState;
    delete musicArtist;
    delete musicTitle;
    delete musicAlbum;
    delete musicAlbumArt;
    delete musicGeneralUri;
    delete musicGenre;
    delete musicType;

    return KErrNone;
    }

STIF_TESTDEFINE( T_PublishRadioContextL )
    {
    _LIT( KTestRadioName, "radio test name" );
    _LIT( KTestRadioUrl, "radio test url" );
    _LIT( KTestRadioFrequency, "radio test frequency" );
    _LIT( KTestRadioRDSPI, "radio test rdspi" );
    
    HBufC* ctxRadioName = NULL;
    HBufC* ctxRadioUrl = NULL;
    HBufC* ctxRadioFrequency = NULL;
    HBufC* ctxRadioRDSPI = NULL;
    
    RArray<TPtrC> testData;
    CleanupClosePushL(testData);
        
    // Empty Values
    iContextUtility->PublishRadioContextL(KNullDesC, KNullDesC, KNullDesC, KNullDesC);
    ctxRadioName = iContextUtility->GetContextL(KHgCFTypeMusicRadioName);
    ctxRadioUrl = iContextUtility->GetContextL(KHgCFTypeMusicRadioUrl);
    ctxRadioFrequency = iContextUtility->GetContextL(KHgCFTypeMusicRadioFrequency);
    ctxRadioRDSPI = iContextUtility->GetContextL(KHgCFTypeMusicRadioRDSPI);
    STIF_ASSERT_TRUE_DESC( ctxRadioName != NULL && !ctxRadioName->Compare( KHgCFValueUnknownInfo ),
        _L( "Empty value: ctxRadioName error." ) );
    STIF_ASSERT_TRUE_DESC( ctxRadioUrl != NULL && !ctxRadioUrl->Compare( KHgCFValueUnknownInfo ),
        _L( "Empty value: ctxRadioUrl error." ) );
    STIF_ASSERT_TRUE_DESC( ctxRadioFrequency != NULL && !ctxRadioFrequency->Compare( KHgCFValueUnknownInfo ),
        _L( "Empty value: ctxRadioFrequency error." ) );
    STIF_ASSERT_TRUE_DESC( ctxRadioRDSPI != NULL && !ctxRadioRDSPI->Compare( KHgCFValueUnknownInfo ),
        _L( "Empty value: ctxRadioRDSPI error." ) );
    delete ctxRadioName;
    delete ctxRadioUrl;
    delete ctxRadioFrequency;
    delete ctxRadioRDSPI;
    
    // One value
    testData.Reset();
    testData.ReserveL(16);
    for(TInt i = 0; i < 12; i++)
        {
        testData.AppendL(KNullDesC());
        }
    testData.Insert(KTestRadioName(), 0);
    testData.Insert(KTestRadioUrl(), 5);
    testData.Insert(KTestRadioFrequency(), 10);
    testData.Insert(KTestRadioRDSPI(), 15);
    for(TInt i = 0; i < 16; i += 4)
        {
        iContextUtility->PublishRadioContextL(testData[i], testData[i+1], testData[i+2], testData[i+3]);
        ctxRadioName = iContextUtility->GetContextL(KHgCFTypeMusicRadioName);
        ctxRadioUrl = iContextUtility->GetContextL(KHgCFTypeMusicRadioUrl);
        ctxRadioFrequency = iContextUtility->GetContextL(KHgCFTypeMusicRadioFrequency);
        ctxRadioRDSPI = iContextUtility->GetContextL(KHgCFTypeMusicRadioRDSPI);
        STIF_ASSERT_TRUE_DESC( ctxRadioName != NULL &&
            !ctxRadioName->Compare( testData[i].Length() ? testData[i] : KHgCFValueUnknownInfo() ),
            _L( "One value: ctxRadioName error." ) );
        STIF_ASSERT_TRUE_DESC( ctxRadioUrl != NULL &&
            !ctxRadioUrl->Compare( testData[i+1].Length() ? testData[i+1] : KHgCFValueUnknownInfo() ),
            _L( "One value: ctxRadioUrl error." ) );
        STIF_ASSERT_TRUE_DESC( ctxRadioFrequency != NULL &&
            !ctxRadioFrequency->Compare( testData[i+2].Length() ? testData[i+2] : KHgCFValueUnknownInfo() ),
            _L( "One value: ctxRadioFrequency error." ) );
        STIF_ASSERT_TRUE_DESC( ctxRadioRDSPI != NULL &&
            !ctxRadioRDSPI->Compare( testData[i+3].Length() ? testData[i+3] : KHgCFValueUnknownInfo() ),
            _L( "One value: ctxRadioRDSPI error." ) );
        delete ctxRadioName;
        delete ctxRadioUrl;
        delete ctxRadioFrequency;
        delete ctxRadioRDSPI;
        }
    
    // Two values
    testData.Reset();
    testData.ReserveL(24);
    for(TInt i = 0; i < 12; i++)
        {
        testData.AppendL(KNullDesC());
        }
    testData.Insert(KTestRadioName(), 0);
    testData.Insert(KTestRadioUrl(), 1);
    testData.Insert(KTestRadioName(), 4);
    testData.Insert(KTestRadioFrequency(), 6);
    testData.Insert(KTestRadioName(), 8);
    testData.Insert(KTestRadioRDSPI(), 11);
    testData.Insert(KTestRadioUrl(), 13);
    testData.Insert(KTestRadioFrequency(), 14);
    testData.Insert(KTestRadioUrl(), 17);
    testData.Insert(KTestRadioRDSPI(), 19);
    testData.Insert(KTestRadioFrequency(), 22);
    testData.Insert(KTestRadioRDSPI(), 23);
    for(TInt i = 0; i < 24; i += 4)
        {
        iContextUtility->PublishRadioContextL(testData[i], testData[i+1], testData[i+2], testData[i+3]);
        ctxRadioName = iContextUtility->GetContextL(KHgCFTypeMusicRadioName);
        ctxRadioUrl = iContextUtility->GetContextL(KHgCFTypeMusicRadioUrl);
        ctxRadioFrequency = iContextUtility->GetContextL(KHgCFTypeMusicRadioFrequency);
        ctxRadioRDSPI = iContextUtility->GetContextL(KHgCFTypeMusicRadioRDSPI);
        STIF_ASSERT_TRUE_DESC( ctxRadioName != NULL &&
            !ctxRadioName->Compare( testData[i].Length() ? testData[i] : KHgCFValueUnknownInfo() ),
            _L( "Two values: ctxRadioName error." ) );
        STIF_ASSERT_TRUE_DESC( ctxRadioUrl != NULL &&
            !ctxRadioUrl->Compare( testData[i+1].Length() ? testData[i+1] : KHgCFValueUnknownInfo() ),
            _L( "Two values: ctxRadioUrl error." ) );
        STIF_ASSERT_TRUE_DESC( ctxRadioFrequency != NULL &&
            !ctxRadioFrequency->Compare( testData[i+2].Length() ? testData[i+2] : KHgCFValueUnknownInfo() ),
            _L( "Two values: ctxRadioFrequency error." ) );
        STIF_ASSERT_TRUE_DESC( ctxRadioRDSPI != NULL &&
            !ctxRadioRDSPI->Compare( testData[i+3].Length() ? testData[i+3] : KHgCFValueUnknownInfo() ),
            _L( "Two values: ctxRadioRDSPI error." ) );
        delete ctxRadioName;
        delete ctxRadioUrl;
        delete ctxRadioFrequency;
        delete ctxRadioRDSPI;
        }
    
    // Three values
    testData.Reset();
    testData.ReserveL(16);
    for(TInt i = 0; i < 4; i++)
        {
        testData.AppendL(KTestRadioName());
        testData.AppendL(KTestRadioUrl());
        testData.AppendL(KTestRadioFrequency());
        testData.AppendL(KTestRadioRDSPI());
        }
    testData.Remove(3);
    testData.Insert(KNullDesC(), 3);
    testData.Remove(6);
    testData.Insert(KNullDesC(), 6);
    testData.Remove(9);
    testData.Insert(KNullDesC(), 9);
    testData.Remove(12);
    testData.Insert(KNullDesC(), 12);
    for(TInt i = 0; i < 16; i += 4)
        {
        iContextUtility->PublishRadioContextL(testData[i], testData[i+1], testData[i+2], testData[i+3]);
        ctxRadioName = iContextUtility->GetContextL(KHgCFTypeMusicRadioName);
        ctxRadioUrl = iContextUtility->GetContextL(KHgCFTypeMusicRadioUrl);
        ctxRadioFrequency = iContextUtility->GetContextL(KHgCFTypeMusicRadioFrequency);
        ctxRadioRDSPI = iContextUtility->GetContextL(KHgCFTypeMusicRadioRDSPI);
        STIF_ASSERT_TRUE_DESC( ctxRadioName != NULL &&
            !ctxRadioName->Compare( testData[i].Length() ? testData[i] : KHgCFValueUnknownInfo() ),
            _L( "Three values: ctxRadioName error." ) );
        STIF_ASSERT_TRUE_DESC( ctxRadioUrl != NULL &&
            !ctxRadioUrl->Compare( testData[i+1].Length() ? testData[i+1] : KHgCFValueUnknownInfo() ),
            _L( "Three values: ctxRadioUrl error." ) );
        STIF_ASSERT_TRUE_DESC( ctxRadioFrequency != NULL &&
            !ctxRadioFrequency->Compare( testData[i+2].Length() ? testData[i+2] : KHgCFValueUnknownInfo() ),
            _L( "Three values: ctxRadioFrequency error." ) );
        STIF_ASSERT_TRUE_DESC( ctxRadioRDSPI != NULL &&
            !ctxRadioRDSPI->Compare( testData[i+3].Length() ? testData[i+3] : KHgCFValueUnknownInfo() ),
            _L( "Three values: ctxRadioRDSPI error." ) );
        delete ctxRadioName;
        delete ctxRadioUrl;
        delete ctxRadioFrequency;
        delete ctxRadioRDSPI;
        }
    
    // Four values
    iContextUtility->PublishRadioContextL(KTestRadioName, KTestRadioUrl, KTestRadioFrequency, KTestRadioRDSPI);
    ctxRadioName = iContextUtility->GetContextL(KHgCFTypeMusicRadioName);
    ctxRadioUrl = iContextUtility->GetContextL(KHgCFTypeMusicRadioUrl);
    ctxRadioFrequency = iContextUtility->GetContextL(KHgCFTypeMusicRadioFrequency);
    ctxRadioRDSPI = iContextUtility->GetContextL(KHgCFTypeMusicRadioRDSPI);
    STIF_ASSERT_TRUE_DESC( ctxRadioName != NULL && !ctxRadioName->Compare( KTestRadioName ),
        _L( "Four values: ctxRadioName error." ) );
    STIF_ASSERT_TRUE_DESC( ctxRadioUrl != NULL && !ctxRadioUrl->Compare( KTestRadioUrl ),
        _L( "Four values: ctxRadioUrl error." ) );
    STIF_ASSERT_TRUE_DESC( ctxRadioFrequency != NULL && !ctxRadioFrequency->Compare( KTestRadioFrequency ),
        _L( "Four values: ctxRadioFrequency error." ) );
    STIF_ASSERT_TRUE_DESC( ctxRadioRDSPI != NULL && !ctxRadioRDSPI->Compare( KTestRadioRDSPI ),
        _L( "Four values: ctxRadioRDSPI error." ) );
    delete ctxRadioName;
    delete ctxRadioUrl;
    delete ctxRadioFrequency;
    delete ctxRadioRDSPI;
    
    CleanupStack::PopAndDestroy(&testData);
    
    return KErrNone;
    }

#endif

// end of file
