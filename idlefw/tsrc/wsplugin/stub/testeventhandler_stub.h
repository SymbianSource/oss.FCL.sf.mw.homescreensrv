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

// testeventhandler.h

#ifndef _TESTEVENTHANDLER_STUB_H
#define _TESTEVENTHANDLER_STUB_H

#include "eventhandler.h"
#include <e32event.h>

namespace T_AiWsPlugin {

using AiWsPlugin::MUiState;

/**
 * Common test event handler base for AiWsPlugin unit tests. 
 */
class CTestEventHandler : 
        public AiWsPlugin::CEventHandler
    {
public:
    ~CTestEventHandler();

public:  // Testing functions
    MUiState* UiState() const          { return iUiState; }
    TBool Focus() const                { return iFocus;   }
    const TRawEvent& LastEvent() const { return iLastEvent; }
    void ResetLastEvent();
    void RespondToEvent( const TRawEvent& aEvent );

private:
// from base class CEventHandler
    void SetUiStateQuery( MUiState& aUiState );
    void FocusChanged( TBool aState );
    TBool OfferRawEvent(const TRawEvent& aRawEvent);

protected:
    CTestEventHandler();

private: // data
    AiWsPlugin::MUiState* iUiState;
    TBool iFocus;
    TRawEvent iLastEvent;
    TRawEvent iRespondToEvent;
    };

} // namespace T_AiWsPlugin


#endif // _TESTEVENTHANDLER_STUB_H

