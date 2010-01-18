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
#include <hscontentcontrol.h>
#include <hscontentinfo.h>
#include <hscontentinfoarray.h>

// User include files
#include "hsccapiclient.h"
#include "ccresource.h"
#include "ccsrvapi.h"
#include "hsccapi.h"

// Local constants

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CHsCcApiClient::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CHsCcApiClient* CHsCcApiClient::NewL(
    MHsContentControl* aControlIf )
    {
    CHsCcApiClient* self = new ( ELeave ) CHsCcApiClient( aControlIf );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return( self ) ;
    }

// -----------------------------------------------------------------------
// CHsCcApiClient::ConstructL()
// -----------------------------------------------------------------------
//
void CHsCcApiClient::ConstructL()
    {
    User::LeaveIfError( iSession.Connect() );
    
    if ( iObserver )
        {
        TPckgBuf<TUint32> provider( ECcHomescreen );
        TPckgBuf<TUint32> address;
        User::LeaveIfError( iSession.RegisterObserver( 
            provider,
            address ) );
        iAddress = address();

        WaitForApiNtfL();
        }
    }

// -----------------------------------------------------------------------
// CHsCcApiClient::CHsCcApiClient()
// -----------------------------------------------------------------------
//
CHsCcApiClient::CHsCcApiClient( 
    MHsContentControl* aControlIf )
    :CActive( EPriorityStandard )
    ,iObserver( aControlIf )
    ,iAddress( 0 )
    ,iApiHeader( NULL )
    ,iApiHeaderPtr( NULL, 0 )
    ,iApiData( NULL )
    ,iApiDataPtr( NULL, 0 )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------
// CHsCcApiClient::~CHsCcApiClient()
// -----------------------------------------------------------------------
//
CHsCcApiClient::~CHsCcApiClient()
    {
    Cancel();
    iSession.Close();
    delete iApiHeader;
    delete iApiData;
    }

// -----------------------------------------------------------------------------
// CHsCcApiClient::RunL()
// -----------------------------------------------------------------------------
//
void CHsCcApiClient::RunL()
    {
    if ( !iStatus.Int() )
        {
        // Get received message header
        CCcSrvMsg* message = CCcSrvMsg::NewL();
        CleanupStack::PushL( message );
        RDesReadStream stream( iApiHeaderPtr );
        CleanupClosePushL( stream );
        message->InternalizeHeaderL( stream );
        CleanupStack::PopAndDestroy( &stream );

        switch ( message->MsgId() )
            {
            case EHsCcWidgetListChangedNtf:
                if ( iObserver )
                    {
                    iObserver->NotifyWidgetListChanged();
                    }
                break;
            case EHsCcViewListChangedNtf:
                if ( iObserver )
                    {
                    iObserver->NotifyViewListChanged();
                    }
                break;
            case EHsCcAppListChangedNtf:
                if ( iObserver )
                    {
                    iObserver->NotifyAppListChanged();
                    }
                break;
            default:
                // No action required
                break;
            }
        CleanupStack::PopAndDestroy( message );
        }
    
    // Receive next API notification
    WaitForApiNtfL();

    }

// -----------------------------------------------------------------------------
// CHsCcApiClient::DoCancel()
// -----------------------------------------------------------------------------
//
void CHsCcApiClient::DoCancel()
    {
    if ( IsActive() )
        {
        TPckgBuf<TInt> function( ECcWaitForApiNtf );
        iSession.CancelReq( function );
        }
    }

