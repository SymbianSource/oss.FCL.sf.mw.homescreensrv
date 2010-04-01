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
* Description:  Keylock handler states for Active Idle WS Plug-in.
*
*/


#ifndef T_AIWSPLUGIN_KEYLOCKSTATES_H
#define T_AIWSPLUGIN_KEYLOCKSTATES_H

#include <e32def.h>
class TRawEvent;

namespace AiWsPlugin {

class MKeyLockHandlerControl;

/**
 *  @ingroup group_wsplugin
 *
 * Internal state structure for class CKeyLockHandler.
 * The states are behaviour-only and thus consist of just function pointers.
 */
class TKeylockState
    {
public:
    /**
     * Returns the state machine's initial startup state.
     */
    static const TKeylockState& StartupState();

    /**
     * Returns this state's default succeeding state.
     */
    inline const TKeylockState& DefaultNextState() const
        { return (*iDefaultNextState)(); }

    /**
     * Handles Active Idle focus changes.
     *
     * @param aControl Key lock control interface.
     * @param aState   current focus state. See CWindowAnim::FocusChaned.
     */
    inline void FocusChanged( MKeyLockHandlerControl& aControl, TBool aState ) const
        { (*iFocusChanged)( aControl, aState ); }
        
    /**
     * Handles raw Window Server events.
     *
     * @param aControl  Key lock control interface.
     * @param aRawEvent The event to handle. See MEventHandler::OfferRawEvent.
     * @return true if the event was consumed by the state, false otherwise.
     */
    inline TBool OfferRawEvent
            (  MKeyLockHandlerControl& aControl, const TRawEvent& aRawEvent ) const
        { return (*iOfferRawEvent)( aControl, aRawEvent ); }

    /**
     * Handles key lock timeout timer elapsed event.
     *
     * @param aControl  Key lock control interface.
     */
    inline void TimerElapsed( MKeyLockHandlerControl& aControl ) const
        { (*iTimerElapsed)( aControl ); }

// Public function pointers

    /**
     * Pointer to state function that returns this state's default succeeding state.
     */
    const TKeylockState& (*iDefaultNextState)();
    
    /**
     * Pointer to state function which handles Active Idle focus changes.
     *
     * @param aControl Key lock control interface.
     * @param aState   current focus state. See CWindowAnim::FocusChaned.
     */
    void  (*iFocusChanged)( MKeyLockHandlerControl& aControl, TBool aState );

    /**
     * Pointer to state function which handles raw Window Server events.
     *
     * @param aControl  Key lock control interface.
     * @param aRawEvent The event to handle. See MEventHandler::OfferRawEvent.
     * @return true if the event was consumed by the state, false otherwise.
     */
    TBool (*iOfferRawEvent)( MKeyLockHandlerControl& aControl, const TRawEvent& aRawEvent );
    
    /**
     * Pointer to state function which handles key lock timeout timer elapsed 
     * event.
     *
     * @param aControl  Key lock control interface.
     */
    void  (*iTimerElapsed)( MKeyLockHandlerControl& aControl );
    };

} // namespace AiWsPlugin

#endif // T_AIWSPLUGIN_KEYLOCKSTATES_H

