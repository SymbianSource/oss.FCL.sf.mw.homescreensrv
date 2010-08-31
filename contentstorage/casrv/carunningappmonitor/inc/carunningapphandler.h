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
* Description: carunnningapphandler.h
*
*/

#ifndef C_CARUNNINGAPPHANDLER_H
#define C_CARUNNINGAPPHANDLER_H

#include "carunningtaskhandler.h"
#include "casrvplugin.h"
#include <e32cmn.h>

class CCaStorageProxy;
/**
 * Class implement task handler and content storage plugin. 
 */
NONSHARABLE_CLASS( CCaRunningAppHandler ): public CCaSrvPlugin,
                                           public MCaRunningTaskHandler
    {
public:
    /**
     * Factory function. Create and initialize handler instance. 
     */
    static CCaRunningAppHandler* NewL( TPluginParams* aParams );
    
    /**
     * Destructor
     */
    ~CCaRunningAppHandler();

private:
    /**
     * First step construction.
     */
    CCaRunningAppHandler();
    
    /**
     * Second step construction. 
     */
    void ConstructL( CCaStorageProxy& aStorage );

private: //form MCaRunningTaskHandler
    /**
     * @see MCaRunningTaskHandler
     */
    void RegisterTaskL( CBase* aTask );
    
    /**
     * @see MCaRunningTaskHandler
     */
    void UnregisterTask( CBase* aTask );

private:
    /**
     * List of running tasks.
     * Own.
     */
    RPointerArray<CBase> iRunningTasks;
    };
#endif //C_CARUNNINGAPPHANDLER_H
