/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Plugin Interface
 *
*/

#ifndef C_ACTION_HANDLER_PLUGIN_H
#define C_ACTION_HANDLER_PLUGIN_H

#include <e32base.h>

class CLiwMap;

/**
 *  Action handler interface for ECOM plugins
 */
class CAHPlugin : public CBase
    {

public:
    
    /** 
     * Method called by Action Handler in order to handle an action.
     * Must be implemented by plugin provider.
     * 
     * @param aMap an action object to be executed
     * @return Error code
     */
    virtual TInt ExecuteActionL( const CLiwMap* aMap ) = 0;

    };

#endif // C_ACTION_HANDLER_PLUGIN_H
