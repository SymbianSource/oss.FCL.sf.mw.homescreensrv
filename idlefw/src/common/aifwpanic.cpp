/*
* Copyright (c) 2005-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Panic utility for AI2
*
*/


#include "aifwpanic.h"
#include <e32std.h>

#ifdef _DEBUG

void AiFwPanic::Panic(AiFwPanic::TAiPanicCodes aPanicCode)
    {
    _LIT(KPanicText, "AiFw");
    User::Panic( KPanicText, aPanicCode );
    
    
    }

#endif // _DEBUG