// -----------------------------------------------------------------------------
// CHsCcApiClient::WidgetListL
// -----------------------------------------------------------------------------
//
TInt CHsCcApiClient::WidgetListL( CHsContentInfoArray& aArray )
    {
    TInt err( KErrNone );
    
    // Create WidgetListReq API request
    CCcSrvMsg* reqMsg = CCcSrvMsg::NewL();
    CleanupStack::PushL( reqMsg );
    reqMsg->SetMsgId( EHsCcWidgetListReq );
    reqMsg->SetTrId( 0 );
    reqMsg->SetData( KNullDesC8() );
    
    // Marshal API request
    HBufC8* msgBuf = reqMsg->MarshalL();
    CleanupStack::PushL( msgBuf );
    TPtr8 msgPtr( NULL, 0 );
    msgPtr.Set( msgBuf->Des() );
 
    // Send API request
    // Sender and receiver address not defined -> message is routed
    // according to the provider id
    TPckgBuf<TUint32> provider( ECcHomescreen );
    TPckgBuf<TUint32> sender;
    TPckgBuf<TUint32> receiver;
    err = iSession.Send( ECcApiReq, provider, sender, receiver, msgPtr );
    
    if ( !err )
        {
        // Internalize WidgetListResp API response
        CCcSrvMsg* respMsg = CCcSrvMsg::NewL();
        CleanupStack::PushL( respMsg );
        RDesReadStream respStream( msgPtr );
        CleanupClosePushL( respStream );
        respMsg->InternalizeHeaderL( respStream );
        CleanupStack::PopAndDestroy( &respStream );
        err = respMsg->Status();
        if ( !err )
            {
            if ( respMsg->DataSize() )
                {
                // Get API response data
                HBufC8* dataBuf = HBufC8::NewL( respMsg->DataSize() );
                CleanupStack::PushL( dataBuf );
                TPtr8 dataPtr( NULL, 0 );
                dataPtr.Set( dataBuf->Des() );
                TPckgBuf<TUint32> trId( respMsg->TrId() );
                err = iSession.GetMsgData( trId, dataPtr );
                if ( !err )
                    {
                    // Internalize API response data
                    RDesReadStream dataStream( dataPtr );
                    CleanupClosePushL( dataStream );
                    aArray.InternalizeL( dataStream );                    
                    CleanupStack::PopAndDestroy( &dataStream );
                    }
                CleanupStack::PopAndDestroy( dataBuf );
                }
            }
        CleanupStack::PopAndDestroy( respMsg );
        }

    // Cleanup
    CleanupStack::PopAndDestroy( msgBuf );
    CleanupStack::PopAndDestroy( reqMsg );

    return err;
    }

// -----------------------------------------------------------------------------
// CHsCcApiClient::ViewListL
// -----------------------------------------------------------------------------
//
TInt CHsCcApiClient::ViewListL( CHsContentInfoArray& aArray )
    {
    TInt err( KErrNone );
    
    // Create ViewListReq API request
    CCcSrvMsg* reqMsg = CCcSrvMsg::NewL();
    CleanupStack::PushL( reqMsg );
    reqMsg->SetMsgId( EHsCcViewListReq );
    reqMsg->SetTrId( 0 );
    reqMsg->SetData( KNullDesC8() );
    
    // Marshal API request
    HBufC8* msgBuf = reqMsg->MarshalL();
    CleanupStack::PushL( msgBuf );
    TPtr8 msgPtr( NULL, 0 );
    msgPtr.Set( msgBuf->Des() );
 
    // Send API request
    // Sender and receiver address not defined -> message is routed
    // according to the provider id
    TPckgBuf<TUint32> provider( ECcHomescreen );
    TPckgBuf<TUint32> sender;
    TPckgBuf<TUint32> receiver;
    err = iSession.Send( ECcApiReq, provider, sender, receiver, msgPtr );
    
    if ( !err )
        {
        // Internalize ViewListResp API response
        CCcSrvMsg* respMsg = CCcSrvMsg::NewL();
        CleanupStack::PushL( respMsg );
        RDesReadStream respStream( msgPtr );
        CleanupClosePushL( respStream );
        respMsg->InternalizeHeaderL( respStream );
        CleanupStack::PopAndDestroy( &respStream );
        err = respMsg->Status();
        if ( !err )
            {
            if ( respMsg->DataSize() )
                {
                // Get API response data
                HBufC8* dataBuf = HBufC8::NewL( respMsg->DataSize() );
                CleanupStack::PushL( dataBuf );
                TPtr8 dataPtr( NULL, 0 );
                dataPtr.Set( dataBuf->Des() );
                TPckgBuf<TUint32> trId( respMsg->TrId() );
                err = iSession.GetMsgData( trId, dataPtr );
                if ( !err )
                    {
                    // Internalize API response data
                    RDesReadStream dataStream( dataPtr );
                    CleanupClosePushL( dataStream );
                    aArray.InternalizeL( dataStream );                    
                    CleanupStack::PopAndDestroy( &dataStream );
                    }
                CleanupStack::PopAndDestroy( dataBuf );
                }
            }
        CleanupStack::PopAndDestroy( respMsg );
        }

    // Cleanup
    CleanupStack::PopAndDestroy( msgBuf );
    CleanupStack::PopAndDestroy( reqMsg );

    return err;
    }

