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
#include <s32strm.h>
#include <s32mem.h>

// User include files
#include "ccsrvsession.h"
#include "ccsrv.h"
#include "ccresource.h"
#include "ccsrvapi.h"

// Local constants


// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------
// CCcSrvSession::CCcSrvSession()
// -----------------------------------------------------------------------
//
CCcSrvSession::CCcSrvSession():
    iLastTrId( 0 )
    {
    }

// -----------------------------------------------------------------------
// CCcSrvSession::~CCcSrvSession()
// -----------------------------------------------------------------------
//
CCcSrvSession::~CCcSrvSession()
    {
    iRequests.ResetAndDestroy();
    iObservers.Close();
    Server().DropSession( this );
    }

// -----------------------------------------------------------------------
// CCcSrvSession::CreateL()
// -----------------------------------------------------------------------
//
void CCcSrvSession::CreateL()
    {
    Server().AddSessionL( this );
    }

// -----------------------------------------------------------------------
// CCcSrvSession::Id()
// -----------------------------------------------------------------------
//
TUint32 CCcSrvSession::Id()
    {
    return iId;
    }

// -----------------------------------------------------------------------
// CCcSrvSession::SetId()
// -----------------------------------------------------------------------
//
void CCcSrvSession::SetId(
    TUint32 aId )
    {
    iId = aId;
    }

// -----------------------------------------------------------------------
// CCcSrvSession::ServiceL()
// -----------------------------------------------------------------------
//
void CCcSrvSession::ServiceL(
    const RMessage2& aMessage )
    {
    RMessage2 msg = aMessage;
    TRAPD( err, DoServiceL( msg ) );
    if ( err )
        {
        aMessage.Complete( err );
        }
    }

// -----------------------------------------------------------------------
// CCcSrvSession::DoServiceL()
// -----------------------------------------------------------------------
//
void CCcSrvSession::DoServiceL(
    RMessage2& aMessage )
    {
    switch ( aMessage.Function() )
        {
        case ECcReqisterProvider:
            HandleRegisterProviderL( aMessage );
            break;
        case ECcRegisterObserver:
            HandleRegisterObserverL( aMessage );
            break;
        case ECcWaitForApiReq:
            HandleWaitForApiReqL( aMessage );
            break;
        case ECcWaitForApiNtf:
            HandleWaitForApiNtfL( aMessage );
            break;
        case ECcCancel:
            HandleCancelReqL( aMessage );
            break;
        case ECcApiReq:
            HandleApiReqL( aMessage );
            break;
        case ECcApiResp:
            HandleApiRespL( aMessage );
            break;
        case ECcGetMsgData:
            HandleGetMsgDataL( aMessage );
            break;
        case ECcApiNtf:
            HandleApiNtfL( aMessage );
            break;
        default:
            User::Leave( KErrNotSupported );
            break;
        }
    }

