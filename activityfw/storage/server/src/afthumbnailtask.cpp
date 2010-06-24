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

#include "afthumbnailtask.h"
#include "afcmd.h"

#include <fbs.h>

// -----------------------------------------------------------------------------
/**
 * Constructor for performing 1st stage construction
 */
CAfThumbnailTask::CAfThumbnailTask(MAfTaskStorage& storage, 
                                   const RMessage2 msg)
:
    mStorage(storage), 
    mMsg(msg)
{
    // No implementation required
}

// -----------------------------------------------------------------------------
/**
 * Destructor.
 */
CAfThumbnailTask::~CAfThumbnailTask()
{
    delete mService;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CAfThumbnailTask::ExecuteLD(MAfTaskStorage& taskStorage,
                                 const RMessage2& message)
{
    CAfThumbnailTask *self = new (ELeave)CAfThumbnailTask(taskStorage, 
                                                                      message);
    CleanupStack::PushL(self);
    self->ConstructL();
    taskStorage.PushL(self);
    CleanupStack::Pop(self);
}

// -----------------------------------------------------------------------------
/**
 * EPOC default constructor for performing 2nd stage construction
 */
void CAfThumbnailTask::ConstructL()
{
    TPckgBuf<int> width(0), height(0);
    RBuf path;
    CleanupClosePushL(path);
    mMsg.ReadL(0, width);
    mMsg.ReadL(1, height);
    path.CreateL(mMsg.GetDesLengthL(2));
    mMsg.ReadL(2, path);
    
    CFbsBitmap *bitmap = new (ELeave) CFbsBitmap();
    CleanupStack::PushL(bitmap);
    User::LeaveIfError(bitmap->Load(path));
    mService = CTsGraphicFileScalingHandler::NewL(*this, 
                                                  *bitmap, 
                                                  TSize(width(), height()), 
                                                  CTsGraphicFileScalingHandler::EKeepAspectRatioByExpanding);
    CleanupStack::PopAndDestroy(bitmap);
    CleanupStack::PopAndDestroy(&path);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CAfThumbnailTask::ImageReadyCallBack(TInt error,const CFbsBitmap *bitmap)
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
const TDesC8& CAfThumbnailTask::Data() const
{
    return KNullDesC8();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CAfThumbnailTask::BroadcastReceivedL(const RMessage2&)
{
}

// -----------------------------------------------------------------------------
/**
 * Returns ETrue if task is related with session argument
 */
TBool CAfThumbnailTask::IsSessionTask(const CSession2* session)
{
    return mMsg.Session() == session ? ETrue : EFalse;
}
