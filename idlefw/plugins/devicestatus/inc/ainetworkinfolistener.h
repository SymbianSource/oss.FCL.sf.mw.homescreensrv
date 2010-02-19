/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Network info listener.
*
*/


#ifndef C_AINETWORKINFOLISTENER_H
#define C_AINETWORKINFOLISTENER_H

// System includes
#include <e32base.h>
#include <NWHandlingEngine.h>

// User includes

// Forward declarations
class MAiNetworkInfoObserver;
class CNWSession;


/**
 *  @ingroup group_devicestatusplugin
 *
 *  Network info listener.
 *
 *  Client can use this object to listen network info changes. Client must implement
 *  MAiNetworkInfoObserver interface to retrieve network info change events.
 *
 *  @since S60 3.2
 */
NONSHARABLE_CLASS( CAiNetworkInfoListener ) : public CBase, 
    public MNWMessageObserver
    {
public:

    /**
     * Get pointer to network info listener.
     * When this is called first time, object is created, later calls
     * gets pointer to previously created object.
     *
     * @since S60 3.2
     * @return pointer to network info listener.
     */
    static CAiNetworkInfoListener* InstanceL();

    /**
     * Release network info listener.
     * This must be called when client doesn't listen network info
     * changes anymore. When all of the listener instances are released,
     * the object is destroyed. After client calls this method, the pointer
     * returned by InstanceL must not be used anymore.
     *
     * @since S60 3.2
     * @param ?arg1
     * @param ?arg2
     * @return
     */
    void Release();

    /**
     * Add network info observer.
     *
     * @since S60 3.2
     * @param aObserver is interface which is used to inform network info changes.
     */
    void AddObserverL( MAiNetworkInfoObserver& aObserver );

    /**
     * Remove network info observer.
     *
     * @since S60 3.2
     * @param aObserver must be exactly same object as was given to AddObserverL call.
     */
    void RemoveObserver( MAiNetworkInfoObserver& aObserver );

    /**
     * Get cached network info structure.
     * When network info changes, it is stored in this listener, client can use this
     * method to receive the latest network info.
     *
     * @since S60 3.2
     * @return latest network info structure.
     */
    const TNWInfo& NetworkInfo() const;

    /**
     * Is the operator indicator allowed to be shown.
     * 
     * @since S60 v3.2
     * @return ETrue if the indicator can be shown.
     */
    TBool IsOperatorIndicatorAllowed() const;
    
    /**
     * Client can use this method to check if certain network message has been already
     * received.
     *
     * @since S60 3.2
     * @param aMessage is type of the message.
     * @return ETrue if the message has been received.
     */
    TBool MessageReceived( MNWMessageObserver::TNWMessages aMessage );

    /**
     * Check if it's allowed to show operator indicator.
     * Number of rules control operator indicator visibility. 
     * Parameter aMessage is used for fulfilling these rules - it can have
     * various values so this method is called many times. The sum of the
     * calls (and thus sum of different aMessage values) defines whether
     * rules are fulfilled and indicator is shown.
     * @param aMessage Network message 
     * @return ETrue if rules are not fulfilled and indicator should 
     *                  not be shown.
     */
	TBool NotAllowedToDisplayOperatorIndicator( const TNWMessages aMessage );
	
	/**
	 * Check if network info has been changed.
	 * @param aMessage Network message
	 * @return ETrue if network info has changed since last check.
	 */
	TBool HasNetworkInfoChanged( const TNWMessages aMessage );
	

// from base class MNWMessageObserver

    /**
     * From MNWMessageObserver.
     * Called by network handling engine when network info changes.
     *
     * @since S60 3.2
     * @param aMessage is type of the change.
     */
    void HandleNetworkMessage( const TNWMessages aMessage );

    /**
     * From MNWMessageObserver.
     * Called fi network handling engine fails.
     *
     * @since S60 3.2
     * @param aOperation is failed operation.
     * @param aErrorCode is fail reason.
     */
    void HandleNetworkError( const TNWOperation aOperation, TInt aErrorCode );

private:

    CAiNetworkInfoListener();

    void ConstructL();

    ~CAiNetworkInfoListener();

    /**
     * Increase access count.
     */
    TInt IncAccessCount();

    /**
     * Decrease access count, when this goes to zero, object is deleted.
     */
    TInt DecAccessCount();

private: // data

    /**
     * Array of observers.
     * Not own.
     */
    RArray<MAiNetworkInfoObserver*> iObservers;

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
     * Message cache. There is only one instance of one type of message.
     * Own.
     */
    CArrayFixFlat<MNWMessageObserver::TNWMessages>* iMessageCache;

    /**
     * Sorting rule.
     */
    TKeyArrayFix iKeyProperties;

    /**
     * Access count.
     */
    TInt iAccessCount;
    
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
     * Is the operator indicator allowed to show
     */
    TBool iShowOpInd;
    };
    


#endif // C_AINETWORKINFOLISTENER_H
