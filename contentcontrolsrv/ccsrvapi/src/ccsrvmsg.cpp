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
#include "ccsrvapi.h"

// Local constants

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CCcSrvMsg::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CCcSrvMsg* CCcSrvMsg::NewL()
    {
    CCcSrvMsg* self = new ( ELeave ) CCcSrvMsg();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return( self ) ;
    }

// -----------------------------------------------------------------------
// CCcSrvMsg::ConstructL()
// -----------------------------------------------------------------------
//
void CCcSrvMsg::ConstructL()
    {
    }

// -----------------------------------------------------------------------
// CCcSrvMsg::CCcSrvMsg()
// -----------------------------------------------------------------------
//
CCcSrvMsg::CCcSrvMsg()
    {
    }

// -----------------------------------------------------------------------
// CCcSrvMsg::~CCcSrvMsg()
// -----------------------------------------------------------------------
//
CCcSrvMsg::~CCcSrvMsg()
    {
    delete iData;
    iData = NULL;
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::MsgId()
// -----------------------------------------------------------------------------
EXPORT_C RMessage2 CCcSrvMsg::Message()
    {
    return( iMessage ) ;
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::SetMsgId()
// -----------------------------------------------------------------------------
EXPORT_C void CCcSrvMsg::SetMessage( RMessage2& aMessage )
    {
    iMessage = aMessage;
    SetFunction( aMessage.Function() );
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::Sender()
// -----------------------------------------------------------------------------
EXPORT_C TInt CCcSrvMsg::Function()
    {
    return( iFunction ) ;
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::SetSender()
// -----------------------------------------------------------------------------
EXPORT_C void CCcSrvMsg::SetFunction( TInt aFunction )
    {
    iFunction = aFunction;
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::Sender()
// -----------------------------------------------------------------------------
EXPORT_C TUint32 CCcSrvMsg::Sender()
    {
    return( iSender ) ;
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::SetSender()
// -----------------------------------------------------------------------------
EXPORT_C void CCcSrvMsg::SetSender( TUint32 aSender )
    {
    iSender = aSender;
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::Receiver()
// -----------------------------------------------------------------------------
EXPORT_C TUint32 CCcSrvMsg::Receiver()
    {
    return( iReceiver ) ;
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::SetReceiver()
// -----------------------------------------------------------------------------
EXPORT_C void CCcSrvMsg::SetReceiver( TUint32 aReceiver )
    {
    iReceiver = aReceiver;
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::MsgId()
// -----------------------------------------------------------------------------
EXPORT_C TUint32 CCcSrvMsg::MsgId()
    {
    return( iMsgId ) ;
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::SetMsgId()
// -----------------------------------------------------------------------------
EXPORT_C void CCcSrvMsg::SetMsgId( TUint32 aMsgId )
    {
    iMsgId = aMsgId;
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::TrId()
// -----------------------------------------------------------------------------
EXPORT_C TUint32 CCcSrvMsg::TrId()
    {
    return( iTrId ) ;
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::SetTrId()
// -----------------------------------------------------------------------------
EXPORT_C void CCcSrvMsg::SetTrId( TUint32 aTrId )
    {
    iTrId = aTrId;
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::Status()
// -----------------------------------------------------------------------------
EXPORT_C TInt CCcSrvMsg::Status()
    {
    return( iStatus ) ;
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::SetStatus()
// -----------------------------------------------------------------------------
EXPORT_C void CCcSrvMsg::SetStatus( TInt aStatus )
    {
    iStatus = aStatus;
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::Data()
// -----------------------------------------------------------------------------
EXPORT_C TPtrC8 CCcSrvMsg::Data()
    {
    if ( iData )
        {
        return iData->Des();
        }
    else
        {
        return KNullDesC8();
        }
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::SetData()
// -----------------------------------------------------------------------------
EXPORT_C void CCcSrvMsg::SetData( const TDesC8& aData )
    {
    if ( iData )
        {
        delete iData;
        }
    iData = aData.Alloc();
    iDataSize = iData->Des().Length();
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::DataSize()
// -----------------------------------------------------------------------------
EXPORT_C TInt CCcSrvMsg::DataSize()
    {
    return iDataSize;
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::ExternalizeL()
// -----------------------------------------------------------------------------
EXPORT_C void CCcSrvMsg::ExternalizeL( RWriteStream& aStream )
    {
    ExternalizeHeaderL( aStream );
    if ( iData )
        {
        aStream << *iData;
        }
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::ExternalizeHeaderL()
// -----------------------------------------------------------------------------
EXPORT_C void CCcSrvMsg::ExternalizeHeaderL( RWriteStream& aStream )
    {
    aStream.WriteUint32L( iMsgId );
    aStream.WriteUint32L( iTrId );
    aStream.WriteInt32L( iStatus );
    aStream.WriteInt32L( iDataSize );
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::InternalizeL()
// -----------------------------------------------------------------------------
EXPORT_C void CCcSrvMsg::InternalizeL( RReadStream& aStream )
    {
    InternalizeHeaderL( aStream );
    if ( iData )
        {
        delete iData;
        iData = NULL;
        }
    if ( iDataSize )
        {
        iData = HBufC8::NewL( aStream, iDataSize );
        }
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::InternalizeHeaderL()
// -----------------------------------------------------------------------------
EXPORT_C void CCcSrvMsg::InternalizeHeaderL( RReadStream& aStream )
    {
    iMsgId = aStream.ReadUint32L();
    iTrId = aStream.ReadUint32L();
    iStatus = aStream.ReadInt32L();
    iDataSize = aStream.ReadInt32L();
    }

// -----------------------------------------------------------------------------
// CCcSrvMsg::MarshalL()
// -----------------------------------------------------------------------------
EXPORT_C HBufC8* CCcSrvMsg::MarshalL()
    {
    // Externalize message
    CBufFlat* reqBuf = CBufFlat::NewL( KCcHeaderSize + iDataSize );
    CleanupStack::PushL( reqBuf );
    RBufWriteStream reqStream( *reqBuf );
    CleanupClosePushL( reqStream );
    ExternalizeL( reqStream );
    CleanupStack::PopAndDestroy( &reqStream );
    
    // Append externalized messgae to a descriptor
    HBufC8* msgDesc = HBufC8::NewL( reqBuf->Size() );
    TPtr8 msgPtr( NULL, 0 ); 
    msgPtr.Set( msgDesc->Des() );
    reqBuf->Read( 0, msgPtr, reqBuf->Size() );
    CleanupStack::PopAndDestroy( reqBuf );
    
    return msgDesc;
    }

// End of file
