/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Shortcut plug-in xSP extension session class.
*
*/


#include <s32mem.h>
#include <badesca.h>
#include <gulicon.h>
#include <fbs.h>

#include <aiscutextdefs.h>

#include "caiscutextsession.h"
#include "maiscutextmessagehandler.h"

#include  "caiscutextserver.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutExtSession::CAiScutExtSession( MAiScutExtMessageHandler& aObserver )
    : iObserver( aObserver )
    {
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutExtSession::~CAiScutExtSession()
    {
    CAiScutExtServer* server = const_cast<CAiScutExtServer*>(
        dynamic_cast<const CAiScutExtServer*>( Server() ) );

    if( iTargetDefinition && server )
        {
        TRAP_IGNORE(
            iObserver.HandleResetPopupLineArrayL( *iTargetDefinition );
            iObserver.HandleResetIconL( *iTargetDefinition );
            );
        }

    delete iTargetDefinition;
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutExtSession::ServiceL( const RMessage2& aMessage )
    {
    CAiScutExtServer* server = const_cast<CAiScutExtServer*>(
        dynamic_cast<const CAiScutExtServer*>( Server() ) );

    __ASSERT_ALWAYS( server, User::Leave( KErrCorrupt ) );

    TUid uid;
    if( !server->HasConnectedBefore(uid, aMessage) )
        {
        server->ShowTipMessage( uid );
        }

    TRAPD( err, DispatchMessageL( aMessage ) );

    if( !aMessage.IsNull() )
        {
        aMessage.Complete( err );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutExtSession::DispatchMessageL( const RMessage2& aMessage )
    {
    switch ( aMessage.Function() )
        {
        case EAiScutExtServSetTargetDefinition:
            {
            SetTargetDefinitionL( aMessage );
            break;
            }
        case EAiScutExtServSetPopupText:
            {
            SetPopupTextL( aMessage );
            break;
            }
        case EAiScutExtServResetPopupText:
            {
            ResetPopupTextL( aMessage );
            break;
            }
        case EAiScutExtServSetIcon:
            {
            SetIconL( aMessage );
            break;
            }
        case EAiScutExtServResetIcon:
            {
            ResetIconL( aMessage );
            break;
            }
        case EAiScutExtServIsInShortcuts:
            {
            IsInShortcutsL( aMessage );
            break;
            }
        case EAiScutExtServIssuePutInShortcuts:
            {
            IssuePutInShortcutsL( aMessage );
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            }
        }
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutExtSession::SetTargetDefinitionL( const RMessage2& aMessage )
    {
    HBufC* targetStringBuf = HBufC::NewLC( aMessage.GetDesLengthL( 0 ) );
    TPtr targetString( targetStringBuf->Des() );
    aMessage.ReadL( 0, targetString, 0 );
    delete iTargetDefinition;
    iTargetDefinition = targetStringBuf;
    CleanupStack::Pop( targetStringBuf );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutExtSession::SetPopupTextL( const RMessage2& aMessage )
    {
    if( !iTargetDefinition )
        {
        return;
        }
    HBufC8* marshalledLineArrayBuf = HBufC8::NewLC(
        aMessage.GetDesLengthL( 0 ) );
    TPtr8 marshalledLineArray( marshalledLineArrayBuf->Des() );
    aMessage.ReadL( 0, marshalledLineArray, 0 );

    RDesReadStream stream( marshalledLineArray );
    CleanupClosePushL( stream );

    const TInt lineCount = stream.ReadUint8L();
    __ASSERT_ALWAYS( lineCount >= 0 && lineCount <= KMaxPopupTextLines,
        User::Leave( KErrArgument ) );

    CDesCArray* array = new( ELeave ) CDesCArrayFlat(
        lineCount > 0 ? lineCount : 1 );
    CleanupStack::PushL( array );

    for( TInt i = 0; i < lineCount; i++ )
        {
        HBufC* lineBuf = HBufC::NewLC( stream, KMaxTInt );
        array->AppendL( *lineBuf );
        CleanupStack::PopAndDestroy( lineBuf );
        }

    iObserver.HandleSetPopupLineArrayL( *iTargetDefinition, array );  //Takes ownership of array

    CleanupStack::Pop( array );
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( marshalledLineArrayBuf );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutExtSession::ResetPopupTextL( const RMessage2& /*aMessage*/ )
    {
    if( iTargetDefinition )
        {
        iObserver.HandleResetPopupLineArrayL( *iTargetDefinition );
        }
    }


// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutExtSession::SetIconL( const RMessage2& aMessage )
    {
    if( !iTargetDefinition )
        {
        return;
        }
    HBufC8* marshallBuf = HBufC8::NewLC( aMessage.GetDesLengthL( 0 ) );
    TPtr8 marshalledData( marshallBuf->Des() );
    aMessage.ReadL( 0, marshalledData, 0 );

    RDesReadStream stream( marshalledData );
    CleanupClosePushL( stream );

    CFbsBitmap* bitmap = new( ELeave ) CFbsBitmap;
    CleanupStack::PushL( bitmap );
    bitmap->InternalizeL( stream );

    CFbsBitmap* mask = new( ELeave ) CFbsBitmap;
    CleanupStack::PushL( mask );
    mask->InternalizeL( stream );

    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::Pop( 2, bitmap );
    CleanupStack::PushL( icon );

    iObserver.HandleSetIconL( *iTargetDefinition, icon );

    CleanupStack::Pop( icon );

    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( marshallBuf );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutExtSession::ResetIconL( const RMessage2& /*aMessage*/ )
    {
    if( iTargetDefinition )
        {
        iObserver.HandleResetIconL( *iTargetDefinition );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutExtSession::IsInShortcutsL( const RMessage2& aMessage )
    {
    if( iTargetDefinition )
        {
        TBool isInShortcuts = iObserver.HandleIsInShortcutsL( *iTargetDefinition );

        TPtr8 isInShortcutsDes(
            reinterpret_cast< TUint8* >( &isInShortcuts ),
            sizeof( isInShortcuts ),
            sizeof( isInShortcuts ) );
        aMessage.WriteL( 0, isInShortcutsDes, 0 );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutExtSession::IssuePutInShortcutsL( const RMessage2& /*aMessage*/ )
    {
    if( iTargetDefinition )
        {
        iObserver.HandleIssuePutInShortcutsL( *iTargetDefinition );
        }
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutExtSession::PanicClient(
    const RMessage2& aMessage, TInt aPanicCode ) const
    {
    aMessage.Panic( KAiScutExtServerName, aPanicCode );
    }

// End of File.
