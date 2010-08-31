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
* Description: ccarunningappmonitor.h
*
*/
#ifndef C_CARUNNINGTASKHANDLER_H
#define C_CARUNNINGTASKHANDLER_H

#include "carunningtaskhandler.h"
#include <e32base.h>
#include <w32std.h>
#include <e32hashtab.h>

class CApaWindowGroupName;
class CCaStorageProxy;
NONSHARABLE_CLASS( CCaRunningAppMonitor ): public CActive
    {
public:
    /**
     * Function create monitor instance and initialize observation process.
     * Monitor instance will be registered on task handler.
     * @param aTaskHandler - task handler instance
     * @param aStorage - content storage proxy instance
     */
    static void ExecuteL( MCaRunningTaskHandler& aTaskHandler, 
                          CCaStorageProxy& aStorage );
    
    /**
     * Destructor
     */
    ~CCaRunningAppMonitor();

private:
    /**
     * First step constructor.
     */
    CCaRunningAppMonitor( MCaRunningTaskHandler& aTaskHandler,
                          CCaStorageProxy& aStorage );
    
    /**
     * Second step constructor
     */
    void ConstructL();
    
    /**
     * Function update content storage with running applications list
     */
    void UpdateRunningAppStorageL();
    
    /**
     * Function subscribe monitor to window server events
     */
    void Subscribe();
    
    /**
     * Function create window group to observe window server events
     */
    void ConstractObserverL();
    
    /**
     * Function mark all storage entries as "not running"
     */
    void ResetStorageL();

private: //from CActive
    /**
     * @see CActive
     */
    void RunL();
    
    /**
     * @see CActive
     */
    TInt RunError( TInt aError );
    
    /**
     * @see CActive
     */
    void DoCancel();

private:
    //members
    
    /**
     * Reference to task handler.
     */
    MCaRunningTaskHandler& iTaskHandler;
    
    /**
     * Reference to constent storage proxy 
     */
    CCaStorageProxy& iStorage;
    
    /**
     * IPC session to window server
     */
    RWsSession iSession;
    
    /**
     * Window group.
     */
    RWindowGroup iWg;
    
    /**
     * Running apps hash map
     * key - application window group id
     * value - application uid
     */
    RHashMap<TInt, TUid> iRunningTasks;
    };

#endif //C_CARUNNINGTASKHANDLER_H
