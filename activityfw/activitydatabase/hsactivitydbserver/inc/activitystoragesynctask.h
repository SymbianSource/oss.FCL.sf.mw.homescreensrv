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
#ifndef ACTIVITYSTORAGESYNCTASK_H
#define ACTIVITYSTORAGESYNCTASK_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "activitytask.h"
#include "activitystorage.h"

/**
 *  CActivityStorageSyncTask
 * 
 */
class ActivityStorageSyncTask
    {
public:
    /**
     * Handle synchronous data storage requests
     * @param dataStorage - data storage
     * @param msg - request message
     */
    static void ExecuteL(CActivityStorage& dataStorage, 
                         const RMessage2& msg);

private:
    /**
     * Handle adding new activity.
     * @param dataStorage - data storage
     * @param msg - request message
     */
    static void AddActivityL(CActivityStorage& dataStorage, 
                             const RMessage2& msg);

    /**
     * Handle updating existing activiy
     * @param dataStorage - data storage
     * @param msg - request message
     */
    static void UpdateActivityL(CActivityStorage& dataStorage, 
                                const RMessage2& msg);

    /**
     * Handle removing activity.
     * @param dataStorage - data storage
     * @param msg - request message
     */
    static void DeleteActivityL(CActivityStorage& dataStorage, 
                                const RMessage2& msg);

    /**
     * Handle removing all application activities.
     * @param dataStorage - data storage
     * @param msg - request message
     */
    static void DeleteApplicationActivitiesL(CActivityStorage& dataStorage, 
                                             const RMessage2& msg);

    };

#endif // ACTIVITYSTORAGESYNCTASK_H
