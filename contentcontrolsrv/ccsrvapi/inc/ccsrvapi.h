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
* Description:  Content control server API definitions
*
*/

#ifndef CCSRVAPI_H
#define CCSRVAPI_H

// System includes
#include <e32base.h>

// Forward declarations
class RWriteStream;
class RReadStream;

// Constants

// ----------------------------------------------------------------------------
// Content control server IPC functions
// ----------------------------------------------------------------------------
enum TCcSrvIPCFunctions
    {
    ECcIPCFunctionBase = 0,
// ----------------------------------------------------------------------------
// RegisterProvider
// ----------------------------------------------------------------------------
// Purpose:
// This request is used to register content control service provider
// to content control server
//
// IPC parameters:
// - Function: ECcReqisterProvider
// - Arg[0]:   Provider id (in), TInt
// - Arg[1]:   Registered provider address (out), TUint32
// - Arg[2]:   Not used
// - Arg[2]:   Not used
    ECcReqisterProvider,
// ----------------------------------------------------------------------------
// RegisterObserver
// ----------------------------------------------------------------------------
// Purpose:
// This request is used to register observer for content control service
// provider
//
// IPC parameters:
// - Function: ECcRegisterObserver
// - Arg[0]:   Provider id (in), TInt
// - Arg[1]:   Registered observer address (out), TUint32
// - Arg[2]:   Not used
// - Arg[2]:   Not used
    ECcRegisterObserver,
// ----------------------------------------------------------------------------
// WaitForApiReq
// ----------------------------------------------------------------------------
// Purpose:
// This request is used by content control service provider to receive
// API requests
//
// IPC parameters:
// - Function: ECcWaitForApiReq
// - Arg[0]:   Provider id (in), TInt
// - Arg[1]:   Provider address (in), TUint32
// - Arg[2]:   API request sender address (out), TUint32
// - Arg[2]:   API request header buffer (out), TPtr8 pointing to 
//             HBufC8 descriptor
    ECcWaitForApiReq,
// ----------------------------------------------------------------------------
// WaitForApiNtf
// ----------------------------------------------------------------------------
// Purpose:
// This request is used by observer of content control service provider to
// receive content change notifications
//
// IPC parameters:
// - Function: ECcWaitForApiNtf
// - Arg[0]:   Provider id (in), TInt
// - Arg[1]:   Observer address (out), TUint32
// - Arg[2]:   Provider address (out), TUint32
// - Arg[2]:   API notification header buffer (out), TPtr8 pointing to 
//             HBufC8 descriptor
    ECcWaitForApiNtf,
// ----------------------------------------------------------------------------
// ApiReq
// ----------------------------------------------------------------------------
// Purpose:
// This request is used to send a API request to the content control service
// provider
//
// IPC parameters:
// - Function: ECcApiReq
// - Arg[0]:   Provider id (in), TInt
// - Arg[1]:   API request sender address (out), TUint32
// - Arg[2]:   Provider sender address (out), TUint32
// - Arg[2]:   API request buffer (in)/API response header (out), 
//             TPtr8 pointing to HBufC8 descriptor
    ECcApiReq,
// ----------------------------------------------------------------------------
// ApiResp
// ----------------------------------------------------------------------------
// Purpose:
// This request is used to send a API response to the API request sender
//
// IPC parameters:
// - Function: ECcApiResp
// - Arg[0]:   Provider id (in), TInt
// - Arg[1]:   Provider address (in), TUint32
// - Arg[2]:   API response receiver address (in), TUint32
// - Arg[2]:   API response buffer (in), TPtr8 pointing to HBufC8 descriptor
    ECcApiResp,
// ----------------------------------------------------------------------------
// ApiNtf
// ----------------------------------------------------------------------------
// Purpose:
// This request is used to send a API notification
//
// IPC parameters:
// - Function: ECcApiNtf
// - Arg[0]:   Provider id (in), TInt
// - Arg[1]:   Provider address (in), TUint32
// - Arg[2]:   API notification receiver address (in), TUint32
// - Arg[2]:   API notification buffer (in), 
//             TPtr8 pointing to HBufC8 descriptor
    ECcApiNtf,
// ----------------------------------------------------------------------------
// GetMsgData
// ----------------------------------------------------------------------------
// Purpose:
// This request is used to read message data
//
// IPC parameters:
// - Function: ECcGetMsgData
// - Arg[0]:   Transaction id of a message which data is requested (in), TUint32
// - Arg[1]:   Message data buffer (out), TPtr8 pointing to HBufC8 descriptor
// - Arg[2]:   None
// - Arg[2]:   None
    ECcGetMsgData,
// ----------------------------------------------------------------------------
// Cancel
// ----------------------------------------------------------------------------
// Purpose:
// This request is used to cancel outstanding WaitForApiReq or 
// WaitForNotification request
//
// IPC parameters:
// - Function: ECcCancel
// - Arg[0]:   Canceled IPC function (WaitForApiReq||WaitForNotification)
// - Arg[1]:   None
// - Arg[2]:   None
// - Arg[2]:   None
    ECcCancel,
    ECcNotSupported
    };

