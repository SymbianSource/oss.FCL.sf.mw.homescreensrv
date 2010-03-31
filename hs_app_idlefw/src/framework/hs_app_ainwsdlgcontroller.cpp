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
* Description:  Network selection dialog controller
*
*/


#include <networkhandlingproxy.h>
#include <cnwsession.h>
#include <centralrepository.h>
#include <e32property.h>

#include <ProfileEngineSDKCRKeys.h>
#include <BTSapDomainPSKeys.h>

#include "hs_app_ainwsdlgcontroller.h"
#include "debug.h"


// 1-minute timeout before showing soft notification
const TInt KPhoneNetworkLostTimeout = 60*1000000;

// Offline profile, from ProfileEngineSDKCRKeys.h
const TInt KOfflineProfileId = 5;


CAiNwSDlgController* CAiNwSDlgController::NewL()
    {
    CAiNwSDlgController* self = new(ELeave) CAiNwSDlgController();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();    // self
    return self;
    }

CAiNwSDlgController::CAiNwSDlgController()
    {
    iRegistered = ETrue;
    }

void CAiNwSDlgController::ConstructL()
    {
    __PRINTS( "XAI: CAiNwSDlgController is initializing" );

    iSoftNotifier = CAknSoftNotifier::NewL();
    iSession = CreateL( *this, iInfo );
    iProfileApi = CRepository::NewL( KCRUidProfileEngine );
    iPeriodic = CPeriodic::NewL( CActive::EPriorityStandard );

    HandleStateChange();

    __PRINTS( "XAI: CAiNwSDlgController initialized succesfully" );
    }

CAiNwSDlgController::~CAiNwSDlgController()
    {
    delete iPeriodic;
    delete iProfileApi;
    delete iSession;
    delete iSoftNotifier;
    }

void CAiNwSDlgController::HandleStateChange()
    {
    switch( iInfo.iRegistrationStatus )
        {
        case ENWNotRegisteredNoService:
            // Fall-through
        case ENWNotRegisteredEmergencyOnly:
            // Fall-through
        case ENWNotRegisteredSearching:
            // Fall-through
        case ENWRegistrationDenied:
            HandleNetworkLost();
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

void CAiNwSDlgController::HandleNetworkFound()
    {
    __PRINTS( "XAI: Network found" );
    iRegistered = ETrue;
    iPeriodic->Cancel();
    CancelDialog();
    }

void CAiNwSDlgController::HandleNetworkLost()
    {
    if( iInfo.iSelectionSetting == ENWNetworkSelectionManual )
        {
        // See if we were registered before
        if( iRegistered )
            {
            iRegistered = EFalse;

            if(!IsOffLineMode() && !IsBluetoothSAPConnected())
                {
                __PRINTS( "XAI: Network lost, show dialog in 1 minute" );
                iPeriodic->Start( KPhoneNetworkLostTimeout,
                    KPhoneNetworkLostTimeout, TCallBack( DelayCallBack, this ));
                }
            }
        }
    }

void CAiNwSDlgController::LaunchDialog()
    {
    iPeriodic->Cancel();
    TRAP_IGNORE( iSoftNotifier->AddNotificationL( ESelectNetworkNotification, 1 ); );
    }

void CAiNwSDlgController::CancelDialog()
    {
    TRAP_IGNORE( iSoftNotifier->CancelSoftNotificationL( ESelectNetworkNotification ); );
    }

TInt CAiNwSDlgController::DelayCallBack(TAny* aParam)
    {
    CAiNwSDlgController* self = (CAiNwSDlgController*) aParam;
    self->LaunchDialog();
    return KErrNone;
    }

void CAiNwSDlgController::HandleNetworkMessage( const TNWMessages aMessage )
    {
    switch(aMessage)
        {
        case ENWMessageNetworkRegistrationStatusChange:
            HandleStateChange();
            break;

        default:
            break;
        }
    }

void CAiNwSDlgController::HandleNetworkError( const TNWOperation /*aOperation*/,
                                             TInt /*aErrorCode*/ )
    {
    // Take no action.
    }

TBool CAiNwSDlgController::IsOffLineMode() const
    {
    TInt profileId;
    TInt err = iProfileApi->Get( KProEngActiveProfile, profileId );
    return profileId == KOfflineProfileId && err == KErrNone;
    }

TBool CAiNwSDlgController::IsBluetoothSAPConnected() const
    {
    TInt btSapState( EBTSapNotConnected );
    TInt err = RProperty::Get( KPSUidBluetoothSapConnectionState,
                               KBTSapConnectionState,
                               btSapState );
    return btSapState != EBTSapNotConnected && err == KErrNone;
    }

// End of file.
