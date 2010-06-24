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
#include <s32mem.h>
#include <fbs.h>
#include <bautils.h>

#include "afstoragesynctask.h"
#include "afcmd.h"
#include "afentry.h"

_LIT(KUnsupportedStorageSyncTask, "Unsupported sync storage task");

// -----------------------------------------------------------------------------
/**
 * Handle synchronous data storage requests
 * @param dataStorage - data storage
 * @param msg - request message
 */
void AfStorageSyncTask::ExecuteL(MAfTaskStorage& observers,
                                 CAfStorage& dataStorage, 
                                 const RMessage2& msg)
{
    switch (msg.Function()) {
    case AddActivity: 
        AddActivityL(dataStorage, msg);
        break;
    case UpdateActivity:
        UpdateActivityL(dataStorage, msg);
        break;
    case RemoveActivity:
        DeleteActivityL(dataStorage, msg);
        break;
    case RemoveApplicationActivities:
        DeleteApplicationActivitiesL(dataStorage, msg);
        break;
    default:
        //this code shouldn't be called. fatal error: means wrong session implementation 
        User::Panic(KUnsupportedStorageSyncTask, KErrGeneral);
    };
    msg.Complete(KErrNone);
    NotifyChangeL(observers, msg);
}

// -----------------------------------------------------------------------------
/**
 * Handle adding new activity.
 * @param dataStorage - data storage
 * @param msg - request message
 */
void AfStorageSyncTask::AddActivityL(CAfStorage& dataStorage, 
                                     const RMessage2& msg)
{
    //Read message and bitmap handle
    TPckgBuf<TInt> bitmapHdl(0);
    CAfEntry *entry = CAfEntry::NewLC(msg);
    msg.ReadL(1, bitmapHdl);
    
    RBuf thumbnailPath;
    CleanupClosePushL(thumbnailPath);
    dataStorage.ThumbnailPathL(thumbnailPath, 
                   dataStorage.Fs(), 
                   entry->ApplicationId(), 
                   entry->ActivityId(),
                   entry->Flags() & CAfEntry::Persistent);
    CreateThumbnailL(thumbnailPath, bitmapHdl());
    entry->SetImageSrcL(thumbnailPath);
    dataStorage.AddActivityL(*entry);
    CleanupStack::PopAndDestroy(&thumbnailPath);
    
    CleanupStack::PopAndDestroy(entry);
}

// -----------------------------------------------------------------------------
/**
 * Handle updating existing activiy
 * @param dataStorage - data storage
 * @param msg - request message
 */
void AfStorageSyncTask::UpdateActivityL(CAfStorage& dataStorage, 
                                        const RMessage2& msg)
{
    TPckgBuf<TInt> bitmapHdl(0);
    CAfEntry *entry = CAfEntry::NewLC(msg);
    msg.ReadL(1, bitmapHdl);
        
    RBuf thumbnailPath;
    CleanupClosePushL(thumbnailPath);
    DeleteActivityScreenshotL(dataStorage, 
                    entry->ApplicationId(), 
                    entry->ActivityId());
    
    dataStorage.ThumbnailPathL(thumbnailPath, 
                   dataStorage.Fs(), 
                   entry->ApplicationId(), 
                   entry->ActivityId(),
                   entry->Flags() & CAfEntry::Persistent);
    CreateThumbnailL(thumbnailPath, bitmapHdl());
    entry->SetImageSrcL(thumbnailPath);
    dataStorage.UpdateActivityL(*entry);
    CleanupStack::PopAndDestroy(&thumbnailPath);
    CleanupStack::PopAndDestroy(entry);
}

// -----------------------------------------------------------------------------
/**
 * Handle removing activity.
 * @param dataStorage - data storage
 * @param msg - request message
 */
void AfStorageSyncTask::DeleteActivityL(CAfStorage& dataStorage, 
                                        const RMessage2& msg)
{
    CAfEntry *entry = CAfEntry::NewLC(msg);
    dataStorage.DeleteActivityL(*entry);
    DeleteActivityScreenshotL(dataStorage, 
                        entry->ApplicationId(), 
                        entry->ActivityId());
    CleanupStack::PopAndDestroy(entry);
}

// -----------------------------------------------------------------------------
/**
 * Handle removing activity.
 * @param dataStorage - data storage
 * @param msg - request message
 */
void AfStorageSyncTask::DeleteActivityScreenshotL(CAfStorage& dataStorage,
                                    TInt uid, 
                                    const TDesC &activityName)
{
    RBuf thumbnailPath;
    CleanupClosePushL(thumbnailPath);
    dataStorage.ThumbnailPathL(thumbnailPath, 
                   dataStorage.Fs(), 
                   uid, 
                   activityName,
                   TRUE);
    dataStorage.Fs().Delete(thumbnailPath);
    thumbnailPath.Zero();
    dataStorage.ThumbnailPathL(thumbnailPath, 
                       dataStorage.Fs(), 
                       uid, 
                       activityName,
                       FALSE);
    dataStorage.Fs().Delete(thumbnailPath);
    thumbnailPath.Zero(); 
    CleanupStack::PopAndDestroy(&thumbnailPath);
}

// -----------------------------------------------------------------------------
/**
 * Handle removing all application activities.
 * @param dataStorage - data storage
 * @param msg - request message
 */
void AfStorageSyncTask::DeleteApplicationActivitiesL(CAfStorage& dataStorage, 
                                                     const RMessage2& msg)
{
    CAfEntry *entry = CAfEntry::NewLC(msg);
    dataStorage.DeleteActivitiesL(*entry);
    CleanupStack::PopAndDestroy(entry);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void AfStorageSyncTask::NotifyChangeL(MAfTaskStorage& observers,
                                      const RMessage2& msg)
{
    const RPointerArray<CAfTask> &table(observers.StorageData());
    for (TInt iter(table.Count() - 1); 0 <= iter; --iter) {
        table[iter]->BroadcastReceivedL(msg);
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void AfStorageSyncTask::CreateThumbnailL(const TDesC &path, TInt hdl)
{
    if (0 >= hdl) {
        User::Leave(KErrCorrupt);
    }
    CFbsBitmap *bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    User::LeaveIfError(bitmap->Duplicate(hdl));
    User::LeaveIfError(bitmap->Save(path));
    CleanupStack::PopAndDestroy(bitmap);
}
