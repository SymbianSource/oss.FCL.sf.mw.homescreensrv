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
* Description:  Keylock handler implementation for Active Idle WS Plug-in
*
*/


#include "hs_app_keylockhandler.h"
#include "hs_app_keyhandlertimer.h"
#include "hs_app_keylockstates.h"
#include "hs_app_aiwspluginanimdef.h"
#include "hs_app_uistate.h"

#include <e32property.h>
#include <activeidle2internalpskeys.h>
#include <keylockpolicyapi.h>

#include "debug.h"

namespace AiWsPlugin {

CKeyLockHandler::CKeyLockHandler() :
    // Initialize to start-up state
    iCurrentState( &TKeylockState::StartupState() ),
    iNextState( &iCurrentState->iDefaultNextState() )
    {
    }
    
void CKeyLockHandler::ConstructL()
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

    iSettings.ReadFromRepositoryL();    
    iKeypadLockTimer = CPeriodic::NewL( CActive::EPriorityUserInput );
    iKeylockApi = CKeyLockPolicyApi::NewL( EPolicyActivateKeyguard );
    if ( !iKeylockApi->HasConfiguration() )
        {
        delete iKeylockApi;
        iKeylockApi = NULL;
        }
    }

CKeyLockHandler* CKeyLockHandler::NewLC()
    {
    CKeyLockHandler* self = new( ELeave ) CKeyLockHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CKeyLockHandler::~CKeyLockHandler()
    {
    delete iKeypadLockTimer;
    delete iKeylockApi;
    }

void CKeyLockHandler::SetUiStateQuery( MUiState& aUiState )
    {
    iUiState = &aUiState;
    }

TBool CKeyLockHandler::HasFocus()
    {
    return iUiState->HasFocus();
    }
    
TBool CKeyLockHandler::TimeoutTimerActive()
    {
    return iKeypadLockTimer->IsActive();
    }

void CKeyLockHandler::FocusChanged( TBool aState )
    {
    iCurrentState->FocusChanged( *this, aState );
    }
    
TBool CKeyLockHandler::OfferRawEvent(const TRawEvent& aRawEvent)
    {
    return iCurrentState->OfferRawEvent( *this, aRawEvent );    
    }

TBool CKeyLockHandler::IsFirstLockKey( TInt aScanCode )
    {  
    if ( iKeylockApi )
        {
        TKeyEvent event;
        event.iModifiers = 0;
        event.iCode = 0;
        event.iRepeats = 0;
        event.iScanCode = aScanCode;
        // Keylock API return ETrue on handlekeyeventL only if the whole key
        // sequence has been inputted (1st + 2nd key pressed)
        TRAP_IGNORE(iKeylockApi->HandleKeyEventL( event,EEventKeyDown ));
        return iKeylockApi->PrimaryKeyPressed();
        }
    else
        {
        return Settings().IsFirstLockKey( aScanCode );
        }
    }

TBool CKeyLockHandler::IsSecondLockKey( TInt aScanCode )
    {   
    TBool returnValue = EFalse;
    if ( iKeylockApi )
        {
        TKeyEvent event;
        event.iModifiers = 0;
        event.iCode = 0;
        event.iRepeats = 0;
        event.iScanCode = aScanCode;
        TRAP_IGNORE(returnValue = iKeylockApi->HandleKeyEventL( event,EEventKeyDown ));
        return returnValue;
        }
    else
        {
        return Settings().IsSecondLockKey( aScanCode );
        }
    }

const TKeypadSettings& CKeyLockHandler::Settings() const
    {
    return iSettings;
    }

void CKeyLockHandler::StartTimeoutTimer( TInt aTimeout )
    {
    __PRINTS( "AiWsPlugin: CKeyLockHandler::StartTimeoutTimer()" );
    
    iKeypadLockTimer->Cancel();
    if( aTimeout < 0 )
        {
        iKeypadLockTimer->Start( 
            iSettings.KeylockTimeout(), iSettings.KeylockTimeout(),
            TCallBack( &CKeyLockHandler::TimerElapsed, this ) );
        }
    else
        {
        iKeypadLockTimer->Start( 
            aTimeout, aTimeout,
            TCallBack( &CKeyLockHandler::TimerElapsed, this ) );
        }
    }
    
void CKeyLockHandler::StopTimeoutTimer()
    {
    __PRINTS( "AiWsPlugin: CKeyLockHandler::StopTimeoutTimer()" );
    iKeypadLockTimer->Cancel();
    }

void CKeyLockHandler::ActivateKeypadLock()
	{
    __PRINTS( "AiWsPlugin: CKeyLockHandler::ActivateKeypadLock()" );
	StopTimeoutTimer();
	// Use Shortcut Plug-in API to set the keylock
	RProperty::Set(
		KUidSystemCategory, 
	    KPSUidShortcutCmd,
	    KAiPSEnableKeyLock );
    }

void CKeyLockHandler::CancelKeypadLock()
	{
    __PRINTS( "AiWsPlugin: CKeyLockHandler::CancelKeypadLock()" );
	StopTimeoutTimer();
	RProperty::Set(
		KUidSystemCategory, 
	    KPSUidShortcutCmd,
	    KAiPSSkipKeyLock );
	} 

void CKeyLockHandler::KeypadLockTimeout()
	{
    __PRINTS( "AiWsPlugin: CKeyLockHandler::CancelKeypadLock()" );
	StopTimeoutTimer();
	RProperty::Set(
		KUidSystemCategory, 
	    KPSUidShortcutCmd,
	    KAiPSKeyLockTimeout );
	} 

void CKeyLockHandler::SetNextState( const TKeylockState& aState )
    {
    iNextState = &aState;
    }

void CKeyLockHandler::ActivateNextState()
    {
    iCurrentState = iNextState;
    iNextState = &(*iCurrentState->iDefaultNextState)();
    }

void CKeyLockHandler::SetLastLockKeyScanCode( TInt aScanCode )
    {
    iLastLockKeyScanCode = aScanCode;
    }
    
TInt CKeyLockHandler::LastLockKeyScanCode() const
    {
    return iLastLockKeyScanCode;
    }
    
TInt CKeyLockHandler::TimerElapsed(TAny* aSelf)
    {
    CKeyLockHandler* self = static_cast<CKeyLockHandler*>(aSelf);
    if( self )
        {
        self->iCurrentState->TimerElapsed( *self );
        }
    return KErrNone;
    }
    
void CKeyLockHandler::TimerDone()
    {
        
    }

} // namespace AiWsPlugin