// -----------------------------------------------------------------------------
// CHsCcApiClient::AppListL
// -----------------------------------------------------------------------------
//
TInt CHsCcApiClient::AppListL( CHsContentInfoArray& aArray )
    {
    TInt err( KErrNone );
    
    // Create AppListReq API request
    CCcSrvMsg* reqMsg = CCcSrvMsg::NewL();
    CleanupStack::PushL( reqMsg );
    reqMsg->SetMsgId( EHsCcAppListReq );
    reqMsg->SetTrId( 0 );
    reqMsg->SetData( KNullDesC8() );
    
    // Marshal API request
    HBufC8* msgBuf = reqMsg->MarshalL();
    CleanupStack::PushL( msgBuf );
    TPtr8 msgPtr( NULL, 0 );
    msgPtr.Set( msgBuf->Des() );
 
    // Send API request
    // Sender and receiver address not defined -> message is routed
    // according to the provider id
    TPckgBuf<TUint32> provider( ECcHomescreen );
    TPckgBuf<TUint32> sender;
    TPckgBuf<TUint32> receiver;
    err = iSession.Send( ECcApiReq, provider, sender, receiver, msgPtr );
    
    if ( !err )
        {
        // Internalize AppListResp API response
        CCcSrvMsg* respMsg = CCcSrvMsg::NewL();
        CleanupStack::PushL( respMsg );
        RDesReadStream respStream( msgPtr );
        CleanupClosePushL( respStream );
        respMsg->InternalizeHeaderL( respStream );
        CleanupStack::PopAndDestroy( &respStream );
        err = respMsg->Status();
        if ( !err )
            {
            if ( respMsg->DataSize() )
                {
                // Get API response data
                HBufC8* dataBuf = HBufC8::NewL( respMsg->DataSize() );
                CleanupStack::PushL( dataBuf );
                TPtr8 dataPtr( NULL, 0 );
                dataPtr.Set( dataBuf->Des() );
                TPckgBuf<TUint32> trId( respMsg->TrId() );
                err = iSession.GetMsgData( trId, dataPtr );
                if ( !err )
                    {
                    // Internalize API response data
                    RDesReadStream dataStream( dataPtr );
                    CleanupClosePushL( dataStream );
                    aArray.InternalizeL( dataStream );                    
                    CleanupStack::PopAndDestroy( &dataStream );
                    }
                CleanupStack::PopAndDestroy( dataBuf );
                }
            }
        CleanupStack::PopAndDestroy( respMsg );
        }

    // Cleanup
    CleanupStack::PopAndDestroy( msgBuf );
    CleanupStack::PopAndDestroy( reqMsg );

    return err;
    }

// -----------------------------------------------------------------------------
// CHsCcApiClient::AddWidgetL
// -----------------------------------------------------------------------------
//
TInt CHsCcApiClient::AddWidgetL( CHsContentInfo& aInfo )
    {
    TInt err( KErrNone );

    // Create AddWidgetReq API request
    CCcSrvMsg* reqMsg = CCcSrvMsg::NewL();
    CleanupStack::PushL( reqMsg );
    reqMsg->SetMsgId( EHsCcAddWidgetReq );
    reqMsg->SetTrId( 0 );

    // Marshal AddWidgetReq data to a descriptor
    HBufC8* dataBuf = aInfo.MarshalL();
    TPtr8 dataPtr( NULL, 0 );
    dataPtr.Set( dataBuf->Des() );
    reqMsg->SetData( dataPtr );
    
    delete dataBuf;
    dataBuf = NULL;
    
    // Marshal API request
    HBufC8* msgBuf = reqMsg->MarshalL();
    CleanupStack::PushL( msgBuf );
    TPtr8 msgPtr( NULL, 0 );
    msgPtr.Set( msgBuf->Des() );
    
    // Send API request
    // Sender and receiver address not defined -> message is routed
    // according to the provider id
    TPckgBuf<TUint32> provider( ECcHomescreen );
    TPckgBuf<TUint32> sender;
    TPckgBuf<TUint32> receiver;
    err = iSession.Send( ECcApiReq, provider, sender, receiver, msgPtr );
    
    if ( !err )
        {
        // Internalize AddWidgetResp API response
        CCcSrvMsg* respMsg = CCcSrvMsg::NewL();
        CleanupStack::PushL( respMsg );
        RDesReadStream respStream( msgPtr );
        CleanupClosePushL( respStream );
        respMsg->InternalizeHeaderL( respStream );
        CleanupStack::PopAndDestroy( &respStream );
        err = respMsg->Status();
        CleanupStack::PopAndDestroy( respMsg );
        }

    // Cleanup
    CleanupStack::PopAndDestroy( msgBuf );
    CleanupStack::PopAndDestroy( reqMsg );
    
    return err;
    }

