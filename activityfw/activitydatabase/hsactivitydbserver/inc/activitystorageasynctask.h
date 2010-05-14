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
#ifndef ACTIVITYSTORAGEASYNCTASK_H
#define ACTIVITYSTORAGEASYNCTASK_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "activitytask.h"
#include "activitytaskstorage.h"
#include "activitystorage.h"

/**
 *  CActivityStorageAsyncTask
 * 
 */
class CActivityStorageAsyncTask : public CActivityTask
{
public:
    // Constructors and destructor

    /**
     * Destructor.
     */
    ~CActivityStorageAsyncTask();

    /**
     * Handle asynchronous data storage requests
     * @param taskStorage - data tasks storage
     * @param dataStorage - data storage
     * @param msg - request message
     */
    static void ExecuteLD(MActivityTaskStorage& taskStorage, 
                          CActivityStorage& dataStorage, 
                          const RMessage2& msg);

    /**
     * Interface implementation
     * @see CActivityTask::Data()
     */
    const TDesC8& Data() const;

    /**
     * Interface implementation
     * @see CActivityTask::BroadcastReceivedL(const RMessage2 &)
     */
    void BroadcastReceivedL(const RMessage2 &);

private:

    
    /**
     * Constructor for performing 1st stage construction
     */
    CActivityStorageAsyncTask();

    /**
     * Handle asynchronous data storage requests
     * @param dataStorage - data storage
     * @param msg - request message
     */
    void ExecuteL(CActivityStorage& dataStorage, const RMessage2& msg);
    
    /**
     * Handle getting all activities request
     * @param dataStorage - data storage
     * @param msg - requested message
     */
    void AllActivitiesL(CActivityStorage& dataStorage, const RMessage2& msg);

    /**
     * Handle getting application activities request
     * @param dataStorage - data storage
     * @param msg - requested message
     */
    void ApplicationActivitiesL(CActivityStorage& dataStorage, const RMessage2& msg);

    /**
     * Write response data into request message
     * @param msg - destination message
     */
    void WriteResponseL(const RMessage2& msg);

private:
    RBuf8 mData;
};

#endif // ACTIVITYSTORAGEASYNCTASK_H
