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
* Description:  SIM registration status publisher
*
*/


#include "aisimregpublisher.h"
#include "aicontentobserver.h"
#include "ainetworkinfolistener.h"
#include <activeidle2domainpskeys.h>
#include <e32property.h>

#include <startupdomainpskeys.h>

// ======== LOCAL FUNCTIONS =======

TBool AllowedToShowSimRegFailed()
    {
    TInt simCardStatus = ESimStatusUninitialized;
    TInt err = RProperty::Get( KPSUidStartup, KPSSimStatus, simCardStatus );

    // Show note only when the SIM is usable
    if( err == KErrNone && simCardStatus == ESimUsable )
        {
        return ETrue;
        }
    return EFalse;
    }


// ======== MEMBER FUNCTIONS ========

CAiSimRegPublisher::CAiSimRegPublisher()
    {
    }


void CAiSimRegPublisher::ConstructL()
    {
    iListener = CAiNetworkInfoListener::InstanceL();
    }


CAiSimRegPublisher* CAiSimRegPublisher::NewL()
    {
    CAiSimRegPublisher* self = new( ELeave ) CAiSimRegPublisher;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


CAiSimRegPublisher::~CAiSimRegPublisher()
    {
    if( iListener )
        {
        iListener->RemoveObserver( *this );
        iListener->Release();
        }
    }


void CAiSimRegPublisher::ResumeL()
    {
    iListener->AddObserverL( *this );
    }


void CAiSimRegPublisher::Subscribe( MAiContentObserver& aObserver,
								    MAiPropertyExtension& aExtension,
                                    MAiPublishPrioritizer& /*aPrioritizer*/,
                                    MAiPublisherBroadcaster& /*aBroadcaster*/ )
    {
    iContentObserver = &aObserver;
    iExtension = &aExtension;
    }


void CAiSimRegPublisher::RefreshL( TBool /*aClean*/ )
    {
    TInt status = EPSSimRegFailedMessageNotReceived;
    RProperty::Get(
	    KPSUidAiInformation,
        KActiveIdleSimRegFailedReceived,
        status );
    //this cannot be refreshed
    switch( iListener->NetworkInfo().iRegistrationStatus )
        {
        case ENWRegistrationUnknown:
        case ENWNotRegisteredNoService:
        case ENWNotRegisteredEmergencyOnly:
        case ENWRegistrationDenied: // fallthrough
            {
            if( status == EPSSimRegFailedMessageReceived &&
                ::AllowedToShowSimRegFailed() )
            	{
            	iContentObserver->Publish( *iExtension,
            	        EAiDeviceStatusContentSIMRegStatus,
            	        EAiDeviceStatusResourceSIMRegFail,
            	        0 );
            	}
            break;
            }
        case ENWRegisteredOnHomeNetwork:
        case ENWRegisteredRoaming:
        case ENWRegisteredBusy: // fallthrough
            {
            if( status == EPSSimRegFailedMessageReceived )
                {
                iContentObserver->Clean( *iExtension,
                	EAiDeviceStatusContentSIMRegStatus,
                	0 );
                }
            break;
            }
        default:
            {
            // no op
            break;
            }
        }
    }


void CAiSimRegPublisher::HandleNetworkInfoChange(
							const MNWMessageObserver::TNWMessages& aMessage,
							const TNWInfo& aInfo,
							const TBool /*aShowOpInd*/ )
    {
    if ( aMessage ==
    		MNWMessageObserver::ENWMessageNetworkConnectionFailure &&
    		::AllowedToShowSimRegFailed())
        {

        // SIM registration failed. Publish SIM registration status.
        iContentObserver->Publish( *iExtension,
                EAiDeviceStatusContentSIMRegStatus,
                EAiDeviceStatusResourceSIMRegFail,
                0 );
        RProperty::Set(
                KPSUidAiInformation,
                KActiveIdleSimRegFailedReceived,
                EPSSimRegFailedMessageReceived );
        }

    else if ( aMessage ==
    		MNWMessageObserver::ENWMessageNetworkRegistrationStatusChange )
        {
        switch ( aInfo.iRegistrationStatus )
            {
            case ENWRegisteredOnHomeNetwork:
            case ENWRegisteredRoaming:
            case ENWRegisteredBusy:
                {
				//Call clean if registration is fine.
                iContentObserver->Clean( *iExtension,
                			EAiDeviceStatusContentSIMRegStatus,
                			0 );
                break;
                }
            default:
                break;
            }
        }
    }


TBool CAiSimRegPublisher::RefreshL( TInt aContentId, TBool aClean )
	{
    if( aContentId == EAiDeviceStatusContentSIMRegStatus )
        {
   	    RefreshL( aClean );
   	    return ETrue;
    	}

    return EFalse;
	}
