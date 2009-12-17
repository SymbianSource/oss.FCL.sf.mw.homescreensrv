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
* Description:  Client session for BS engine
 *
*/


#include "bsclient.h"
#include "bsengineglobals.h"

// Number of message slots to reserve for this client server session.
const TUint KDefaultMessageSlots( 5);

// Function prototypes
static TInt StartServer();
static TInt CreateServerProcess();

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
RBSClient::RBSClient() :
    RSessionBase()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// Connect to Active Data Server session. 
// -----------------------------------------------------------------------------
//
TInt RBSClient::Connect()
    {
    TInt error =:: StartServer();

    if ( error == KErrNone )
        {
        error = CreateSession( KBSEngineName,
                Version(),
                KDefaultMessageSlots );
        }
    return error;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------

TInt RBSClient::Initialize( TUid aApp )
    {
    return SendReceive( EBSEngineInitialize, TIpcArgs( aApp.iUid ) );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------

TInt RBSClient::ForwardActivationEvent( const TDesC8& aState, TBool aIsItem )
    {
    TPtr8 state( (TUint8*) aState.Ptr( ), aState.Size( ), aState.Size( ));
    return SendReceive( EBSEngineHandleActivationEvent, TIpcArgs( &state,
        aIsItem ) );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------

TInt RBSClient::HandleBackCommand( const TDesC8& aState, TBool aCheckOnly )
    {
    TPtr8 state( (TUint8*) aState.Ptr( ), aState.Size( ), aState.Size( ));
    return SendReceive( EBSEngineHandleBackCommand, TIpcArgs( &state,
        aCheckOnly ) );
    }

// -----------------------------------------------------------------------------
// Version information.
// -----------------------------------------------------------------------------
//
TVersion RBSClient::Version() const
    {
    return (TVersion( KBSEngineMajorVersionNumber,
        KBSEngineMinorVersionNumber, KBSEngineBuildVersionNumber ) );
    }

// -----------------------------------------------------------------------------
// Static method to start the server.
// -----------------------------------------------------------------------------
//
static TInt StartServer()
    {
    TInt result;

    TFindServer findServer( KBSEngineName );
    TFullName name;

    result = findServer.Next( name );
    if ( result != KErrNone )
        {
        // Server not running
        result = CreateServerProcess( );
        }
    return result;
    }

// -----------------------------------------------------------------------------
// Static method to create the server process.
// -----------------------------------------------------------------------------
//
static TInt CreateServerProcess()
    {
    const TUidType serverUid( KNullUid, KNullUid, KServerUid3);
    RProcess server;
    TInt result;
    result = server.Create( KBSEngineFilename, KNullDesC, serverUid );
    if ( result == KErrNone )
        {
        TRequestStatus stat = KRequestPending;
        server.Rendezvous( stat );
        if ( stat != KRequestPending )
            {
            server.Kill( 0 ); // abort startup
            }
        else
            {
            server.Resume( ); // logon OK - start the server
            }

        User::WaitForRequest( stat ); // wait for start or death
        // we can't use the 'exit reason' if the server panicked as this
        // is the panic 'reason' and may be '0' which cannot be distinguished
        // from KErrNone
        result = (server.ExitType( ) == EExitPanic ) ? KErrGeneral
            : stat.Int( );
        }
    server.Close( );
    return result;
    }
