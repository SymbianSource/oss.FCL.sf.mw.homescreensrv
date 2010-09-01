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
* Description:  Closed user group (CUG) publisher.
*
*/


#include <aidevstaplgres.rsg>
#include <StringLoader.h>
#include <aicontentobserver.h>
#include "aicugmcnpublisher.h"
#include "ainetworkinfolistener.h"

// ======== MEMBER FUNCTIONS ========

CAiCUGMCNPublisher::CAiCUGMCNPublisher()
    {
    }


void CAiCUGMCNPublisher::ConstructL()
    {
    iListener = CAiNetworkInfoListener::InstanceL();
    User::LeaveIfError( iSSSettings.Open() );
    
    // Initialize the periodic timer.
    iPeriodic = CPeriodic::NewL( CActive::EPriorityStandard );
    }


CAiCUGMCNPublisher* CAiCUGMCNPublisher::NewL()
    {
    CAiCUGMCNPublisher* self = new( ELeave ) CAiCUGMCNPublisher;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( );
    //CleanupStack::Pop( self );
    return self;
    }


CAiCUGMCNPublisher::~CAiCUGMCNPublisher()
    {
    if( iPeriodic )
            {
            iPeriodic->Cancel();
            delete iPeriodic;
            }
    if( iListener )
            {
            iListener->RemoveObserver( *this );
            iListener->Release();
            }
    //Cancel listenning CUG changes.
    iSSSettings.CancelAll( *this );
    iSSSettings.Close();
    
    delete iCUG;
    delete iMCN;
    }

TInt CAiCUGMCNPublisher::PeriodicTimerCallBack(TAny* aAny)
	{
	CAiCUGMCNPublisher* self = static_cast<CAiCUGMCNPublisher*>( aAny );
  
	if( self )
	        {
	        self->DoAnimation();
	        }

	    return KErrNone;
	}

void CAiCUGMCNPublisher::DoAnimation()
	{
	if ( iAnimationDone )
		{
		PublishCUG();
		iPeriodic->Cancel();
		}
	else
		{
		iAnimationDone = ETrue;
		PublishMCN();
		}

	}


void CAiCUGMCNPublisher::ResumeL()
    {
    iListener->AddObserverL( *this );
    
	//Start listening CUG changes.
    TInt err = iSSSettings.Register( ESSSettingsCug, *this );

    if( err == KErrNotSupported ||
        err == KErrAlreadyExists  )
        {
        //CUG not supported, or already registered
        err = KErrNone;
        }

    User::LeaveIfError( err );
    }


void CAiCUGMCNPublisher::Subscribe( MAiContentObserver& aObserver, 
							        CHsContentPublisher& aExtension,
                                    MAiPublishPrioritizer& /*aPrioritizer*/,
                                    MAiPublisherBroadcaster& /*aBroadcaster*/ )
    {
    iContentObserver = &aObserver;
    iExtension = &aExtension;
    }


void CAiCUGMCNPublisher::RefreshL( TBool /*aClean*/ )
    {
	//Get current CUG value and publish it.
    TInt value;
    if ( iSSSettings.Get( ESSSettingsCug, value ) != KErrNone )
        {
        value = ESSSettingsCugDefault;
        }

    UpdateCUGMCNIndicatorL( value );
    }


void CAiCUGMCNPublisher::PhoneSettingChanged( TSSSettingsSetting aSetting, 
											TInt aNewValue )
    {
    if( aSetting == ESSSettingsCug )
        {
        TRAP_IGNORE( UpdateCUGMCNIndicatorL( aNewValue ) );
        }
    }

void CAiCUGMCNPublisher::UpdateCUGMCNIndicatorL( TInt aValue )
    {
	//Check if the CUG is valid...
    if( iSSSettings.IsValueValidCugIndex( aValue ) )
        {
		//...and then read CUG indicator format from resource file and publish 
		// content as a text.
        if (iCUG)
        	{
        	delete iCUG;
        	iCUG = NULL;
        	}
        iCUG = StringLoader::LoadL( 
        						R_ACTIVEIDLE_CUG_INDICATOR_FORMAT, 
        						aValue );
        						
		Animate();
        }
    else
        {            
		//CUG is not valid, call clean.
		if ( iCUG )
			{
			iContentObserver->Clean( *iExtension, 
        					EAiDeviceStatusContentCUGMCNIndicator, 
        					0 );
			iCUG = NULL;
			}
        }
    }

void CAiCUGMCNPublisher::HandleNetworkInfoChange( 
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
            if (iMCN)
            	{
            	delete iMCN;
            	iMCN = NULL;
            	}
            	
            TRAP_IGNORE(iMCN = msg.AllocL());
			Animate();
            }
        else if ( aInfo.iMCNIndicatorType == ENWMCNIndicatorTypeNone )
            {
			if ( iMCN )
				{
				//MCN is not valid, call clean.
				iContentObserver->Clean( *iExtension, 
								EAiDeviceStatusContentCUGMCNIndicator,
			            		0 );
				iMCN = NULL;
				}
            }
        }
    }


TBool CAiCUGMCNPublisher::RefreshL( TInt aContentId, TBool aClean )
	{
    if( ( aContentId == EAiDeviceStatusContentCUGMCNIndicator ) 
    		|| ( aContentId == EAiDeviceStatusContentNetworkIdentity ) )
        {
   	    RefreshL( aClean );
   	    return ETrue;
    	}
    	
    return EFalse;
	}

void CAiCUGMCNPublisher::Animate()
	{
	if ( iCUG && iMCN )
		{
		if ( !iPeriodic->IsActive() )
			{
			iAnimationDone = EFalse;
			PublishCUG();
			iPeriodic->Start( KAnimDelay, KAnimDelay, TCallBack( PeriodicTimerCallBack, this ) );
			}
		}
	else
		{
		if ( iCUG )
			{
			PublishCUG();
			}
		else if ( iMCN )
			{
			PublishMCN();
			}
		}
	}

void CAiCUGMCNPublisher::PublishCUG()
	{
	iContentObserver->Publish( *iExtension, 
			        	EAiDeviceStatusContentCUGMCNIndicator, 
			        	*iCUG, 
			        	0 );
	}

void CAiCUGMCNPublisher::PublishMCN()
	{
	iContentObserver->Publish( *iExtension, 
						EAiDeviceStatusContentCUGMCNIndicator, 
		            	*iMCN, 
		            	0 );
	}

