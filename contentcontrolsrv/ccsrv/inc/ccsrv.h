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

#ifndef CCSRV_H
#define CCSRV_H

// System includes
#include <e32base.h>

// Forward declarations
class CCcSrvSession;
class CCcSrvMsg;

/**
 *  Content control server
 *
 *
 *  @code
 *
 *  @endcode
 *
 *  @lib ccsrv.exe
 *  @since S60 v5.0
 */
class CCcSrv : public CPolicyServer
    {
public: // Constructor and destructor

    /**
     * NewLC
     * @since S60 3.1
     */
    static CServer2* NewLC();


private: // Constructors and destructor

    /**
     * Constructor
     */
    CCcSrv();

    /**
     * Destructor
     */
    ~CCcSrv();

    /** 
     * Second phase constructor 
     */
    void ConstructL();

public: // New functions
    /**
     * Add session
     * @param aSession Added session
     */
    void AddSessionL(
        CCcSrvSession* aSession );

    /**
     * Drop session
     * @param aSession Dropped session
     */
    void DropSession(
        CCcSrvSession* aSession );

    /**
     * Registers provider session
     * @param aProvider Registered provider
     * @param aId Provider session id
     */
    void RegisterProviderL(
        TUint32 aProvider,
        CCcSrvSession* aSession );

    /**
     * Registers observer session
     * @param aProvider Registered provider
     * @param aId Provider session id
     */
    void RegisterObserverL(
        TUint32 aProvider,
        CCcSrvSession* aSession );

    /**
     * Resolves registered provider address
     * @param aProvider Registered provider
     * @param aAddress Resolved address
     */
    void ResolveProviderAddressL(
        TUint32 aProvider,
        TUint32& aAddress );

    /**
     * Calculates next available transaction id
     * @return next available transaction id
     */
    TUint32 GetTrId();

    /**
     * Sends message to defined session
     * @param aSender Sender session
     * @param aReceiver Receiver session
     * @param aMsgBuf Message buffer
     */
    void SendMsgL(
        TUint32 aSender,
        TUint32 aReceiver,
        CCcSrvMsg& aMessage );

private:

    /**
     * NewSessionL
     * @since S60 3.1
     */
    CSession2* NewSessionL(
        const TVersion& aVersion, 
        const RMessage2& aMessage ) const;
    
    
private: // Data
    /**
     * Session list
     */
    RPointerArray<CCcSrvSession> iSessions;

    /**
     * Last session id
     */
    TUint32 iLastSessionId;

    /**
     * Last used transaction id
     */
    TUint32 iLastTrId;

    /**
     * Provider list
     */
    struct TCcProvider
        {
        TUint32 iId;
        CCcSrvSession* iSession;
        };
    RArray<TCcProvider> iProviders;
    
    };


#endif // CCSRV_H

// End of file
