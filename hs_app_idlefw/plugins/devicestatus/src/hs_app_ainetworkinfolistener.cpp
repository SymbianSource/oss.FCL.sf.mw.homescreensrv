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


#include <NetworkHandlingProxy.h>
#include <CNWSession.h>
#include "hs_app_ainetworkinfolistener.h"
#include "hs_app_ainetworkinfoobserver.h"
#include "debug.h"

const TInt KAiMessageCacheGranularity = 4;

// ======== MEMBER FUNCTIONS ========

CAiNetworkInfoListener::CAiNetworkInfoListener()
: iKeyProperties( 0, ECmpTInt )
    {
    }


void CAiNetworkInfoListener::ConstructL()
    {
	//Store pointer in TLS
    User::LeaveIfError( Dll::SetTls( this ) );

    //Create network handling engine session.
    iSession = CreateL( *this, iInfo );
    
    iShowOpInd = EFalse;
    //Create message cache
    iMessageCache = new( ELeave )CArrayFixFlat
        <MNWMessageObserver::TNWMessages>( KAiMessageCacheGranularity );
    }


CAiNetworkInfoListener* CAiNetworkInfoListener::InstanceL()
    {
    CAiNetworkInfoListener* self = static_cast<CAiNetworkInfoListener*>( Dll::Tls() );

    if( !self )
        {
		//If instance of network listener is not already constructed, create it
        self = new( ELeave ) CAiNetworkInfoListener;
        CleanupStack::PushL( self );
        self->ConstructL();
        CleanupStack::Pop( self );
        }

	//increase access count
    self->IncAccessCount();
    return self;
    }


void CAiNetworkInfoListener::Release()
    {
	//Decrease access count, if it goes to zero, delete object.
    if( !DecAccessCount() )
        {
        delete this;
        }
    }


CAiNetworkInfoListener::~CAiNetworkInfoListener()
    {
	//Remove object from TLS
    Dll::SetTls( NULL );
    iObservers.Reset();
    delete iSession;
    delete iMessageCache;
    }


TInt CAiNetworkInfoListener::IncAccessCount()
    {
    return iAccessCount++;
    }

TInt CAiNetworkInfoListener::DecAccessCount()
    {
    return --iAccessCount;
    }


void CAiNetworkInfoListener::AddObserverL( MAiNetworkInfoObserver& aObserver )
    {
	//Removing observer doesn't remove slots from array, removed observers are only
	//set to NULL. Reason for this is found out later on the code. Adding observer
	//first tries to find free slot, if it is not found, observer is appended to the
	//array.
    TInt freeSlot = iObservers.Find( NULL );

    if( freeSlot == KErrNotFound )
        {
        User::LeaveIfError( iObservers.Append( &aObserver ) );
        }
    else
        {
        User::LeaveIfError( iObservers.Insert( &aObserver, freeSlot ) );
        }
    }
    
    
void CAiNetworkInfoListener::RemoveObserver( MAiNetworkInfoObserver& aObserver )
    {
	//Remove observer, removing is done by replacing it with NULL pointer.
    const TInt count( iObservers.Count() );

    for( TInt i( 0 ); i < count; i++ )
        {
        if( iObservers[i] == &aObserver )
            {
            //replace it with NULL
            iObservers.Remove( i );
            iObservers.Insert( NULL, i );
            break;
            }
        }
    }


const TNWInfo& CAiNetworkInfoListener::NetworkInfo() const
    {
    return iInfo;
    }

TBool CAiNetworkInfoListener::IsOperatorIndicatorAllowed() const
    {
    return iShowOpInd;
    }

TBool CAiNetworkInfoListener::MessageReceived( MNWMessageObserver::TNWMessages aMessage )
    {
	//check if the message is in message cache.
    TInt index( KErrNotFound );
    TBool found = ( iMessageCache->FindIsq( aMessage, iKeyProperties, index ) == 0 );
    return found;
    }


