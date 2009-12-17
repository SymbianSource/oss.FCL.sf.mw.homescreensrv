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
* Description:  Modifier key status tracker for Active Idle WS Plug-in.
*
*/


#ifndef T_AIWSPLUGIN_MODIFIERKEYTRACKER_H
#define T_AIWSPLUGIN_MODIFIERKEYTRACKER_H

#include <e32def.h>

class TRawEvent;

namespace AiWsPlugin {

/**
 *  @ingroup group_wsplugin
 *
 * Numeric key forwarding handler for Active Idle WS Plug-in. 
 *
 * Monitors and forwards numeric key presses that should be handled by Phone 
 * application.
 *
 *  @since S60 3.2
 */
class TModifierKeyTracker
    {
public:
    TModifierKeyTracker();

    /**
     * Returns true if modifier key states were updated.
     */
    TBool Update(const TRawEvent& aRawEvent);
    
    /**
     * Returns the current modifier key status as bits, see enum TEventModifier.
     */
    TUint Status() const { return iModifiers; }

private: // data
    TUint iModifiers;
    };

} // namespace AiWsPlugin


#endif // T_AIWSPLUGIN_MODIFIERKEYTRACKER_H
