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
* Description:  Server session for BS engine
 *
*/


// INCLUDE FILES
#include "bsserver.h"
#include "bsengine.h"
#include "bsengineglobals.h"

#include "bsserversession.h"
#include "bspanic.h"
#include "bsdebug.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CBSServerSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBSServerSession* CBSServerSession::NewL( CBSServer* aServer )
    {
    CBSServerSession* self = CBSServerSession::NewLC( aServer );
    CleanupStack::Pop( self ) ;
    return self;
    }

// -----------------------------------------------------------------------------
// CBSServerSession::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBSServerSession* CBSServerSession::NewLC( CBSServer* aServer )
    {
    CBSServerSession* self = new ( ELeave ) CBSServerSession(aServer);
    CleanupStack::PushL( self );
    self->ConstructL( ) ;
    return self;
    }

// -----------------------------------------------------------------------------
// CBSServerSession::~CBSServerSession
// Destructor.
// -----------------------------------------------------------------------------
//
CBSServerSession::~CBSServerSession()
    {
    if ( iBSServer )
        {
        iBSServer->Engine().RemoveAppRecord( iAppUid );
        iBSServer->DecrementSessions( );
        }
    }

// -----------------------------------------------------------------------------
// CBSServerSession::ReadTextL
// 
// -----------------------------------------------------------------------------
//
HBufC8* CBSServerSession::ReadTextLC( const RMessage2& aMessage, TInt aSlot )
    {
    TInt length = aMessage.GetDesMaxLength( aSlot );
    HBufC8* result = HBufC8::NewLC( length );
    TPtr8 ptr(result->Des( ));
    User::LeaveIfError( aMessage.Read( aSlot, ptr ) );

    DEBUG(("ReadTextL slot (%d) - %S", aSlot, result));
    return result;
    }

// -----------------------------------------------------------------------------
// CBSServerSession::ServiceL
// Handle client requests.
// -----------------------------------------------------------------------------
//
void CBSServerSession::ServiceL( const RMessage2& aMessage )
    {
    DEBUG(("CBSServerSession::ServiceL"));
    HBufC8* state(NULL);
    TInt result(0);
    switch ( aMessage.Function( ) )
        {
        case EBSEngineInitialize:
            iAppUid = TUid::Uid( aMessage.Int0( ) );
            DEBUG(("EBSEngineInitialize %x", iAppUid.iUid));
            aMessage.Complete( EBSEngineCommandWasConsumed );
            break;
        case EBSEngineHandleActivationEvent:
            state = ReadTextLC( aMessage, 0 );
            DEBUG(("EBSEngineHandleActivationEvent 0x%X, %S, 0x%X",
                            iAppUid.iUid, state, aMessage.Int1() ));
            result = iBSServer->Engine().
            HandleActivationEventL( iAppUid, *state, aMessage.Int1( ) );
            CleanupStack::PopAndDestroy( state );
            aMessage.Complete( result );
            break;
        case EBSEngineHandleBackCommand:
            state = ReadTextLC( aMessage, 0 );

            DEBUG(("EBSEngineHandleBackCommand 0x%X, %S",
                            iAppUid.iUid, state));
            result = iBSServer->Engine().
            HandleBackEventL( iAppUid, *state, aMessage.Int1( ) );
            CleanupStack::PopAndDestroy( state );
            aMessage.Complete( result );
            break;
        default:
            TBSEnginePanic tmp = EBSEngineBadRequest;
            iBSServer->PanicClient( aMessage, tmp );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CBSServerSession::CBSServerSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CBSServerSession::CBSServerSession( CBSServer* aServer ) :
    CSession2()
    {
    iBSServer = aServer;
    iBSServer->IncrementSessions( );
    }

// -----------------------------------------------------------------------------
// CBSServerSession::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBSServerSession::ConstructL()
    {
    DEBUG(("CBSServerSession::ConstructL"));

    }

// End of File