void CAiNetworkInfoListener::HandleNetworkMessage( const TNWMessages aMessage )
    {
    __PRINTS("XAI: Handle NW message");
	//Insert message into the message cache. Only one messsage of one type.
    TRAPD( err, iMessageCache->InsertIsqL( aMessage, iKeyProperties ) );
    if( err == KErrAlreadyExists )
        {
        err = KErrNone;
        }
        
    if( err != KErrNone )
        {
        return;        
        }

	iShowOpInd 		= !NotAllowedToDisplayOperatorIndicator( aMessage );
	
	
	TBool hasNetInfoChanged = HasNetworkInfoChanged( aMessage );
	
	if ( !hasNetInfoChanged )
		{
		return;
		}
	
	__PRINT(__DBG_FORMAT("XAI: Show operator indicator %d, info changed %d"), iShowOpInd, hasNetInfoChanged );
	
    const TInt count( iObservers.Count() );


    for( TInt i( 0 ); i < count; i++ )
        {
		//Observer might be NULL. When HandleNetworkInfoChange is called, it may
		//cause deleting of the publisher which means that observer is removed from listener.
		//This is the reason why observer pointers are replaced by NULL when it is removed
		//from this listener. If the slot would be removed, it would easily cause index overflow
		//or missing HandleNetworkInfoChange calls.
		//For example lets assume that we have 5 observers, we are going in loop 3, so variable i
		//has value 2. When third call is done, it would cause two publishers to be deleted and
		//observers removed. Lets assume that these observers are in place 0 and 1. So now we have
		//only 3 observers and the count is still 2. So we miss a call to a observers in index 3 and 4.

        if( iObservers[i] )
            {
            iObservers[i]->HandleNetworkInfoChange( aMessage, iInfo, iShowOpInd );
            }
        }
    }


void CAiNetworkInfoListener::HandleNetworkError( const TNWOperation aOperation, TInt aErrorCode )
    {
    __PRINT(__DBG_FORMAT("XAI: Error code %d"), aErrorCode );
    switch ( aOperation )
        {
        case MNWMessageObserver::ENWGetNetworkProviderName:
            iReceivedMessageFlags |= ENetworkProviderNameReceived;
            iReceivedMessageFlags &= ~ENetworkProviderNameOk;
            iInfo.iNPName.Zero();
            __PRINTS("XAI: NPN error received");
            break;
        case MNWMessageObserver::ENWGetProgrammableOperatorName:
            iReceivedMessageFlags |= EProgrammableOperatorInfoReceived;
            iReceivedMessageFlags &= ~EProgrammableOperatorInfoReceivedOk;
            iInfo.iOperatorNameInfo.iName.Zero();
            __PRINTS("XAI: PON error received");
            break;
        case MNWMessageObserver::ENWGetServiceProviderName:
            iReceivedMessageFlags |= EServiceProviderNameReceived;
            iReceivedMessageFlags &= ~EServiceProviderNameOk;
            iInfo.iServiceProviderNameDisplayReq = RMobilePhone::KDisplaySPNNotRequired;
            iInfo.iSPName.Zero();
            iInfo.iPLMNField.Zero();
            __PRINTS("XAI: SPN error received");
            break;
        default:
            break;
        }
    
    HandleNetworkMessage( TNWMessages( KErrGeneral ) );
    }
    
    
