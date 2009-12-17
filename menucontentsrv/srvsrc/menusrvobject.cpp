/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

// INCLUDE FILES

#include "menusrvobject.h"
#include "menusrvdef.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuSrvObject::SetPending
// ---------------------------------------------------------
//
void CMenuSrvObject::SetPending( const RMessage2 &aMessage )
    {
    __ASSERT_DEBUG( !iPending, User::Invariant() );
    iMessage = aMessage;
    iPending = ETrue;
    }

// ---------------------------------------------------------
// CMenuSrvObject::PanicIfPendingL
// ---------------------------------------------------------
//
void CMenuSrvObject::PanicIfPendingL( const RMessage2& aMessage )
    {
    if ( iPending )
        {
        // Another message is already being served.
        aMessage.Panic( KMenuSrvName, KErrInUse );
        User::Leave( KErrInUse );
        }
    }

// ---------------------------------------------------------
// CMenuSrvObject::Complete()
// ---------------------------------------------------------
//
void CMenuSrvObject::Complete( TInt aReason )
    {
    if ( iPending )
        {
        iMessage.Complete( aReason );
        iPending = EFalse;
        }
    }

//  End of File  
