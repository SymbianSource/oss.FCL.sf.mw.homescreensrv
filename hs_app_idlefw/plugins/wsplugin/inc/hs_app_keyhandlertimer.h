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
* Description:  Key handler timer for Active Idle WS Plug-in.
*
*/

#ifndef C_AIWSPLUGIN_KEYHANDLERTIMER_H
#define C_AIWSPLUGIN_KEYHANDLERTIMER_H

#include <e32base.h>

namespace AiWsPlugin {

/**
 * Handler timer interface.
 */
class MHandlerTimer
    {
public:
    virtual void TimerDone() = 0;
    };  

/**
 *  @ingroup group_wsplugin
 *
 *  Logs app launch handler for Active Idle WS Plug-in. 
 */
class CKeyHandlerTimer : 
        public CTimer
    {
public:

// Construction

    static CKeyHandlerTimer* NewL( MHandlerTimer* aHandler );
    
    static CKeyHandlerTimer* NewLC( MHandlerTimer* aHandler );
    
    ~CKeyHandlerTimer();

// new methods

    void StopTimer();

    TBool IsActive();

protected: // From CActive

    void RunL();

    TInt RunError( TInt aError );

private:

// Construction

    CKeyHandlerTimer( MHandlerTimer* aHandler );

    void ConstructL();

private: // data

    /// Pointer to handler timer. Not owned.
    MHandlerTimer* iHandler;
    }; 

} // namespace AiWsPlugin


#endif // C_AIWSPLUGIN_KEYHANDLERTIMER_H