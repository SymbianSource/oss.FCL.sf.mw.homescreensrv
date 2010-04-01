/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SIND app launch handler for Active Idle WS Plug-in.
*
*/


#include "hs_app_sindlaunchhandler.h"
#include "hs_app_keyhandlertimer.h"
#include "hs_app_uistate.h"
#include <e32property.h>
#include <e32event.h>
#include <e32keys.h>
#include <activeidle2internalpskeys.h>

//#define AI_ENABLE_RD_LOGGING
#define AI_RD_LOG_TO_DEBUG_OUTPUT

#include "debug.h"

namespace AiWsPlugin {


CSINDLaunchHandler::CSINDLaunchHandler()
    {
    }
    
void CSINDLaunchHandler::ConstructL()
    {
    // Read capability: ReadDeviceData.
    _LIT_SECURITY_POLICY_C1( KReadDevicePolicy, ECapabilityReadDeviceData );
    // Write capability: WriteDeviceData.
    _LIT_SECURITY_POLICY_C1( KWriteDevicePolicy, ECapabilityWriteDeviceData );

    // Initialize Shortcut Plug-in command API
    RProperty::Define( 
    	KUidSystemCategory,
    	KPSUidShortcutCmd,  
      	RProperty::EText,
        KReadDevicePolicy,
        KWriteDevicePolicy 
        );
        
    iTimer = CKeyHandlerTimer::NewL( this );
    
    iSINDKeyDown = EFalse;
    }

CSINDLaunchHandler* CSINDLaunchHandler::NewLC()
    {
    CSINDLaunchHandler* self = new(ELeave) CSINDLaunchHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
CSINDLaunchHandler::~CSINDLaunchHandler()
    {
    delete iTimer;
    }

void CSINDLaunchHandler::SetUiStateQuery( MUiState& aUiState )
    {
    iUiState = &aUiState;
    }
    
void CSINDLaunchHandler::FocusChanged( TBool /*aState*/ )
    {
    // Focus status is queried from iUiState
    }
    
TBool CSINDLaunchHandler::OfferRawEvent(const TRawEvent& aRawEvent)
    {
    switch( aRawEvent.Type() )
        {
        case TRawEvent::EKeyDown:
            {
            if ( iUiState->HasFocus() && aRawEvent.ScanCode() == EStdKeyDevice1 )
        		{		
				__PRINTS( "XAI: CSINDLaunchHandler: SIND key down, start timer");
				const TTimeIntervalMicroSeconds32 KLongKeyPress(600000);
				iTimer->Cancel();
				iTimer->After(KLongKeyPress);
                iSINDLaunched = EFalse;
                iSINDKeyDown = ETrue;                
        		}        		
            else if( iUiState->HasFocus() && iSINDKeyDown )
                {
                __PRINTS( "XAI: CSINDLaunchHandler: SIND key down, other key pressed, cancel timer");
                iTimer->Cancel();
                SkipVoiceDial();
                }
            break;
            }
        case TRawEvent::EKeyUp:
            {
            if ( iUiState->HasFocus() && aRawEvent.ScanCode() == EStdKeyDevice1 && !iSINDLaunched && iTimer->IsActive() )
                {                
                __PRINTS( "XAI: SIND key up, cancel timer");
                iTimer->Cancel();
                SkipVoiceDial();
                }
            break;
            }
        }
    return EFalse;
    }

void CSINDLaunchHandler::SkipVoiceDial()
    {
    __PRINTS( "XAI: CSINDLaunchHandler::SkipVoiceDial()");
    // Handle skip scenario only if voice dial ui hasn't been launched
    if( !iSINDLaunched )
        {
        RProperty::Set(
	    	KUidSystemCategory, 
		    KPSUidShortcutCmd,
		    KAiPSSkipNameDialer );  
        }
    iSINDKeyDown = EFalse; 
    }
    
void CSINDLaunchHandler::TimerDone()
    {
    __PRINTS( "XAI: CSINDLaunchHandler::TimerDone()");
    __PRINTS( "XAI: Start Voice Dial UI");
    RProperty::Set(
		KUidSystemCategory, 
		KPSUidShortcutCmd,
		KAiPSLaunchNameDialer );    		
    iSINDLaunched = ETrue;
    iSINDKeyDown = EFalse; 
    }    

} // namespace AiWsPlugin
