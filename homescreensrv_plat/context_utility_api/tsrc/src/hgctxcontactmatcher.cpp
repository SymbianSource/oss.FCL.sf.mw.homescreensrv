/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contact matching class
*
*/


// System includes
#include <e32base.h>
#include <bamdesca.h>
#include <cntitem.h>
#include <eikenv.h>
#include <bautils.h>
#include <fbs.h>
#include <imageconversion.h>
#include <data_caging_path_literals.hrh>

#include <CVPbkContactManager.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactLinkArray.h>
#include <MVPbkContactLink.h>
#include <MVPbkFieldType.h>
#include <MVPbkStoreContact.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldUriData.h>
#include <MVPbkContactFieldDateTimeData.h>
#include <MVPbkContactFieldBinaryData.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactStoreProperties.h>
#include <TVPbkContactStoreUriPtr.h>
#include <VPbkContactStoreUris.h>
#include <MPbk2ContactNameFormatter.h>
#include <CVPbkFieldTypeSelector.h>
#include <TVPbkFieldVersitProperty.h>
#include <CVPbkFieldTypeRefsList.h>
#include <TVPbkWordParserCallbackParam.h>
#include <CVPbkContactViewDefinition.h>
#include <MVPbkContactViewBase.h>
#include <coemain.h>
#include <CPbk2SortOrderManager.h>
#include <Pbk2ContactNameFormatterFactory.h>

#include "hgctxcontactmatcher.h"


#include "hgctxutilslogging.h"


// ================= Static Constant Data ===================

typedef const TDesC& (*UriFuncPtr)();

// Number match store URIs in priority order.
// When doing number matching, order of the stores in the uri array will
// determine which stores are searched first (sequential match). We stop
// the search when first match is found.
static const UriFuncPtr NumberMatchStoreUris[] =
    {
    VPbkContactStoreUris::DefaultCntDbUri,
    // If we don't manage to open some store, we remove it from our array
    VPbkContactStoreUris::SimGlobalAdnUri,
    VPbkContactStoreUris::SimGlobalSdnUri,
    NULL,   // end marker
    };

// All store URIs except own number store
static const UriFuncPtr AllStoreUris[] =
    {
    VPbkContactStoreUris::DefaultCntDbUri,
    // If we don't manage to open some store, we remove it from our array
    VPbkContactStoreUris::SimGlobalAdnUri,
    VPbkContactStoreUris::SimGlobalSdnUri,
    VPbkContactStoreUris::SimGlobalFdnUri,
    NULL,   // end marker
    };

// Own number store URIs
static const UriFuncPtr OwnNumberStoreUris[] =
    {
    VPbkContactStoreUris::SimGlobalOwnNumberUri,
    NULL,   // end marker
    };

// number of digits that must match from the right side of a phone number
const TInt KNumberMatchLenFromRight = 7;

// granularity for CDesCArray
const TInt KArrayGranularity = 4;

//               YYYYMMDD:HHMMSS.MMMMMM
_LIT(KNullTime, "11110000:010101.00000");

// ================= STATIC FUNCTIONS =======================

// ---------------------------------------------------------
// FindWordSplitterL
// ---------------------------------------------------------
//
static TInt FindWordSplitterL( TAny* aParams )
	{
	TVPbkWordParserCallbackParam* parser = 
        static_cast<TVPbkWordParserCallbackParam*>( aParams );
        
    const TText* ptr = parser->iStringToParse->Ptr();
	const TText* end = ptr + parser->iStringToParse->Length();

	const TText* startOfWord=NULL;
	for ( ; ; )
		{
		if ( ptr==end || TChar(*ptr).IsSpace() || *ptr == ',' || *ptr == ';' )
			{
			if ( startOfWord )
				{
				TPtrC addWord( startOfWord,ptr - startOfWord );
				parser->iWordArray->AppendL( addWord );
				startOfWord = NULL;
				}
			if ( ptr == end )
                {
				break;
                }
			}
		else if ( !startOfWord )
            {
			startOfWord = ptr;
            }
		ptr++;
		}
	return( KErrNone );
	}

static HBufC* CombineStringsLC( CDesCArray& aArray )
    {
    TInt len = 0;
    for ( TInt i = 0, ie = aArray.Count(); i != ie; ++i )
        {
        len += aArray[i].Length() + 1;
        }
    HBufC* result = HBufC::NewLC( len );
    TPtr p( result->Des() );
    for ( TInt i = 0, ie = aArray.Count(); i != ie; ++i )
        {
        if ( i )
            {
            p.Append( ' ' );
            }
        p.Append( aArray[i] );
        }
    return result;
    }
    
// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// Two-phase constructor for CHgCtxContactMatcher class.
// ----------------------------------------------------------------------------
EXPORT_C CHgCtxContactMatcher* CHgCtxContactMatcher::NewL(
    RFs* aFsSession )
    {
    CHgCtxContactMatcher* self = CHgCtxContactMatcher::NewLC( aFsSession );
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Two-phase constructor for CHgCtxContactMatcher class.
// ----------------------------------------------------------------------------
EXPORT_C CHgCtxContactMatcher* CHgCtxContactMatcher::NewLC(
    RFs* aFsSession )
    {
    CHgCtxContactMatcher* self = new ( ELeave ) CHgCtxContactMatcher( aFsSession );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// C++ destructor.
// ----------------------------------------------------------------------------
EXPORT_C CHgCtxContactMatcher::~CHgCtxContactMatcher()
    {
    delete iNameFieldTypes;
    delete iNumberFieldTypes;
    delete iEmailFieldTypes;
    delete iXspIdFieldTypes;
    delete iAddressFieldTypes;
	delete iWebAddressFieldTypes;
    delete iHomeAddressFieldTypes;
    delete iWorkAddressFieldTypes;
    
    FreeOldOperation();
    CleanupNumberMatch();
    delete iStoreUris;
    delete iContactManager;
    delete iSortOrderManager;
    delete iNameFormatter;

    if ( iClientStatus )
        {
        User::RequestComplete( iClientStatus, KErrCancel );
        }
    if ( iASchedulerWait.IsStarted() )
        {
        iASchedulerWait.AsyncStop();
        }
        
    iContactObservers.Close();
    
    if ( iFsSessionOwned && iFsSession )
        {
        iFsSession->Close();
        delete iFsSession;
        }
    }

// ----------------------------------------------------------------------------
// C++ Constructor.
// ----------------------------------------------------------------------------
CHgCtxContactMatcher::CHgCtxContactMatcher( RFs* aFsSession) : iFsSession( aFsSession )
    {
    }

// ----------------------------------------------------------------------------
// Second phase constructor
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::ConstructL()
    {
    if ( !iFsSession )
        {
        // The contact manager would be okay with NULL but some of our own functions
        // need an RFs.
        iFsSessionOwned = ETrue;
        iFsSession = new ( ELeave ) RFs;
        User::LeaveIfError( iFsSession->Connect() );
        }
        
    iContactManager = CVPbkContactManager::NewL(
        *CVPbkContactStoreUriArray::NewLC(), iFsSession );
    CleanupStack::PopAndDestroy(); // CVPbkContactStoreUriArray

    // No stores open yet
    iStoreUris = CVPbkContactStoreUriArray::NewL();
    }

//******************* API-methods *********************************************

// ----------------------------------------------------------------------------
// Synchronous version
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::OpenStoreL(
    const CVPbkContactStoreUriArray& aUriArray )
    {
    InitOperationL( EOpenStore );
    OpenStoreCommonL( aUriArray );
    if ( iApiMethodStatus != EFinished )
        {
        iApiMethodStatus = EExecuting;
        // Wait until stores are open
        iASchedulerWait.Start();
        }
    User::LeaveIfError( iError );
    }

// ----------------------------------------------------------------------------
// Asynchronous version
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::OpenStoreL(
    const CVPbkContactStoreUriArray& aUriArray, TRequestStatus&  aStatus )
    {
    InitOperationL( EOpenStore );
    OpenStoreCommonL( aUriArray );
    InitOperation( &aStatus );

    if ( iApiMethodStatus != EFinished )
        {
        iApiMethodStatus = EExecuting;
        }
    }


// ----------------------------------------------------------------------------
// Common code to sync/async versions.
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::OpenStoreCommonL(
    const CVPbkContactStoreUriArray& aUriArray )
    {
    if (iStoreUris->Count())
        {
        // Opening more stores when some stores are already open is not
        // supported. Support would require managing iStoreUris properly
        // so that it contains all open stores.
        User::Leave(KErrGeneral);
        }

    const TInt count = aUriArray.Count();
    
    for (TInt i = 0; i < count; ++i)
        {
        // Appended Uri:s to the array. If store fails to open it is removed
        // from the array. This keeps Uri's in priority order in the array.
        TVPbkContactStoreUriPtr uriPtr = aUriArray[i];
        iStoreUris->AppendL( uriPtr );

        iContactManager->LoadContactStoreL( uriPtr );
        }
    MVPbkContactStoreList& storeList = iContactManager->ContactStoresL();
    storeList.OpenAllL( *this );
    }

// ----------------------------------------------------------------------------
// Synchronous version
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::OpenAllStoresL()
    {
    OpenStoreL(AllStoreUris);
    }

// ----------------------------------------------------------------------------
// Asynchronous version
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::OpenAllStoresL( TRequestStatus& aStatus )
    {
    OpenStoreL(AllStoreUris, aStatus);
    }

// ----------------------------------------------------------------------------
// Synchronous version
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::OpenDefaultMatchStoresL()
    {
    OpenStoreL(NumberMatchStoreUris);
    }

// ----------------------------------------------------------------------------
// Asynchronous version
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::OpenDefaultMatchStoresL( TRequestStatus& aStatus )
    {
    OpenStoreL(NumberMatchStoreUris, aStatus);
    }

// ----------------------------------------------------------------------------
// Open OwnNumber stores.
// Synchronous version
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::OpenOwnNumberStoresL()
    {
    OpenStoreL(OwnNumberStoreUris);
    }

// ----------------------------------------------------------------------------
// Open OwnNumber stores.
// Asynchronous version
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::OpenOwnNumberStoresL( TRequestStatus& aStatus )
    {
    OpenStoreL(OwnNumberStoreUris, aStatus);
    }

// ----------------------------------------------------------------------------
// Close all open stores.
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::CloseStoresL()
    {
    // Closing stores does not work. MatchDataL() finds contacts from
    // closed stores.

    InitOperationL( ECloseStores );

    iApiMethodStatus = EExecuting;
    TRAPD( err, iContactManager->ContactStoresL().CloseAll( *this ) );
    iApiMethodStatus = EFinished;
    if ( err == KErrNone)
        {
        delete iStoreUris; iStoreUris = NULL;
        iStoreUris = CVPbkContactStoreUriArray::NewL();        
        }
    else
        {
        User::Leave(err);
        }
    }

// ----------------------------------------------------------------------------
// Synchronous version
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::MatchPhoneNumberL(
    const TDesC& aData, TInt aDigits,
    CVPbkPhoneNumberMatchStrategy::TVPbkPhoneNumberMatchFlags aFlags,
    CVPbkContactLinkArray& aLinkArray )
    {
    InitOperationL( EMatchPhoneNumber );
    iResultContactLinkArray = &aLinkArray;

    // Start asynchronous matching and wait until results are ready
    MatchPhoneNumberCommonL( aData, aDigits, aFlags );
    if ( iApiMethodStatus != EFinished )
        {
        iApiMethodStatus = EExecuting;
        iASchedulerWait.Start();
        }
    User::LeaveIfError( iError );
    }


// ----------------------------------------------------------------------------
// Asynchronous version
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::MatchPhoneNumberL(
    const TDesC& aData, TInt aDigits,
    CVPbkPhoneNumberMatchStrategy::TVPbkPhoneNumberMatchFlags aFlags,
    CVPbkContactLinkArray& aLinkArray, TRequestStatus& aStatus )
    {
    InitOperationL( EMatchPhoneNumber );
    iResultContactLinkArray = &aLinkArray;
    // Start asynchronous matching
    MatchPhoneNumberCommonL( aData, aDigits, aFlags );
    InitOperation( &aStatus );
    if ( iApiMethodStatus != EFinished )
        {
        iApiMethodStatus = EExecuting;
        }
    }

// ----------------------------------------------------------------------------
// Common code for sync and async versions
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::MatchPhoneNumberCommonL(
    const TDesC& aData, TInt aDigits,
    CVPbkPhoneNumberMatchStrategy::TVPbkPhoneNumberMatchFlags aFlags )
    {
    // Delete resources allocated for previous match
    CleanupNumberMatch();

    // iStoreUris is filled when stores are opened

    iStratConfig = new (ELeave) CVPbkPhoneNumberMatchStrategy::TConfig(
        aDigits,
        *iStoreUris,
        CVPbkPhoneNumberMatchStrategy::EVPbkSequentialMatch,
        aFlags);
    iMatchStrategy = CVPbkPhoneNumberMatchStrategy::NewL(
                *iStratConfig,
                *iContactManager,
                *this);
    // Start asynchronous matching
    iMatchStrategy->MatchL( aData );
    }


// ----------------------------------------------------------------------------
// Find from a store succeeded
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::FindFromStoreSucceededL( MVPbkContactStore& /*aStore*/,
        MVPbkContactLinkArray* aResultsFromStore )
    {
    __ASSERT_DEBUG( aResultsFromStore, HgContactMatcherPanics::Panic(
        HgContactMatcherPanics::EPanNullPointer ));

    // Take the ownership of the result immediately
    CleanupDeletePushL( aResultsFromStore );

    CopyFindResultsL( aResultsFromStore );

    CleanupStack::PopAndDestroy(); // aResultsFromStore
    }

// ----------------------------------------------------------------------------
// Copy the found results for a store into array
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::CopyFindResultsL( MVPbkContactLinkArray*
    aResults )
    {
    const TInt count = aResults->Count();
    if ( iResultContactLinkArray )
        {
        // Copy links to the member array
        for ( TInt i = 0; i < count; ++i )
            {
            iResultContactLinkArray->AppendL( aResults->At( i ).CloneLC() );
            CleanupStack::Pop(); // cloned link
            }
        }
    else
        {
        iResultContactLinkCnt += count;
        }
    }


// ----------------------------------------------------------------------------
// Find failed
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::FindFromStoreFailed( MVPbkContactStore& /*aStore*/, TInt /*aError*/ )
    {
    //no operation, search to continue from the other stores
    }


// ----------------------------------------------------------------------------
// Find complete
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::FindFromStoresOperationComplete()
    {
    if (!iResultContactLinkArray)
        {
        // Links were not copied. Result is whether any links found or not.
        OperationComplete( iResultContactLinkCnt ? KErrNone:KErrNotFound );
        }
    else
        {
        OperationComplete();
        iResultContactLinkArray = NULL;
        }
    }

// ----------------------------------------------------------------------------
// Return global list of field types.
// ----------------------------------------------------------------------------
EXPORT_C const MVPbkFieldTypeList& CHgCtxContactMatcher::FieldTypes() const
    {
    return iContactManager->FieldTypes();
    }

// ----------------------------------------------------------------------------
// Synchronous version
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::GetStoreContactL(
    const MVPbkContactLink& aLink, MVPbkStoreContact** aStoreContact )
    {
    InitOperationL( EGetStoreContact );
    iResultStoreContact = aStoreContact;

    // Start asynchronous operation and wait until results are ready
    FreeOldOperation();
    iOperation = iContactManager->RetrieveContactL( aLink, *this );
    if ( iApiMethodStatus != EFinished )
        {
        iApiMethodStatus = EExecuting;
        iASchedulerWait.Start();
        }
    User::LeaveIfError( iError );
    }

// ----------------------------------------------------------------------------
// Asynchronous version
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::GetStoreContactL(
    const MVPbkContactLink& aLink, MVPbkStoreContact** aStoreContact,
    TRequestStatus& aStatus )
    {
    InitOperationL( EGetStoreContact );
    iResultStoreContact = aStoreContact;
    // Start asynchronous operation
    FreeOldOperation();
    iOperation = iContactManager->RetrieveContactL( aLink, *this );
    InitOperation( &aStatus );
    if ( iApiMethodStatus != EFinished )
        {
        iApiMethodStatus = EExecuting;
        }
    }

// ----------------------------------------------------------------------------
// Synchronous version
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::IsOwnNumberL( const TDesC& aNumber, TBool& aResult )
    {
    InitOperationL( EMatchPhoneNumber );

     // Not interested in links, only whether found or not
    iResultContactLinkArray = NULL;
    iResultContactLinkCnt = 0;

    // Start asynchronous matching and wait until results are ready
    MatchPhoneNumberCommonL( aNumber, aNumber.Length(),
        CVPbkPhoneNumberMatchStrategy::EVPbkStopOnFirstMatchFlag );
    if ( iApiMethodStatus != EFinished )
        {
        iApiMethodStatus = EExecuting;
        iASchedulerWait.Start();
        }
    User::LeaveIfError( iError );

    aResult = iResultContactLinkCnt > 0;
    }

// ----------------------------------------------------------------------------
// Asynchronous version
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::IsOwnNumberL( const TDesC& aNumber,
    TRequestStatus& aStatus )
    {
    InitOperationL( EMatchPhoneNumber );

     // Not interested in links, only whether found or not
    iResultContactLinkArray = NULL;
    iResultContactLinkCnt = 0;

    // Start asynchronous matching
    MatchPhoneNumberCommonL( aNumber, aNumber.Length(),
        CVPbkPhoneNumberMatchStrategy::EVPbkStopOnFirstMatchFlag );
    InitOperation( &aStatus );
    if ( iApiMethodStatus != EFinished )
        {
        iApiMethodStatus = EExecuting;
        }
    }

// ----------------------------------------------------------------------------
// Cancel asynchronous operation
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::CancelOperation()
    {
    if (iApiMethodStatus != EExecuting)
        {
        return;
        }

    __ASSERT_DEBUG(!iSync, HgContactMatcherPanics::Panic(
        HgContactMatcherPanics::EPanInvalidOp));

    switch(iApiMethod)
        {
        case EMatchData:
        case EGetStoreContact:
            FreeOldOperation(); // deleting the operation cancels it
            break;
        case EMatchPhoneNumber:
            CleanupNumberMatch();
            break;
        default:
            ;
        }

    User::RequestComplete( iClientStatus, KErrCancel );

    iApiMethod = ENoMethod;
    iApiMethodStatus = EFinished;
    }

// ----------------------------------------------------------------------------
// GetFieldData, for EVPbkFieldStorageTypeText
// ----------------------------------------------------------------------------
EXPORT_C TPtrC CHgCtxContactMatcher::GetFieldDataTextL(
    const MVPbkStoreContact& aContact,
    const MVPbkFieldType& aFType ) const
    {
    TPtrC ret(KNullDesC);
    const MVPbkStoreContactField* field = FindField( aContact, aFType);
    if (field)
        {
        const MVPbkContactFieldData& fdata = field->FieldData();
        if (fdata.DataType() == EVPbkFieldStorageTypeText)
            {
            const MVPbkContactFieldTextData& fdata2 =
                MVPbkContactFieldTextData::Cast(fdata);
            ret.Set( fdata2.Text() );
            }
        else
            {
            User::Leave( KErrArgument );
            }
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// GetFieldData, for EVPbkFieldStorageTypeDateTime
// ----------------------------------------------------------------------------
EXPORT_C TTime CHgCtxContactMatcher::GetFieldDataDateTimeL(
    const MVPbkStoreContact& aContact,
    const MVPbkFieldType& aFType ) const
    {
    TTime ret(KNullTime);
    const MVPbkStoreContactField* field = FindField( aContact, aFType);
    if (field)
        {
        const MVPbkContactFieldData& fdata = field->FieldData();
        if (fdata.DataType() == EVPbkFieldStorageTypeDateTime)
            {
            const MVPbkContactFieldDateTimeData& fdata2 =
                MVPbkContactFieldDateTimeData::Cast( fdata );
            ret = fdata2.DateTime();
            }
        else
            {
            User::Leave( KErrArgument );
            }
        }
    return ret;
    }

// ----------------------------------------------------------------------------
// GetFieldData, for EVPbkFieldStorageTypeBinary
// ----------------------------------------------------------------------------
EXPORT_C TPtrC8 CHgCtxContactMatcher::GetFieldDataBinaryL(
    const MVPbkStoreContact& aContact,
    const MVPbkFieldType& aFType ) const
    {
    TPtrC8 ret(KNullDesC8);
    const MVPbkStoreContactField* field = FindField( aContact, aFType);
    if (field)
        {
        const MVPbkContactFieldData& fdata = field->FieldData();
        if (fdata.DataType() == EVPbkFieldStorageTypeBinary)
            {
            const MVPbkContactFieldBinaryData& fdata2 =
                MVPbkContactFieldBinaryData::Cast( fdata );
            ret.Set( fdata2.BinaryData() );
            }
        else
            {
            User::Leave( KErrArgument );
            }
        }
    return ret;
    }


//******************************** Private Methods ***************************

// ----------------------------------------------------------------------------
// Finds a field of given type from contact.
// Returns pointer to field or NULL if not found.
// ----------------------------------------------------------------------------
 const MVPbkStoreContactField* CHgCtxContactMatcher::FindField(
    const MVPbkStoreContact& aContact,
    const MVPbkFieldType& aFType ) const
    {
    const MVPbkStoreContactFieldCollection& coll = aContact.Fields();
    TInt n = coll.FieldCount();

    const MVPbkStoreContactField* field = NULL;
    TBool bFound = EFalse;
    for(TInt i=0; i < n && !bFound; ++i)
        {
        field = &coll.FieldAt( i );
        const MVPbkFieldType* ftype = field->MatchFieldType( 0 );
        if ( ftype )
            {
            if ( ftype->IsSame( aFType ))
                {
                bFound = ETrue;
                }
            }
        }
    if ( !bFound )
        {
        field = NULL;
        }
    return field;
    }

// ----------------------------------------------------------------------------
// Get URI array with stores
// ----------------------------------------------------------------------------
CVPbkContactStoreUriArray* CHgCtxContactMatcher::GetStoreArrayLC(
    const TDesC& (* const aFuncPtrs[])() )
    {
    CVPbkContactStoreUriArray* uriArray = CVPbkContactStoreUriArray::NewLC();

    // Add stores
    for(TInt i = 0; aFuncPtrs[i]; i++)
        {
        TVPbkContactStoreUriPtr uriPtr(aFuncPtrs[i]());
        uriArray->AppendL(uriPtr);
        }
    return uriArray;
    }

// ----------------------------------------------------------------------------
// Open stores. Synchronous version
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::OpenStoreL(const TDesC& (* const aFuncPtrs[])())
    {
    CVPbkContactStoreUriArray* uriArray = GetStoreArrayLC(aFuncPtrs);

    CHgCtxContactMatcher::OpenStoreL(*uriArray);
    CleanupStack::PopAndDestroy(uriArray);
    }

// ----------------------------------------------------------------------------
// Open stores. Asynchronous version
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::OpenStoreL(const TDesC& (* const aFuncPtrs[])(),
    TRequestStatus&  aStatus)
    {
    CVPbkContactStoreUriArray* uriArray = GetStoreArrayLC(aFuncPtrs);

    CHgCtxContactMatcher::OpenStoreL(*uriArray, aStatus);
    CleanupStack::PopAndDestroy(uriArray);
    }

// ----------------------------------------------------------------------------
// Called when the opening process is complete,
// ie. all stores have been reported either failed or successfully opened.
// ----------------------------------------------------------------------------
//
void CHgCtxContactMatcher::OpenComplete()
    {
    TInt error = KErrNone;
    if ( iStoreUris->Count() == 0 )
        {
        // unable to open any of the specified stores
        error = KErrNotSupported;
        }
    OperationComplete( error );
    }

// ----------------------------------------------------------------------------
// Called when a contact store is ready to use.
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::StoreReady( MVPbkContactStore& /*aContactStore*/ )
    {
    }

// ----------------------------------------------------------------------------
// Called when a contact store becomes unavailable.
// Client may inspect the reason of the unavailability and decide whether or not
// it will keep the store opened (ie. listen to the store events).
// @param aContactStore The store that became unavailable.
// @param aReason The reason why the store is unavailable.
//                This is one of the system wide error codes.
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::StoreUnavailable( MVPbkContactStore& aContactStore,
    TInt /*aReason*/ )
    {
    // Remove contact store from uri list
    iStoreUris->Remove( aContactStore.StoreProperties().Uri() );
    }

// ----------------------------------------------------------------------------
// Called when changes occur in the contact store.
// @see TVPbkContactStoreEvent
//
// @param aStoreEvent Event that has occured.
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::HandleStoreEventL(
        MVPbkContactStore& /*aContactStore*/,
        TVPbkContactStoreEvent aStoreEvent)
    {
    // Contact and group events can be ignored, but we pass backup events for the observer.
    switch ( aStoreEvent.iEventType )
        {
        case TVPbkContactStoreEvent::EStoreBackupBeginning:
        case TVPbkContactStoreEvent::EStoreRestoreBeginning:
            {
            iBackup = ETrue;
            break;
            }
        case TVPbkContactStoreEvent::EStoreBackupRestoreCompleted:
            {
            iBackup = EFalse;
            break;
            }
        case TVPbkContactStoreEvent::EContactAdded:
        case TVPbkContactStoreEvent::EContactDeleted:
        case TVPbkContactStoreEvent::EContactChanged:
            {
            for ( TInt i = 0, ie = iContactObservers.Count(); i != ie; ++i )
                {
                iContactObservers[i]->HandleContactEventL();
                }
            break;
            }
        default:
            break;
        }
    }


// ----------------------------------------------------------------------------
// Called when find is complete. Callee takes ownership of the results.
// In case of an error during find, the aResults may contain only
// partial results of the find.
//
// @param aResults Array of contact links that matched the find.
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::FindCompleteL( MVPbkContactLinkArray* aResults )
    {
    __ASSERT_DEBUG( aResults, HgContactMatcherPanics::Panic(
        HgContactMatcherPanics::EPanNullPointer ));

    // Take the ownership of the result immediately
    CleanupDeletePushL( aResults );

    CopyFindResultsL( aResults );

    CleanupStack::PopAndDestroy(); // aResults

    if (!iResultContactLinkArray)
        {
        // No need to copy links. Only interested whether found or not
        OperationComplete( iResultContactLinkCnt ? KErrNone:KErrNotFound );
        }
    else
        {
        OperationComplete();
        iResultContactLinkArray = NULL;
        }
    }

// ----------------------------------------------------------------------------
// Called in case the find fails for some reason.
//
// @param aError One of the system wide error codes.
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::FindFailed( TInt aError )
    {
    OperationFailed( aError );
    iResultContactLinkArray = NULL;
    }

// ----------------------------------------------------------------------------
// Free old VPbk-operation.
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::FreeOldOperation()
    {
    delete iOperation;
    iOperation = NULL;
    }

// ----------------------------------------------------------------------------
// Called when operation is completed.
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/, MVPbkStoreContact* aContact)
    {
    *iResultStoreContact = aContact;
    iResultStoreContact  = NULL;
    OperationComplete();
    }

// ----------------------------------------------------------------------------
// Called if the operation fails.
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::VPbkSingleContactOperationFailed(
    MVPbkContactOperationBase& /*aOperation*/, TInt aError)
    {
    OperationFailed( aError );
    }

// ----------------------------------------------------------------------------
// Set member variables for sync operation
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::InitOperationL( TMethodId aMethod )
    {
    if ( iBackup )
        {
        User::Leave( KErrAccessDenied );
        }

    // Check whether operation is in progress
    if ( iApiMethodStatus == EExecuting )
        {
        User::Leave( KErrInUse );
        }

    iSync  = ETrue;
    iError = KErrNone;
    iApiMethod = aMethod;
    iApiMethodStatus = EIdle;
    }

// ----------------------------------------------------------------------------
// Set member variables for async operation
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::InitOperationL( TMethodId aMethod, TRequestStatus* aStatus )
    {
    InitOperationL( aMethod );

    iSync  = EFalse;
    iClientStatus  = aStatus;
    *iClientStatus = KRequestPending;
    }
    
// ----------------------------------------------------------------------------
// Set member variables for async operation
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::InitOperation( TRequestStatus* aStatus )
    {
    iSync  = EFalse;
    iClientStatus  = aStatus;
    *iClientStatus = KRequestPending;
    }

// ----------------------------------------------------------------------------
// Sync/async operation finished succesfully, return results to method caller.
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::OperationComplete( TInt aErrorCode )
    {
    if (iSync)
        {
        if ( iASchedulerWait.IsStarted() )
            {
            iASchedulerWait.AsyncStop();
            }
        }
    else
        {
        if ( iClientStatus )
            {
            User::RequestComplete( iClientStatus, aErrorCode );
            iClientStatus = NULL;
            }
        }
    iApiMethodStatus = EFinished;
    }

// ----------------------------------------------------------------------------
// Sync/async operation failed, return results to method caller.
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::OperationFailed( TInt aError )
    {
    iError = aError;
    OperationComplete( aError );
    }

// ----------------------------------------------------------------------------
// Free resources allocated for number matching
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::CleanupNumberMatch()
{
    delete iMatchStrategy;
    iMatchStrategy = NULL;

    delete iStratConfig;
    iStratConfig = NULL;

    // store uris are not deleted here - opened array remains valid
    // until new set of stores is opened.
}

// ---------------------------------------------------------------------------
// CHgCtxContactMatcher::GetContactStoresL
// ---------------------------------------------------------------------------
EXPORT_C MVPbkContactStoreList& CHgCtxContactMatcher::GetContactStoresL()
    {
    return iContactManager->ContactStoresL();
    }


// -----------------------------------------------------------------------------
// TInt CHgCtxContactMatcher::GetName
//
// Returns the formatted name fo the contact
// -----------------------------------------------------------------------------
EXPORT_C HBufC* CHgCtxContactMatcher::GetNameL( MVPbkStoreContactFieldCollection&
                                                                aFieldCollection )
    {
    MPbk2ContactNameFormatter& nameFormatter = ContactNameFormatterL();
    
    HBufC* formattedName = nameFormatter.GetContactTitleOrNullL( aFieldCollection, 
    						                                     MPbk2ContactNameFormatter::EUseSeparator );
    return formattedName;
    }

// -----------------------------------------------------------------------------
// CVPbkContactManager& CHgCtxContactMatcher::GetContactManager( )
// -----------------------------------------------------------------------------
EXPORT_C CVPbkContactManager& CHgCtxContactMatcher::GetContactManager()
    {
    return *iContactManager;
    }
    

// ----------------------------------------------------------------------------
// Synchronous version
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::MatchDataL( const TDesC& aData,
    const MVPbkFieldTypeList& aFieldTypes,
    CVPbkContactLinkArray& aLinkArray)
    {
    InitOperationL( EMatchData );
    iResultContactLinkArray = &aLinkArray;

    // Start asynchronous matching and wait until results are ready
    FreeOldOperation();
    iOperation = iContactManager->FindL(aData, aFieldTypes, *this);
    if ( iApiMethodStatus != EFinished )
        {
        iApiMethodStatus = EExecuting;
        iASchedulerWait.Start();
        }

    User::LeaveIfError( iError );
    RemoveSimilarEmailAddressesL( aData, aLinkArray, aFieldTypes );
   	}

// ----------------------------------------------------------------------------
// Remove contacts that do not have exactly the correct email address
// e.g. if cbd@test.com address is requested, the for example a contact with address abcd@test.com will be removed
// from the result.
// This filtering is done only in the syncronous version of MatchDataL
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::RemoveSimilarEmailAddressesL( const TDesC& aData, CVPbkContactLinkArray& aLinkArray, const MVPbkFieldTypeList& aFieldTypes )
    {
    TVPbkFieldVersitProperty prop;
    prop.SetName( EVPbkVersitNameEMAIL );
    // do extra checks for email addresses
    
    const MVPbkFieldType* foundType = NULL;
    // Continue only if at least one type is EVPbkVersitNameEMAIL
    TInt i;
    for ( i = 0 ; i < aFieldTypes.FieldTypeCount() ; i++ )
        {
        foundType = &(aFieldTypes.FieldTypeAt( i ));
        if ( foundType->VersitProperties().Count() > 0
            && foundType->VersitProperties()[0].Name() == prop.Name() )
            {
            break;
            }
        }
    if ( i == aFieldTypes.FieldTypeCount() )
    	{
    	// no email types
    	return;
    	}
    
    const MVPbkFieldTypeList& fieldTypeList = FieldTypes();

    TInt index = 0;
	TBool isExactMatch;
    while( index < aLinkArray.Count() )
    	{
	    MVPbkStoreContact* storeContact;
	    GetStoreContactL( aLinkArray.At( index ), &storeContact );
	    storeContact->PushL();
	    
	    isExactMatch = EFalse;
        for ( TInt i = 0; i < fieldTypeList.FieldTypeCount(); i++ )
            {
            // find the email property
            foundType = &(fieldTypeList.FieldTypeAt( i ));
            if ( foundType->VersitProperties().Count() > 0
                && foundType->VersitProperties()[0].Name() == prop.Name() )
                {
                TPtrC src = GetFieldDataTextL(*storeContact, *foundType );
                if ( aData.CompareF( src ) == 0 )
        	    	{
        	    	isExactMatch = ETrue;
        	    	}
                }
            }
	    if ( isExactMatch )
            {
            // go for the next contact
            index++;
            }
        else
        	{
            // remove the contact, because the email address does not match the one queried. 
            // the next one will take plce of this contact in the list (=do not increase index)
            aLinkArray.Delete( index ); 
        	}
	    CleanupStack::PopAndDestroy( storeContact );
    	}
    }

// ----------------------------------------------------------------------------
// Asynchronous version
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::MatchDataL( const TDesC& aData,
    const MVPbkFieldTypeList& aFieldTypes,
    CVPbkContactLinkArray& aLinkArray,
    TRequestStatus& aStatus)
    {
    InitOperationL( EMatchData );
    iResultContactLinkArray = &aLinkArray;

    // Start asynchronous matching
    FreeOldOperation();
    iOperation = iContactManager->FindL(aData, aFieldTypes, *this);
    InitOperation( &aStatus );
    if ( iApiMethodStatus != EFinished )
        {
        iApiMethodStatus = EExecuting;
        }
    }
// ----------------------------------------------------------------------------
// MatchData for searchstrings
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::MatchDataL( const MDesC16Array& aSearchStrings,
    const MVPbkFieldTypeList& aFieldTypes,
    CVPbkContactLinkArray& aLinkArray,
    const TCallBack& aWordParserCallBack )
    {
    InitOperationL( EMatchData );
    iResultContactLinkArray = &aLinkArray;

    // Start asynchronous matching and wait here until results are ready
    FreeOldOperation();
    iOperation = iContactManager->FindL( aSearchStrings, aFieldTypes,
        *this, aWordParserCallBack );

    if ( iApiMethodStatus != EFinished )
        {
        iApiMethodStatus = EExecuting;
        iASchedulerWait.Start();
        }
    User::LeaveIfError( iError );
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::ContactNameFormatterL
// ----------------------------------------------------------------------------
EXPORT_C MPbk2ContactNameFormatter& CHgCtxContactMatcher::ContactNameFormatterL()
    {
    //first initialise, if not already initialised
    if ( !iSortOrderManager )
        {
        iSortOrderManager = CPbk2SortOrderManager::NewL( FieldTypes() );
        }
        
    if ( !iNameFormatter )
        {
        iNameFormatter = Pbk2ContactNameFormatterFactory::CreateL( FieldTypes(),
                                                                  *iSortOrderManager );
        }
    return *iNameFormatter;
    }


// ---------------------------------------------------------------------------
// HgContactMatcherPanics::Panic
//
// Panic function
// ---------------------------------------------------------------------------
void HgContactMatcherPanics::Panic( TPanic aPanic )
    {
    _LIT(KPanicCategory, "ContactMatcher");
    User::Panic( KPanicCategory, aPanic );
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::SplitAndMatchL
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::SplitAndMatchL( const TDesC& aData,
        const MVPbkFieldTypeList& aFieldTypes,
        CVPbkContactLinkArray& aLinkArray)
    {
    CDesCArray* wordArray = SplitFindStringL( aData );
    CleanupStack::PushL( wordArray );
    TCallBack findParser( FindWordSplitterL );
    MatchDataL( *wordArray, aFieldTypes, aLinkArray, findParser );
    CleanupStack::PopAndDestroy( wordArray );
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::RegisterContactObserverL
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::RegisterContactObserverL(
        MHgCtxContactObserver& aObserver )
    {
    if ( iContactObservers.Find( &aObserver ) == KErrNotFound )
        {
        iContactObservers.AppendL( &aObserver );
        }
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::UnregisterContactObserver
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::UnregisterContactObserver(
        MHgCtxContactObserver& aObserver )
    {
    TInt pos = iContactObservers.Find( &aObserver );
    if ( pos >= 0 )
        {
        iContactObservers.Remove( pos );
        }
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::LookupL
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::LookupL( const TDesC& aData,
        CVPbkContactLinkArray& aLinkArray )
    {
    HGLOG_CONTEXT( LookupL, HGLOG_LOCAL );
    HGLOG1_IN( "'%S'", &aData );

    // Take part_A from "part_A <part_B>"
    // or part_A from "part_A".
    TPtrC input( aData );
    TInt ltPos = input.Locate( '<' );
    TInt gtPos = input.Locate( '>' );
    if ( ltPos != KErrNotFound && gtPos> ltPos )
        {
        input.Set( aData.Mid( 0, ltPos ) );
        }
    HBufC* trimmedInput = input.AllocLC();
    trimmedInput->Des().Trim();

    TInt oldCount = aLinkArray.Count();
    if ( IsPhoneNumberL( *trimmedInput ) )
        {
        TryNumberLookupL( *trimmedInput, aLinkArray );
        }
    else
        {
        TryTextLookupL( *trimmedInput, aLinkArray );
        }

    CleanupStack::PopAndDestroy( trimmedInput );

    if ( aLinkArray.Count() == oldCount && ltPos != KErrNotFound && gtPos > ltPos )
        {
        // lookup for part_A was not successful so try part_B
        trimmedInput = aData.Mid( ltPos + 1, gtPos - ltPos - 1 ).AllocLC();
        trimmedInput->Des().Trim();
        if ( IsPhoneNumberL( *trimmedInput ) )
            {
            TryNumberLookupL( *trimmedInput, aLinkArray );
            }
        else
            {
            TryTextLookupL( *trimmedInput, aLinkArray );
            }
        CleanupStack::PopAndDestroy( trimmedInput );
        }
        
    HGLOG1_OUT( "got %d results", aLinkArray.Count() - oldCount );
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::PreCreateNameFieldTypesL
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::PreCreateNameFieldTypesL()
    {
    HGLOG_CONTEXT( PreCreateNameFieldTypesL, HGLOG_LOCAL );
    HGLOG_IN();

    iNameFieldTypes = CVPbkFieldTypeRefsList::NewL();
    const MVPbkFieldTypeList& types( FieldTypes() );
    TVPbkFieldVersitProperty prop;
    const MVPbkFieldType* t;

    prop.SetName( EVPbkVersitNameFN );
    prop.SetSubField( EVPbkVersitSubFieldNone );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iNameFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try FN" );
        }

    prop.SetName( EVPbkVersitNameN );
    prop.SetSubField( EVPbkVersitSubFieldGivenName );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iNameFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try given name" );
        }
    
    prop.SetName( EVPbkVersitNameN );
    prop.SetSubField( EVPbkVersitSubFieldFamilyName );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iNameFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try family name" );
        }
    
    prop.SetName( EVPbkVersitNameORG );
    prop.SetSubField( EVPbkVersitSubFieldOrgName );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iNameFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try org name" );
        }
        
    HGLOG_OUT();
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::PreCreateEmailFieldTypesL
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::PreCreateEmailFieldTypesL()
    {
    HGLOG_CONTEXT( PreCreateEmailFieldTypesL, HGLOG_LOCAL );
    HGLOG_IN();

    iEmailFieldTypes = CVPbkFieldTypeRefsList::NewL();
    const MVPbkFieldTypeList& types( FieldTypes() );
    TVPbkFieldVersitProperty prop;
    const MVPbkFieldType* t;

    prop.SetName( EVPbkVersitNameEMAIL );
    prop.SetSubField( EVPbkVersitSubFieldNone );
    prop.Parameters().Reset();
    prop.Parameters().Add( EVPbkVersitParamINTERNET );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iEmailFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try general email" );
        }

    prop.SetName( EVPbkVersitNameEMAIL );
    prop.SetSubField( EVPbkVersitSubFieldNone );
    prop.Parameters().Reset();
    prop.Parameters().Add( EVPbkVersitParamINTERNET );
    prop.Parameters().Add( EVPbkVersitParamWORK );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iEmailFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try work email" );
        }

    prop.SetName( EVPbkVersitNameEMAIL );
    prop.SetSubField( EVPbkVersitSubFieldNone );
    prop.Parameters().Reset();
    prop.Parameters().Add( EVPbkVersitParamINTERNET );
    prop.Parameters().Add( EVPbkVersitParamHOME );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iEmailFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try home email" );
        }
        
    HGLOG_OUT();
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::PreCreateXspIdFieldTypesL
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::PreCreateXspIdFieldTypesL()
    {
    HGLOG_CONTEXT( PreCreateXspIdFieldTypesL, HGLOG_LOCAL );
    HGLOG_IN();

    iXspIdFieldTypes = CVPbkFieldTypeRefsList::NewL();
    const MVPbkFieldTypeList& types( FieldTypes() );
    TVPbkFieldVersitProperty prop;
    const MVPbkFieldType* t;

    prop.SetName( EVPbkVersitNameIMPP );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iXspIdFieldTypes->AppendL( *t );
        }
        
    HGLOG1_OUT( "found %d xsp id field types",
        iXspIdFieldTypes->FieldTypeCount() );
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::PreCreateNumberFieldTypesL
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::PreCreateNumberFieldTypesL()
    {
    HGLOG_CONTEXT( PreCreateNumberFieldTypesL, HGLOG_LOCAL );
    HGLOG_IN();

    iNumberFieldTypes = CVPbkFieldTypeRefsList::NewL();
    const MVPbkFieldTypeList& types( FieldTypes() );
    TVPbkFieldVersitProperty prop;
    const MVPbkFieldType* t;

    prop.SetName( EVPbkVersitNameTEL );
    prop.SetSubField( EVPbkVersitSubFieldNone );
    prop.Parameters().Reset();
    prop.Parameters().Add( EVPbkVersitParamCELL );
    prop.Parameters().Add( EVPbkVersitParamHOME );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iNumberFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try home mobile" );
        }

    prop.SetName( EVPbkVersitNameTEL );
    prop.SetSubField( EVPbkVersitSubFieldNone );
    prop.Parameters().Reset();
    prop.Parameters().Add( EVPbkVersitParamCELL );
    prop.Parameters().Add( EVPbkVersitParamWORK );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iNumberFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try work mobile" );
        }

    prop.SetName( EVPbkVersitNameTEL );
    prop.SetSubField( EVPbkVersitSubFieldNone );
    prop.Parameters().Reset();
    prop.Parameters().Add( EVPbkVersitParamCELL );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iNumberFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try general mobile" );
        }

    prop.SetName( EVPbkVersitNameTEL );
    prop.SetSubField( EVPbkVersitSubFieldNone );
    prop.Parameters().Reset();
    prop.Parameters().Add( EVPbkVersitParamHOME );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iNumberFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try home landline" );
        }

    prop.SetName( EVPbkVersitNameTEL );
    prop.SetSubField( EVPbkVersitSubFieldNone );
    prop.Parameters().Reset();
    prop.Parameters().Add( EVPbkVersitParamWORK );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iNumberFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try work landline" );
        }

    prop.SetName( EVPbkVersitNameTEL );
    prop.SetSubField( EVPbkVersitSubFieldNone );
    prop.Parameters().Reset();
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iNumberFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try general landline" );
        }
        
    HGLOG_OUT();
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::PreCreateAddressFieldTypesL
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::PreCreateAddressFieldTypesL()
    {
    HGLOG_CONTEXT( PreCreateAddressFieldTypesL, HGLOG_LOCAL );
    HGLOG_IN();

    iAddressFieldTypes = CVPbkFieldTypeRefsList::NewL();
    const MVPbkFieldTypeList& types( FieldTypes() );
    TVPbkFieldVersitProperty prop;
    const MVPbkFieldType* t;

    prop.SetName( EVPbkVersitNameADR );
    prop.SetSubField( EVPbkVersitSubFieldCountry );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iAddressFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try general country" );
        }

    prop.SetName( EVPbkVersitNameADR );
    prop.SetSubField( EVPbkVersitSubFieldRegion );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iAddressFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try general region" );
        }

    prop.SetName( EVPbkVersitNameADR );
    prop.SetSubField( EVPbkVersitSubFieldLocality );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iAddressFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try general locality" );
        }

    prop.SetName( EVPbkVersitNameADR );
    prop.SetSubField( EVPbkVersitSubFieldStreet );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iAddressFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try general street" );
        }
        
    HGLOG_OUT();
    }


// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::PreCreateWebAddressFieldTypesL
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::PreCreateWebAddressFieldTypesL()
    {
    HGLOG_CONTEXT( PreCreateWebAddressFieldTypesL, HGLOG_LOCAL );
    HGLOG_IN();

    iWebAddressFieldTypes = CVPbkFieldTypeRefsList::NewL();
    const MVPbkFieldTypeList& types( FieldTypes() );
    TVPbkFieldVersitProperty prop;
    const MVPbkFieldType* t;

    prop.Parameters().Add( EVPbkVersitParamPREF );
    prop.SetName( EVPbkVersitNameURL );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iWebAddressFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try prefered url" );
        }        
    
    HGLOG_OUT();
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::PreCreateWebAddressFieldTypesL
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::PreCreateWebAddressHomeFieldTypesL()
    {
    HGLOG_CONTEXT( PreCreateWebAddressHomeFieldTypesL, HGLOG_LOCAL );
    HGLOG_IN();

    iWebAddressHomeFieldTypes = CVPbkFieldTypeRefsList::NewL();
    const MVPbkFieldTypeList& types( FieldTypes() );
    TVPbkFieldVersitProperty prop;
    const MVPbkFieldType* t;

    prop.Parameters().Add( EVPbkVersitParamHOME );
    prop.SetName( EVPbkVersitNameURL );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iWebAddressHomeFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try home url" );
        }   
    
    HGLOG_OUT();
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::PreCreateWebAddressFieldTypesL
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::PreCreateWebAddressWorkFieldTypesL()
    {
    HGLOG_CONTEXT( PreCreateWebAddressWorkFieldTypesL, HGLOG_LOCAL );
    HGLOG_IN();

    iWebAddressWorkFieldTypes = CVPbkFieldTypeRefsList::NewL();
    const MVPbkFieldTypeList& types( FieldTypes() );
    TVPbkFieldVersitProperty prop;
    const MVPbkFieldType* t;

    prop.Parameters().Add( EVPbkVersitParamWORK );
    prop.SetName( EVPbkVersitNameURL );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iWebAddressWorkFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try work url" );
        }      
    
    HGLOG_OUT();
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::PreCreateHomeAddressFieldTypesL
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::PreCreateHomeAddressFieldTypesL()
    {
    HGLOG_CONTEXT( PreCreateHomeAddressFieldTypesL, HGLOG_LOCAL );
    HGLOG_IN();

    iHomeAddressFieldTypes = CVPbkFieldTypeRefsList::NewL();
    const MVPbkFieldTypeList& types( FieldTypes() );
    TVPbkFieldVersitProperty prop;
    const MVPbkFieldType* t;
    prop.Parameters().Add( EVPbkVersitParamHOME );

    prop.SetName( EVPbkVersitNameADR );
    prop.SetSubField( EVPbkVersitSubFieldCountry );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iHomeAddressFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try home country" );
        }

    prop.SetName( EVPbkVersitNameADR );
    prop.SetSubField( EVPbkVersitSubFieldRegion );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iHomeAddressFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try home region" );
        }

    prop.SetName( EVPbkVersitNameADR );
    prop.SetSubField( EVPbkVersitSubFieldLocality );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iHomeAddressFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try home locality" );
        }

    prop.SetName( EVPbkVersitNameADR );
    prop.SetSubField( EVPbkVersitSubFieldStreet );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iHomeAddressFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try home street" );
        }
        
    HGLOG_OUT();
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::PreCreateWorkAddressFieldTypesL
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::PreCreateWorkAddressFieldTypesL()
    {
    HGLOG_CONTEXT( PreCreateWorkAddressFieldTypesL, HGLOG_LOCAL );
    HGLOG_IN();

    iWorkAddressFieldTypes = CVPbkFieldTypeRefsList::NewL();
    const MVPbkFieldTypeList& types( FieldTypes() );
    TVPbkFieldVersitProperty prop;
    const MVPbkFieldType* t;
    prop.Parameters().Add( EVPbkVersitParamWORK );

    prop.SetName( EVPbkVersitNameADR );
    prop.SetSubField( EVPbkVersitSubFieldCountry );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iWorkAddressFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try work country" );
        }

    prop.SetName( EVPbkVersitNameADR );
    prop.SetSubField( EVPbkVersitSubFieldRegion );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iWorkAddressFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try work region" );
        }

    prop.SetName( EVPbkVersitNameADR );
    prop.SetSubField( EVPbkVersitSubFieldLocality );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iWorkAddressFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try work locality" );
        }

    prop.SetName( EVPbkVersitNameADR );
    prop.SetSubField( EVPbkVersitSubFieldStreet );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        iWorkAddressFieldTypes->AppendL( *t );
        HGLOG0( HGLOG_INFO, "will try work street" );
        }
        
    HGLOG_OUT();
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::TryTextLookupL
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::TryTextLookupL( const TDesC& aName,
        CVPbkContactLinkArray& aLinkArray )
    {
    HGLOG_CONTEXT( TryTextLookupL, HGLOG_LOCAL );
    HGLOG_IN();

    CVPbkFieldTypeRefsList* fieldTypes = CVPbkFieldTypeRefsList::NewL();
    CleanupStack::PushL( fieldTypes );

    // try name and email and ovi id fields
    if ( !iNameFieldTypes )
        {
        PreCreateNameFieldTypesL();
        }
    for ( TInt i = 0, ie = iNameFieldTypes->FieldTypeCount(); i != ie; ++i )
        {
        fieldTypes->AppendL( iNameFieldTypes->FieldTypeAt( i ) );
        }
    if ( !iEmailFieldTypes )
        {
        PreCreateEmailFieldTypesL();
        }
    for ( TInt i = 0, ie = iEmailFieldTypes->FieldTypeCount(); i != ie; ++i )
        {
        fieldTypes->AppendL( iEmailFieldTypes->FieldTypeAt( i ) );
        }
    if ( !iXspIdFieldTypes )
        {
        PreCreateXspIdFieldTypesL();
        }
    for ( TInt i = 0, ie = iXspIdFieldTypes->FieldTypeCount(); i != ie; ++i )
        {
        fieldTypes->AppendL( iXspIdFieldTypes->FieldTypeAt( i ) );
        }

    SplitAndMatchL( aName, *fieldTypes, aLinkArray );

    CleanupStack::PopAndDestroy( fieldTypes );
    HGLOG_OUT();
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::TryNumberLookupL
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::TryNumberLookupL( const TDesC& aNumber,
        CVPbkContactLinkArray& aLinkArray )
    {
    HGLOG_CONTEXT( TryNumberLookupL, HGLOG_LOCAL );
    HGLOG_IN();
    
    MatchPhoneNumberL( aNumber,
        KNumberMatchLenFromRight,
        CVPbkPhoneNumberMatchStrategy::EVPbkMatchFlagsNone,
        aLinkArray );
        
    HGLOG_OUT();
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::GetTextFieldsL
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::GetTextFieldsL(
        const CVPbkFieldTypeRefsList& aList,
        const MVPbkStoreContactFieldCollection& aFieldCollection,
        CDesCArray& aArray )
    {
    HGLOG_CONTEXT( GetTextFieldsL, HGLOG_LOCAL );
    HGLOG_IN();
        
    for ( TInt i = 0, ie = aFieldCollection.FieldCount(); i != ie; ++i )
        {
        const MVPbkStoreContactField& field( aFieldCollection.FieldAt( i ) );
        const MVPbkFieldType* type = field.BestMatchingFieldType();
        if ( type && aList.ContainsSame( *type ) )
            {
            const MVPbkContactFieldData& fdata( field.FieldData() );
            if ( fdata.DataType() == EVPbkFieldStorageTypeText )
                {
                const MVPbkContactFieldTextData& fdata2 =
                    MVPbkContactFieldTextData::Cast( fdata );
                const TDesC& text( fdata2.Text() );
                aArray.AppendL( text );
                HGLOG1( HGLOG_INFO, "found: '%S'", &text );
                }
            }
        }
        
    HGLOG_OUT();
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::GetNumbersL
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::GetNumbersL(
        MVPbkStoreContactFieldCollection& aFieldCollection,
        CDesCArray& aArray )
    {
    HGLOG_CONTEXT( GetNumbersL, HGLOG_LOCAL );
    HGLOG_IN();

    if ( !iNumberFieldTypes )
        {
        PreCreateNumberFieldTypesL();
        }
    GetTextFieldsL( *iNumberFieldTypes, aFieldCollection, aArray );

    HGLOG_OUT();
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::GetEmailAddressesL
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::GetEmailAddressesL(
        MVPbkStoreContactFieldCollection& aFieldCollection,
        CDesCArray& aArray )
    {
    HGLOG_CONTEXT( GetEmailAddressesL, HGLOG_LOCAL );
    HGLOG_IN();

    if ( !iEmailFieldTypes )
        {
        PreCreateEmailFieldTypesL();
        }
    GetTextFieldsL( *iEmailFieldTypes, aFieldCollection, aArray );

    HGLOG_OUT();
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::GetAddressesL
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::GetAddressesL(
        MVPbkStoreContactFieldCollection& aFieldCollection,
        CDesCArray& aArray )
    {
    HGLOG_CONTEXT( GetAddressesL, HGLOG_LOCAL );
    HGLOG_IN();
    
    CDesC16ArrayFlat* arr = new ( ELeave ) CDesC16ArrayFlat( KArrayGranularity );
    CleanupStack::PushL( arr );

    if ( !iAddressFieldTypes )
        {
        PreCreateAddressFieldTypesL();
        }
    GetTextFieldsL( *iAddressFieldTypes, aFieldCollection, *arr );
    if ( arr->Count() )
        {
        HBufC* combined = CombineStringsLC( *arr );
        aArray.AppendL( *combined );
        HGLOG1( HGLOG_INFO, "added '%S'", combined );
        CleanupStack::PopAndDestroy( combined );
        }

    arr->Reset();
    if ( !iHomeAddressFieldTypes )
        {
        PreCreateHomeAddressFieldTypesL();
        }
    GetTextFieldsL( *iHomeAddressFieldTypes, aFieldCollection, *arr );
    if ( arr->Count() )
        {
        HBufC* combined = CombineStringsLC( *arr );
        aArray.AppendL( *combined );
        HGLOG1( HGLOG_INFO, "added '%S'", combined );
        CleanupStack::PopAndDestroy( combined );
        }

    arr->Reset();
    if ( !iWorkAddressFieldTypes )
        {
        PreCreateWorkAddressFieldTypesL();
        }
    GetTextFieldsL( *iWorkAddressFieldTypes, aFieldCollection, *arr );
    if ( arr->Count() )
        {
        HBufC* combined = CombineStringsLC( *arr );
        aArray.AppendL( *combined );
        HGLOG1( HGLOG_INFO, "added '%S'", combined );
        CleanupStack::PopAndDestroy( combined );
        }

    CleanupStack::PopAndDestroy( arr );
    HGLOG_OUT();
    }


// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::GetWebAddressesL
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::GetWebAddressesL(
        MVPbkStoreContactFieldCollection& aFieldCollection,
        CDesCArray& aArray,
        TWebAddressesType aType  )
    {
    HGLOG_CONTEXT( GetWebAddressesL, HGLOG_LOCAL );
    HGLOG_IN();
    CVPbkFieldTypeRefsList* addressFieldTypes( NULL );
    
    switch ( aType )
        {
        case EWebAddresses:
            {
            if ( !iWebAddressFieldTypes )
               {
               PreCreateWebAddressFieldTypesL();
               }
            addressFieldTypes = iWebAddressFieldTypes;
            }
            break;
            
        case EWebAddressesHome:
            {
            if ( !iWebAddressHomeFieldTypes )
               {
               PreCreateWebAddressHomeFieldTypesL();
               }
            addressFieldTypes = iWebAddressHomeFieldTypes;
            }
            break;

        case EWebAddressesWork:
            {
            if ( !iWebAddressWorkFieldTypes )
                {
                PreCreateWebAddressWorkFieldTypesL();
                }
            addressFieldTypes = iWebAddressWorkFieldTypes;
            }
            break;
            
        default:
            break;
        }
    
    if( addressFieldTypes )
        {
        GetTextFieldsL( *addressFieldTypes, aFieldCollection, aArray );
        }
    
    HGLOG_OUT();
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::GetThumbnailL
// ----------------------------------------------------------------------------
EXPORT_C CFbsBitmap* CHgCtxContactMatcher::GetThumbnailL(
        MVPbkStoreContactFieldCollection& aFieldCollection )
    {
    HGLOG_CONTEXT( GetThumbnailL, HGLOG_LOCAL );
    HGLOG_IN();
    
    CFbsBitmap* result = 0;
    TVPbkFieldVersitProperty prop;
    prop.SetName( EVPbkVersitNamePHOTO );
    const MVPbkFieldType* t = FieldTypes().FindMatch( prop, 0 );
    if ( t )
        {
        HGLOG0( HGLOG_INFO, "photo field type found" );
        for ( TInt i = 0, ie = aFieldCollection.FieldCount(); i != ie; ++i )
            {
            const MVPbkStoreContactField& field( aFieldCollection.FieldAt( i ) );
            const MVPbkFieldType* type = field.BestMatchingFieldType();
            if ( type && type->IsSame( *t ) )
                {
                const MVPbkContactFieldData& fdata( field.FieldData() );
                if ( fdata.DataType() == EVPbkFieldStorageTypeBinary )
                    {
                    HGLOG0( HGLOG_INFO, "found thumbnail" );
                    const MVPbkContactFieldBinaryData& fdata2 =
                        MVPbkContactFieldBinaryData::Cast( fdata );
                    TPtrC8 data( fdata2.BinaryData() );
                    CImageDecoder* decoder = 0;
                    // DataNewL does not seem to work properly with
                    // EOptionAlwaysThread, it will hang in WaitForRequest
                    // for ever, at least in the panel app.
                    // So write the image to a temporary file (duhhh...)
                    // and use FileNewL.
                    RFile f;
                    TFileName tempFileName;
                    iFsSession->CreatePrivatePath( EDriveC );
                    iFsSession->PrivatePath( tempFileName );
                    _LIT( KDriveC, "C:" );
                    _LIT( KTempName, "hgctxthumb" );
                    tempFileName.Insert( 0, KDriveC );
                    tempFileName.Append( KTempName );
                    HGLOG1( HGLOG_INFO, "tempfn='%S'", &tempFileName );
                    User::LeaveIfError( f.Replace( *iFsSession, tempFileName,
                                    EFileWrite ) );
                    f.Write( data, data.Length() );
                    f.Close();
                    TRAPD( err, decoder = CImageDecoder::FileNewL( *iFsSession,
                                    tempFileName,
                                    CImageDecoder::EOptionAlwaysThread ) );
                    HGLOG1( HGLOG_INFO, "decoder NewL result: %d", err );
                    if ( err == KErrNone )
                        {
                        CleanupStack::PushL( decoder );
                        result = new ( ELeave ) CFbsBitmap;
                        CleanupStack::PushL( result );
                        TSize sz( decoder->FrameInfo().iOverallSizeInPixels );
                        TDisplayMode mode( decoder->FrameInfo().iFrameDisplayMode );
                        HGLOG3( HGLOG_INFO, "size=%dx%d, mode=%d", sz.iWidth,
                                sz.iHeight, mode );
                        User::LeaveIfError( result->Create( sz, mode ) );
        
                        TRequestStatus status;
                        HGLOG0( HGLOG_INFO, "starting to convert" );
                        decoder->Convert( &status, *result );
                        User::WaitForRequest( status );
                        HGLOG1( HGLOG_INFO, "decoder Convert result: %d",
                                status.Int() );
                        CleanupStack::Pop( result );
                        CleanupStack::PopAndDestroy( decoder );
        
                        if ( status.Int() != KErrNone )
                            {
                            delete result;
                            result = 0;
                            }
                        else
                            {
                            // stop and return the bitmap
                            break;
                            }
                        }                    
                    }
                }
            }
        }
        
    HGLOG_OUT();
    return result;
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::IsPhoneNumberL
// ----------------------------------------------------------------------------
EXPORT_C TBool CHgCtxContactMatcher::IsPhoneNumberL( const TDesC& aData )
    {
    TBool result = EFalse;
    CFindItemEngine::SFoundItem item;
    CFindItemEngine* search = CFindItemEngine::NewL( aData,
        CFindItemEngine::EFindItemSearchPhoneNumberBin );
    if ( search->ItemCount() )
        {
        search->Item( item );
        result = item.iStartPos == 0;
        }
    delete search;
    return result;
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::IsEmailAddressL
// ----------------------------------------------------------------------------
EXPORT_C TBool CHgCtxContactMatcher::IsEmailAddressL( const TDesC& aData )
    {
    TBool result = EFalse;
    CFindItemEngine::SFoundItem item;
    CFindItemEngine* search = CFindItemEngine::NewL( aData,
        CFindItemEngine::EFindItemSearchMailAddressBin );
    if ( search->ItemCount() )
        {
        search->Item( item );
        result = item.iStartPos == 0;
        }
    delete search;
    return result;
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::GetNamesForFindL
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::GetNamesForFindL(
        MVPbkStoreContactFieldCollection& aFieldCollection,
        CDesCArray& aArray )
    {
    CVPbkFieldTypeRefsList* nameTypes = CVPbkFieldTypeRefsList::NewL();
    CleanupStack::PushL( nameTypes );
    const MVPbkFieldTypeList& types( FieldTypes() );
    TVPbkFieldVersitProperty prop;
    const MVPbkFieldType* t;
    
    prop.SetName( EVPbkVersitNameN );
    prop.SetSubField( EVPbkVersitSubFieldGivenName );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        nameTypes->AppendL( *t );
        }
    
    prop.SetName( EVPbkVersitNameN );
    prop.SetSubField( EVPbkVersitSubFieldFamilyName );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        nameTypes->AppendL( *t );
        }
    
    for ( TInt i = 0, ie = aFieldCollection.FieldCount(); i != ie; ++i )
        {
        const MVPbkStoreContactField& field( aFieldCollection.FieldAt( i ) );
        t = field.MatchFieldType( 0 );
        if ( t && nameTypes->ContainsSame( *t ) )
            {
            const MVPbkContactFieldData& fdata( field.FieldData() );
            if ( fdata.DataType() == EVPbkFieldStorageTypeText )
                {
                const MVPbkContactFieldTextData& fdata2 =
                    MVPbkContactFieldTextData::Cast( fdata );
                aArray.AppendL( fdata2.Text() );
                }
            }
        }

    CleanupStack::PopAndDestroy( nameTypes );
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::GetNamesForFindL
// ----------------------------------------------------------------------------
EXPORT_C HBufC* CHgCtxContactMatcher::GetNamesForFindL(
        MVPbkStoreContactFieldCollection& aFieldCollection )
    {
    CDesC16ArrayFlat* arr = new ( ELeave ) CDesC16ArrayFlat( KArrayGranularity );
    CleanupStack::PushL( arr );
    GetNamesForFindL( aFieldCollection, *arr );
    HBufC* result = CombineStringsLC( *arr );
    CleanupStack::Pop( result );
    CleanupStack::PopAndDestroy( arr );
    return result;
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::SplitFindStringL
// ----------------------------------------------------------------------------
EXPORT_C CDesCArray* CHgCtxContactMatcher::SplitFindStringL(const TDesC& aFindString)
    {
	CDesCArray* wordArray = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
	CleanupStack::PushL( wordArray );	
	
    TVPbkWordParserCallbackParam parser( &aFindString, wordArray );
	FindWordSplitterL( &parser );
	
	CleanupStack::Pop(); // wordArray
	return parser.iWordArray;
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::SplitMsgContactL
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::SplitMsgContactL( const TDesC& aString,
        CDesCArray& aArray )
    {
    TInt pos = aString.Locate( '<' );
    if ( pos >= 0 )
        {
        // skip spaces before '<'
        TInt endPos = pos - 1;
        while ( endPos > 0 && TChar( aString[endPos] ).IsSpace() )
            {
            --endPos;
            }
        // take the text before '<'
        aArray.AppendL( aString.Left( endPos + 1 ) );
        // take the text between '<' and '>'
        TInt closePos = aString.Locate( '>' );
        if ( closePos > pos )
            {
            aArray.AppendL( aString.Mid( pos + 1, closePos - pos - 1 ) );
            }
        }
    else
        {
        aArray.AppendL( aString );
        }
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::GetCustomFieldL
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::GetCustomFieldL(
        MVPbkStoreContactFieldCollection& aFieldCollection,
        CDesCArray& aArray,
        TVPbkFieldTypeName aVersitName,
        TVPbkFieldTypeParameter aVersitParam )
    {
    HGLOG_CONTEXT( GetCustomFieldL, HGLOG_LOCAL );
    HGLOG_IN();
    
    CVPbkFieldTypeRefsList* typeList = GetCustomFieldTypeLC(
        aVersitName, aVersitParam );

    GetTextFieldsL( *typeList, aFieldCollection, aArray );
    
    CleanupStack::PopAndDestroy( typeList );
    HGLOG_OUT();
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::GetCustomFieldTypeLC
// ----------------------------------------------------------------------------
EXPORT_C CVPbkFieldTypeRefsList* CHgCtxContactMatcher::GetCustomFieldTypeLC(
        TVPbkFieldTypeName aVersitName,
        TVPbkFieldTypeParameter aVersitParam )
    {
    HGLOG_CONTEXT( GetCustomFieldTypeLC, HGLOG_LOCAL );
    HGLOG_IN();
    
    CVPbkFieldTypeRefsList* typeList = CVPbkFieldTypeRefsList::NewL();
    CleanupStack::PushL( typeList );
    const MVPbkFieldTypeList& types( FieldTypes() );
    TVPbkFieldVersitProperty prop;
    const MVPbkFieldType* t;

    prop.SetName( aVersitName );
    prop.Parameters().Add( aVersitParam );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        typeList->AppendL( *t );
        HGLOG0( HGLOG_INFO, "field found" );
        }

    HGLOG_OUT();
    return typeList;
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::GetImppFieldL
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::GetImppFieldL(
        MVPbkStoreContactFieldCollection& aFieldCollection,
        CDesCArray* aSchemeOnlyArray,
        CDesCArray* aUriOnlyArray,
        CDesCArray* aFullArray )
    {
    HGLOG_CONTEXT( GetImppFieldL, HGLOG_LOCAL );
    HGLOG_IN();

    // this function will not build on TUBE
    CVPbkFieldTypeRefsList* typeList = CVPbkFieldTypeRefsList::NewL();
    CleanupStack::PushL( typeList );
    const MVPbkFieldTypeList& types( FieldTypes() );
    TVPbkFieldVersitProperty prop;
    const MVPbkFieldType* t;
    prop.SetName( EVPbkVersitNameIMPP );
    t = types.FindMatch( prop, 0 );
    if ( t )
        {
        typeList->AppendL( *t );
        HGLOG0( HGLOG_INFO, "type found" );
        }
    for ( TInt i = 0, ie = aFieldCollection.FieldCount(); i != ie; ++i )
        {
        const MVPbkStoreContactField& field( aFieldCollection.FieldAt( i ) );
        const MVPbkFieldType* type = field.BestMatchingFieldType();
        if ( type && typeList->ContainsSame( *type ) )
            {
            const MVPbkContactFieldData& fdata( field.FieldData() );
            HGLOG1( HGLOG_INFO, "field found %d", fdata.DataType() );
            if ( fdata.DataType() == EVPbkFieldStorageTypeUri )
                {
                const MVPbkContactFieldUriData& fdata2 =
                    MVPbkContactFieldUriData::Cast( fdata );
                const TDesC& schemeOnly( fdata2.Scheme() );
                const TDesC& uriOnly( fdata2.Text() );
                const TDesC& fullText( fdata2.Uri() );
                HGLOG3( HGLOG_INFO, "'%S' + '%S' = '%S'",
                    &schemeOnly, &uriOnly, &fullText );
                if ( aSchemeOnlyArray )
                    {
                    aSchemeOnlyArray->AppendL( schemeOnly );
                    }
                if ( aUriOnlyArray )
                    {
                    aUriOnlyArray->AppendL( uriOnly );
                    }
                if ( aFullArray )
                    {
                    aFullArray->AppendL( fullText );
                    }
                }
            }
        }                    
    CleanupStack::PopAndDestroy( typeList );
    
    HGLOG_OUT();
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::FindContactWithBirthdayL
// ----------------------------------------------------------------------------
EXPORT_C void CHgCtxContactMatcher::FindContactWithBirthdayL(
        const TTime& aDate,
        CVPbkContactLinkArray& aLinkArray )
    {
    HGLOG_CONTEXT( FindContactWithBirthdayL, HGLOG_LOCAL );
    HGLOG1_IN( "%Ld", aDate.Int64() );

    // extract month and day
    TDateTime dt = aDate.DateTime();
    TInt month = dt.Month();
    TInt day = dt.Day();
    HGLOG2( HGLOG_INFO, "wanted month = %d day = %d", month, day );

    CVPbkFieldTypeRefsList* emptyList = CVPbkFieldTypeRefsList::NewL();
    CleanupStack::PushL( emptyList );

    // create view with all contacts
    CVPbkContactViewDefinition* def = CVPbkContactViewDefinition::NewLC();
    def->SetType( EVPbkContactsView );
    def->SetUriL( VPbkContactStoreUris::DefaultCntDbUri() );
    MVPbkContactViewBase* view = iContactManager->CreateContactViewLC(
        *this, *def, *emptyList );

    HGLOG0( HGLOG_INFO, "starting wait" );
    iASchedulerWait.Start();
    HGLOG0( HGLOG_INFO, "after wait" );

    // view is ready
    TInt ctCount = view->ContactCountL();
    HGLOG1( HGLOG_INFO, "contact count: %d", ctCount );

    // find the birthday field type
    const MVPbkFieldTypeList& types( FieldTypes() );
    TVPbkFieldVersitProperty prop;
    const MVPbkFieldType* bdayFt;
    prop.SetName( EVPbkVersitNameBDAY );
    prop.SetSubField( EVPbkVersitSubFieldNone );
    bdayFt = types.FindMatch( prop, 0 );

    if ( bdayFt && ctCount )
        {
        HGLOG0( HGLOG_INFO, "found bday field type" );
        TTime nullTime( KNullTime );
        // go through all contacts and check birthday field values
        for ( TInt i = 0; i < ctCount; ++i )
            {
            MVPbkContactLink* link( view->CreateLinkLC( i ) );
            MVPbkStoreContact* contact = 0;
            GetStoreContactL( *link, &contact );
            if ( contact )
                {
                HGLOG1( HGLOG_INFO, "got contact, idx %d", i );
                contact->PushL();
                TTime bday;
                bday = GetFieldDataDateTimeL( *contact, *bdayFt );
                if ( bday != nullTime )
                    {
                    HGLOG1( HGLOG_INFO, "found bday %Ld", bday.Int64() );
                    dt = bday.DateTime();
                    TInt thisMonth = dt.Month();
                    TInt thisDay = dt.Day();
                    HGLOG2( HGLOG_INFO, "for this contact month = %d day = %d",
                        thisMonth, thisDay );
                    if ( thisMonth == month && thisDay == day )
                        {
                        HGLOG0( HGLOG_INFO, "match" );
                        aLinkArray.AppendL( link );
                        link = 0;
                        }
                    }
                CleanupStack::PopAndDestroy(); // contact
                }
            CleanupStack::Pop(); // if matched then no ownership and link is NULL by now
            delete link;
            }
        }

    CleanupStack::PopAndDestroy(); // view
    CleanupStack::PopAndDestroy( def );
    CleanupStack::PopAndDestroy( emptyList );

    HGLOG_OUT();
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::ContactViewReady
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::ContactViewReady(
        MVPbkContactViewBase& aView ) 
    {
    HGLOG_CONTEXT( ContactViewReady, HGLOG_LOCAL );
    HGLOG_IN();

    if ( iASchedulerWait.IsStarted() )
        {
        iASchedulerWait.AsyncStop();
        }

    aView.RemoveObserver( *this ); 

    HGLOG_OUT();
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::ContactViewUnavailable
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::ContactViewUnavailable(
        MVPbkContactViewBase& /*aView*/ )
    {
    // means that view is unavailable for now
    // but ContactViewReady will be called at some point
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::ContactAddedToView
// ----------------------------------------------------------------------------
void CHgCtxContactMatcher::ContactAddedToView(
        MVPbkContactViewBase& /*aView*/, 
        TInt /*aIndex*/,
        const MVPbkContactLink& /*aContactLink*/ )
    {
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::ContactRemovedFromView
// ----------------------------------------------------------------------------
 void CHgCtxContactMatcher::ContactRemovedFromView(
        MVPbkContactViewBase& /*aView*/, 
        TInt /*aIndex*/, 
        const MVPbkContactLink& /*aContactLink*/ )
    {
    }

// ----------------------------------------------------------------------------
// CHgCtxContactMatcher::ContactViewError
// ----------------------------------------------------------------------------
 void CHgCtxContactMatcher::ContactViewError(
        MVPbkContactViewBase& aView, 
        TInt aError, 
        TBool /*aErrorNotified*/ )
    {
    HGLOG_CONTEXT( ContactViewError, HGLOG_LOCAL );
    HGLOG1_IN( "aError = %d", aError );

    if ( iASchedulerWait.IsStarted() )
        {
        iASchedulerWait.AsyncStop();
        }

    aView.RemoveObserver( *this ); 
    
    HGLOG_OUT();
    }


// End of File
