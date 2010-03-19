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
* Description: carunningtaskhandler.h
*
*/

#ifndef M_CARUNNINGTASKHANDLER_H
#define M_CARUNNINGTASKHANDLER_H

#include <e32base.h>

/**
 * Running task handler.
 * Interface declaration.
 */
NONSHARABLE_CLASS( MCaRunningTaskHandler )
    {
public:
    /**
     * Function register running task on handler.
     * Ownership is transfered to handler
     * @param aTask - running task
     */
    virtual void RegisterTaskL( CBase* aTask )=0;
    
    /**
     * Function unregister running task on handler.
     * Ownership is transfered to caller.
     * @param aTask - running task
     */
    virtual void UnregisterTask( CBase* aTask )=0;
    };

#endif //M_CARUNNINGTASKHANDLER_H

