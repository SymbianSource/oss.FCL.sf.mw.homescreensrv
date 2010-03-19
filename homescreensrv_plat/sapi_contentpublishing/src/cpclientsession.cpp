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


#include <s32mem.h>
#include <liwcommon.h>

#include "cpserverdef.h"
#include "cpliwmap.h"
#include "cpclientsession.h"
#include "cpglobals.h"
#include "cpdebug.h"
#include "cpclientactivenotifier.h"

// Number of message slots to reserve for this client server session.
const TUint KDefaultMessageSlots( 1 );

// Function prototypes
static TInt StartServer();
static TInt CreateServerProcess();

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// RCPServerClient::RCPServerClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
RCPServerClient::RCPServerClient() :
    RSessionBase()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// RCPServerClient::Connect
// Connect to Content publisher server session. 
// -----------------------------------------------------------------------------
//
TInt RCPServerClient::Connect()
    {
    TInt error =:: StartServer();

    if ( error == KErrNone )
        {
        error = CreateSession( KCPServerName,
                Version(),
                KDefaultMessageSlots );
        }
    return error;
    }

// -----------------------------------------------------------------------------
//
//
// -----------------------------------------------------------------------------
//
void RCPServerClient::Close()
    {
    RSessionBase::Close( );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void RCPServerClient::AddL( const CCPLiwMap& aMap,
    CLiwGenericParamList& aOutParamList )
    {
    CP_DEBUG( _L8("RCPServerClient::AddL()") );
    HBufC8 *inbuf = aMap.PackForServerLC( );
    TInt32 id( 0);
    TPckg<TInt32> sizeDes(id);
    TIpcArgs args;
    args.Set( KDescriptorPosition, &*inbuf );
    args.Set( KReturnPosition, &sizeDes );
    User::LeaveIfError( SendReceive( ECpServerAddData, args ) );
    TLiwGenericParam param( KItemId, id);
    aOutParamList.AppendL( param );
    CleanupStack::PopAndDestroy( inbuf );
    }



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void RCPServerClient::GetListL( const CCPLiwMap& aMap,
    CLiwGenericParamList& aOutParamList )
    {
    CP_DEBUG( _L8("RCPServerClient::GetListL()") );
    TIpcArgs args;
    HBufC8 *inbuf = aMap.PackForServerLC( );
    TInt error(KErrNone);
    TInt sizeList( 0);
    TPckg<TInt> sizeDes(sizeList);
    args.Set( KDescriptorPosition, &*inbuf );
    args.Set( KReturnPosition, &sizeDes );
    error = SendReceive( ECpServerGetListSize, args );
    if ( error == KErrNone )
        {
        TIpcArgs args;
        RBuf8 outbuf;
        outbuf.CleanupClosePushL( );
        outbuf.CreateL( sizeList );
        args.Set( KDescriptorPosition, &outbuf );
        error = SendReceive( ECpServerGetListData, args );
        if ( error == KErrNone )
            {
            RDesReadStream stream(outbuf);
            CleanupClosePushL( stream );
            aOutParamList.InternalizeL( stream ) ;
            CleanupStack::PopAndDestroy( &stream );
            }
        CleanupStack::PopAndDestroy( &outbuf );
        }
    User::LeaveIfError( error );
    CleanupStack::PopAndDestroy( inbuf );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void RCPServerClient::ExecuteActionL( const CCPLiwMap& aMap )
    {
    CP_DEBUG( _L8("RCPServerClient::ExecuteActionL()") );
    HBufC8 *inbuf = aMap.PackForServerLC( );
    TIpcArgs args;
    args.Set( KDescriptorPosition, &*inbuf );
    User::LeaveIfError( SendReceive( ECpServerExecuteAction, args ) );
    CleanupStack::PopAndDestroy( inbuf );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void RCPServerClient::DeleteL( const CCPLiwMap& aMap )
    {
    CP_DEBUG( _L8("RCPServerClient::DeleteL()") );
    HBufC8 *inbuf = aMap.PackForServerLC( );
    TIpcArgs args;
    args.Set( KDescriptorPosition, &*inbuf );
    User::LeaveIfError( SendReceive( ECpServerRemoveData, args ) );
    CleanupStack::PopAndDestroy( inbuf );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void RCPServerClient::RegisterObserverL( TRequestStatus& aStatus,
    TIpcArgs& aArgs )
    {
    CP_DEBUG( _L8("RCPServerClient::RegisterObserver()") );
    SendReceive( ECpServerRegisterObserver, aArgs, aStatus );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void RCPServerClient::AddObserver( TIpcArgs& aArgs )
    {
    CP_DEBUG( _L8("RCPServerClient::AddObserver()") );
    SendReceive( ECpServerAddObserver, aArgs );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void RCPServerClient::RemoveObserver( TIpcArgs& aArgs )
    {
    CP_DEBUG( _L8("RCPServerClient::AddObserver()") );
    SendReceive( ECpServerRemoveObserver, aArgs );
    }
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
TInt RCPServerClient::GetChangeInfoData( TDes8& aBuf )
    {
    CP_DEBUG( _L8("RCPServerClient::GetChangeInfoData()") );
    TIpcArgs args;
    args.Set( KDescriptorPosition, &aBuf );
    TInt error = SendReceive( ECpServerGetChangeInfoData, args );
    return error;
    }
 
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void RCPServerClient::UnregisterObserver()
    {
    CP_DEBUG( _L8("RCPServerClient::UnregisterObserver()") );
    SendReceive( ECpServerUnRegisterObserver );
    }

// -----------------------------------------------------------------------------
// RCPServerClient::Version
// Version information.
// -----------------------------------------------------------------------------
//
TVersion RCPServerClient::Version() const
    {
    CP_DEBUG( _L8("RCPServerClient::Version()") );
    return (TVersion( KCPServerMajorVersionNumber,
        KCPServerMinorVersionNumber, KCPServerBuildVersionNumber ) );
    }

// -----------------------------------------------------------------------------
// StartServer
// Static method to start the server.
// -----------------------------------------------------------------------------
//
static TInt StartServer()
    {
    TInt result;

    TFindServer findServer( KCPServerName );
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
    CP_DEBUG( _L8("RCPServerClient::CreateServerProcess()") );
    const TUidType serverUid( KNullUid, KNullUid, KServerUid3);
    RProcess server;
    TInt result;
    result = server.Create( KCPServerFilename, KNullDesC, serverUid );
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
