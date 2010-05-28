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
#ifndef ACTIVITYTHUMBNAILTASK_H
#define ACTIVITYTHUMBNAILTASK_H

#ifndef __E32SVR_H__
#define __E32SVR_H__
#endif

#include <e32std.h>
#include <e32base.h>
#include <f32file.h>

#include "activitytask.h"
#include "activitytaskstorage.h"
#include "tsgraphicfilescalinghandler.h"

class CFbsBitmap;

/**
 *  CActivityThumbnailTask
 * 
 */
class CActivityThumbnailTask : public CActivityTask,
                               public MImageReadyCallBack
    {
public:
    /**
     * Destructor.
     */
    ~CActivityThumbnailTask();

    static void ExecuteLD(RFs& session,
                          MActivityTaskStorage& taskStorage,
                          const RMessage2& message);

private:
    /**
     * Constructor for performing 1st stage construction
     */
    CActivityThumbnailTask(MActivityTaskStorage& storage, const RMessage2 msg);

    /**
     * EPOC default constructor for performing 2nd stage construction
     */
    void ConstructL(RFs& session);

    /**
     * 
     */
    void ImageReadyCallBack(TInt error, const CFbsBitmap *bitmap );

    const TDesC8& Data()const;

    void BroadcastReceivedL(const RMessage2& );

private:
    MActivityTaskStorage& mStorage; 
    const RMessage2 mMsg;
    CBase* mService;
    };

#endif // ACTIVITYTHUMBNAILTASK_H
