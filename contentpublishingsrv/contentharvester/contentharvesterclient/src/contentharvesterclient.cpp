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

#include "contentharvesterclient.h"
#include "contentharvesterglobals.h"

// Number of message slots to reserve for this client server session.
const TUint KDefaultMessageSlots( 5);

// Function prototypes
static TInt StartServer();
static TInt CreateServerProcess();

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// RContentHarvesterClient::RContentHarvesterClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RContentHarvesterClient::RContentHarvesterClient()
:RSessionBase()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// RContentHarvesterClient::Connect
// Connect to Content Harvester Server session. 
// exist
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RContentHarvesterClient::Connect()
    {
    TInt error = ::StartServer();

    if ( error == KErrNone )
        {
        error = CreateSession( KContentHarvesterName,
                Version(),
                KDefaultMessageSlots );
        }
    return error;
    }

// -----------------------------------------------------------------------------
// RContentHarvesterClient::Update
// Client requests to trigger update process
// -----------------------------------------------------------------------------

EXPORT_C TInt RContentHarvesterClient::Update()
    {
    return SendReceive( EContentHarvesterUpdate );
    }

// -----------------------------------------------------------------------------
// RContentHarvesterClient::Stop
// Client requests to trigger stop process
// -----------------------------------------------------------------------------

EXPORT_C TInt RContentHarvesterClient::Stop()
    {
    return SendReceive( EContentHarvesterStop );
    }

// -----------------------------------------------------------------------------
// RContentHarvesterClient::Version
// Version information.
// -----------------------------------------------------------------------------
//
TVersion RContentHarvesterClient::Version() const
    {
    return (TVersion( KContentHarvesterMajorVersionNumber,
        KContentHarvesterMinorVersionNumber,
        KContentHarvesterBuildVersionNumber ) );
    }

// -----------------------------------------------------------------------------
// StartServer
// Static method to start the server.
// -----------------------------------------------------------------------------
//
static TInt StartServer()
    {
    TInt result;

    TFindServer findServer( KContentHarvesterName );
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
// CreateServerProcess
// Static method to create the server process.
// -----------------------------------------------------------------------------
//
static TInt CreateServerProcess()
    {
    const TUidType serverUid( KNullUid, KNullUid, KServerUid3);
    RProcess server;
    TInt result;
    result = server.Create( KContentHarvesterFilename, KNullDesC, serverUid );
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
