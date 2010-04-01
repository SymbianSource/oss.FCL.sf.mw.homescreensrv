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


#include "hs_app_ainwspublisher.h"
#include "aicontentobserver.h"
#include "hs_app_ainetworkinfolistener.h"

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
    if( iListener )
        {
        iListener->RemoveObserver( *this );
        iListener->Release();
        }
    delete iProfileApi;
    }


void CAiNwsPublisher::ResumeL()
    {
    iListener->AddObserverL( *this );
    }


void CAiNwsPublisher::Subscribe( MAiContentObserver& aObserver,
								    MAiPropertyExtension& aExtension,
                                    MAiPublishPrioritizer& /*aPrioritizer*/,
                                    MAiPublisherBroadcaster& /*aBroadcaster*/ )
    {
    iContentObserver = &aObserver;
    iExtension = &aExtension;
    }


void CAiNwsPublisher::RefreshL( TBool /*aClean*/ )
    {
    HandleStateChange( iListener->NetworkInfo() );
    }

TBool CAiNwsPublisher::RefreshL( TInt aContentId, TBool aClean )
	{
    if( aContentId == EAiDeviceStatusContentNWStatus )
        {
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
    __PRINTS( "XAI: Network found, clean state" );
    iRegistered = ETrue;
    if ( iContentObserver && iExtension )
        {
        iContentObserver->Clean( *iExtension, EAiDeviceStatusContentNWStatus, 0 );
        }
    }

void CAiNwsPublisher::HandleNetworkLost( const TNWNetworkSelectionSetting &aSelectionSetting )
    {
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