// -----------------------------------------------------------------------------
// CHsCcApiClient::RemoveWidgetL
// -----------------------------------------------------------------------------
//
TInt CHsCcApiClient::RemoveWidgetL( CHsContentInfo& aInfo )
    {
    TInt err( KErrNone );

    // Create RemoveWidgetReq API request
    CCcSrvMsg* reqMsg = CCcSrvMsg::NewL();
    CleanupStack::PushL( reqMsg );
    reqMsg->SetMsgId( EHsCcRemoveWidgetReq );
    reqMsg->SetTrId( 0 );

    // Marshal RemoveWidgetReq data to a descriptor
    HBufC8* dataBuf = aInfo.MarshalL();
    TPtr8 dataPtr( NULL, 0 );
    dataPtr.Set( dataBuf->Des() );
    reqMsg->SetData( dataPtr );
    
    delete dataBuf;
    dataBuf = NULL;
    
    // Marshal API request
    HBufC8* msgBuf = reqMsg->MarshalL();
    CleanupStack::PushL( msgBuf );
    TPtr8 msgPtr( NULL, 0 );
    msgPtr.Set( msgBuf->Des() );
    
    // Send API request
    // Sender and receiver address not defined -> message is routed
    // according to the provider id
    TPckgBuf<TUint32> provider( ECcHomescreen );
    TPckgBuf<TUint32> sender;
    TPckgBuf<TUint32> receiver;
    err = iSession.Send( ECcApiReq, provider, sender, receiver, msgPtr );
    
    if ( !err )
        {
        // Internalize RemoveWidgetResp API response
        CCcSrvMsg* respMsg = CCcSrvMsg::NewL();
        CleanupStack::PushL( respMsg );
        RDesReadStream respStream( msgPtr );
        CleanupClosePushL( respStream );
        respMsg->InternalizeHeaderL( respStream );
        CleanupStack::PopAndDestroy( &respStream );
        err = respMsg->Status();
        CleanupStack::PopAndDestroy( respMsg );
        }

    // Cleanup
    CleanupStack::PopAndDestroy( msgBuf );
    CleanupStack::PopAndDestroy( reqMsg );
    
    return err;
    }

// -----------------------------------------------------------------------------
// CHsCcApiClient::AddViewL
// -----------------------------------------------------------------------------
//
TInt CHsCcApiClient::AddViewL( CHsContentInfo& aInfo )
    {
    TInt err( KErrNone );

    // Create AddViewReq API request
    CCcSrvMsg* reqMsg = CCcSrvMsg::NewL();
    CleanupStack::PushL( reqMsg );
    reqMsg->SetMsgId( EHsCcAddViewReq );
    reqMsg->SetTrId( 0 );

    // Marshal AddViewReq data to a descriptor
    HBufC8* dataBuf = aInfo.MarshalL();
    TPtr8 dataPtr( NULL, 0 );
    dataPtr.Set( dataBuf->Des() );
    reqMsg->SetData( dataPtr );
    
    delete dataBuf;
    dataBuf = NULL;
    
    // Marshal API request
    HBufC8* msgBuf = reqMsg->MarshalL();
    CleanupStack::PushL( msgBuf );
    TPtr8 msgPtr( NULL, 0 );
    msgPtr.Set( msgBuf->Des() );
    
    // Send API request
    // Sender and receiver address not defined -> message is routed
    // according to the provider id
    TPckgBuf<TUint32> provider( ECcHomescreen );
    TPckgBuf<TUint32> sender;
    TPckgBuf<TUint32> receiver;
    err = iSession.Send( ECcApiReq, provider, sender, receiver, msgPtr );
    
    if ( !err )
        {
        // Internalize AddViewResp API response
        CCcSrvMsg* respMsg = CCcSrvMsg::NewL();
        CleanupStack::PushL( respMsg );
        RDesReadStream respStream( msgPtr );
        CleanupClosePushL( respStream );
        respMsg->InternalizeHeaderL( respStream );
        CleanupStack::PopAndDestroy( &respStream );
        err = respMsg->Status();
        CleanupStack::PopAndDestroy( respMsg );
        }

    // Cleanup
    CleanupStack::PopAndDestroy( msgBuf );
    CleanupStack::PopAndDestroy( reqMsg );
    
    return err;
    }

