/*
 * Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

#include "caclientsubsession.h"
#include "caclientnotifiersession.h"
#include "caclientnotifier.h"
#include "cainnernotifierfilter.h"
#include "casrvdef.h"
#include "cainnerentry.h"
#include "caclientproxy.h"
#include "caclientnotifierproxy.h"
#include "cautils.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
RCaClientSubSession::RCaClientSubSession(
        const RCaClientNotifierSession* aSession,
        const IDataObserver* aObserver,
        const CCaInnerNotifierFilter *aInnerNotifierFilter ) :
    RSubSessionBase(), iSession( aSession ), iObserver( aObserver ),
    iInnerNotifierFilter( aInnerNotifierFilter ),
    iNotifier( NULL ), iMessageSize( NULL )
    {

    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RCaClientSubSession::CreateL()
    {
    iMessageSize = new ( ELeave ) TPckgBuf<TInt> ();
    User::LeaveIfError( CreateSubSession( *iSession,
        static_cast<TInt>( EContentArsenalNotifierOpen ) ) );
    iNotifier = CCaClientNotifier::NewL( this );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RCaClientSubSession::Close()
    {
    delete iMessageSize;
    iMessageSize = NULL;
    delete iNotifier;
    iNotifier = NULL;
    CloseSubSession( EContentArsenalNotifierClose );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RCaClientSubSession::RegisterForNotificationsL(
        TRequestStatus& aStatus ) const
    {
    TIpcArgs args;
    HBufC8* inbuf = MenuUtils::MarshalDataL( *iInnerNotifierFilter,
            KDefaultExpandSize );
    args.Set( KInputPosition1, inbuf );
    args.Set( KOutputPosition, iMessageSize );
    RSubSessionBase::SendReceive( EContentArsenalNotifierNotify, args,
            aStatus );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RCaClientSubSession::UnregisterForNotifications() const
    {
    RSubSessionBase::SendReceive( EContentArsenalNotifierCancel );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RCaClientSubSession::NotifyObserver(
        CCaInnerEntry* aEntry,
        TChangeType aChangeType ) const
    {
    switch( iInnerNotifierFilter->GetNotifierType() )
        {
        case CCaInnerNotifierFilter::EEntryChangedWithId:
            {
            iObserver->entryChanged( aEntry->GetId(), aChangeType );
            break;
            }
        case CCaInnerNotifierFilter::EEntryChangedWithEntry:
            {
            iObserver->entryChanged( *aEntry, aChangeType );
            break;
            }
        case CCaInnerNotifierFilter::EEntryTouched:
            {
            iObserver->entryTouched( aEntry->GetId() );
            break;
            }
        case CCaInnerNotifierFilter::EGroupContentChanged:
            {
            iObserver->groupContentChanged( aEntry->GetId() );
            break;
            }
        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RCaClientSubSession::GetChangeInfoAndNotifyObserverL() const
    {
    TIpcArgs args;
    RBuf8 outbuf;
    outbuf.CleanupClosePushL();
    outbuf.CreateL( ( *iMessageSize )() );
    TPckg<TChangeType> changeTypePckg( EAddChangeType );
    args.Set( KInputPosition1, &outbuf );
    args.Set( KInputPosition2, &changeTypePckg );
    TInt error = RSubSessionBase::SendReceive(
            EContentArsenalGetChangeInfo, args );
    if( error == KErrNone )
        {
        CCaInnerEntry* entry = CCaInnerEntry::NewLC();
        RDesReadStream stream( outbuf );
        CleanupClosePushL( stream );
        entry->InternalizeL( stream );
        CleanupStack::PopAndDestroy( &stream );
        TChangeType changeType = changeTypePckg();
        NotifyObserver( entry, changeType );
        CleanupStack::PopAndDestroy( entry );
        }
    CleanupStack::PopAndDestroy( &outbuf );
    }
