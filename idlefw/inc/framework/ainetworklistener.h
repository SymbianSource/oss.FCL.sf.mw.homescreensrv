/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Network info listener
*
*/

#ifndef C_NETWORKLISTENER_H
#define C_NETWORKLISTENER_H

#include <e32base.h>
#include <NWHandlingEngine.h>
#include "ainetworkobserver.h"

class MAiNetworkObserver;
class CNWSession;

/**
 *  @ingroup group_xmluicontroller
 *
 *  Network info listener.
 *
 *  Client can use this object to listen network state changes. Client must implement
 *  MAiNetworkObserver interface to retrieve network state changes
 *
 *  @since S60 5.0
 */
NONSHARABLE_CLASS(CAiNetworkListener) : public CBase, 
                           public MNWMessageObserver
    {
public:

    static CAiNetworkListener* CAiNetworkListener::NewL(MAiNetworkObserver& aNetworkObserver);
 
    virtual ~CAiNetworkListener();
    
    /**
     * Get current network status
     * @return TNetworkState
     */
    MAiNetworkObserver::TNetworkState NetworkState();

// from base class MNWMessageObserver

    /**
     * From MNWMessageObserver.
     * Called by network handling engine when network info changes.
     *
     * @since S60 5.0
     * @param aMessage is type of the change.
     */
    void HandleNetworkMessage( const TNWMessages aMessage );

    /**
     * From MNWMessageObserver.
     * Called if network handling engine fails.
     *
     * @since S60 5.0
     * @param aOperation is failed operation.
     * @param aErrorCode is fail reason.
     */
    void HandleNetworkError( const TNWOperation aOperation, TInt aErrorCode );

private:
    TBool HasNetworkInfoChanged( const TNWMessages aMessage );

    MAiNetworkObserver::TNetworkState InterpretNWMessage(const TNWMessages aMessage, const TNWInfo);

    CAiNetworkListener(MAiNetworkObserver& aNetworkObserver);

    void ConstructL();

 
private: // data

     /**
     * Session to network handling engine.
     * Own.
     */
    CNWSession* iSession;

    /**
     * Cached network info structure.
     */
    TNWInfo iInfo;
    
    /**
     * Previous network information.
     */
    TNWInfo iOldInfo;

    /**
      * Reference to client observing
      */
      MAiNetworkObserver& iObserver;


    /**
     * Network related message flags
     */
     
    enum TNetInfoFlags 
        {
        ENetworkProviderNameReceived      = 0x00000001,
        ENetworkProviderNameOk            = 0x00000002,
        EServiceProviderNameReceived      = 0x00000004,
        EServiceProviderNameOk            = 0x00000008,
        ERegistrationStatusReceived       = 0x00000010,
        ENetworkInfoChangeReceived        = 0x00000020,
        EProgrammableOperatorInfoReceived = 0x00000040,
        EProgrammableOperatorInfoReceivedOk = 0x00000080
        };
        
    /**
     * Subset of sum of TNetInfoFlags.
     */    
    TUint iReceivedMessageFlags;
    
    /**
     * Subset of sum of old TNetInfoFlags.
     */    
    TUint iOldReceivedMessageFlags;
    
    /**
     * current state of network
     */    
    MAiNetworkObserver::TNetworkState iCurrentNwState;
     };
//}    

#endif // C_AINETWORKINFOLISTENER_H