// -----------------------------------------------------------------------------
// CHsCcApiClient::RemoveViewL
// -----------------------------------------------------------------------------
//
TInt CHsCcApiClient::RemoveViewL( CHsContentInfo& aInfo )
    {
    TInt err( KErrNone );

    // Create RemoveViewReq API request
    CCcSrvMsg* reqMsg = CCcSrvMsg::NewL();
    CleanupStack::PushL( reqMsg );
    reqMsg->SetMsgId( EHsCcRemoveViewReq );
    reqMsg->SetTrId( 0 );

    // Marshal RemoveViewReq data to a descriptor
    HBufC8* dataBuf = aInfo.MarshalL();
    TPtr8 dataPtr( NULL, 0 );
    dataPtr.Set( dataBuf->Des() );
    reqMsg->SetData( dataPtr );
    
    delete dataBuf;
    dataBuf = NULL;
    
    // Marshal API request
    HBufC8* msgBuf = reqMsg->MarshalL();
    CleanupStack::PushL( msgBuf );
    TPtr8 msgPtr( NULL, 0 );
    msgPtr.Set( msgBuf->Des() );
    
    // Send API request
    // Sender and receiver address not defined -> message is routed
    // according to the provider id
    TPckgBuf<TUint32> provider( ECcHomescreen );
    TPckgBuf<TUint32> sender;
    TPckgBuf<TUint32> receiver;
    err = iSession.Send( ECcApiReq, provider, sender, receiver, msgPtr );
    
    if ( !err )
        {
        // Internalize RemoveViewResp API response
        CCcSrvMsg* respMsg = CCcSrvMsg::NewL();
        CleanupStack::PushL( respMsg );
        RDesReadStream respStream( msgPtr );
        CleanupClosePushL( respStream );
        respMsg->InternalizeHeaderL( respStream );
        CleanupStack::PopAndDestroy( &respStream );
        err = respMsg->Status();
        CleanupStack::PopAndDestroy( respMsg );
        }

    // Cleanup
    CleanupStack::PopAndDestroy( msgBuf );
    CleanupStack::PopAndDestroy( reqMsg );
    
    return err;
    }

// -----------------------------------------------------------------------------
// CHsCcApiClient::ActivateViewL
// -----------------------------------------------------------------------------
//
TInt CHsCcApiClient::ActivateViewL( CHsContentInfo& aInfo )
    {
    TInt err( KErrNone );

    // Create ActivateViewReq API request
    CCcSrvMsg* reqMsg = CCcSrvMsg::NewL();
    CleanupStack::PushL( reqMsg );
    reqMsg->SetMsgId( EHsCcActivateViewReq );
    reqMsg->SetTrId( 0 );

    // Marshal ActivateViewReq data to a descriptor
    HBufC8* dataBuf = aInfo.MarshalL();
    TPtr8 dataPtr( NULL, 0 );
    dataPtr.Set( dataBuf->Des() );
    reqMsg->SetData( dataPtr );
    
    delete dataBuf;
    dataBuf = NULL;
    
    // Marshal API request
    HBufC8* msgBuf = reqMsg->MarshalL();
    CleanupStack::PushL( msgBuf );
    TPtr8 msgPtr( NULL, 0 );
    msgPtr.Set( msgBuf->Des() );
    
    // Send API request
    // Sender and receiver address not defined -> message is routed
    // according to the provider id
    TPckgBuf<TUint32> provider( ECcHomescreen );
    TPckgBuf<TUint32> sender;
    TPckgBuf<TUint32> receiver;
    err = iSession.Send( ECcApiReq, provider, sender, receiver, msgPtr );
    
    if ( !err )
        {
        // Internalize ActivateViewResp API response
        CCcSrvMsg* respMsg = CCcSrvMsg::NewL();
        CleanupStack::PushL( respMsg );
        RDesReadStream respStream( msgPtr );
        CleanupClosePushL( respStream );
        respMsg->InternalizeHeaderL( respStream );
        CleanupStack::PopAndDestroy( &respStream );
        err = respMsg->Status();
        CleanupStack::PopAndDestroy( respMsg );
        }

    // Cleanup
    CleanupStack::PopAndDestroy( msgBuf );
    CleanupStack::PopAndDestroy( reqMsg );
    
    return err;
    }

