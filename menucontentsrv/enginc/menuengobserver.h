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

#ifndef __MENUENGOBSERVER_H__
#define __MENUENGOBSERVER_H__

#include <e32base.h>

/**
* Menu Engine Observer.
*/
NONSHARABLE_CLASS( MMenuEngObserver )
    {

public:     // new methods

    /**
    * Engine events.
    * @param aFolder Events occurred in this folder. (0==unspecified.)
    * @param aEvents Events. A combination of TEvent bits.
    */
    virtual void EngineEvents( TInt aFolder, TInt aEvents ) = 0;

    /**
    * Unrecoverable Engine error. The engine stopped working and is useless.
    * Delete the engine.
    * @param aErr Error code.
    */
    virtual void EngineError( TInt aErr ) = 0;
    /**
     * Engine tree reload events. 
     * Happens when loading tree from Ram failed
     * and was reloaded from Rom.
     */
    virtual void EngineTreeReloaded() = 0;

    };

#endif // __MENUENGOBSERVER_H__
