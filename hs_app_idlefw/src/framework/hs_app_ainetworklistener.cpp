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

#include <NetworkHandlingProxy.h>
#include <CNWSession.h>
#include "hs_app_ainetworklistener.h"
#include "hs_app_ainetworkobserver.h"

/// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// Two-phased constructor. Can leave.
// -----------------------------------------------------------------------------
// 
CAiNetworkListener* CAiNetworkListener::NewL(MAiNetworkObserver& aNetworkObserver)
    {
    CAiNetworkListener* self = new (ELeave) CAiNetworkListener(aNetworkObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

CAiNetworkListener::CAiNetworkListener(MAiNetworkObserver& aNetworkObserver)
:iObserver(aNetworkObserver)
    {
    }

void CAiNetworkListener::ConstructL()
    {
    //Create network handling engine session.
    iSession = CreateL( *this, iInfo );
    iCurrentNwState = MAiNetworkObserver::ENone;
    }

CAiNetworkListener::~CAiNetworkListener()
    {
    delete iSession;
    }

void CAiNetworkListener::HandleNetworkMessage( const TNWMessages aMessage )
    {
    TBool hasNetInfoChanged = HasNetworkInfoChanged( aMessage );
    if ( !hasNetInfoChanged )
        {
        return;
        }
        
    // Interpret new nw state
    MAiNetworkObserver::TNetworkState newState;
    newState= InterpretNWMessage(aMessage,iInfo);

    // Inform observer of only new nw states (TNetworkState)
    if (newState != iCurrentNwState)
        {
        iObserver.HandleNetworkStateChange(newState);
        }    
    
    // Store new nw state
    iCurrentNwState= newState;
  
    }

MAiNetworkObserver::TNetworkState CAiNetworkListener::InterpretNWMessage(const TNWMessages aMessage, const TNWInfo aNWInfo)
    {
    MAiNetworkObserver::TNetworkState nwstate = MAiNetworkObserver::ENone;
    
    switch (aMessage)
        {
        case MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange:
        case MNWMessageObserver::ENWMessageCurrentHomeZoneMessage:
            {
            switch (aNWInfo.iRegistrationStatus)
                {
                case ENWRegisteredRoaming:
                    nwstate = MAiNetworkObserver::ERoaming;
                    break;
                case ENWRegisteredOnHomeNetwork:
                    nwstate = MAiNetworkObserver::EHomeNetwork;
                    break;  
                    
                default:
                    // unknown state
                    break;
                }
            }
       }
    
    return nwstate;
    }

void CAiNetworkListener::HandleNetworkError( const TNWOperation aOperation, TInt /*aErrorCode*/ )
    {
     switch ( aOperation )
        {
        case MNWMessageObserver::ENWGetNetworkProviderName:
            iReceivedMessageFlags |= ENetworkProviderNameReceived;
            iReceivedMessageFlags &= ~ENetworkProviderNameOk;
            iInfo.iNPName.Zero();
            break;
        case MNWMessageObserver::ENWGetProgrammableOperatorName:
            iReceivedMessageFlags |= EProgrammableOperatorInfoReceived;
            iReceivedMessageFlags &= ~EProgrammableOperatorInfoReceivedOk;
            iInfo.iOperatorNameInfo.iName.Zero();
            break;
        case MNWMessageObserver::ENWGetServiceProviderName:
            iReceivedMessageFlags |= EServiceProviderNameReceived;
            iReceivedMessageFlags &= ~EServiceProviderNameOk;
            iInfo.iServiceProviderNameDisplayReq = RMobilePhone::KDisplaySPNNotRequired;
            iInfo.iSPName.Zero();
            iInfo.iPLMNField.Zero();
            break;
        default:
            break;
        }
    
    HandleNetworkMessage( TNWMessages( KErrGeneral ) );
    }

TBool CAiNetworkListener::HasNetworkInfoChanged( const TNWMessages aMessage )
    {
    TBool result = ETrue;
    
    // pass through
    if ( aMessage == MNWMessageObserver::ENWMessageCurrentHomeZoneMessage   ||
         aMessage == MNWMessageObserver::ENWMessageNetworkConnectionFailure ||
         aMessage == MNWMessageObserver::ENWMessageCurrentCellInfoMessage   ||
         aMessage == MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange )
        {
        return result;
        }
    
    result = ( iReceivedMessageFlags != iOldReceivedMessageFlags );
    
    if ( !result )
        {
         result = 
            iInfo.iRegistrationStatus != iOldInfo.iRegistrationStatus;
        }

    iOldReceivedMessageFlags = iReceivedMessageFlags;
    iOldInfo = iInfo;

    return result;
    }

MAiNetworkObserver::TNetworkState CAiNetworkListener::NetworkState()
    {
    return iCurrentNwState;
    }
    
