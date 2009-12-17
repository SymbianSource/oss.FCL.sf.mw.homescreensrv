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
* Description:  Focus observer for Active idle 2
*
*/


#include <aipspropertyobserver.h>
#include <activeidle2domainpskeys.h>
#include "aistatemanager.h"
#include "aifocusobserver.h"
#include "aifwpanic.h"
#include "debug.h"

CAiFocusObserver::CAiFocusObserver()
    {
    }
    
CAiFocusObserver::~CAiFocusObserver()
    {
    }

CAiFocusObserver* CAiFocusObserver::NewL(
                                        MAiStateManager* aStateManager )
    {
    CAiFocusObserver* self = new (ELeave) CAiFocusObserver();
    CleanupStack::PushL(self);
    self->ConstructL( aStateManager );
    CleanupStack::Pop(self);
    return self;
    }

void CAiFocusObserver::ConstructL( MAiStateManager* aStateManager )
    {
 //++HV
    BaseConstructL( TCallBack( StaticHandleFocusChangeEvent, this ),
                    KPSUidAiInformation, 
                    KActiveIdleState, 
                    aStateManager );
//--HV
    }
    
TAiStateChanges CAiFocusObserver::Status()
    {
    TInt value = 0;
    TInt err = iObserver->Get( value );
    if( ( value == EPSAiForeground ) &&
        ( err == KErrNone ) )
        {
        return ESMAIIdleForeground;
        }
    else
        {
        return ESMAIIdleBackground;
        } 
    }

 //++HV
                
 TInt CAiFocusObserver::StaticHandleFocusChangeEvent( TAny* aPtr )
    {
    CAiFocusObserver* self =
                static_cast<CAiFocusObserver*>( aPtr );
    
    __ASSERT_DEBUG( self, 
                    AiFwPanic::Panic( AiFwPanic::EAiFwPanic_NullPointerReference ) );
                    
    return( self->HandleFocusChangeEvent() );                
    }
    
    
TInt CAiFocusObserver::HandleFocusChangeEvent()
    {
    if( iTfxEffectActive )
		{
       	return KErrNone;
        }  
    
    TInt value = 0;
    TInt err = iObserver->Get( value );
    
    // Check the PS keys value and call manager with approriate parameter.
    // Repowrt either "idle foreground" or "idle background"
    if( ( value == EPSAiForeground ) &&
        ( err == KErrNone ) )
        {
       	// Check if the transition effect is active
       	
       	// This has to be called first, otherwise the state might not be valid.
		CAknTransitionUtils::AddObserver( this, CAknTransitionUtils::EEventWsBufferRedirection );

 		TInt redirState = 0;
 		CAknTransitionUtils::GetState( CAknTransitionUtils::EEventWsBufferRedirection, &redirState );
 		if ( (TBool)redirState )
 			{
 			// The effect is on-going. Prevent view refresh until the effect is finished.
			iTfxEffectActive = ETrue; 
 			} 
 		else
 			{			
    		// No effect on-going. Observer is not needed.    
			CAknTransitionUtils::RemoveObserver( this, CAknTransitionUtils::EEventWsBufferRedirection );
        	iStateManager->ReportStateChange( ESMAIIdleForeground );
 			}
        }
    else if( value == EPSAiBackground )
        {
    	// Do not receive callbacks in background. Remove observer if it still exists.
    	CAknTransitionUtils::RemoveObserver( this, CAknTransitionUtils::EEventWsBufferRedirection );
    	iTfxEffectActive = EFalse;
    		
        iStateManager->ReportStateChange( ESMAIIdleBackground );
        } 

    return KErrNone;
    }


TInt CAiFocusObserver::AknTransitionCallback( TInt aEvent, TInt aState, const TDesC8* /*aParams*/ )
	{
	if ( ( aEvent & CAknTransitionUtils::EEventWsBufferRedirection ) && ( !(TBool)aState ) )
		{
		// The effect has been finished
		iTfxEffectActive = EFalse;
   		// Observer is not needed any more.    
    	CAknTransitionUtils::RemoveObserver( this, CAknTransitionUtils::EEventWsBufferRedirection );
    	
    	// Issue one focus change event
    	TInt value = 0;
    	TInt err = iObserver->Get( value );
    	if( ( value == EPSAiForeground ) &&
        	( err == KErrNone ) )
        	{
    	 	iStateManager->ReportStateChange( ESMAIIdleForeground );
        	}
    	else if( value == EPSAiBackground )
        	{
        	iStateManager->ReportStateChange( ESMAIIdleBackground );
        	} 
		}
	
	return 0;	
	}
	

//--HV