// ----------------------------------------------------------------------------
// Content control server message header
// ----------------------------------------------------------------------------
// Items:
// - Message id, TInt
// - Transaction id, TInt
// - Status, TInt
// - Data size, TInt
// Externalized message header size:
const TInt KCcHeaderSize = 16;

// ----------------------------------------------------------------------------
// Content control server messages
// ----------------------------------------------------------------------------
enum TCcSrvMessages
    {
    ECcMessageBase,
// ----------------------------------------------------------------------------
// RegisterObserverNtf
// ----------------------------------------------------------------------------
// Purpose:
// This notification is used to inform content provider of registerd observer
//
// Message parameters:
// - Message id:     EHsCcRegisterObserverNtf
// - Transaction id: None
// - Message data:   None
    ECcRegisterObserverNtf,
// ----------------------------------------------------------------------------
// UnregisterObserverNtf
// ----------------------------------------------------------------------------
// Purpose:
// This notification is used to inform content provider of unregisterd observer
//
// Message parameters:
// - Message id:     EHsCcUnregisterObserverNtf
// - Transaction id: None
// - Message data:   None
    ECcUnregisterObserverNtf,
// ----------------------------------------------------------------------------
// ProviderMessageBase
// ----------------------------------------------------------------------------
// Purpose:
// ProviderMessageBase is used to define the start of provider message ids
//
    ECcProviderMessageBase
    };

/**
 *  Content control server message
 *
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib ccsrvapi.lib
 *  @since S60 v5.0
 */
class CCcSrvMsg : public CBase
    {
public: // Constructor and destructor
    /**
     * Two-phased constructor.
     */
    IMPORT_C static CCcSrvMsg* NewL();

    /**
     * Destructor.
     */
    ~CCcSrvMsg();

private: // Constructors
    /**
     * Constructor
     */
    CCcSrvMsg();

    /** Second phase constructor */
    void ConstructL();


public: // New functions
    /**
     * Returns pending request
     */
    IMPORT_C RMessage2 Message();

    /**
     * Stores pending request
     * @param aMessage Pending request
     */
    IMPORT_C void SetMessage( RMessage2& aMessage );

    /**
     * Returns IPC function
     */
    IMPORT_C TInt Function();
    
    /**
     * Stores IPC function
     * @param aFunction IPC function
     */
    IMPORT_C void SetFunction( TInt aFunction );

    /**
     * Returns sender of the message
     */
    IMPORT_C TUint32 Sender();
    
    /**
     * Stores sender of the message
     * @param aSender Message sender
     */
    IMPORT_C void SetSender( TUint32 );

    /**
     * Returns receiver of the message
     */
    IMPORT_C TUint32 Receiver();
    
    /**
     * Stores receiver of the message
     * @param aReceiver Message receiver
     */
    IMPORT_C void SetReceiver( TUint32 );

    /**
     * Returns message id
     */
    IMPORT_C TUint32 MsgId();

    /**
     * Stores message id
     * @param aMsgId Message id
     */
    IMPORT_C void SetMsgId( TUint32 aMsgId );

    /**
     * Returns transaction id
     */
    IMPORT_C TUint32 TrId();

    /**
     * Stores transaction id
     * @param aTrId Transaction id
     */
    IMPORT_C void SetTrId( TUint32 aTrId );

    /**
     * Returns message status
     */
    IMPORT_C TInt Status();

    /**
     * Stores message status
     * @param aStatus Transaction status
     */
    IMPORT_C void SetStatus( TInt aStatus );

    /**
     * Returns pointer to message data
     */
    IMPORT_C TPtrC8 Data();

    /**
     * Stores message data
     * @param aData Message data
     */
    IMPORT_C void SetData( const TDesC8& aData );

    /**
     * Returns message data size
     */
    IMPORT_C TInt DataSize();

    /**
     * Externalizes message (header and data)
     * @param aStream Write stream where message is externalized
     */
    IMPORT_C void ExternalizeL( RWriteStream& aStream );

    /**
     * Externalizes message header
     * @param aStream Write stream where message header is externalized
     */
    IMPORT_C void ExternalizeHeaderL( RWriteStream& aStream );

    /**
     * Internalizes message (header and data)
     * @param aStream Read stream where message is internalized
     */
    IMPORT_C void InternalizeL( RReadStream& aStream );

    /**
     * Internalizes message header
     * @param aStream Read stream where message header is internalized
     */
    IMPORT_C void InternalizeHeaderL( RReadStream& aStream );

    /**
     * Marshals message to a descrptor
     * @return Pointer to created descriptor
     */
    IMPORT_C HBufC8* MarshalL();

private: // Data
    /**
     * Pending request
     */
    RMessage2 iMessage;

    /**
     * IPC function
     */
    TInt iFunction;
    
    /**
     * Sender
     */
    TUint32 iSender;
    
    /**
     * Receiver
     */
    TUint32 iReceiver;
    
    /**
     * Message id
     */
    TUint32 iMsgId;

    /**
     * Transaction id
     */
    TUint32 iTrId;

    /**
     * Operation status
     */
    TInt iStatus;

    /**
     * Data size
     */
    TInt iDataSize;

    /**
     * Data
     */
    HBufC8* iData;

    };


#endif // CCSRVAPI_H

// End of file
