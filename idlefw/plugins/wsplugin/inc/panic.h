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
* Description:  Panic codes and helper function for Active Idle WS Plug-in.
*
*/


#ifndef AIWSPLUGIN_PANIC_H
#define AIWSPLUGIN_PANIC_H

#include "panic.h" // Include AI2 panic.h

namespace AiWsPlugin {

#ifndef NDEBUG

/**
 * Panic codes for Active Idle WS Plug-in.
 */
enum TPanicCode
    {
    /**
     * Key lock state machine received an invalid event.
     */
    EPanicInvalidKeylockEvent = 1
    };

void Panic( TPanicCode aPanicCode );

#endif  // !NDEBUG

} // namespace AiWsPlugin 

#endif // AIWSPLUGIN_PANIC_H
