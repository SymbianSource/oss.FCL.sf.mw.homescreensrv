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
#ifndef ACTIVITYTASKSTORAGE_H
#define ACTIVITYTASKSTORAGE_H
#include <e32base.h>
class CActivityTask;
class MActivityTaskStorage
{
public:
    /**
     * Register new task instance in storage. Ownership is transfered to storage.
     * @param task - instance that need to be registered
     */
    virtual void PushL(CActivityTask * task)=0;
    
    /**
     * Unregister task instance from storage. Ownership is transfered to caller.
     * @param task - instance that need to be unregistered
     */
    virtual void Pop(CActivityTask *)=0;
    
    /**
     * List of registered tasks
     */
    virtual const RPointerArray<CActivityTask>& StorageData() const =0;

    /**
     * Removes not valid task
     */
    virtual void RemoveNotValidTasks(const CSession2* session) =0;
};

#endif  //ACTIVITYTASKSTORAGE_H

