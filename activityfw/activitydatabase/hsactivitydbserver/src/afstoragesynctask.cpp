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
#include <hash.h>
#include <fbs.h>
#include <bautils.h>

#include "afstoragesynctask.h"
#include "activitycmd.h"
#include "afentry.h"
const TInt KMaxPathLength = 256;

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
    CAfEntry *entry = CAfEntry::NewLC();
    ReadEntryL(*entry, msg);
    msg.ReadL(1, bitmapHdl);
    
    RBuf thumbnailPath;
    CleanupClosePushL(thumbnailPath);
    ThumbnailPathL(thumbnailPath, 
                   dataStorage.Fs(), 
                   entry->ApplicationId(), 
                   entry->ActivityId());
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
    CAfEntry *entry = CAfEntry::NewLC();
    ReadEntryL(*entry, msg);
    msg.ReadL(1, bitmapHdl);
        
    RBuf thumbnailPath;
    CleanupClosePushL(thumbnailPath);
    ThumbnailPathL(thumbnailPath, 
                   dataStorage.Fs(), 
                   entry->ApplicationId(), 
                   entry->ActivityId());
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
    CAfEntry *entry = CAfEntry::NewLC();
    ReadEntryL(*entry, msg);
    dataStorage.DeleteActivityL(*entry);
    CleanupStack::PopAndDestroy(entry);
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
    CAfEntry *entry = CAfEntry::NewLC();
    ReadEntryL(*entry, msg);
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
void AfStorageSyncTask::ReadEntryL(CAfEntry& entry, const RMessage2& msg)
{
    RBuf8 serializedEntry;
    CleanupClosePushL(serializedEntry);
    serializedEntry.CreateL(msg.GetDesLengthL(0));
    msg.ReadL(0, serializedEntry);
    RDesReadStream reader(serializedEntry);
    CleanupClosePushL(reader);
    
    reader >> entry;
    
    CleanupStack::PopAndDestroy(&reader);
    CleanupStack::PopAndDestroy(&serializedEntry);
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void AfStorageSyncTask::ThumbnailPathL(RBuf &dst, 
                                       RFs& fileSystem, 
                                       TInt uid, 
                                       const TDesC &activityName)
{
    _LIT(KUidFormat, "%+08x\\");
    _LIT(KExtFormat, ".mbm");
    //Generate activity hash
    RBuf8 buff8;
    CleanupClosePushL(buff8);
    buff8.CreateL(activityName.Length());
    buff8.Copy(activityName);
    HBufC8 *activityHash = Md5HexDigestL(buff8);
    CleanupStack::PopAndDestroy(&buff8);
    CleanupStack::PushL(activityHash);
    
    //Get private path
    RBuf privatePath;
    CleanupClosePushL(privatePath);
    privatePath.CreateL(KMaxPathLength);
    User::LeaveIfError(fileSystem.PrivatePath(privatePath));
    
    //Format activity path
    privatePath.AppendFormat( KUidFormat, uid);
    
    const TInt requiredSize(privatePath.Length() + 
                            activityHash->Length() + 
                            KExtFormat().Length());
    CAfEntry::ReallocL(dst, requiredSize);
    
    //Copy path
    dst.Copy(privatePath);
    privatePath.Copy(*activityHash);//reuse already allocated buffer to convert 8 -> 16
    dst.Append(privatePath);
    dst.Append(KExtFormat());
    
    CleanupStack::PopAndDestroy(&privatePath);
    CleanupStack::PopAndDestroy(activityHash);
    
    BaflUtils::EnsurePathExistsL(fileSystem, dst);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
HBufC8* AfStorageSyncTask::Md5HexDigestL(const TDes8 &string)
{
    _LIT8(KMd5HexFormat, "%+02x");
    CMD5* md5 = CMD5::NewL();
    CleanupStack::PushL(md5);
    
    TPtrC8 hashedSig(md5->Hash(string));
    
    HBufC8* buf = HBufC8::NewL(hashedSig.Length() * 2);
    TPtr8 bufPtr = buf->Des();
    
    for(TInt i(0); i< hashedSig.Length(); ++i) {
        bufPtr.AppendFormat(KMd5HexFormat,hashedSig[i]);
    }
    CleanupStack::PopAndDestroy(md5);
    return buf;
}
