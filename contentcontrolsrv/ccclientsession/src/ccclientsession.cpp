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
 * Description:  
 *
 */

// System include files

// User include files
#include "ccclientsession.h"
#include "ccresource.h"
#include "ccsrvapi.h"


// Local constants
// Home screen content control server definitions
const TUid KCcServerUid3 = {0x20026f53};
_LIT( KCcSrvName,"ccserver" );
const TUint KCcSrvMajorVersionNumber = 1;
const TUint KCcSrvMinorVersionNumber = 0;
const TUint KCcSrvBuildVersionNumber = 0;
const TInt KDefaultMessageSlots = 3;


// -----------------------------------------------------------------------------
// StartServer()
// -----------------------------------------------------------------------------
//
static TInt StartServer()
    {
    const TUidType serverUid( KNullUid,KNullUid,KCcServerUid3 );
    RProcess server;
    TInt r = server.Create( KCcSrvName,KNullDesC );
    if ( r != KErrNone )
        {
        return r;
        }
    TRequestStatus stat;
    server.Rendezvous( stat );
    if ( stat != KRequestPending )
        {
        server.Kill( 0 );   // abort startup
        }
    else
        {
        server.Resume();  // logon OK - start the server
        }

    User::WaitForRequest( stat );   // wait for start or death

    r=( server.ExitType() == EExitPanic ) ? KErrGeneral : stat.Int();
    server.Close();
    return r;
    }

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// RCcClientSession::Connect
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RCcClientSession::Connect()
    {
    TVersion ver = TVersion(
        KCcSrvMajorVersionNumber,
        KCcSrvMinorVersionNumber,
        KCcSrvBuildVersionNumber);    
    TInt retry=2;
    for (;;)
        {
        TInt r = CreateSession( KCcSrvName, ver, KDefaultMessageSlots );
  
        if (r != KErrNotFound && r != KErrServerTerminated)
            {
            return r;
            }
        
        if ( --retry==0 )
            {
            return r;
            }
        
        r = StartServer();
        if ( r!=KErrNone && r!=KErrAlreadyExists )
            {
            return r;
            }
        }
    }
  
// -----------------------------------------------------------------------------
// RCcClientSession::Close()
// -----------------------------------------------------------------------------
//
EXPORT_C void RCcClientSession::Close()
    {
    RSessionBase::Close();
    }

// -----------------------------------------------------------------------------
// RCcClientSession::RegisterProvider()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RCcClientSession::RegisterProvider(
    TPckgBuf<TUint32>& aProvider,
    TPckgBuf<TUint32>& aAddress )
    {
    TInt err = SendReceive( 
        ECcReqisterProvider, 
        TIpcArgs( &aProvider,&aAddress,0,0 ) );
    return err;
    }

// -----------------------------------------------------------------------------
// RCcClientSession::RegisterObserver()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RCcClientSession::RegisterObserver(
    TPckgBuf<TUint32>& aProvider,
    TPckgBuf<TUint32>& aAddress )
    {
    TInt err = SendReceive( 
        ECcRegisterObserver, 
        TIpcArgs( &aProvider,&aAddress,0,0 ) );
    return err;
    }

// -----------------------------------------------------------------------------
// RCcClientSession::CancelReq()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RCcClientSession::CancelReq(
    TPckgBuf<TInt>& aFunction )
    {
    TInt err = SendReceive( 
        ECcCancel, 
        TIpcArgs( &aFunction,0,0,0 ) );
    return err;
    }

// -----------------------------------------------------------------------------
// RCcClientSession::WaitForApiReq()
// -----------------------------------------------------------------------------
//
EXPORT_C void RCcClientSession::WaitForApiReq(
    TPckgBuf<TUint32>& aProvider,
    TPckgBuf<TUint32>& aSender,
    TPckgBuf<TUint32>& aReceiver,
    TPtr8& aBuf,
    TRequestStatus& aStatus )
    {
    SendReceive( 
        ECcWaitForApiReq, 
        TIpcArgs( &aProvider,
                  &aSender, 
                  &aReceiver, 
                  &aBuf ),
        aStatus );
    }

// -----------------------------------------------------------------------------
// RCcClientSession::WaitForApiNtf()
// -----------------------------------------------------------------------------
//
EXPORT_C void RCcClientSession::WaitForApiNtf(
    TPckgBuf<TUint32>& aProvider,
    TPckgBuf<TUint32>& aSender,
    TPckgBuf<TUint32>& aReceiver,
    TPtr8& aBuf,
    TRequestStatus& aStatus )
    {
    SendReceive( 
        ECcWaitForApiNtf, 
        TIpcArgs( &aProvider,
                  &aSender, 
                  &aReceiver, 
                  &aBuf ),
        aStatus );
    }

// -----------------------------------------------------------------------------
// RCcClientSession::Send()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RCcClientSession::Send(
    TInt aFunction,
    TPckgBuf<TUint32>& aProvider,
    TPckgBuf<TUint32>& aSender,
    TPckgBuf<TUint32>& aReceiver,
    TPtr8& aBuf )
    {    
    TInt err = SendReceive( 
        aFunction, 
        TIpcArgs( &aProvider, 
                  &aSender, 
                  &aReceiver, 
                  &aBuf ) );
    return err;
    }

// -----------------------------------------------------------------------------
// RCcClientSession::GetMsgData()
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RCcClientSession::GetMsgData(
    TPckgBuf<TUint32>& aTrId,
    TPtr8& aBuf )
    {    
    TInt err = SendReceive( 
        ECcGetMsgData, 
        TIpcArgs( &aTrId, &aBuf, 0, 0) );
    return err;
    }

// End of file
