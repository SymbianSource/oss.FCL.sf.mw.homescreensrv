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

#ifndef CCSRVSESSION_H
#define CCSRVSESSION_H

// System includes
#include <e32base.h>

// Forward declarations
#include "ccsrv.h"

/**
 *  Home screen content control server session
 *
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib ccsrv.exe
 *  @since S60 v5.0
 */
class CCcSrvSession : public CSession2
    {
public: // Constructor and destructor
    /**
     * Constructor
     */
    CCcSrvSession();

    /**
     * Destructor.
     */
    ~CCcSrvSession();

public: // From CSession2
    /**
     * 
     */
    void CreateL();
    
    /**
     *
     */
    void ServiceL( 
        const RMessage2& aMessage );

public: // New functions
    /**
     * 
     */
    TUint32 Id();

    /**
     * 
     */
    void SetId( TUint32 aId );

    /**
     *
     *
     */
    void RegisterObserverSessionL(
        TUint32 aObserver );

    /**
     *
     *
     */
    void UnregisterObserverSessionL(
        TUint32 aObserver );

    /**
     *
     *
     */
    void ReceiveMsgL(
        TUint32 aSender,
        TUint32 aReceiver,
        CCcSrvMsg& aMessage );

private:
    /**
     *
     *
     */
    inline CCcSrv& Server()
        {
        return *static_cast<CCcSrv*>( const_cast<CServer2*>( CSession2::Server() ) );
        };

    /**
     *
     */
    void DoServiceL( 
        RMessage2& aMessage );


    /**
     *
     *
     */
    void HandleRegisterProviderL(
        RMessage2& aMessage );

    /**
     *
     *
     */
    void HandleRegisterObserverL(
        RMessage2& aMessage );

    /**
     *
     *
     */
    void HandleCancelReqL(
        RMessage2& aMessage );

    /**
     *
     *
     */
    void HandleWaitForApiReqL(
        RMessage2& aMessage );

    /**
     *
     *
     */
    void HandleWaitForApiNtfL(
        RMessage2& aMessage );

    /**
     *
     *
     */
    void HandleApiReqL(
        RMessage2& aMessage );

    /**
     *
     *
     */
    void HandleApiRespL(
        RMessage2& aMessage );

    /**
     *
     *
     */
    void HandleGetMsgDataL(
        RMessage2& aMessage );

    /**
     *
     *
     */
    void HandleApiNtfL(
        RMessage2& aMessage );

    /**
     *
     *
     */
    void SendObserverNtfL(
        TUint32 aSender,
        TUint32 aNtf );

private: // Data
    /**
     * Session id
     */
    TUint32 iId;

    /**
     * Last used transaction id
     */
    TInt iLastTrId;
    
    /**
     * Outstanding transactions
     */
    RPointerArray<CCcSrvMsg> iRequests;
    
    /**
     * Observers
     */
    RArray<TUint32> iObservers;
    
    };

#endif // CCSRVSESSION_H

// End of file
