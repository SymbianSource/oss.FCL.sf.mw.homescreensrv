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
* Description:  Viag Home Zone (VHZ) publisher.
*
*/


#include <centralrepository.h>
#include "activeidle2domaincrkeys.h"
#include "hs_app_aivhzpublisher.h"
#include "aicontentobserver.h"
#include "hs_app_ainetworkinfolistener.h"


// ======== MEMBER FUNCTIONS ========

CAiVHZPublisher::CAiVHZPublisher()
    {
    }
    

void CAiVHZPublisher::ConstructL()
    {
    iListener = CAiNetworkInfoListener::InstanceL();
    
    CRepository* rep = CRepository::NewL( TUid::Uid( KCRUidActiveIdleLV ) ); 
    CleanupStack::PushL( rep );
    
    TBool value = EFalse;
    TInt err = rep->Get( KAIVHZInMainpane, value );
    if( err == KErrNone )
        {
        iVhzInMainpane = value;
        }
    CleanupStack::PopAndDestroy( rep );
    }


CAiVHZPublisher* CAiVHZPublisher::NewL()
    {
    CAiVHZPublisher* self = new( ELeave ) CAiVHZPublisher;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


CAiVHZPublisher::~CAiVHZPublisher()
    {
    if( iListener )
        {
        iListener->RemoveObserver( *this );
        iListener->Release();
        }
    }


void CAiVHZPublisher::ResumeL()
    {
    iListener->AddObserverL( *this );
    }


void CAiVHZPublisher::Subscribe( MAiContentObserver& aObserver, 
							        MAiPropertyExtension& aExtension,
                                    MAiPublishPrioritizer& /*aPrioritizer*/,
                                    MAiPublisherBroadcaster& /*aBroadcaster*/ )
    {
    iContentObserver = &aObserver;
    iExtension = &aExtension;
    }


void CAiVHZPublisher::RefreshL( TBool /*aClean*/ )
    {
    const TNWInfo& nwInfo = iListener->NetworkInfo();

    //Check if the Viag indicator in network info is different than none...
    if( nwInfo.iViagIndicatorType != ENWViagIndicatorTypeNone )
        {
		//... and then publish it.
		if( !iVhzInMainpane )
		    {
            iContentObserver->Publish( *iExtension, 
                                        EAiDeviceStatusContentVHZIndicator, 
                                        nwInfo.iViagTextTag, 
                                        0 );
		    }
        else
		    {
            iContentObserver->Publish( *iExtension, 
                                        EAiDeviceStatusContentVHZText, 
                                        nwInfo.iViagTextTag, 
                                        0 );
		    }
        }
    else
    	{
		if( !iVhzInMainpane )
		    {
        	iContentObserver->Clean( *iExtension,
        							  EAiDeviceStatusContentVHZIndicator,
    	    						  0 );
		    }
		else
		    {
        	iContentObserver->Clean( *iExtension,
        							  EAiDeviceStatusContentVHZText,
    	    						  0 );
		    }
    	}
    }


void CAiVHZPublisher::HandleNetworkInfoChange( 
                        const MNWMessageObserver::TNWMessages& aMessage, 
                        const TNWInfo& /*aInfo*/,
                        const TBool /*aShowOpInd*/ )
    {
	//if home zone message has arrived, call refresh
    if ( aMessage == MNWMessageObserver::ENWMessageCurrentHomeZoneMessage )
        {
        TRAP_IGNORE( RefreshL( ETrue ) );
        }
    }


TBool CAiVHZPublisher::RefreshL( TInt aContentId, TBool aClean )
	{
    if( aContentId == EAiDeviceStatusContentVHZIndicator )
        {
   	    RefreshL( aClean );
   	    return ETrue;
    	}
    	
    return EFalse;
	}
