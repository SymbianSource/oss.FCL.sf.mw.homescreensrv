/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#include    "hs_app_aistate.h"
#include    "hs_app_aifwpanic.h"
#include    <e32property.h>
#include    <activeidle2domainpskeys.h>
#include    "debug.h"

CActiveIdleState::CActiveIdleState()
    {
    }

void CActiveIdleState::ConstructL()
    {
    iUpdater = 
        CIdle::NewL( CActive::EPriorityStandard );

    //for the sake of safe, this function is called here
    UpdateStateAsync();
    }

CActiveIdleState* CActiveIdleState::NewL()
    {
    CActiveIdleState* self = 
        new (ELeave) CActiveIdleState();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }

CActiveIdleState::~CActiveIdleState()
    {
    delete iUpdater;
    }

// -----------------------------------------------------------------------------
// CActiveIdleState::UpdateState
// -----------------------------------------------------------------------------
//
void CActiveIdleState::SetIsIdleForeground(
        TBool aIsForeground )
    {
    __PRINT( __DBG_FORMAT("XAI: CActiveIdleState::SetIsIdleForeground(%d), iIsIdleForeground=%d"), 
        aIsForeground, iIsIdleForeground );
    if ( iIsIdleForeground != aIsForeground )
        {
        iIsIdleForeground = aIsForeground;
        UpdateStateAsync();
        }
    }

// -----------------------------------------------------------------------------
// CActiveIdleState::UpdateStateAsync
// -----------------------------------------------------------------------------
//
void CActiveIdleState::UpdateStateAsync()
    {
    // Asynchronous change is done only when going to idle state.
    if ( iIsIdleForeground )
        {
        if ( !iUpdater->IsActive() )
            {
            iUpdater->Start( 
                TCallBack( DoUpdateState, this ) );
            }
        }
    else
        {
        iUpdater->Cancel();
        UpdateActiveIdleState();
        }
    }

// -----------------------------------------------------------------------------
// CActiveIdleState::UpdateActiveIdleState
// -----------------------------------------------------------------------------
//
void CActiveIdleState::UpdateActiveIdleState()
    {
    const EPSActiveIdleState state =
        iIsIdleForeground ? EPSAiForeground : EPSAiBackground;

    TInt setResult = 
        RProperty::Set(
            KPSUidAiInformation, 
            KActiveIdleState, 
            state );

    __PRINT( __DBG_FORMAT( "XAI: Set CActiveIdleState::UpdateActiveIdleState: KTelephonyIdleStatus=%d, P&S result=%d" ),
        TInt(state), setResult );
    }

// -----------------------------------------------------------------------------
// CActiveIdleState::DoUpdateSaSetting
// -----------------------------------------------------------------------------
//
TInt CActiveIdleState::DoUpdateState( TAny* aAny )
    {
    __ASSERT_DEBUG( aAny, AiFwPanic::Panic(AiFwPanic::EAiFwPanic_NullPointerReference ) );
    static_cast< CActiveIdleState* >( aAny )->UpdateActiveIdleState();
    return KErrNone;
    }

