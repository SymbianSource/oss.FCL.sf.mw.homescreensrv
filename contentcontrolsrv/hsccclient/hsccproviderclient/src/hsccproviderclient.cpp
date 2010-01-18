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
#include <hscontentcontroller.h>
#include <hscontentinfoarray.h>
#include <hscontentinfo.h>

// User include files
#include "hsccproviderclient.h"
#include "ccresource.h"
#include "ccsrvapi.h"
#include "hsccapi.h"

// Local constants

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CHsCcProviderClient::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CHsCcProviderClient* CHsCcProviderClient::NewL(
    MHsContentController& aController )
    {
    CHsCcProviderClient* self = new ( ELeave ) CHsCcProviderClient( aController );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return( self ) ;
    }

// -----------------------------------------------------------------------
// CHsCcProviderClient::ConstructL()
// -----------------------------------------------------------------------
//
void CHsCcProviderClient::ConstructL()
    {    
    User::LeaveIfError( iSession.Connect() );
    
    TPckgBuf<TUint32> provider( ECcHomescreen );
    TPckgBuf<TUint32> address;
    User::LeaveIfError( iSession.RegisterProvider( 
        provider,
        address ) );
    iAddress = address();
    
    WaitForApiReqL();
    
    }

// -----------------------------------------------------------------------
// CHsCcProviderClient::CHsCcProviderClient()
// -----------------------------------------------------------------------
//
CHsCcProviderClient::CHsCcProviderClient(
    MHsContentController& aController )
    :CActive( EPriorityStandard )
    ,iController( aController )
    ,iAddress( 0 )
    ,iApiHeader( NULL )
    ,iApiHeaderPtr( NULL, 0 )
    ,iApiData( NULL )
    ,iApiDataPtr( NULL, 0 )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------
// CHsCcProviderClient::~CHsCcProviderClient()
// -----------------------------------------------------------------------
//
    CHsCcProviderClient::~CHsCcProviderClient()
    {
    Cancel();
    iSession.Close();
    delete iApiHeader;
    delete iApiData;
    iObservers.Close();
    }

