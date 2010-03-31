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
* Description:  Micro Cellular Network (MCN) publisher.
*
*/


#include "hs_app_aimcnpublisher.h"
#include "aicontentobserver.h"
#include "hs_app_ainetworkinfolistener.h"


// ======== MEMBER FUNCTIONS ========

CAiMCNPublisher::CAiMCNPublisher()
    {
    }


void CAiMCNPublisher::ConstructL()
    {
    iListener = CAiNetworkInfoListener::InstanceL();
    }


CAiMCNPublisher* CAiMCNPublisher::NewL()
    {
    CAiMCNPublisher* self = new( ELeave ) CAiMCNPublisher;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


CAiMCNPublisher::~CAiMCNPublisher()
    {
    if( iListener )
        {
        iListener->RemoveObserver( *this );
        iListener->Release();
        }
    }


void CAiMCNPublisher::ResumeL()
    {
    iListener->AddObserverL( *this );
    }


void CAiMCNPublisher::Subscribe( MAiContentObserver& aObserver, 
							        MAiPropertyExtension& aExtension,
                                    MAiPublishPrioritizer& /*aPrioritizer*/,
                                    MAiPublisherBroadcaster& /*aBroadcaster*/ )
    {
    iContentObserver = &aObserver;
    iExtension = &aExtension;
    }
    
    
void CAiMCNPublisher::RefreshL( TBool /*aClean*/ )
    {
    //cannot be refreshed
    }


void CAiMCNPublisher::HandleNetworkInfoChange( 
						const MNWMessageObserver::TNWMessages& aMessage, 
						const TNWInfo& aInfo,
						const TBool /*aShowOpInd*/ )
    {
	//If MSN message was received, publish it.
    if ( aMessage == MNWMessageObserver::ENWMessageCurrentCellInfoMessage )
        {
        if ( aInfo.iMCNIndicatorType == ENWMCNIndicatorTypeActive )
            {
            TPtrC msg = aInfo.iMCNName;
            iContentObserver->Publish( *iExtension, 
            							EAiDeviceStatusContentMCNIndicator, 
            							msg, 
            							0 );
            }
        else if ( aInfo.iMCNIndicatorType == ENWMCNIndicatorTypeNone )
            {
            iContentObserver->Clean( *iExtension, 
            							EAiDeviceStatusContentMCNIndicator,
            							0 );
            }
        }
    }


TBool CAiMCNPublisher::RefreshL( TInt aContentId, TBool aClean )
	{
    if( aContentId == EAiDeviceStatusContentMCNIndicator )
        {
   	    RefreshL( aClean );
   	    return ETrue;
    	}
    	
    return EFalse;
	}
