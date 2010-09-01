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

#ifndef __MENU_SRV_BUF_INL__
#define __MENU_SRV_BUF_INL__

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// HMenuSrvBuf::HMenuSrvBuf()
// ---------------------------------------------------------
//
HMenuSrvBuf::HMenuSrvBuf(): iBuf( NULL )
    {
    }

// ---------------------------------------------------------
// HMenuSrvBuf::~HMenuSrvBuf()
// ---------------------------------------------------------
//
HMenuSrvBuf::~HMenuSrvBuf()
    {
    delete iBuf;
    }

#endif // __MENU_SRV_BUF_INL__

// End of File
