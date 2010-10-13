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


#include    "aistate.h"
#include    "aifwpanic.h"
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
    TInt state( 0 );
    TInt err( RProperty::Get( KPSUidAiInformation, KActiveIdleState, state ) );
    if ( !iIsIdleForeground && KErrNone == err &&
        EPSAiNumberEntry == (EPSActiveIdleState)state )
        {
        __PRINTS( "*** CActiveIdleState::UpdateActiveIdleState - Background & EPSAiNumberEntry, skip state update" );
        }
    else
        {
        state = iIsIdleForeground ? EPSAiForeground : EPSAiBackground;
        err = RProperty::Set(
            KPSUidAiInformation, 
            KActiveIdleState, 
            (EPSActiveIdleState)state );

        __PRINT( __DBG_FORMAT( "XAI: Set CActiveIdleState::UpdateActiveIdleState: KTelephonyIdleStatus=%d, P&S result=%d" ),
        TInt(state), err );
        }
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