TBool CAiNetworkInfoListener::NotAllowedToDisplayOperatorIndicator( const TNWMessages aMessage )
	{
	// Service provider name must have been fetched.
    // Network provider name must have been fetched.
    // Registration status and network information must have been received.
    // Operator name information must have been received.
    // Device must be camped to a network.
	
	switch ( aMessage )
    	{
        case MNWMessageObserver::ENWMessageNetworkInfoChange:
            iReceivedMessageFlags |= ENetworkInfoChangeReceived;
            break;
        case MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange:
            iReceivedMessageFlags |= ERegistrationStatusReceived;
            break;
        case MNWMessageObserver::ENWMessageNetworkProviderNameChange:
            iReceivedMessageFlags |= 
                ( ENetworkProviderNameReceived + ENetworkProviderNameOk );
            break;
        case MNWMessageObserver::ENWMessageServiceProviderNameChange:
            iReceivedMessageFlags |= 
                ( EServiceProviderNameReceived + EServiceProviderNameOk );
            break;
        case MNWMessageObserver::ENWMessageProgrammableOperatorInfoChange:
            iReceivedMessageFlags |= 
                ( EProgrammableOperatorInfoReceived + 
                  EProgrammableOperatorInfoReceivedOk );
            break;
        case MNWMessageObserver::ENWMessageNetworkProviderNameUpdating:
            iReceivedMessageFlags &= 
                ~( ENetworkProviderNameReceived + ENetworkProviderNameOk );
            break;
        case MNWMessageObserver::ENWMessageServiceProviderNameUpdating:
            iReceivedMessageFlags &= 
                ~( EServiceProviderNameReceived + EServiceProviderNameOk );
            break;
        case MNWMessageObserver::ENWMessageProgrammableOperatorInfoUpdating:
            iReceivedMessageFlags &= 
                ~( EProgrammableOperatorInfoReceived + 
                   EProgrammableOperatorInfoReceivedOk );
            break;       
        default:
            break;
        }
    
    TBool serviceProviderNameFetched( EServiceProviderNameReceived & iReceivedMessageFlags );
    TBool networkProviderNameFetched( ENetworkProviderNameReceived & iReceivedMessageFlags );
    TBool registrationStatusReceived( ERegistrationStatusReceived & iReceivedMessageFlags );
    TBool networkInformationReceived( ENetworkInfoChangeReceived & iReceivedMessageFlags );
    TBool operatorNameInformationReceived(
    	 EProgrammableOperatorInfoReceived & iReceivedMessageFlags );  
    TBool currentNetworkOk( 
    	( ENetworkInfoChangeReceived  & iReceivedMessageFlags ) && 
        ( ERegistrationStatusReceived & iReceivedMessageFlags ) &&
        ( iInfo.iStatus == ENWStatusCurrent ) );  
      
  	return
        !serviceProviderNameFetched ||
        !networkProviderNameFetched ||
        !( registrationStatusReceived && networkInformationReceived 
        	&& operatorNameInformationReceived ) ||
        !currentNetworkOk;              
	}


TBool CAiNetworkInfoListener::HasNetworkInfoChanged( const TNWMessages aMessage )
    {
    TBool result = ETrue;
    
    // pass through
   	if ( aMessage == MNWMessageObserver::ENWMessageCurrentHomeZoneMessage 	||
   	 	 aMessage == MNWMessageObserver::ENWMessageNetworkConnectionFailure	||
   	 	 aMessage == MNWMessageObserver::ENWMessageCurrentCellInfoMessage )
   		{
   		return result;
   		}
    
    result = ( iReceivedMessageFlags != iOldReceivedMessageFlags );
    
    if ( !result )
        {
        // Check if contents of iInfo has changed. Most probable to the 
        // beginning of the expression.
        result = 
            iInfo.iRegistrationStatus != iOldInfo.iRegistrationStatus ||
            iInfo.iStatus != iOldInfo.iStatus ||
            iInfo.iCountryCode != iOldInfo.iCountryCode ||
            iInfo.iNetworkId != iOldInfo.iNetworkId ||
            iInfo.iOperatorNameInfo.iType != 
                iOldInfo.iOperatorNameInfo.iType ||
            iInfo.iOperatorNameInfo.iName != 
                iOldInfo.iOperatorNameInfo.iName ||
            iInfo.iDisplayTag != iOldInfo.iDisplayTag ||
            iInfo.iShortName != iOldInfo.iShortName ||
            iInfo.iLongName != iOldInfo.iLongName ||
            iInfo.iSPName != iOldInfo.iSPName ||
            iInfo.iServiceProviderNameDisplayReq != 
                iOldInfo.iServiceProviderNameDisplayReq ||
            iInfo.iNPName != iOldInfo.iNPName ||
            iInfo.iPLMNField != iOldInfo.iPLMNField;
        }

    iOldReceivedMessageFlags = iReceivedMessageFlags;
    iOldInfo = iInfo;

    return result;
    }

