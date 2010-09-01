/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Content Harvester Server Session
 *
*/


// INCLUDE FILES
#include "contentharvestersession.h"
#include "contentharvesterengine.h"
#include "contentharvesterserver.h"
#include "contentharvesterglobals.h"
#include "cpdebug.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CContentHarvesterSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CContentHarvesterSession* CContentHarvesterSession::NewL(
    CContentHarvesterServer* aServer )
    {
    CContentHarvesterSession* self =
            CContentHarvesterSession::NewLC( aServer );
    CleanupStack::Pop( self ) ;
    return self;
    }

// -----------------------------------------------------------------------------
// CContentHarvesterSession::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CContentHarvesterSession* CContentHarvesterSession::NewLC(
    CContentHarvesterServer* aServer )
    {
    CContentHarvesterSession* self = new ( ELeave )
    CContentHarvesterSession(aServer);
    CleanupStack::PushL( self );
    self->ConstructL( ) ;
    return self;
    }

// -----------------------------------------------------------------------------
// CContentHarvesterSession::~CContentHarvesterSession
// Destructor.
// -----------------------------------------------------------------------------
//
CContentHarvesterSession::~CContentHarvesterSession()
    {

    }

// -----------------------------------------------------------------------------
// CContentHarvesterSession::ServiceL
// Handle client requests.
// -----------------------------------------------------------------------------
//
void CContentHarvesterSession::ServiceL( const RMessage2& aMessage )
    {
    CP_DEBUG(_L8("CContentHarvesterSession::ServiceL"));
    switch ( aMessage.Function( ) )
        {
        case EContentHarvesterUpdate:
            CP_DEBUG(_L8("Update command"));
            aMessage.Complete( EContentHarvesterUpdateComplete );
            iContentHarvesterServer->Engine().Update( );
            break;
        case EContentHarvesterStop:
            CP_DEBUG(_L8("Stop command"));
            aMessage.Complete( EContentHarvesterStopComplete );
            iContentHarvesterServer->Stop( );
            break;
        default:
            iContentHarvesterServer->PanicClient( aMessage,
                EContentHarvesterBadRequest );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CContentHarvesterSession::CContentHarvesterSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CContentHarvesterSession::CContentHarvesterSession(
    CContentHarvesterServer* aServer ) :
    CSession2()
    {
    iContentHarvesterServer = aServer;
    }

// -----------------------------------------------------------------------------
// CContentHarvesterSession::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CContentHarvesterSession::ConstructL()
    {
    CP_DEBUG(_L8("CContentHarvesterSession::ConstructL"));
    }

// End of File
