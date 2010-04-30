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
#ifndef ACTIVITYDATAPROVIDERTASK_H
#define ACTIVITYDATAPROVIDERTASK_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

#include "activitytaskstorage.h"
#include "activitytask.h"

/**
 *  CActivityDataProviderTask
 * 
 */
class ActivityDataProviderTask
{
public:
    /**
     * Handle data request. Deliver data stored in other task.
     * @param storage - data tasks storage
     * @param msg - request message
     */
    static void ExecuteL(MActivityTaskStorage& storage, const RMessage2& msg);

private:
    /**
     * Write response data to request message
     * @param msg - destination message
     * @param src - data source
     */
    static void ProvideDataL(const RMessage2& msg, const CActivityTask& src);

};

#endif // ACTIVITYDATAPROVIDERTASK_H