// -----------------------------------------------------------------------------
// CHsCcApiClient::ActivateAppL
// -----------------------------------------------------------------------------
//
TInt CHsCcApiClient::ActivateAppL( CHsContentInfo& aInfo )
    {
    TInt err( KErrNone );

    // Create ActivateAppReq API request
    CCcSrvMsg* reqMsg = CCcSrvMsg::NewL();
    CleanupStack::PushL( reqMsg );
    reqMsg->SetMsgId( EHsCcActivateAppReq );
    reqMsg->SetTrId( 0 );

    // Marshal ActivateAppReq data to a descriptor
    HBufC8* dataBuf = aInfo.MarshalL();
    TPtr8 dataPtr( NULL, 0 );
    dataPtr.Set( dataBuf->Des() );
    reqMsg->SetData( dataPtr );
    
    delete dataBuf;
    dataBuf = NULL;
    
    // Marshal API request
    HBufC8* msgBuf = reqMsg->MarshalL();
    CleanupStack::PushL( msgBuf );
    TPtr8 msgPtr( NULL, 0 );
    msgPtr.Set( msgBuf->Des() );
    
    // Send API request
    // Sender and receiver address not defined -> message is routed
    // according to the provider id
    TPckgBuf<TUint32> provider( ECcHomescreen );
    TPckgBuf<TUint32> sender;
    TPckgBuf<TUint32> receiver;
    err = iSession.Send( ECcApiReq, provider, sender, receiver, msgPtr );
    
    if ( !err )
        {
        // Internalize ActivateAppResp API response
        CCcSrvMsg* respMsg = CCcSrvMsg::NewL();
        CleanupStack::PushL( respMsg );
        RDesReadStream respStream( msgPtr );
        CleanupClosePushL( respStream );
        respMsg->InternalizeHeaderL( respStream );
        CleanupStack::PopAndDestroy( &respStream );
        err = respMsg->Status();
        CleanupStack::PopAndDestroy( respMsg );
        }

    // Cleanup
    CleanupStack::PopAndDestroy( msgBuf );
    CleanupStack::PopAndDestroy( reqMsg );
    
    return err;
    }

// -----------------------------------------------------------------------------
// CHsCcApiClient::ActiveViewL
// -----------------------------------------------------------------------------
//
TInt CHsCcApiClient::ActiveViewL( CHsContentInfo& aInfo )
    {
    TInt err( KErrNone );

    // Create ActiveViewReq API request
    CCcSrvMsg* reqMsg = CCcSrvMsg::NewL();
    CleanupStack::PushL( reqMsg );
    reqMsg->SetMsgId( EHsCcActiveViewReq );
    reqMsg->SetTrId( 0 );
    reqMsg->SetData( KNullDesC8() );
    
    // Marshal API request
    HBufC8* msgBuf = reqMsg->MarshalL();
    CleanupStack::PushL( msgBuf );
    TPtr8 msgPtr( NULL, 0 );
    msgPtr.Set( msgBuf->Des() );
 
    // Send API request
    // Sender and receiver address not defined -> message is routed
    // according to the provider id
    TPckgBuf<TUint32> provider( ECcHomescreen );
    TPckgBuf<TUint32> sender;
    TPckgBuf<TUint32> receiver;
    err = iSession.Send( ECcApiReq, provider, sender, receiver, msgPtr );
    
    if ( !err )
        {
        // Internalize AppListResp API response
        CCcSrvMsg* respMsg = CCcSrvMsg::NewL();
        CleanupStack::PushL( respMsg );
        RDesReadStream respStream( msgPtr );
        CleanupClosePushL( respStream );
        respMsg->InternalizeHeaderL( respStream );
        CleanupStack::PopAndDestroy( &respStream );
        err = respMsg->Status();
        if ( !err )
            {
            if ( respMsg->DataSize() )
                {
                // Get API response data
                HBufC8* dataBuf = HBufC8::NewL( respMsg->DataSize() );
                CleanupStack::PushL( dataBuf );
                TPtr8 dataPtr( NULL, 0 );
                dataPtr.Set( dataBuf->Des() );
                TPckgBuf<TUint32> trId( respMsg->TrId() );
                err = iSession.GetMsgData( trId, dataPtr );
                if ( !err )
                    {
                    // Internalize API response data
                    RDesReadStream dataStream( dataPtr );
                    CleanupClosePushL( dataStream );
                    aInfo.InternalizeL( dataStream );                    
                    CleanupStack::PopAndDestroy( &dataStream );
                    }
                CleanupStack::PopAndDestroy( dataBuf );
                }
            }
        CleanupStack::PopAndDestroy( respMsg );
        }

    // Cleanup
    CleanupStack::PopAndDestroy( msgBuf );
    CleanupStack::PopAndDestroy( reqMsg );
    
    return err;
    }

