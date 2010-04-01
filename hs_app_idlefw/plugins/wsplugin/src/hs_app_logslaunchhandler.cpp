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
* Description:  Logs app launch handler for Active Idle WS Plug-in.
*
*/


#include "hs_app_logslaunchhandler.h"
#include "hs_app_uistate.h"
#include <e32property.h>
#include <e32event.h>
#include <e32keys.h>
#include <activeidle2internalpskeys.h>
#include <activeidle2domainpskeys.h>

namespace AiWsPlugin {


CLogsLaunchHandler::CLogsLaunchHandler()
    {
    }

void CLogsLaunchHandler::ConstructL()
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
    }

CLogsLaunchHandler* CLogsLaunchHandler::NewLC()
    {
    CLogsLaunchHandler* self = new(ELeave) CLogsLaunchHandler;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

CLogsLaunchHandler::~CLogsLaunchHandler()
    {
     if ( iTimer)
     {
     iTimer->Cancel();
     delete iTimer;
     }
    }

void CLogsLaunchHandler::SetUiStateQuery( MUiState& aUiState )
    {
    iUiState = &aUiState;
    }

void CLogsLaunchHandler::FocusChanged( TBool /*aState*/ )
    {
    // Focus status is queried from iUiState
    }

TBool CLogsLaunchHandler::OfferRawEvent(const TRawEvent& aRawEvent)
    {
    TInt reactOnSendKey = 1;
    TInt err = RProperty::Get( KPSUidAiInformation, KActiveIdleActOnSendKey, reactOnSendKey );
    if ( err != KErrNone )
        {
        reactOnSendKey = 1;
        }

    TBool consumed = EFalse;

    if ( reactOnSendKey )
        {
        switch( aRawEvent.Type() )
            {
            case TRawEvent::EKeyDown:
                {
                if ( iUiState->HasFocus() && aRawEvent.ScanCode() == EStdKeyYes )
                    {
                    const TTimeIntervalMicroSeconds32 KLongKeyPress(600000);
                    iTimer->Cancel();
                    iTimer->After(KLongKeyPress);
                    consumed = ETrue;
                    }
                break;
                }
            case TRawEvent::EKeyUp:
                {
                if ( iUiState->HasFocus() && aRawEvent.ScanCode() == EStdKeyYes && iTimer->IsActive() )
                    {
                    iTimer->Cancel();
                    RProperty::Set(
                        KUidSystemCategory,
                        KPSUidShortcutCmd,
                        KAiPSLaunchLogs );
                    consumed = ETrue;
                    }
                break;
                }
            }
        }

    return consumed;
    }

void CLogsLaunchHandler::TimerDone()
    {
    RProperty::Set(
        KUidSystemCategory,
        KPSUidShortcutCmd,
        KAiPSLaunchNameDialer );
    }

} // namespace AiWsPlugin