// -----------------------------------------------------------------------
// CCcSrvSession::HandleRegisterProviderL()
// -----------------------------------------------------------------------
//
void CCcSrvSession::HandleRegisterProviderL(
    RMessage2& aMessage )
    {
    TUint32 provider;
    TPckgBuf<TUint32> packagedProvider;    
    aMessage.ReadL(0, packagedProvider);                        
    provider = packagedProvider();
    
    Server().RegisterProviderL(
        provider,
        this );
    
    TPckgBuf<TUint32> packagedSessionId( iId );
    aMessage.WriteL( 1, packagedSessionId );
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------
// CCcSrvSession::HandleRegisterObserverL()
// -----------------------------------------------------------------------
//
void CCcSrvSession::HandleRegisterObserverL(
    RMessage2& aMessage )
    {
    TUint32 provider;
    TPckgBuf<TUint32> packagedProvider;    
    aMessage.ReadL(0, packagedProvider);                        
    provider = packagedProvider();
    
    Server().RegisterObserverL(
        provider,
        this );
    
    TPckgBuf<TUint32> packagedSessionId( iId );
    aMessage.WriteL( 1, packagedSessionId );
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------
// CCcSrvSession::RegisterObserverSessionL()
// -----------------------------------------------------------------------
//
void CCcSrvSession::RegisterObserverSessionL(
    TUint32 aObserver )
    {
    // Notify provider of registered observer
    SendObserverNtfL( aObserver, ECcRegisterObserverNtf );
    iObservers.AppendL( aObserver );
    }

// -----------------------------------------------------------------------
// CCcSrvSession::UnregisterObserverSessionL()
// -----------------------------------------------------------------------
//
void CCcSrvSession::UnregisterObserverSessionL(
    TUint32 aObserver )
    {
    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        if ( iObservers[ i ] == aObserver )
            {
            // Notify provider of unregistered observer
            SendObserverNtfL( aObserver, ECcUnregisterObserverNtf );
            iObservers.Remove( i );
            break;
            }
        }
    }

// -----------------------------------------------------------------------
// CCcSrvSession::HandleWaitForApiReqL()
// -----------------------------------------------------------------------
//
void CCcSrvSession::HandleWaitForApiReqL(
    RMessage2& aMessage )
    {
    CCcSrvMsg* tr = CCcSrvMsg::NewL();
    CleanupStack::PushL( tr );
    tr->SetMessage( aMessage );
    iRequests.AppendL( tr );
    CleanupStack::Pop( tr );
    }

// -----------------------------------------------------------------------
// CCcSrvSession::HandleWaitForApiNtfL()
// -----------------------------------------------------------------------
//
void CCcSrvSession::HandleWaitForApiNtfL(
    RMessage2& aMessage )
    {
    CCcSrvMsg* tr = CCcSrvMsg::NewL();
    CleanupStack::PushL( tr );
    tr->SetMessage( aMessage );
    iRequests.AppendL( tr );
    CleanupStack::Pop( tr );
    }

// -----------------------------------------------------------------------
// CCcSrvSession::HandleCancelReqL()
// -----------------------------------------------------------------------
//
void CCcSrvSession::HandleCancelReqL(
    RMessage2& aMessage )
    {
    TInt function;
    TPckgBuf<TInt> packagedFunction;    
    aMessage.ReadL( 0, packagedFunction );                        
    function = packagedFunction();
    
    for ( TInt i = 0; i < iRequests.Count(); i++ )
        {
        CCcSrvMsg* req = iRequests[ i ];
        if ( req->Message().Function() == function &&
             !req->Message().IsNull() )
            {
            // Complete canceled request
            req->Message().Complete( KErrCancel );
            iRequests.Remove( i );
            delete req;
            }
        }
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------
// CCcSrvSession::HandleApiReqL()
// -----------------------------------------------------------------------
//
void CCcSrvSession::HandleApiReqL(
    RMessage2& aMessage )
    {
    TUint32 provider;
    TPckgBuf<TUint32> packagedProvider;    
    aMessage.ReadL( 0, packagedProvider );                        
    provider = packagedProvider();
    TUint32 providerAddress;
    Server().ResolveProviderAddressL( provider, providerAddress );

    HBufC8* msgBuf = HBufC8::NewL( aMessage.GetDesLengthL( 3 ) );
    CleanupStack::PushL( msgBuf );
    TPtr8 msgBufPtr = msgBuf->Des();
    aMessage.ReadL( 3, msgBufPtr, 0 );
    
    RDesReadStream stream( msgBufPtr );
    
    CCcSrvMsg* message = CCcSrvMsg::NewL();
    CleanupStack::PushL( message );
    message->InternalizeL( stream );
    message->SetTrId( Server().GetTrId() );
    message->SetMessage( aMessage );
    
    Server().SendMsgL( iId, providerAddress, *message );

    iRequests.AppendL( message );
    
    CleanupStack::Pop( message );    
    CleanupStack::PopAndDestroy( msgBuf );
        
    }

// -----------------------------------------------------------------------
// CCcSrvSession::HandleApiReqL()
// -----------------------------------------------------------------------
//
void CCcSrvSession::HandleApiRespL(
    RMessage2& aMessage )
    {
    // Read sender and receiver of a response
    TPckgBuf<TUint32> pckg;    
    aMessage.ReadL( 1, pckg );                        
    TUint32 sender = pckg();
    aMessage.ReadL( 2, pckg );                        
    TUint32 receiver = pckg();
    
    // Read message data
    HBufC8* msgBuf = HBufC8::NewL( aMessage.GetDesLengthL( 3 ) );
    CleanupStack::PushL( msgBuf );
    TPtr8 msgBufPtr = msgBuf->Des();
    aMessage.ReadL( 3, msgBufPtr, 0 );
    
    // Internalize message
    RDesReadStream stream( msgBufPtr );
    CCcSrvMsg* message = CCcSrvMsg::NewL();
    CleanupStack::PushL( message );
    message->InternalizeL( stream );
    message->SetMessage( aMessage );
    
    // Forward message to receiver
    Server().SendMsgL( sender, receiver, *message );

    CleanupStack::PopAndDestroy( message );    
    CleanupStack::PopAndDestroy( msgBuf );

    aMessage.Complete( KErrNone );

    }

// -----------------------------------------------------------------------
// CCcSrvSession::HandleGetMsgDataL()
// -----------------------------------------------------------------------
//
void CCcSrvSession::HandleGetMsgDataL(
    RMessage2& aMessage )
    {
    // Read sender and receiver of a response
    TPckgBuf<TUint32> pckg;    
    aMessage.ReadL( 0, pckg );                        
    TUint32 trId = pckg();
    
    CCcSrvMsg* req( NULL );
    TInt index( 0 );
    for( TInt i = 0; i < iRequests.Count() && !req; i++ )
        {
        if ( iRequests[ i ]->TrId() == trId )
            {
            req = iRequests[ i ];
            index = i;
            }
        }
    
    TInt err( KErrNone );
    if ( req )
        {
        // Write message data
        aMessage.WriteL( 1, req->Data(), 0);
        // Remove request
        iRequests.Remove( index );
        delete req;
        }
    else
        {
        err = KErrNotFound;
        }
    
    aMessage.Complete( err );

    }

// -----------------------------------------------------------------------
// CCcSrvSession::HandleApiNtfL()
// -----------------------------------------------------------------------
//
void CCcSrvSession::HandleApiNtfL(
    RMessage2& aMessage )
    {
    // Read notification data
    HBufC8* msgBuf = HBufC8::NewL( aMessage.GetDesLengthL( 3 ) );
    CleanupStack::PushL( msgBuf );
    TPtr8 msgBufPtr = msgBuf->Des();
    aMessage.ReadL( 3, msgBufPtr, 0 );
    
    // Internalize notification
    RDesReadStream stream( msgBufPtr );
    CCcSrvMsg* message = CCcSrvMsg::NewL();
    CleanupStack::PushL( message );
    message->InternalizeL( stream );
    message->SetMessage( aMessage );

    // Forward notification to observers
    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        Server().SendMsgL( iId, iObservers[ i ], *message );
        }
    
    message->Message().Complete( KErrNone );
    
    CleanupStack::PopAndDestroy( message );
    CleanupStack::PopAndDestroy( msgBuf );
    }

// -----------------------------------------------------------------------
// CCcSrvSession::ReceiveMsgL()
// -----------------------------------------------------------------------
//
void CCcSrvSession::ReceiveMsgL(
    TUint32 aSender,
    TUint32 aReceiver,
    CCcSrvMsg& aMessage )
    {
    TBool found( EFalse );
    CCcSrvMsg* req( NULL );
    TInt index( 0 );
    for ( TInt i = 0; i < iRequests.Count() && !found; i++ )
        {
        req = iRequests[ i ];
        if ( aMessage.Message().Function() == ECcApiReq &&
             req->Message().Function() == ECcWaitForApiReq &&  
             !req->Message().IsNull() )
            {
            // Pending WaitForApiReq transaction found
            index = i;
            found = ETrue;
            }
        else if ( aMessage.Message().Function() == ECcApiResp &&
                  req->TrId() == aMessage.TrId() )
            {
            // Pending ApiReq transaction found
            index = i;
            found = ETrue;
            }
        else if ( aMessage.Message().Function()  == ECcApiNtf &&
                  req->Message().Function() == ECcWaitForApiNtf &&
                  !req->Message().IsNull() )
            {
            // Pending WaitForApiNtf transaction found
            index = i;
            found = ETrue;
            }
        }
    
    if ( found )
        {
        // Write sender of message
        TPckgBuf<TUint32> packagedSender( aSender );
        req->Message().WriteL( 1, packagedSender, 0 );
        
        // Write receiver of message
        TPckgBuf<TUint32> packagedReceiver( aReceiver );
        req->Message().WriteL( 2, packagedReceiver, 0 );
        
        // Externalize message header
        CBufFlat* buf = CBufFlat::NewL( KCcHeaderSize );
        CleanupStack::PushL( buf );
        RBufWriteStream stream( *buf );
        CleanupClosePushL( stream );
        aMessage.ExternalizeHeaderL( stream );
        CleanupStack::PopAndDestroy( &stream );
        
        //Create a heap descriptor from the buffer
        HBufC8* des = HBufC8::NewL( buf->Size() );
        CleanupStack::PushL( des );
        TPtr8 ptr( des->Des() );
        buf->Read( 0, ptr, buf->Size() );
        
        // Write request header
        req->Message().WriteL( 3, ptr, 0);
        CleanupStack::PopAndDestroy( des );
        CleanupStack::PopAndDestroy( buf );
        
        // Complete request
        req->Message().Complete( KErrNone );
        if (  aMessage.DataSize() )
            {
            // Store request data to be read later
            // with GetMsgData()
            req->SetTrId( aMessage.TrId() );
            req->SetData( aMessage.Data() );
            }
        else
            {
            // Received request does not contain any data
            // -> remove it from request array
            iRequests.Remove( index );
            delete req;
            }
        }
    else
        {
        User::Leave( KErrNotFound );
        }    
    }

// -----------------------------------------------------------------------
// CCcSrvSession::SendObserverNtfL()
// -----------------------------------------------------------------------
//
void CCcSrvSession::SendObserverNtfL(
    TUint32 aSender,
    TUint32 aNtf )
    {
    // Notify provider of registered observer
    TBool found( EFalse );
    CCcSrvMsg* req( NULL );
    TInt index( 0 );
    for ( TInt i = 0; i < iRequests.Count() && !found; i++ )
        {
        req = iRequests[ i ];
        if ( req->Message().Function() == ECcWaitForApiReq &&
             !req->Message().IsNull() )
            {
            // Pending WaitForApiReq transaction found
            index = i;
            found = ETrue;
            }
        }
    if ( found )
        {
        // Write sender of message
        TPckgBuf<TUint32> packagedSender( aSender );
        req->Message().WriteL( 1, packagedSender, 0 );
        
        // Write receiver of message
        TPckgBuf<TUint32> packagedReceiver( iId );
        req->Message().WriteL( 2, packagedReceiver, 0 );
        
        // Externalize notification
        CCcSrvMsg* ntf = CCcSrvMsg::NewL();
        CleanupStack::PushL( ntf );
        ntf->SetMsgId( aNtf );
        HBufC8* ntfBuf = ntf->MarshalL();
        CleanupStack::PushL( ntfBuf );
        TPtr8 ntfPtr( NULL, 0);
        ntfPtr.Set( ntfBuf->Des() );
        
        // Write request header
        req->Message().WriteL( 3, ntfPtr, 0);
        CleanupStack::PopAndDestroy( ntfBuf );
        CleanupStack::PopAndDestroy( ntf );
        
        // Complete request
        req->Message().Complete( KErrNone );
        iRequests.Remove( index );
        delete req;

        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }

// End of file
