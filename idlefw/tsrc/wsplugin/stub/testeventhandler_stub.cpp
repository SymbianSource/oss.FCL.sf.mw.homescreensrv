/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Keylock handler for Active Idle WS Plug-in.
*
*/

// System includes

// User includes
#include "testeventhandler_stub.h"

// Constants

namespace T_AiWsPlugin {

// ======== LOCAL FUNCTIONS ========
    
// ======== MEMBER FUNCTIONS =======
// ----------------------------------------------------------------------------
// operator==
//
// ----------------------------------------------------------------------------
//
static TBool operator==( const TRawEvent& aLhs, const TRawEvent& aRhs )
    {
    const TUint8* lhs = reinterpret_cast<const TUint8*>( &aLhs );
    const TUint8* rhs = reinterpret_cast<const TUint8*>( &aRhs );
    
    return ( Mem::Compare( lhs, sizeof(TRawEvent), rhs, sizeof(TRawEvent) ) == 0 );
    }

// ----------------------------------------------------------------------------
// ResetEvent
//
// ----------------------------------------------------------------------------
//
static void ResetEvent( TRawEvent& aEvent )
    {
    Mem::FillZ( &aEvent, sizeof( TRawEvent ) );
    }
  
// ----------------------------------------------------------------------------
// CTestEventHandler::~CTestEventHandler()
//
// ----------------------------------------------------------------------------
//
CTestEventHandler::~CTestEventHandler()
    {
    }

// ----------------------------------------------------------------------------
// CTestEventHandler::ResetLastEvent()
//
// ----------------------------------------------------------------------------
//
void CTestEventHandler::ResetLastEvent()
    {
    ResetEvent( iLastEvent );
    }

// ----------------------------------------------------------------------------
// CTestEventHandler::RespondToEvent()
//
// ----------------------------------------------------------------------------
//
void CTestEventHandler::RespondToEvent( const TRawEvent& aEvent )
    {
    iRespondToEvent = aEvent;
    }

// ----------------------------------------------------------------------------
// CTestEventHandler::SetUiStateQuery()
//
// ----------------------------------------------------------------------------
//
void CTestEventHandler::SetUiStateQuery( MUiState& aUiState )
    {
    iUiState = &aUiState;
    }
  
// ----------------------------------------------------------------------------
// CTestEventHandler::FocusChanged()
//
// ----------------------------------------------------------------------------
//
void CTestEventHandler::FocusChanged( TBool aState )
    {
    iFocus = aState;
    }

// ----------------------------------------------------------------------------
// CTestEventHandler::OfferRawEvent()
//
// ----------------------------------------------------------------------------
//
TBool CTestEventHandler::OfferRawEvent( const TRawEvent& aRawEvent )
    {    
    if ( iRespondToEvent == aRawEvent )
        {
        iLastEvent = aRawEvent;
        
        ResetEvent( iRespondToEvent );
        return ETrue;
        }
    
    return EFalse;    
    }

// ----------------------------------------------------------------------------
// CTestEventHandler::CTestEventHandler()
//
// ----------------------------------------------------------------------------
//
CTestEventHandler::CTestEventHandler()
    {    
    }

} // namespace T_AiWsPlugin

// End of file
