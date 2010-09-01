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

#include "menusrvstream.h"
#include "menusrvdef.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuSrvStream::ReadL()
// ---------------------------------------------------------
//
void CMenuSrvStream::ReadL( const RMessage2& aMessage, TInt& aLength )
    {
    TUint8 buf[KMenuStreamBufSize];
    TInt len = iHost.ReadL( buf, KMenuStreamBufSize );
    if ( len )
        {
        aMessage.WriteL( 0, TPtrC8( buf, len ) );
        }
    aLength = len;
    }

// ---------------------------------------------------------
// CMenuSrvStream::WriteL()
// ---------------------------------------------------------
//
void CMenuSrvStream::WriteL( const RMessage2& aMessage )
    {
    TBuf8<KMenuStreamBufSize> buf;
    aMessage.ReadL( 0, buf );
    iHost.WriteL( buf.Ptr(), buf.Size() );
    }

//  End of File  
