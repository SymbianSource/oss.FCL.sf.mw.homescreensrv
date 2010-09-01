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
* Description:  Numeric key forwarding handler for Active Idle WS Plug-in.
*
*/


#ifndef _NUMERICKEYHANDLER_STUB_H
#define _NUMERICKEYHANDLER_STUB_H

#include "testeventhandler_stub.h"

class MAnimGeneralFunctionsWindowExtension;
class MAiPSPropertyObserver;

namespace AiWsPlugin {

/**
 * (STUB) Numeric key forwarding handler for Active Idle WS Plug-in. 
 */
class CNumericKeyHandler : 
        public T_AiWsPlugin::CTestEventHandler
    {
public:
    /**
     * Creates a new instance of this class.
     *
     * @param aTargetWgId Window Group id of the target application where 
     *                    numeric key events are forwarded.
     * @return A new object of this class. The returned object is left on the
     *         cleanup stack.
     */
    static CNumericKeyHandler* NewLC
        ( TInt aTargetWgId, MAnimGeneralFunctionsWindowExtension* aWindowExt );

    ~CNumericKeyHandler();

private:
    CNumericKeyHandler
        ( TInt aTargetWgId, MAnimGeneralFunctionsWindowExtension* aWindowExt );

public: // data
    TInt iTargetWgId;
    MAnimGeneralFunctionsWindowExtension* iWindowExt;
    };

} // namespace AiWsPlugin


#endif // _NUMERICKEYHANDLER_STUB_H