// -----------------------------------------------------------------------------
// CHsCcClient::RunL()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::RunL()
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
        if ( message->DataSize() )
            {
            // Get message data
            HBufC8* dataBuf = HBufC8::NewL( message->DataSize() );
            CleanupStack::PushL( dataBuf );
            TPtr8 dataPtr( NULL, 0 );
            dataPtr.Set( dataBuf->Des() );
            TPckgBuf<TUint32> trId( message->TrId() );
            iSession.GetMsgData( trId, dataPtr );
            message->SetData( dataPtr );
            CleanupStack::PopAndDestroy( dataBuf );
            }

        switch ( message->MsgId() )
            {
            case ECcRegisterObserverNtf:
                HandleRegisterObserverNtfL( *message );
                break;
            case ECcUnregisterObserverNtf:
                HandleUnregisterObserverNtfL( *message );
                break;
            case EHsCcWidgetListReq:
                HandleWidgetListReqL( *message );
                break;
            case EHsCcAddWidgetReq:
                HandleAddWidgetReqL( *message );
                break;
            case EHsCcRemoveWidgetReq:
                HandleRemoveWidgetReqL( *message );
                break;
            case EHsCcViewListReq:
                HandleViewListReqL( *message );
                break;
            case EHsCcAddViewReq:
                HandleAddViewReqL( *message );
                break;
            case EHsCcRemoveViewReq:
                HandleRemoveViewReqL( *message );
                break;
            case EHsCcActivateViewReq:
                HandleActivateViewReqL( *message );
                break;
            case EHsCcAppListReq:
                HandleAppListReqL( *message );
                break;
            case EHsCcActivateAppReq:
                HandleActivateAppReqL( *message );
                break;
            case EHsCcActiveAppReq:
                HandleActiveAppReqL( *message );
                break;
            case EHsCcActiveViewReq:
                HandleActiveViewReqL( *message );
                break;
            default:
                HandleNotSupportedReqL( *message );
                break;
            }
        CleanupStack::PopAndDestroy( message );
        }
    
    // Receive next API request
    WaitForApiReqL();
    
    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::DoCancel()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::DoCancel()
    {
    if ( IsActive() )
        {
        TPckgBuf<TInt> function( ECcWaitForApiReq );
        iSession.CancelReq( function );
        }
    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::NotifyWidgetListChanged()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::NotifyWidgetListChanged()
    {
    if ( iObservers.Count() )
        {
        TRAP_IGNORE( SendNtfL( EHsCcWidgetListChangedNtf ) );
        }
    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::NotifyViewListChanged()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::NotifyViewListChanged()
    {
    if ( iObservers.Count() )
        {
        TRAP_IGNORE( SendNtfL( EHsCcViewListChangedNtf ) );
        }
    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::NotifyAppListChanged()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::NotifyAppListChanged()
    {
    if ( iObservers.Count() )
        {
        TRAP_IGNORE( SendNtfL( EHsCcAppListChangedNtf ) );
        }
    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::WaitForApiReqL()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::WaitForApiReqL()
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
    
    iSession.WaitForApiReq( provider, iPckgSender, iPckgReceiver, iApiHeaderPtr, iStatus );
    SetActive();
    
    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::HandleRegisterObserverNtfL()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::HandleRegisterObserverNtfL(
    CCcSrvMsg& /* aMessage */ )
    {
    TUint32 observer = iPckgSender();
    iObservers.AppendL( observer );
    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::HandleUnregisterObserverNtfL()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::HandleUnregisterObserverNtfL(
    CCcSrvMsg& /* aMessage */ )
    {
    TUint32 observer = iPckgSender();
    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        if ( iObservers[ i ] == observer )
            {
            iObservers.Remove( i );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::HandleWidgetListReqL()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::HandleWidgetListReqL(
    CCcSrvMsg& aMessage )
    {
    // Get widget list
    CHsContentInfoArray* widgets = CHsContentInfoArray::NewL();
    CleanupStack::PushL( widgets );
    TInt err = iController.WidgetListL( *widgets );

    // Create and send WidgetListResp 
    CCcSrvMsg* message = CCcSrvMsg::NewL();
    CleanupStack::PushL( message );
    message->SetMsgId( EHsCcWidgetListResp );
    message->SetTrId( aMessage.TrId() );
    message->SetStatus( err );
    
    if ( !err )
        {
        // Externalize widget list
        HBufC8* dataBuf = widgets->MarshalL();
        CleanupStack::PushL( dataBuf );
        TPtr8 dataPtr( NULL, 0 );
        dataPtr.Set( dataBuf->Des() );
        message->SetData( dataPtr );
        CleanupStack::PopAndDestroy( dataBuf );
        }
    
    SendRespL( *message );
    
    CleanupStack::PopAndDestroy( message );    
    CleanupStack::PopAndDestroy( widgets );
   
    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::HandleAddWidgetReqL()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::HandleAddWidgetReqL(
    CCcSrvMsg& aMessage )
    {
    TInt err( KErrNone );
    if ( aMessage.DataSize() )
        {
        // Internalize message data
        RDesReadStream dataStream( aMessage.Data() );
        CleanupClosePushL( dataStream );
        CHsContentInfo* info = CHsContentInfo::NewL( dataStream );
        CleanupStack::PopAndDestroy( &dataStream );
        CleanupStack::PushL( info );

        // Add widget
        err = iController.AddWidgetL( *info );
        
        CleanupStack::PopAndDestroy( info );        
        }
    else
        {
        // Mandatory message data missing
        err = KErrArgument;
        }

    // Create and send AddWidgetResp
    CCcSrvMsg* message = CCcSrvMsg::NewL();
    CleanupStack::PushL( message );
    message->SetMsgId( EHsCcAddWidgetResp );
    message->SetTrId( aMessage.TrId() );
    message->SetStatus( err );
    message->SetData( KNullDesC8() );

    SendRespL( *message );
    
    CleanupStack::PopAndDestroy( message );    
    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::HandleRemoveWidgetReqL()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::HandleRemoveWidgetReqL(
    CCcSrvMsg& aMessage )
    {
    TInt err( KErrNone );
    if ( aMessage.DataSize() )
        {
        // Internalize message data
        RDesReadStream dataStream( aMessage.Data() );
        CleanupClosePushL( dataStream );
        CHsContentInfo* info = CHsContentInfo::NewL( dataStream );
        CleanupStack::PopAndDestroy( &dataStream );
        CleanupStack::PushL( info );

        // Remove widget
        err = iController.RemoveWidgetL( *info );
        
        CleanupStack::PopAndDestroy( info );        
        }
    else
        {
        // Mandatory message data missing
        err = KErrArgument;
        }

    // Create and send RemoveWidgetResp
    CCcSrvMsg* message = CCcSrvMsg::NewL();
    CleanupStack::PushL( message );
    message->SetMsgId( EHsCcRemoveWidgetResp );
    message->SetTrId( aMessage.TrId() );
    message->SetStatus( err );
    message->SetData( KNullDesC8() );

    SendRespL( *message );
    
    CleanupStack::PopAndDestroy( message );    
    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::HandleViewListReqL()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::HandleViewListReqL(
    CCcSrvMsg& aMessage )
    {
    // Get view list
    CHsContentInfoArray* views = CHsContentInfoArray::NewL();
    CleanupStack::PushL( views );
    TInt err = iController.ViewListL( *views );

    // Create and send ViewListResp
    CCcSrvMsg* message = CCcSrvMsg::NewL();
    CleanupStack::PushL( message );
    message->SetMsgId( EHsCcViewListResp );
    message->SetTrId( aMessage.TrId() );
    message->SetStatus( err );
    
    if ( !err )
        {
        // Externalize view list
        HBufC8* dataBuf = views->MarshalL();
        CleanupStack::PushL( dataBuf );
        TPtr8 dataPtr( NULL, 0 );
        dataPtr.Set( dataBuf->Des() );
        message->SetData( dataPtr );
        CleanupStack::PopAndDestroy( dataBuf );
        }
    
    SendRespL( *message );
    
    CleanupStack::PopAndDestroy( message );    
    CleanupStack::PopAndDestroy( views );
   
    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::HandleAddViewReqL()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::HandleAddViewReqL(
    CCcSrvMsg& aMessage )
    {
    TInt err( KErrNone );
    if ( aMessage.DataSize() )
        {
        // Internalize message data
        RDesReadStream dataStream( aMessage.Data() );
        CleanupClosePushL( dataStream );
        CHsContentInfo* info = CHsContentInfo::NewL( dataStream );
        CleanupStack::PopAndDestroy( &dataStream );
        CleanupStack::PushL( info );

        // Add view
        err = iController.AddViewL( *info );
        
        CleanupStack::PopAndDestroy( info );        
        }
    else
        {
        // Mandatory message data missing
        err = KErrArgument;
        }

    // Create and send AddViewResp
    CCcSrvMsg* message = CCcSrvMsg::NewL();
    CleanupStack::PushL( message );
    message->SetMsgId( EHsCcAddViewResp );
    message->SetTrId( aMessage.TrId() );
    message->SetStatus( err );
    message->SetData( KNullDesC8() );

    SendRespL( *message );
    
    CleanupStack::PopAndDestroy( message );    
    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::HandleRemoveViewReqL()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::HandleRemoveViewReqL(
    CCcSrvMsg& aMessage )
    {
    TInt err( KErrNone );
    if ( aMessage.DataSize() )
        {
        // Internalize message data
        RDesReadStream dataStream( aMessage.Data() );
        CleanupClosePushL( dataStream );
        CHsContentInfo* info = CHsContentInfo::NewL( dataStream );
        CleanupStack::PopAndDestroy( &dataStream );
        CleanupStack::PushL( info );

        // Remove view
        err = iController.RemoveViewL( *info );
        
        CleanupStack::PopAndDestroy( info );        
        }
    else
        {
        // Mandatory message data missing
        err = KErrArgument;
        }

    // Create and send RemoveViewResp
    CCcSrvMsg* message = CCcSrvMsg::NewL();
    CleanupStack::PushL( message );
    message->SetMsgId( EHsCcRemoveViewResp );
    message->SetTrId( aMessage.TrId() );
    message->SetStatus( err );
    message->SetData( KNullDesC8() );

    SendRespL( *message );
    
    CleanupStack::PopAndDestroy( message );    
    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::HandleActivateViewReqL()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::HandleActivateViewReqL(
    CCcSrvMsg& aMessage )
    {
    TInt err( KErrNone );
    if ( aMessage.DataSize() )
        {
        // Internalize message data
        RDesReadStream dataStream( aMessage.Data() );
        CleanupClosePushL( dataStream );
        CHsContentInfo* info = CHsContentInfo::NewL( dataStream );
        CleanupStack::PopAndDestroy( &dataStream );
        CleanupStack::PushL( info );

        // Activate view
        err = iController.ActivateViewL( *info );
        
        CleanupStack::PopAndDestroy( info );        
        }
    else
        {
        // Mandatory message data missing
        err = KErrArgument;
        }

    // Create and send ActivateViewResp
    CCcSrvMsg* message = CCcSrvMsg::NewL();
    CleanupStack::PushL( message );
    message->SetMsgId( EHsCcActivateViewResp );
    message->SetTrId( aMessage.TrId() );
    message->SetStatus( err );
    message->SetData( KNullDesC8() );

    SendRespL( *message );
    
    CleanupStack::PopAndDestroy( message );    
    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::HandleActiveViewReqL()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::HandleActiveViewReqL(
    CCcSrvMsg& aMessage )
    {
    // Get active view
    CHsContentInfo* view = CHsContentInfo::NewL();
    CleanupStack::PushL( view );
    TInt err = iController.ActiveViewL( *view );

    // Create and send ActiveViewResp
    CCcSrvMsg* message = CCcSrvMsg::NewL();
    CleanupStack::PushL( message );
    message->SetMsgId( EHsCcActiveViewResp );
    message->SetTrId( aMessage.TrId() );
    message->SetStatus( err );
    
    if ( !err )
        {
        // Externalize view list
        HBufC8* dataBuf = view->MarshalL();
        CleanupStack::PushL( dataBuf );
        TPtr8 dataPtr( NULL, 0 );
        dataPtr.Set( dataBuf->Des() );
        message->SetData( dataPtr );
        CleanupStack::PopAndDestroy( dataBuf );
        }
    
    SendRespL( *message );
    
    CleanupStack::PopAndDestroy( message );    
    CleanupStack::PopAndDestroy( view );

    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::HandleAppListReqL()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::HandleAppListReqL(
    CCcSrvMsg& aMessage )
    {
    // Get app list
    CHsContentInfoArray* apps = CHsContentInfoArray::NewL();
    CleanupStack::PushL( apps );
    TInt err = iController.AppListL( *apps );

    // Create and send AppListResp
    CCcSrvMsg* message = CCcSrvMsg::NewL();
    CleanupStack::PushL( message );
    message->SetMsgId( EHsCcAppListResp );
    message->SetTrId( aMessage.TrId() );
    message->SetStatus( err );
    if ( !err )
        {
        // Externalize app list
        HBufC8* dataBuf = apps->MarshalL();
        CleanupStack::PushL( dataBuf );
        TPtr8 dataPtr( NULL, 0 );
        dataPtr.Set( dataBuf->Des() );
        message->SetData( dataPtr );
        CleanupStack::PopAndDestroy( dataBuf );
        }

    SendRespL( *message );
    
    CleanupStack::PopAndDestroy( message );    
    CleanupStack::PopAndDestroy( apps );
   
    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::HandleActivateAppReqL()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::HandleActivateAppReqL(
    CCcSrvMsg& aMessage )
    {
    TInt err( KErrNone );
    if ( aMessage.DataSize() )
        {
        // Internalize message data
        RDesReadStream dataStream( aMessage.Data() );
        CleanupClosePushL( dataStream );
        CHsContentInfo* info = CHsContentInfo::NewL( dataStream );
        CleanupStack::PopAndDestroy( &dataStream );
        CleanupStack::PushL( info );

        // Activate view
        err = iController.ActivateAppL( *info );
        
        CleanupStack::PopAndDestroy( info );        
        }
    else
        {
        // Mandatory message data missing
        err = KErrArgument;
        }

    // Create and send ActivateAppResp
    CCcSrvMsg* message = CCcSrvMsg::NewL();
    CleanupStack::PushL( message );
    message->SetMsgId( EHsCcActivateAppResp );
    message->SetTrId( aMessage.TrId() );
    message->SetStatus( err );
    message->SetData( KNullDesC8() );

    SendRespL( *message );
    
    CleanupStack::PopAndDestroy( message );    
    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::HandleActiveAppReqL()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::HandleActiveAppReqL(
    CCcSrvMsg& aMessage )
    {
    // Get active application info
    CHsContentInfo* app = CHsContentInfo::NewL();
    CleanupStack::PushL( app );
    TInt err = iController.ActiveAppL( *app );

    // Create and send ActiveViewResp
    CCcSrvMsg* message = CCcSrvMsg::NewL();
    CleanupStack::PushL( message );
    message->SetMsgId( EHsCcActiveAppResp );
    message->SetTrId( aMessage.TrId() );
    message->SetStatus( err );
    
    if ( !err )
        {
        // Externalize application info
        HBufC8* dataBuf = app->MarshalL();
        CleanupStack::PushL( dataBuf );
        TPtr8 dataPtr( NULL, 0 );
        dataPtr.Set( dataBuf->Des() );
        message->SetData( dataPtr );
        CleanupStack::PopAndDestroy( dataBuf );
        }
    
    SendRespL( *message );
    
    CleanupStack::PopAndDestroy( message );    
    CleanupStack::PopAndDestroy( app );
    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::HandleNotSupportedReqL()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::HandleNotSupportedReqL(
    CCcSrvMsg& aMessage )
    {
    // Create and send NotSupportedResp
    CCcSrvMsg* message = CCcSrvMsg::NewL();
    CleanupStack::PushL( message );
    message->SetMsgId( EHsCcNotSupportedResp );
    message->SetTrId( aMessage.TrId() );
    message->SetStatus( KErrNone );
    message->SetData( KNullDesC8() );

    SendRespL( *message );
    
    CleanupStack::PopAndDestroy( message );    

    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::SendNtfL()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::SendNtfL(
    TUint32 aNtf )
    {
    // Notify registered observers
    CCcSrvMsg* ntf = CCcSrvMsg::NewL();
    CleanupStack::PushL( ntf );
    ntf->SetMsgId( aNtf );
    HBufC8* ntfBuf = ntf->MarshalL();
    CleanupStack::PushL( ntfBuf );
    TPtr8 ntfPtr( NULL, 0 );
    ntfPtr.Set( ntfBuf->Des() );
    
    TPckgBuf<TUint32> provider( ECcHomescreen );
    TPckgBuf<TUint32> sender( iAddress );
    TPckgBuf<TUint32> receiver( 0 );
    iSession.Send( ECcApiNtf, provider, sender, receiver, ntfPtr );
    
    CleanupStack::PopAndDestroy( ntfBuf );
    CleanupStack::PopAndDestroy( ntf );
    }

// -----------------------------------------------------------------------------
// CHsCcProviderClient::SendRespL()
// -----------------------------------------------------------------------------
//
void CHsCcProviderClient::SendRespL(
    CCcSrvMsg& aMessage )
    {
    HBufC8* msgBuf = aMessage.MarshalL();
    CleanupStack::PushL( msgBuf );
    TPtr8 msgPtr( NULL, 0 );
    msgPtr.Set( msgBuf->Des() );

    TPckgBuf<TUint32> provider( ECcHomescreen );
    iSession.Send( ECcApiResp, provider, iPckgReceiver, iPckgSender, msgPtr  );

    CleanupStack::PopAndDestroy( msgBuf );    
    }

// End of file
