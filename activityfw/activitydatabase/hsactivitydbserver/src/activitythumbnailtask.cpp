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

#include "activitythumbnailtask.h"
#include "activitycmd.h"

#include <fbs.h>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CActivityThumbnailTask::CActivityThumbnailTask(MActivityTaskStorage& storage, 
                                               const RMessage2 msg)
:
    mStorage(storage), 
    mMsg(msg)
{
    // No implementation required
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CActivityThumbnailTask::~CActivityThumbnailTask()
{
    delete mService;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityThumbnailTask::ExecuteLD(RFs& session,
                                       MActivityTaskStorage& taskStorage,
                                       const RMessage2& message)
{
    CActivityThumbnailTask *self = new (ELeave)CActivityThumbnailTask(taskStorage, 
                                                                      message);
    CleanupStack::PushL(self);
    self->ConstructL(session);
    taskStorage.PushL(self);
    CleanupStack::Pop(self);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityThumbnailTask::ConstructL(RFs& session)
{
    TPckgBuf<int> width(0), height(0);
    RBuf path;
    RBuf8 mime;
    CleanupClosePushL(path);
    CleanupClosePushL(mime);
    mMsg.ReadL(0, width);
    mMsg.ReadL(1, height);
    path.CreateL(mMsg.GetDesLengthL(2));
    mMsg.ReadL(2, path);
    mime.CreateL(mMsg.GetDesLengthL(3));
    mMsg.ReadL(3, mime);

    mService = CGraphicsSalingHandler::NewL(*this, 
                                            session, 
                                            path, 
                                            mime, 
                                            TSize(width(), height()), 
                                            CGraphicsSalingHandler::EKeepAspectRatio);
    
    CleanupStack::PopAndDestroy(&mime);
    CleanupStack::PopAndDestroy(&path);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityThumbnailTask::ImageReadyCallBack(TInt error,const CFbsBitmap *bitmap)
{
    if (EFalse == mMsg.IsNull() &&
        KErrNone == error) {
        mMsg.Write(0, TPckgBuf<int>(const_cast<CFbsBitmap*>(bitmap)->Handle()));
        mMsg.Write(1, TPckgBuf<void *>(this));
        mMsg.Complete(error);
    } else {
        if (EFalse == mMsg.IsNull()) {
         mMsg.Complete(error);
        }
        mStorage.Pop(this);
        delete this;
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const TDesC8& CActivityThumbnailTask::Data() const
{
    return KNullDesC8();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CActivityThumbnailTask::BroadcastReceivedL(const RMessage2&)
{
}

