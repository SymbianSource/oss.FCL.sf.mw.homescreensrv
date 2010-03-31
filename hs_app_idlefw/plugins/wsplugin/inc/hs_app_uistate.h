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
* Description:  UI State query interface for Active Idle WS Plug-in.
*
*/


#ifndef M_AIWSPLUGIN_UISTATE_H
#define M_AIWSPLUGIN_UISTATE_H


#include <e32def.h>

namespace AiWsPlugin {

/**
 *  @ingroup group_wsplugin
 *
 * UI State query interface for Active Idle WS Plug-in
 */
class MUiState
    {
public:
    /**
     * Returns the current modifier key status.
     * @see TEventModifier
     */
    virtual TUint Modifiers() const =0;

    /**
     * Returns true if Active Idle currently has focus.
     */
    virtual TBool HasFocus() const =0;
    
protected:
    /**
     * Protected destructor to prevent deletion through this interface.
     */
    ~MUiState() { }
    };

} // namespace AiWsPlugin

#endif // M_AIWSPLUGIN_UISTATE_H
