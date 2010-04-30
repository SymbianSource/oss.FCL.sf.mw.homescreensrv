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
#ifndef ACTIVITYOBSERVERTASK_H
#define ACTIVITYOBSERVERTASK_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "activitytask.h"
#include "activitytaskstorage.h"

/**
 *  CActivityObserverTask
 * 
 */
class CActivityObserverTask : public CActivityTask
{
public:
    /**
     * Destructor.
     */
    ~CActivityObserverTask();

    /**
     * Handle observer request. 
     * @param globalStorage - global observers storage
     * @param localStorage - local observers storage
     * @param msg - request message
     */
    static void ExecuteLD(MActivityTaskStorage& globalStorage, 
                          MActivityTaskStorage& localStorage,
                          const RMessage2& msg);

    /**
     * Interface implementation
     * @see CActivityTask::Data()
     */
    const TDesC8& Data() const;

    /**
     * Interface implementation
     * @see CActivityTask::BroadcastReceivedL(const RMessage2&)
     */
    void BroadcastReceivedL(const RMessage2& );

private:

    /**
     * Constructor for performing 1st stage construction
     * @param globalStorage - global observers storage
     * @param localStorage - local observers storage
     * @param msg - request message
     */
    CActivityObserverTask(MActivityTaskStorage& globalStorage, 
                          MActivityTaskStorage& localStorage,
                          const RMessage2& msg);
    
    /**
     * Write response data to requested message 
     */
    void WriteResponseL();

private:
    MActivityTaskStorage& mGlobalStorage;
    MActivityTaskStorage& mLocalStorage;
    const RMessage2 mMsg;
    RBuf8 mData;
};

#endif // ACTIVITYOBSERVERTASK_H