// -----------------------------------------------------------------------------
// CHsCcApiClient::ActiveAppL
// -----------------------------------------------------------------------------
//
TInt CHsCcApiClient::ActiveAppL( CHsContentInfo& aInfo )
    {
    TInt err( KErrNone );

    // Create ActiveViewReq API request
    CCcSrvMsg* reqMsg = CCcSrvMsg::NewL();
    CleanupStack::PushL( reqMsg );
    reqMsg->SetMsgId( EHsCcActiveAppReq );
    reqMsg->SetTrId( 0 );
    reqMsg->SetData( KNullDesC8() );
    
    // Marshal API request
    HBufC8* msgBuf = reqMsg->MarshalL();
    CleanupStack::PushL( msgBuf );
    TPtr8 msgPtr( NULL, 0 );
    msgPtr.Set( msgBuf->Des() );
 
    // Send API request
    // Sender and receiver address not defined -> message is routed
    // according to the provider id
    TPckgBuf<TUint32> provider( ECcHomescreen );
    TPckgBuf<TUint32> sender;
    TPckgBuf<TUint32> receiver;
    err = iSession.Send( ECcApiReq, provider, sender, receiver, msgPtr );
    
    if ( !err )
        {
        // Internalize AppListResp API response
        CCcSrvMsg* respMsg = CCcSrvMsg::NewL();
        CleanupStack::PushL( respMsg );
        RDesReadStream respStream( msgPtr );
        CleanupClosePushL( respStream );
        respMsg->InternalizeHeaderL( respStream );
        CleanupStack::PopAndDestroy( &respStream );
        err = respMsg->Status();
        if ( !err )
            {
            if ( respMsg->DataSize() )
                {
                // Get API response data
                HBufC8* dataBuf = HBufC8::NewL( respMsg->DataSize() );
                CleanupStack::PushL( dataBuf );
                TPtr8 dataPtr( NULL, 0 );
                dataPtr.Set( dataBuf->Des() );
                TPckgBuf<TUint32> trId( respMsg->TrId() );
                err = iSession.GetMsgData( trId, dataPtr );
                if ( !err )
                    {
                    // Internalize API response data
                    RDesReadStream dataStream( dataPtr );
                    CleanupClosePushL( dataStream );
                    aInfo.InternalizeL( dataStream );                    
                    CleanupStack::PopAndDestroy( &dataStream );
                    }
                CleanupStack::PopAndDestroy( dataBuf );
                }
            }
        CleanupStack::PopAndDestroy( respMsg );
        }

    // Cleanup
    CleanupStack::PopAndDestroy( msgBuf );
    CleanupStack::PopAndDestroy( reqMsg );
    
    return err;
    }

// -----------------------------------------------------------------------------
// CHsCcApiClient::WaitForApiNtfL()
// -----------------------------------------------------------------------------
//
void CHsCcApiClient::WaitForApiNtfL()
    {

    if ( iApiHeader )
        {
        delete iApiHeader;
        iApiHeader = NULL;
        }
    iApiHeader = HBufC8::NewL( KCcHeaderSize );
    iApiHeaderPtr.Set( iApiHeader->Des() );
    
    TPckgBuf<TUint32> provider( ECcHomescreen );
    iPckgSender = 0;
    iPckgReceiver = 0;
    
    iSession.WaitForApiNtf( provider, iPckgSender, iPckgReceiver, iApiHeaderPtr, iStatus );
    SetActive();
    
    }

// End of file
