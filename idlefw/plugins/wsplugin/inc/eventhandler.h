/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Event handler base class for Active Idle WS Plug-in.
*
*/


#ifndef C_AIWSPLUGIN_EVENTHANDLER_H
#define C_AIWSPLUGIN_EVENTHANDLER_H


#include <e32base.h>

class TRawEvent;

namespace AiWsPlugin {

class MUiState;

/**
 *  @ingroup group_wsplugin
 *
 *  Event handler base class for Active Idle WS Plug-in.
 *
 *  Window server plug-in animation class CAiWsPluginAnim forwards events to 
 * instances of this class for processing.
 *
 *  @since S60 3.2
 */
class CEventHandler : public CBase
    {
public:
    /**
     * Sets the UI State query interface for this event handler.
     */
    virtual void SetUiStateQuery( MUiState& aUiState ) =0;

    /**
     * Calls to CWindowAnim::FocusChanged are forwarded to this function.
     *
     * @param Indicates whether the focus has or has not changed.
     */
    virtual void FocusChanged( TBool aState ) =0;
    
    /**
     * Calls to CWindowAnim::OfferRawEvent are forwarded to this function.
     * Forwarding is stopped if this function returns true.
     *
     * @param aRawEvent The raw event to be processed.
     * @return ETrue if the raw event is handled by this function, EFalse if 
     *         the function chooses not to process it. 
     */
    virtual TBool OfferRawEvent(const TRawEvent& aRawEvent) =0;
    };

} // namespace AiWsPlugin

#endif // C_AIWSPLUGIN_EVENTHANDLER_H
