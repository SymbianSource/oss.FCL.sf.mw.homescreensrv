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

#ifndef CCCLIENTSESSION_H
#define CCCLIENTSESSION_H

// System includes
#include <e32base.h>

// Forward declarations

/**
 *  Content control server client side session
 *
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib hsccsrv.lib
 *  @since S60 v5.0
 */
class RCcClientSession : public RSessionBase
    {
protected:
    /**
     * Connect.
     * 
     * @since S60 5.0
     * @return Symbian error code.
     */
    IMPORT_C TInt Connect();

    /**
     * Close.
     *   
     * @since S60 5.0
     */
    IMPORT_C void Close();

    /**
     * Registers content control API provider
     * @param aProvider Provider id
     * @param aAddress Registered address of service provider
     * @return KErrNone if registration succeeded
     *         Symbian error code if registration fails  
     * @since S60 5.0
     */
    IMPORT_C TInt RegisterProvider(
        TPckgBuf<TUint32>& aProvider,
        TPckgBuf<TUint32>& aAddress );

    /**
     * Registers content control API observer
     * @param aProvider Provider id
     * @param aAddress Registered address of observer
     * @return KErrNone if registration succeeded
     *         Symbian error code if registration fails  
     * @since S60 5.0
     */
    IMPORT_C TInt RegisterObserver(
        TPckgBuf<TUint32>& aProvider,
        TPckgBuf<TUint32>& aAddress );

    /**
     * Cancels outstanding async request
     * @param aFunction Canceled function
     * @return KErrNone if registration succeeded
     *         Symbian error code if registration fails  
     * @since S60 5.0
     */
    IMPORT_C TInt CancelReq(
        TPckgBuf<TInt>& aFunction );

    /**
     * Starts receiving API requests
     * @param aProvider Provider id
     * @param aSender Sender of Api request
     * @param aReceiver Service provider
     * @param aBuf Message buffer for received API request header
     * @param aStatus Completion status for IPC request
     * @since S60 5.0
     */
    IMPORT_C void WaitForApiReq(
        TPckgBuf<TUint32>& aProvider,
        TPckgBuf<TUint32>& aSender,
        TPckgBuf<TUint32>& aReceiver,
        TPtr8& aBuf,
        TRequestStatus& aStatus );

    /**
     * Starts receiving API notifications
     * @param aProvider Provider id
     * @param aSender Sender of Api request
     * @param aReceiver Service provider
     * @param aBuf Message buffer for received API notification header
     * @param aStatus Completion status for IPC request
     * @since S60 5.0
     */
    IMPORT_C void WaitForApiNtf(
        TPckgBuf<TUint32>& aProvider,
        TPckgBuf<TUint32>& aSender,
        TPckgBuf<TUint32>& aReceiver,
        TPtr8& aBuf,
        TRequestStatus& aStatus );

    /**
     * Sends API request/response to Home screen content control server
     * @param aFunction IPC function EHsCcApiReq/EHsCcApiResp
     * @param aProvider Provider id
     * @param aSender Sender of Api request
     * @param aReceiver Service provider
     * @param aBuf Message buffer: 
     *             in: service request 
     *             out: service response header
     * @return KErrNone if sending succeeded
     *         Symbian error code if sending fails
     * @since S60 5.0
     */
    IMPORT_C TInt Send(
        TInt aFunction,
        TPckgBuf<TUint32>& aProvider,
        TPckgBuf<TUint32>& aSender,
        TPckgBuf<TUint32>& aReceiver,
        TPtr8& aBuf );

    /**
     * Reads message data
     * @param aTrId Transaction id
     * @param aBuf Data buffer
     * @return KErrNone if reading succeeded
     *         Symbian error code if reading fails
     * @since S60 5.0
     */
    IMPORT_C TInt GetMsgData(
        TPckgBuf<TUint32>& aTrId,
        TPtr8& aBuf );

public:
    friend class CHsCcProviderClient;
    friend class CHsCcApiClient;

    };


#endif // CCCLIENTSESSION_H

// End of file
