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
* Description:  Stub Numeric key forwarding handler implementation for Active Idle 
*                WS Plug-in.
*
*/


// System includes

// User includes
#include "numerickeyhandler_stub.h"

#include "ut_aiwspluginanimtls.h"

namespace AiWsPlugin {

// Constants

// ======== LOCAL FUNCTIONS ========
    
// ======== MEMBER FUNCTIONS =======
// ----------------------------------------------------------------------------
// CNumericKeyHandler::CNumericKeyHandler()
//
// ----------------------------------------------------------------------------
//
CNumericKeyHandler::CNumericKeyHandler( TInt aTargetWgId, 
    MAnimGeneralFunctionsWindowExtension* aWindowExt ) :
    iTargetWgId( aTargetWgId ), iWindowExt( aWindowExt )     
    {
    }

// ----------------------------------------------------------------------------
// CNumericKeyHandler::NewLC()
//
// ----------------------------------------------------------------------------
//        
CNumericKeyHandler* CNumericKeyHandler::NewLC( TInt aTargetWgId, 
    MAnimGeneralFunctionsWindowExtension* aWindowExt )
    {
    CNumericKeyHandler* self = 
        new ( ELeave ) CNumericKeyHandler( aTargetWgId, aWindowExt );
           
    CleanupStack::PushL( self );
    
    UT_AiWsPluginAnimTls::Instance()->AddEventHandlerL( *self );
    
    return self;
    }

// ----------------------------------------------------------------------------
// CNumericKeyHandler::~CNumericKeyHandler()
//
// ----------------------------------------------------------------------------
//        
CNumericKeyHandler::~CNumericKeyHandler()
    {
    }

} // namespace AiWsPlugin

// End of file
