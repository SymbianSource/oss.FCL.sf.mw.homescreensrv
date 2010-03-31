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
* Description:  Listens Bluetooth SIM Access Profile (BT SAP) and publishes
*					text when BT SAP is activated.
*
*/


#include <aidevstaplgres.rsg>
#include <centralrepository.h>
#include <BTSapDomainPSKeys.h>
#include <StringLoader.h>
#include <featmgr.h>
#include "hs_app_aibtsappublisher.h"
#include "hs_app_aiprioritizer.h"
#include "hs_app_ainwidpriorities.h"


// ======== MEMBER FUNCTIONS ========

CAiBTSAPPublisher::CAiBTSAPPublisher()
: CActive( CActive::EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }


void CAiBTSAPPublisher::ConstructL()
    {
	//if platform doesn't support BTSAP, there is no reason
	//to create this object.
    if( !FeatureManager::FeatureSupported( KFeatureIdBtSap ) )
        {
        User::Leave( KErrNotSupported );
        }
    }


CAiBTSAPPublisher* CAiBTSAPPublisher::NewL()
    {
    CAiBTSAPPublisher* self = new( ELeave ) CAiBTSAPPublisher;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


CAiBTSAPPublisher::~CAiBTSAPPublisher()
    {
    Cancel();
    iPubSub.Close();
    }


void CAiBTSAPPublisher::ResumeL()
    {
    Cancel();

    iPubSub.Close();
	//attach to BTSAP key.
    User::LeaveIfError( iPubSub.Attach( KPSUidBluetoothSapConnectionState,
    									KBTSapConnectionState ) );

    iFirstPublish = ETrue;

    //subscribe to get notification when key changes.
    iPubSub.Subscribe( iStatus );
    SetActive();
    }


void CAiBTSAPPublisher::Subscribe( MAiContentObserver& /*aObserver*/,
								    MAiPropertyExtension& aExtension,
                                    MAiPublishPrioritizer& aPrioritizer,
                                    MAiPublisherBroadcaster& aBroadcaster )
    {
    iExtension = &aExtension;
    iPrioritizer = &aPrioritizer;
    iBroadcaster = &aBroadcaster;
    }


void CAiBTSAPPublisher::RefreshL( TBool aClean )
    {
    iSuccess = EFalse;
    if( aClean )
        {
        iPrioritizer->TryToCleanL( *iBroadcaster,
        							EAiDeviceStatusContentNetworkIdentity,
        							EAiBTSAP );
        }

    //Get BTSAP state
    TInt sapState( 0 );
    TInt err = RProperty::Get( KPSUidBluetoothSapConnectionState,
                               KBTSapConnectionState,
                               sapState );

    //If key is not found, it is treated same way as the BTSAP is not on.
    if( err == KErrNotFound )
        {
        err = KErrNone;
        sapState = EBTSapNotConnected;
        }

    User::LeaveIfError( err );

    if( sapState == EBTSapConnected )
        {
        iPrioritizer->TryToPublishL( *iBroadcaster,
        							EAiDeviceStatusContentNetworkIdentity,
        							R_ACTIVEIDLE_BT_SIM_ACCESS_PROFILE_STRING,
        							EAiBTSAP );
        iSuccess = ETrue;
        }

    else if( !iFirstPublish )
        {
		//BTSAP is not anymore active, call clean.
        iPrioritizer->TryToCleanL( *iBroadcaster,
        							EAiDeviceStatusContentNetworkIdentity,
        							EAiBTSAP );
        }
    iFirstPublish = EFalse;
    }


void CAiBTSAPPublisher::RunL()
    {
    if( iStatus.Int() == KErrNone )
        {
	    //PS key changed, refresh publisher.
        RefreshL( ETrue );
        iPubSub.Subscribe( iStatus );
        SetActive();
        }
    }


void CAiBTSAPPublisher::DoCancel()
    {
    iPubSub.Cancel();
    }


TInt CAiBTSAPPublisher::RunError( TInt /*aError*/ )
    {
    iPubSub.Cancel();
    //failed to publish, lets however ignore it
    return KErrNone;
    }


TBool CAiBTSAPPublisher::RefreshL( TInt aContentId, TBool aClean )
	{
    if( aContentId == EAiDeviceStatusContentNetworkIdentity )
        {
   	    RefreshL( aClean );
        if( iSuccess )
	        {
	        return ETrue;
	        }
        }
    return EFalse;
	}


TBool CAiBTSAPPublisher::RefreshContentWithPriorityL( TInt aContentId,
                                                        TInt aPriority )
	{
	if( aContentId == EAiDeviceStatusContentNetworkIdentity &&
	    aPriority == EAiBTSAP )
        {
	    RefreshL( EFalse );
        if( iSuccess )
	        {
	        return ETrue;
	        }
        }
    return EFalse;
	}
