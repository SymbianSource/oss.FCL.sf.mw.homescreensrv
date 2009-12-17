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
* Description:  Panic helper function for Active Idle WS Plug-in.
*
*/


#include "panic.h"
#include <e32std.h>

namespace AiWsPlugin {


#ifndef NDEBUG

void Panic( TPanicCode aPanicCode )
    {
    _LIT( KPanicText, "AiWsPlugin" );
    User::Panic( KPanicText, aPanicCode );    
    }

#endif  // !NDEBUG

} // namespace AiWsPlugin
