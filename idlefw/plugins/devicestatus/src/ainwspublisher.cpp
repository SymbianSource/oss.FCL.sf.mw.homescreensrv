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
* Description:  Network status publisher
*
*/


#include "ainwspublisher.h"
#include <aicontentobserver.h>
#include "ainetworkinfolistener.h"

#include <activeidle2domainpskeys.h>
#include <e32property.h>
#include <centralrepository.h>
#include <ProfileEngineSDKCRKeys.h>
#include <BTSapDomainPSKeys.h>

#include "debug.h"

// Offline profile, from ProfileEngineSDKCRKeys.h
const TInt KOfflineProfileId = 5;

// ======== MEMBER FUNCTIONS ========

CAiNwsPublisher::CAiNwsPublisher()
    {
    iRegistered = ETrue;
    }


void CAiNwsPublisher::ConstructL()
    {
    __PRINTS( "XAI: CAiNwsPublisher > ConstructL" );
    iListener = CAiNetworkInfoListener::InstanceL();
    iProfileApi = CRepository::NewL( KCRUidProfileEngine );
    }

CAiNwsPublisher* CAiNwsPublisher::NewL()
    {
    CAiNwsPublisher* self = new( ELeave ) CAiNwsPublisher;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


CAiNwsPublisher::~CAiNwsPublisher()
    {
    __PRINTS( "XAI: CAiNwsPublisher > ~CAiNwsPublisher" );
    if( iListener )
        {
        iListener->RemoveObserver( *this );
        iListener->Release();
        }
    delete iProfileApi;
    }


void CAiNwsPublisher::ResumeL()
    {
    __PRINTS( "XAI: CAiNwsPublisher > ResumeL" );
    iListener->AddObserverL( *this );
    }


void CAiNwsPublisher::Subscribe( MAiContentObserver& aObserver,
								    CHsContentPublisher& aExtension,
                                    MAiPublishPrioritizer& /*aPrioritizer*/,
                                    MAiPublisherBroadcaster& /*aBroadcaster*/ )
    {
    __PRINTS( "XAI: CAiNwsPublisher > Subscribe" );
    iContentObserver = &aObserver;
    iExtension = &aExtension;
    }


void CAiNwsPublisher::RefreshL( TBool /*aClean*/ )
    {
    HandleStateChange( iListener->NetworkInfo() );
    }

TBool CAiNwsPublisher::RefreshL( TInt aContentId, TBool aClean )
	{
    __PRINTS( "XAI: CAiNwsPublisher::RefreshL" );
    if( aContentId == EAiDeviceStatusContentNWStatus )
        {
        __PRINTS( "XAI: content EAiDeviceStatusContentNWStatus" );
   	    RefreshL( aClean );
   	    return ETrue;
    	}

    return EFalse;
	}

void CAiNwsPublisher::HandleNetworkInfoChange(
                            const MNWMessageObserver::TNWMessages& /*aMessage*/,
                            const TNWInfo& aInfo,
                            const TBool /*aShowOpInd*/ )
    {
    __PRINTS( "XAI: CAiNwsPublisher::HandleNetworkInfoChange" );
    HandleStateChange( aInfo );
    }

void CAiNwsPublisher::HandleStateChange( const TNWInfo& aInfo )
    {
    switch( aInfo.iRegistrationStatus )
           {
           case ENWNotRegisteredNoService:
               // Fall-through
           case ENWNotRegisteredEmergencyOnly:
               // Fall-through
           case ENWNotRegisteredSearching:
               // Fall-through
           case ENWRegistrationDenied:
               HandleNetworkLost( aInfo.iSelectionSetting );
               break;

           case ENWRegisteredBusy:
               // Fall-through
           case ENWRegisteredOnHomeNetwork:
               // Fall-through
           case ENWRegisteredRoaming:
               HandleNetworkFound();
               break;

           case ENWRegistrationUnknown:
               // Take no action
           default:
               break;
           }
    }

void CAiNwsPublisher::HandleNetworkFound()
    {
    __PRINTS( "XAI: Network found" );
    iRegistered = ETrue;
    if ( iContentObserver && iExtension )
        {
        __PRINTS( "XAI: Network found, clean state" );
        iContentObserver->Clean( *iExtension, EAiDeviceStatusContentNWStatus, 0 );
        }
    }

void CAiNwsPublisher::HandleNetworkLost( const TNWNetworkSelectionSetting &aSelectionSetting )
    {
    __PRINTS( "XAI: Network lost" );
    if( aSelectionSetting == ENWNetworkSelectionManual )
        {
        // See if we were registered before
        if( iRegistered )
            {
            iRegistered = EFalse;

            if( !IsOffLineMode() &&
                    !IsBluetoothSAPConnected() &&
                    iContentObserver &&
                    iExtension )
                {
                __PRINTS( "XAI: Network lost, publish state" );
                iContentObserver->Publish( *iExtension,
                            EAiDeviceStatusContentNWStatus,
                            EAiDeviceStatusResourceNWLost,
                            0 );
                }
            }
        }
    }

TBool CAiNwsPublisher::IsOffLineMode() const
    {
    TInt profileId;
    TInt err = iProfileApi->Get( KProEngActiveProfile, profileId );
    return profileId == KOfflineProfileId && err == KErrNone;
    }

TBool CAiNwsPublisher::IsBluetoothSAPConnected() const
    {
    TInt btSapState( EBTSapNotConnected );
    TInt err = RProperty::Get( KPSUidBluetoothSapConnectionState,
                               KBTSapConnectionState,
                               btSapState );
    return btSapState != EBTSapNotConnected && err == KErrNone;
    }
