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

#include "mcsmenusapi.h"
#include "menumsg.h"




// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RMenu::GetListL
// ---------------------------------------------------------
//
EXPORT_C void RMenuSapi::GetListL( TDes8& aSerializedIn,
		RBuf8& aSerializedOut )
	{
    TInt sizeList(0);
    TPckg<TInt> sizeDes(sizeList);
    TIpcArgs args1( &aSerializedIn, &sizeDes );
    User::LeaveIfError( SendReceive( EMenuGetListSize, args1 ) );
    aSerializedOut.CreateL( sizeList );
    TIpcArgs args2( &aSerializedOut );
    User::LeaveIfError( SendReceive( EMenuGetListData, args2 ) );
	}
