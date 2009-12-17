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
* Description:  SIND launch handler for Active Idle WS Plug-in.
*
*/


#ifndef C_AIWSPLUGIN_SINDLAUNCHANDLER_H
#define C_AIWSPLUGIN_SINDLAUNCHANDLER_H

#include "eventhandler.h"
#include "keyhandlertimer.h"

#include <e32base.h>

namespace AiWsPlugin {

/**
 *  @ingroup group_wsplugin
 *
 *  Logs app launch handler for Active Idle WS Plug-in. 
 */
class CSINDLaunchHandler : 
        public CEventHandler,
        public MHandlerTimer        
    {
public:

// Construction

    static CSINDLaunchHandler* NewLC();
    
    ~CSINDLaunchHandler();

// from base class CEventHandler

    void SetUiStateQuery( MUiState& aUiState );
    
    void FocusChanged( TBool aState );
    
    TBool OfferRawEvent(const TRawEvent& aRawEvent);
    
// from MHandlerTimer

    void TimerDone();

private:

// Construction

    CSINDLaunchHandler();
    
    void ConstructL();
    
// New methods

    void SkipVoiceDial();

private: // data

    /// Pointer to state. Not owned.
    MUiState* iUiState;
    
    /// Timer for long key down event. Owned.
    CKeyHandlerTimer* iTimer;
    
    /// Flag for SIND key down
    TBool iSINDKeyDown;
    
    /// Flag for SIND launched
    TBool iSINDLaunched;
    };

} // namespace AiWsPlugin


#endif // C_AIWSPLUGIN_SINDLAUNCHANDLER_H