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
* Description:  Context publishing helper dll
*
*/


#include <cfcontextobject.h>
#include <cfclient.h>
#include <mdesession.h>
#include <mdeobject.h>
#include <s32mem.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <MVPbkStreamable.h>
#include <CVPbkContactLinkArray.h>
#include <e32debug.h>
#include <w32std.h>
#include "hgcontextutilityimpl.h"
#include "hgcontexttypes.h"

// max number of entries processed when aContextData is an array in PublishContextL
const TInt KMaxEntriesInMulti = 20;

// separator character in combined string for multiple entries
const TInt KMultiSepChar = 0x0001;

// granularity for string array
const TInt KArrayGranularity = 4;

// argument for CBufFlat ctor when serializing contact links
const TInt KBufGranularity = 64;

// default security policy (use LocalServices cap) for contexts
_LIT_SECURITY_POLICY_C1( KContextSecurity, ECapabilityLocalServices );

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::NewL
// -----------------------------------------------------------------------------
//
CHgContextUtilityImpl* CHgContextUtilityImpl::NewL()
    {
    CHgContextUtilityImpl* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::NewLC
// -----------------------------------------------------------------------------
//
CHgContextUtilityImpl* CHgContextUtilityImpl::NewLC()
    {
    CHgContextUtilityImpl* self = new ( ELeave ) CHgContextUtilityImpl;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::CHgContextUtilityImpl
// -----------------------------------------------------------------------------
//
CHgContextUtilityImpl::CHgContextUtilityImpl()
        : CTimer( CActive::EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::ConstructL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::ConstructL()
    {
    CTimer::ConstructL();

    iEnv = CCoeEnv::Static(); // may be NULL

    // Do not create iCFClient here as cf server may not be available yet
    // if we are early in the boot phase.
    
    // set defaults
    RePublishWhenFgL( EFalse );
    AllowPublishFromBackground( EFalse );
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::~CHgContextUtilityImpl
// -----------------------------------------------------------------------------
//
CHgContextUtilityImpl::~CHgContextUtilityImpl()
    {
    Cancel();
    delete iPendingContextType;
    delete iPendingContextData;
    delete iPendingContextDataArray;
    delete iCFClient;
    delete iLastContextType;
    delete iLastContextData;
    if ( iFgWatchEnabled && iEnv )
        {
        iEnv->RemoveForegroundObserver( *this );
        }
    
    iMusicContextInfo.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::CFReady
// -----------------------------------------------------------------------------
//
TBool CHgContextUtilityImpl::CFReady()
    {
    if ( !iCFClient )
        {
        TRAPD( err, iCFClient = CCFClient::NewL( *this ) );
        if ( err != KErrNone )
            {
            RDebug::Printf( "[hgctxutil] cfw not ready" );
            return EFalse;
            }
        }
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishContextL
// All other non-static versions of this function will fall back to this one.
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::PublishContextL( const TDesC& aContextType,
        const TDesC& aContextData )
    {
    RDebug::Print( _L("[hgctxutil] PublishContextL [%S] [%S]"),
        &aContextType, &aContextData );
    // create cf client instance if not yet done
    // and check foreground status if needed
    if ( CFReady() && AllowedToPublish() )
        {
        // call static version with our cf client instance
        PublishContextL( *iCFClient, aContextType, aContextData );
        }
    // store type and value for later use
    // (even when cfserver is not available yet, the data may still be
    //  used later when the app comes to foreground, for example)
    if ( iLastContextType != &aContextType )
        {
        delete iLastContextType; iLastContextType = 0;
        iLastContextType = aContextType.AllocL();
        }
    if ( iLastContextData != &aContextData )
        {
        delete iLastContextData; iLastContextData = 0;
        iLastContextData = aContextData.AllocL();
        }
    }

// -----------------------------------------------------------------------------
// AppendCharL
// Appends a char to aDst, calls ReAllocL when needed, assumes that aDst
// is also on cleanupstack (at top position) so it updates that pointer too.
// -----------------------------------------------------------------------------
//
LOCAL_C void AppendCharL( HBufC*& aDst, TChar aChar )
    {
    TPtr des( aDst->Des() );
    if ( des.Length() == des.MaxLength() )
        {
        HBufC* oldDst = aDst;
        aDst = aDst->ReAllocL( des.MaxLength() * 2 );
        CleanupStack::Pop( oldDst ); // pop the old pointer
        CleanupStack::PushL( aDst ); // and push the new (possibly different) one
        des.Set( aDst->Des() );
        }
    des.Append( aChar );
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::BuildCombinedStringL
// -----------------------------------------------------------------------------
//
HBufC* CHgContextUtilityImpl::BuildCombinedStringL(
        const MDesCArray& aArray )
    {
    TInt arrayCount = aArray.MdcaCount();
    if ( arrayCount >= 2 )
        {
        // Rules:
        // 1. escape all separator chars in the input with a preceeding \
        // 2. escape all \ chars in the input with \\
        // 3. take only the first KMaxEntriesInMulti elements from the array
        // 4. append a separator also after last entry
        // 5. prepend two separators to the combined string
        TInt processedEntryCount = Min( arrayCount, KMaxEntriesInMulti );
        // calculate a big enough size so we can avoid ReAllocL calls later
        TInt sz = 0;
        for ( TInt i = 0; i < processedEntryCount; ++i )
            {
            sz += aArray.MdcaPoint( i ).Length() + 1;
            }
        sz += 2; // for the magic prefix
        HBufC* value = HBufC::NewLC( sz );
        AppendCharL( value, KMultiSepChar );
        AppendCharL( value, KMultiSepChar );
        for ( TInt i = 0; i < processedEntryCount; ++i )
            {
            TPtrC entry( aArray.MdcaPoint( i ) );
            // append, also do the escaping
            for ( TInt j = 0, je = entry.Length(); j != je; ++j )
                {
                TChar c = entry[j];
                if ( c == KMultiSepChar || c == '\\' )
                    {
                    AppendCharL( value, '\\' );
                    }
                AppendCharL( value, c );
                }
            AppendCharL( value, KMultiSepChar );
            }
        CleanupStack::Pop( value );
        return value;
        }
    return 0;
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::SplitCombinedStringL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::SplitCombinedStringL(
        const TDesC& aString, CDesCArray& aArray )
    {
    TInt inputLength = aString.Length();
    TBool isMulti = inputLength > 2
        && aString[0] == KMultiSepChar && aString[1] == KMultiSepChar;
    if ( isMulti )
        {
        // allocate a work buffer that is big enough for sure
        HBufC* buf = HBufC::NewLC( inputLength );
        TPtr des( buf->Des() );
        TBool esc = EFalse;
        // go through the string, find entries, and add them to output array
        for ( TInt i = 2; i < inputLength; ++i ) // start from 2 because of the magic prefix
            {
            TChar c = aString[i];
            if ( c == '\\' && !esc )
                {
                esc = ETrue;
                }
            else if ( c == KMultiSepChar && !esc )
                {
                // found separator: append to output array, clear buffer, and continue
                aArray.AppendL( des );
                des.Zero();
                }
            else
                {
                esc = EFalse;
                des.Append( c );
                }
            }
        CleanupStack::PopAndDestroy( buf );
        }
    else
        {
        // not a combined string: append to array as it is
        aArray.AppendL( aString );
        }
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishContextL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::PublishContextL( const TDesC& aContextType,
        const MDesCArray& aContextData )
    {
    TInt entryCount = aContextData.MdcaCount();
    // do nothing if array is empty
    if ( !entryCount )
        {
        return;
        }
    // nothing special when having only 1 item
    if ( entryCount == 1 )
        {
        PublishContextL( aContextType, aContextData.MdcaPoint( 0 ) );
        return;
        }
    // at least two items: create the special combined string
    HBufC* value = BuildCombinedStringL( aContextData );
    CleanupStack::PushL( value );
    // publish the combined string
    PublishContextL( aContextType, *value );
    CleanupStack::PopAndDestroy( value );
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishContextL
// This is the version of the function where the real work is performed.
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::PublishContextL( CCFClient& aCFClient,
        const TDesC& aContextType, const TDesC& aContextData )
    {
    CCFContextObject* context = CCFContextObject::NewLC();
    context->SetSourceL( KHgCFSource );
    context->SetTypeL( aContextType );
    context->SetValueL( aContextData );
    TInt err = aCFClient.PublishContext( *context );
    if ( err == KErrNotFound )
        {
        User::LeaveIfError( aCFClient.DefineContext( KHgCFSource,
            aContextType, KContextSecurity ) );
        err = aCFClient.PublishContext( *context );
        if ( err != KErrArgument ) // ignore -6 which comes e.g. when trying to publish an empty value
            {
            User::LeaveIfError( err );
            }
        }
    else if ( err != KErrArgument )
        {
        User::LeaveIfError( err );
        }
    CleanupStack::PopAndDestroy( context );
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::GetContextL
// -----------------------------------------------------------------------------
//
HBufC* CHgContextUtilityImpl::GetContextL( const TDesC& aContextSource,
        const TDesC& aContextType )
    {
    HBufC* ret = 0;
    if ( CFReady() )
        {
        CCFContextQuery* query = CCFContextQuery::NewLC();
        query->SetSourceL( aContextSource );
        query->SetTypeL( aContextType );
        RContextObjectArray result;
        TInt err = iCFClient->RequestContext( *query, result );
        if ( err == KErrNone && result.Count() )
            {
            ret = result[0]->Value().Alloc();
            }
        result.ResetAndDestroy();
        CleanupStack::PopAndDestroy( query );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::GetContextL
// -----------------------------------------------------------------------------
//
HBufC* CHgContextUtilityImpl::GetContextL( const TDesC& aContextType )
    {
    return GetContextL( KHgCFSource, aContextType );
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishContextDelayedL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::PublishContextDelayedL( const TDesC& aContextType,
        const TDesC& aContextData, TTimeIntervalMicroSeconds32 aDelay )
    {
    Cancel();
    delete iPendingContextType; iPendingContextType = 0;
    iPendingContextType = aContextType.AllocL();
    delete iPendingContextData; iPendingContextData = 0;
    iPendingContextData = aContextData.AllocL();
    delete iPendingContextDataArray; iPendingContextDataArray = 0;
    After( aDelay );
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishContextDelayedL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::PublishContextDelayedL( const TDesC& aContextType,
        const MDesCArray& aContextData, TTimeIntervalMicroSeconds32 aDelay )
    {
    Cancel();
    delete iPendingContextType; iPendingContextType = 0;
    iPendingContextType = aContextType.AllocL();
    delete iPendingContextData; iPendingContextData = 0;
    if ( iPendingContextDataArray )
        {
        iPendingContextDataArray->Reset();
        }
    else
        {
        iPendingContextDataArray = new ( ELeave ) CDesC16ArrayFlat( KArrayGranularity );
        }
    for ( TInt i = 0, ie = aContextData.MdcaCount(); i != ie; ++i )
        {
        iPendingContextDataArray->AppendL( aContextData.MdcaPoint( i ) );
        }
    After( aDelay );
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::RunL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::RunL()
    {
    if ( iPendingContextType )
        {
        if ( iPendingContextData )
            {
            PublishContextL( *iPendingContextType, *iPendingContextData );
            }
        else if ( iPendingContextDataArray )
            {
            PublishContextL( *iPendingContextType, *iPendingContextDataArray );
            }
        }
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::RunError
// -----------------------------------------------------------------------------
//
TInt CHgContextUtilityImpl::RunError( TInt /*aError*/ )
    {
    return KErrNone;
    }

// empty implementations for cfw

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::ContextIndicationL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::ContextIndicationL(
		const CCFContextIndication& /*aChangedContext*/ )
	{
	// empty
	}

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::ActionIndicationL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::ActionIndicationL(
		const CCFActionIndication& /*aActionToExecute*/ )
	{
	// empty
	}

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::HandleContextFrameworkError
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::HandleContextFrameworkError( TCFError /*aError*/,
	    const TDesC& /*aSource*/,
	    const TDesC& /*aType*/ )
	{
	// empty
	}

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::Extension
// -----------------------------------------------------------------------------
//
TAny* CHgContextUtilityImpl::Extension( const TUid& /*aExtensionUid*/ ) const
	{
	return 0;
	}

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::MakeLinkPublishableLC
// -----------------------------------------------------------------------------
//
HBufC* CHgContextUtilityImpl::MakeLinkPublishableLC(
        const MVPbkContactLink& aLink )
    {
    HBufC* ret = 0;
    // serialize the link and place it into a 16-bit descriptor
    // prefixed with one special mark character
    const MVPbkStreamable* strm = aLink.Streamable();
    User::LeaveIfNull(strm);
    CBufFlat* buf = CBufFlat::NewL( KBufGranularity );
    CleanupStack::PushL( buf );
    RBufWriteStream ws;
    CleanupClosePushL( ws );
    ws.Open( *buf );
    strm->ExternalizeL( ws );
    CleanupStack::PopAndDestroy( &ws );
    TPtr8 p( buf->Ptr( 0 ) );
    ret = HBufC::NewLC( p.Length() + 1 );
    TPtr des( ret->Des() );
    des.Copy( p );
    _LIT( KTemp, " " );
    des.Insert( 0, KTemp );
    des[0] = KHgCFValueLinkMarker; // codescanner::accessArrayElementWithoutCheck2
    CleanupStack::Pop( ret );
    CleanupStack::PopAndDestroy( buf );
    CleanupStack::PushL( ret );
    return ret;
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishContactContextL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::PublishContactContextL(
        const MVPbkStoreContact& aContact,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    MVPbkContactLink* link = aContact.CreateLinkLC();
    if ( link )
        {
        HBufC* pubstr = MakeLinkPublishableLC( *link );
        PublishContactContextL( *pubstr, aDelay );
        CleanupStack::PopAndDestroy( pubstr );
        }
    CleanupStack::PopAndDestroy( );//link
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishContactContextL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::PublishContactContextL(
        const MVPbkContactLink& aContactLink,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    HBufC* pubstr = MakeLinkPublishableLC( aContactLink );
    PublishContactContextL( *pubstr, aDelay );
    CleanupStack::PopAndDestroy( pubstr );
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishContactContextL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::PublishContactContextL(
        const TDesC& aContactName,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    if ( !aDelay.Int() )
        {
        PublishContextL( KHgCFTypeContact, aContactName );
        }
    else
        {
        PublishContextDelayedL( KHgCFTypeContact, aContactName, aDelay );
        }
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishContactContextL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::PublishContactContextL(
        const RPointerArray<MVPbkStoreContact>& aContacts,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    CDesCArray* arr = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( arr );
    for ( TInt i = 0, ie = aContacts.Count(); i != ie; ++i )
        {
        MVPbkContactLink* link = aContacts[i]->CreateLinkLC();
        if ( link )
            {
            HBufC* pubstr = MakeLinkPublishableLC( *link );
            arr->AppendL( *pubstr );
            CleanupStack::PopAndDestroy( pubstr );
            }
        CleanupStack::PopAndDestroy( );//link
        }
    PublishContactContextL( *arr, aDelay );
    CleanupStack::PopAndDestroy( arr );
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishContactContextL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::PublishContactContextL(
        const CVPbkContactLinkArray& aContactLinks,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    CDesCArray* arr = new ( ELeave ) CDesCArrayFlat( KArrayGranularity );
    CleanupStack::PushL( arr );
    for ( TInt i = 0, ie = aContactLinks.Count(); i != ie; ++i )
        {
        HBufC* pubstr = MakeLinkPublishableLC( aContactLinks.At( i ) );
        arr->AppendL( *pubstr );
        CleanupStack::PopAndDestroy( pubstr );
        }
    PublishContactContextL( *arr, aDelay );
    CleanupStack::PopAndDestroy( arr );
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishContactContextL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::PublishContactContextL(
        const MDesCArray& aContactNames,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    if ( !aDelay.Int() )
        {
        PublishContextL( KHgCFTypeContact, aContactNames );
        }
    else
        {
        PublishContextDelayedL( KHgCFTypeContact, aContactNames, aDelay );
        }
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishTextContextL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::PublishTextContextL( const TDesC& aText,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    if ( !aDelay.Int() )
        {
        PublishContextL( KHgCFTypeText, aText );
        }
    else
        {
        PublishContextDelayedL( KHgCFTypeText, aText, aDelay );
        }
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishUrlContextL
// -----------------------------------------------------------------------------
//   
void CHgContextUtilityImpl::PublishUrlContextL( const TDesC& aUrl,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    if ( !aDelay.Int() )
        {
        PublishContextL( KHgCFTypeUrl, aUrl );
        }
    else
        {
        PublishContextDelayedL( KHgCFTypeUrl, aUrl, aDelay );
        }
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishTimeContextL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::PublishTimeContextL( const TTime& aTime,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    // YYYYMMDD:HHMMSS.MMMMMM
    const TInt KDateTimeLength = 22;
    const TInt KYearLength = 4;
    const TInt KMonthLength = 2;
    const TInt KDayLength = 2;
    _LIT( KTimeZero, ":010101.000000");

    TDateTime dt = aTime.DateTime();
    TBuf<KDateTimeLength> buf;
    buf.AppendNumFixedWidth( dt.Year(), EDecimal, KYearLength );
    buf.AppendNumFixedWidth( dt.Month(), EDecimal, KMonthLength );
    buf.AppendNumFixedWidth( dt.Day(), EDecimal, KDayLength );
    buf.Append( KTimeZero );
    if ( !aDelay.Int() )
        {
        PublishContextL( KHgCFTypeActiveDate, buf );
        }
    else
        {
        PublishContextDelayedL( KHgCFTypeActiveDate, buf, aDelay );
        }
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishPhotoContextL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::PublishPhotoContextL(
        const TDesC& aFilename,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    if ( !aDelay.Int() )
        {
        PublishContextL( KHgCFTypePhoto, aFilename );
        }
    else
        {
        PublishContextDelayedL( KHgCFTypePhoto, aFilename, aDelay );
        }
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishPhotoContextL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::PublishPhotoContextL(
        TItemId aMdeItemId,
        CMdESession& aMdeSession,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    CMdEObject* obj = aMdeSession.GetObjectL( aMdeItemId );
    if ( obj )
        {
        CleanupStack::PushL( obj );
        PublishPhotoContextL( obj->Uri(), aDelay );
        CleanupStack::PopAndDestroy( obj );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishTvContextL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::PublishTvContextL( const TDesC& aChannelName,
        const TDesC& aProgramName, const TDesC& aProgramDescription,
        const TDesC& aGenre )
    {
    TPtrC channelName( aChannelName.Length() ? aChannelName
        : KHgCFValueUnknownInfo );
    TPtrC programName( aProgramName.Length() ? aProgramName
        : KHgCFValueUnknownInfo );
    TPtrC programDesc( aProgramDescription.Length() ? aProgramDescription
        : KHgCFValueUnknownInfo );
    TPtrC programGenre( aGenre.Length() ? aGenre : KHgCFValueUnknownInfo );

    // Publish description/genre first because it is unlikely to have those
    // in rules so their content will be available for sure when an action
    // is triggered.
    PublishContextL( KHgCFTypeTvProgramDesc, programDesc );
    PublishContextL( KHgCFTypeTvProgramGenre, programGenre );
    PublishContextL( KHgCFTypeTvChannelName, channelName );
    PublishContextL( KHgCFTypeTvProgramName, programName );
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishServiceIdL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::PublishServiceIdL( const TDesC& aServiceId,
        const TDesC& aAccountId,
        const TTimeIntervalMicroSeconds32& aDelay )
    {
    HBufC* combinedIdBuf = HBufC::NewLC( aServiceId.Length()
        + aAccountId.Length() + 1 );
    TPtr combinedId( combinedIdBuf->Des() );
    _LIT( KCombinedFormat, "%S:%S" );
    combinedId.Format( KCombinedFormat, &aServiceId, &aAccountId );
    PublishContactContextL( combinedId, aDelay );
    CleanupStack::PopAndDestroy( combinedIdBuf );
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::RePublishWhenFgL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::RePublishWhenFgL( TBool aEnable )
    {
    if ( iEnv )
        {
        if ( iFgWatchEnabled )
            {
            iEnv->RemoveForegroundObserver( *this );
            }
        iFgWatchEnabled = aEnable;
        if ( iFgWatchEnabled )
            {
            iEnv->AddForegroundObserverL( *this );
            }
        }
    }

// callbacks from CCoeEnv

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::HandleGainingForeground
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::HandleGainingForeground()
    {
    if ( iLastContextType && iLastContextData )
        {
        TRAP_IGNORE( PublishContextL( *iLastContextType, *iLastContextData ) );
        }
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::HandleLosingForeground
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::HandleLosingForeground()
    {
    // nothing to do here
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::IsForeground
// -----------------------------------------------------------------------------
//
TBool CHgContextUtilityImpl::IsForeground()
    {
    if ( iEnv )
        {
        TInt rootWgId = iEnv->RootWin().WindowGroupId();
        TInt focusWgId = iEnv->WsSession().GetFocusWindowGroup();
        return rootWgId == focusWgId;
        }
    else
        {
        return ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::AllowedToPublish
// -----------------------------------------------------------------------------
//
TBool CHgContextUtilityImpl::AllowedToPublish()
    {
    TBool result = !iEnv || iAllowPublishFromBackground || IsForeground();
    RDebug::Printf( "[hgctxutil] AllowedToPublish = %d", result );
    return result;
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::AllowPublishFromBackground
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::AllowPublishFromBackground( TBool aAllow )
    {
    iAllowPublishFromBackground = aAllow;
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::AddMusicContextInfoL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::AddMusicContextInfoL( 
    const TDesC& aKey, const TDesC& aData )
    {   
    // Key needs to be provided and also it shouldn't exist in the table.
    // Latter case is simple safe measure, as RPtrHasMap won't delete existing
    // objects in InsertL, so adding same key twice would cause memory leak.
    // The use case of adding same key twice is not 'real world' case, so 
    // this method can simply leave, when same key is offered again.
    __ASSERT_ALWAYS( aKey.Length(), User::Leave( KErrNotFound ) );
    __ASSERT_ALWAYS( 
        !iMusicContextInfo.Find( aKey ), User::Leave( KErrAlreadyExists ) );
    
    // Hash table needs pointers and it should own the pointers, so allocate
    // key and data, and add them to table. In case the data is empty, add
    // unknown information, since some data needs to be in the action field.
    HBufC* key = aKey.AllocLC();
    HBufC* data = aData.Length() ? 
        aData.AllocLC() : KHgCFValueUnknownInfo().AllocLC();
    iMusicContextInfo.InsertL( key, data );
    CleanupStack::Pop( 2, key );
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishMusicContextL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::PublishMusicContextL( 
    const TTimeIntervalMicroSeconds32& aDelay )
    {
    // If nothing has been done, just leave. No point of publishing entirely
    // empty music context.
    __ASSERT_ALWAYS( iMusicContextInfo.Count(), User::Leave( KErrNotReady ) );    
    
    // Before publishing anything, make sure all keys contain at least some
    // data.
    VerifyAndPublishMusicContextL( KHgCFTypeMusicState, aDelay );
    VerifyAndPublishMusicContextL( KHgCFTypeMusicArtist, aDelay );
    VerifyAndPublishMusicContextL( KHgCFTypeMusicTitle, aDelay );
    VerifyAndPublishMusicContextL( KHgCFTypeMusicAlbum, aDelay );
    VerifyAndPublishMusicContextL( KHgCFTypeMusicAlbumArt, aDelay );
    VerifyAndPublishMusicContextL( KHgCFTypeMusicUri, aDelay );
    VerifyAndPublishMusicContextL( KHgCFTypeMusicGenre, aDelay );
    VerifyAndPublishMusicContextL( KHgCFTypeMusicType, aDelay );
    
    // Clear all data from hash table, so new music context can be published.
    iMusicContextInfo.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::VerifyAndPublishMusicContextL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::VerifyAndPublishMusicContextL( 
    const TDesC& aKey,
    const TTimeIntervalMicroSeconds32& aDelay )
    {
    TDesC* data = iMusicContextInfo.Find( aKey );
    if ( !data )
        {
        // Key didn't contain any data, just create the key with empty info.
        AddMusicContextInfoL( aKey, KNullDesC );
        data = iMusicContextInfo.Find( aKey );
        }
    
    PublishContextL( aKey, *data, aDelay );
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishContextL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::PublishContextL(
    const TDesC & aContextType, 
    const TDesC & aContextData, 
    const TTimeIntervalMicroSeconds32& aDelay )
    {
    if ( !aDelay.Int() )
        {
        PublishContextL( aContextType, aContextData );
        }
    else
        {
        PublishContextDelayedL( aContextType, aContextData, aDelay );
        }
    }

// -----------------------------------------------------------------------------
// CHgContextUtilityImpl::PublishRadioContextL
// -----------------------------------------------------------------------------
//
void CHgContextUtilityImpl::PublishRadioContextL( 
        const TDesC& aRadioName,
        const TDesC& aRadioUrl,
        const TDesC& aRadioFrequency,
        const TDesC& aRadioRDSPI )
    {
    TPtrC radioName( aRadioName.Length() ? aRadioName
        : KHgCFValueUnknownInfo );
    TPtrC radioUrl( aRadioUrl.Length() ? aRadioUrl
        : KHgCFValueUnknownInfo );
    TPtrC radioFrequency( aRadioFrequency.Length() ? aRadioFrequency
        : KHgCFValueUnknownInfo );
    TPtrC radioRDSPI( aRadioRDSPI.Length() ? aRadioRDSPI
        : KHgCFValueUnknownInfo );

    PublishContextL( KHgCFTypeMusicRadioRDSPI, radioRDSPI );
    PublishContextL( KHgCFTypeMusicRadioFrequency, radioFrequency );
    PublishContextL( KHgCFTypeMusicRadioUrl, radioUrl );
    PublishContextL( KHgCFTypeMusicRadioName, radioName );
    }

// end of file
