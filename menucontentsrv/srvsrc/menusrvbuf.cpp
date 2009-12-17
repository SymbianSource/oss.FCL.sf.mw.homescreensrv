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

#include "menusrvbuf.h"

// CONSTANTS

/// Buffer granularity.
LOCAL_D const TInt KMenuSrvBufGranularity = 512;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// HMenuSrvBuf::NewL()
// ---------------------------------------------------------
//
HMenuSrvBuf* HMenuSrvBuf::NewL()
    {
    HMenuSrvBuf* buf = NewLC();
    CleanupStack::Pop( buf );
    return buf;
    }

// ---------------------------------------------------------
// HMenuSrvBuf::NewLC()
// ---------------------------------------------------------
//
HMenuSrvBuf* HMenuSrvBuf::NewLC()
    {
    HMenuSrvBuf* buf = new (ELeave) HMenuSrvBuf;
    buf->PushL();
    buf->ConstructL();
    return buf;
    }

// ---------------------------------------------------------
// HMenuSrvBuf::ConstructL()
// ---------------------------------------------------------
//
void HMenuSrvBuf::ConstructL()
    {
    iBuf = CBufSeg::NewL( KMenuSrvBufGranularity );
    Set( *iBuf, 0, ERead | EWrite );
    }

// ---------------------------------------------------------
// HMenuSrvBuf::DoRelease()
// ---------------------------------------------------------
//
void HMenuSrvBuf::DoRelease()
    {
    delete this;
    }

//  End of File  
