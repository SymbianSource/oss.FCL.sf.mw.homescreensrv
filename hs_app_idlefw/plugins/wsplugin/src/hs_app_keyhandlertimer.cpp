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
* Description:  Key handler timer for Active Idle WS Plug-in.
*
*/


#include <e32base.h>

#include "hs_app_keyhandlertimer.h"
#include "hs_app_sindlaunchhandler.h"

namespace AiWsPlugin {

CKeyHandlerTimer::CKeyHandlerTimer( MHandlerTimer* aHandler )
 :  CTimer( CTimer::EPriorityStandard )
    {
    iHandler = aHandler;
    }
    
void CKeyHandlerTimer::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );        
    }

CKeyHandlerTimer* CKeyHandlerTimer::NewL( MHandlerTimer* aHandler )
    {
    CKeyHandlerTimer* self = CKeyHandlerTimer::NewLC( aHandler );
    CleanupStack::Pop( self );
    return self;
    }


CKeyHandlerTimer* CKeyHandlerTimer::NewLC( MHandlerTimer* aHandler )
    {
    CKeyHandlerTimer* self = new(ELeave) CKeyHandlerTimer( aHandler );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
CKeyHandlerTimer::~CKeyHandlerTimer()
    {
    Cancel();
    }

void CKeyHandlerTimer::RunL()
    {
    if( iHandler )
        {
        iHandler->TimerDone();
        }
    }
    
TInt CKeyHandlerTimer::RunError( TInt /*aError*/ )
    {
    return 0;
    }

TBool CKeyHandlerTimer::IsActive()
	{
	return CActive::IsActive();
	}

} // namespace AiWsPlugin
