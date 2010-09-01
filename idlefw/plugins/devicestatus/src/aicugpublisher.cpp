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
#include "aicugpublisher.h"
#include "ainetworkinfolistener.h"


// ======== MEMBER FUNCTIONS ========

CAiCUGPublisher::CAiCUGPublisher()
    {
    }


void CAiCUGPublisher::ConstructL()
    {
    User::LeaveIfError( iSSSettings.Open() );    
    }


CAiCUGPublisher* CAiCUGPublisher::NewL()
    {
    CAiCUGPublisher* self = new( ELeave ) CAiCUGPublisher;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


CAiCUGPublisher::~CAiCUGPublisher()
    {
    //Cancel listenning CUG changes.
    iSSSettings.CancelAll( *this );
    iSSSettings.Close();
    delete iCugText;
    }


void CAiCUGPublisher::ResumeL()
    {
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


void CAiCUGPublisher::Subscribe( MAiContentObserver& aObserver, 
							        CHsContentPublisher& aExtension,
                                    MAiPublishPrioritizer& /*aPrioritizer*/,
                                    MAiPublisherBroadcaster& /*aBroadcaster*/ )
    {
    iContentObserver = &aObserver;
    iExtension = &aExtension;
    }


void CAiCUGPublisher::RefreshL( TBool /*aClean*/ )
    {
	//Get current CUG value and publish it.
    TInt value;
    if ( iSSSettings.Get( ESSSettingsCug, value ) != KErrNone )
        {
        value = ESSSettingsCugDefault;
        }

    UpdateCUGIndicatorL( value );
    }


void CAiCUGPublisher::PhoneSettingChanged( TSSSettingsSetting aSetting, 
											TInt aNewValue )
    {
    if( aSetting == ESSSettingsCug )
        {
        TRAP_IGNORE( UpdateCUGIndicatorL( aNewValue ) );
        }
    }


void CAiCUGPublisher::UpdateCUGIndicatorL( TInt aValue )
    {
	//Check if the CUG is valid
    if( iSSSettings.IsValueValidCugIndex( aValue ) )
        {
        // Information already current, no need to update text
        if ( iCugValue != aValue )
            {
            iCugValue = aValue;
            delete iCugText;
            iCugText = NULL;
            // Read CUG indicator format from resource file and publish 
            // content as a text.
            iCugText = StringLoader::LoadL( 
                                    R_ACTIVEIDLE_CUG_INDICATOR_FORMAT, 
                                    aValue );

            }
        if ( iCugText )
            {
            iContentObserver->Publish( *iExtension, 
                                EAiDeviceStatusContentCUGIndicator, 
                                *iCugText, 
                                0 );            
            } 
        else
            {           
            iContentObserver->Clean( *iExtension, 
                                EAiDeviceStatusContentCUGIndicator, 
                                0 );            
            }
        }
    else
        {            
		//CUG is not valid, call clean.
        iContentObserver->Clean( *iExtension, 
        					EAiDeviceStatusContentCUGIndicator, 
        					0 );
        }
    }
    

TBool CAiCUGPublisher::RefreshL( TInt aContentId, TBool aClean )
	{
    if( aContentId == EAiDeviceStatusContentCUGIndicator )
        {
   	    RefreshL( aClean );
   	    return ETrue;
    	}
    	
    return EFalse;
	}
