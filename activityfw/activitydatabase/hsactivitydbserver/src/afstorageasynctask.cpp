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

#include "afstorageasynctask.h"
#include "activitycmd.h"

_LIT(KUnsupportedStorageAsyncTask, "Unsupported async storage task");

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LOCAL_C void ReadEntryL(CAfEntry& entry, const RMessage2& msg)
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
/**
 * Constructor for performing 1st stage construction
 */
CAfStorageAsyncTask::CAfStorageAsyncTask()
{
    // No implementation required
}

// -----------------------------------------------------------------------------
/**
 * Destructor.
 */
CAfStorageAsyncTask::~CAfStorageAsyncTask()
{
    mExternalizedData.Close();
    mInternalizedData.ResetAndDestroy();
}

// -----------------------------------------------------------------------------
/**
 * Handle asynchronous data storage requests
 * @param taskStorage - data tasks storage
 * @param dataStorage - data storage
 * @param msg - request message
 */
void CAfStorageAsyncTask::ExecuteLD(MAfTaskStorage& taskStorage, 
                                    CAfStorage& dataStorage, 
                                    const RMessage2& msg)
{
    CAfStorageAsyncTask* self = new (ELeave) CAfStorageAsyncTask();
    CleanupStack::PushL(self);
    self->ExecuteL(dataStorage, msg);
    taskStorage.PushL(self);
    CleanupStack::Pop(self);
    if (EFalse == msg.IsNull()) {
        msg.Complete(KErrNone);
    }
}

// -----------------------------------------------------------------------------
/**
 * Interface implementation
 * @see CActivityTask::Data()
 */
const TDesC8& CAfStorageAsyncTask::CAfStorageAsyncTask::Data() const
{
    return mExternalizedData;
}

// -----------------------------------------------------------------------------
/**
 * Interface implementation
 * @see CActivityTask::BroadcastReceivedL(const RMessage2 &)
 */
void CAfStorageAsyncTask::BroadcastReceivedL(const RMessage2& )
{
    // No implementation required
}

// -----------------------------------------------------------------------------
/**
 * Handle asynchronous data storage requests
 * @param dataStorage - data storage
 * @param msg - request message
 */
void CAfStorageAsyncTask::ExecuteL(CAfStorage& dataStorage, 
                                   const RMessage2& msg)
{
    switch (msg.Function()) {
    case Activities:
        AllActivitiesL(dataStorage, msg);
        break;
    case ApplicationActivities:
        ApplicationActivitiesL(dataStorage, msg);
        break;
    case ApplicationActivity:
        ApplicationActivityL(dataStorage, msg);
        break;
    default:
        //this code shouldn't be called. fatal error: means wrong session implementation 
        User::Panic(KUnsupportedStorageAsyncTask, KErrGeneral);
    };
}

// -----------------------------------------------------------------------------
/**
 * Handle getting all activities request
 * @param dataStorage - data storage
 * @param msg - requested message
 */
void CAfStorageAsyncTask::AllActivitiesL(CAfStorage& dataStorage, 
                                         const RMessage2& msg)
{
    dataStorage.ActivitiesL(mInternalizedData);
    ExternalizeL();
    WriteResponseL(msg);
}

// -----------------------------------------------------------------------------
/**
 * Handle getting application activities request
 * @param dataStorage - data storage
 * @param msg - requested message
 */
void CAfStorageAsyncTask::ApplicationActivitiesL(CAfStorage& dataStorage, 
                                                 const RMessage2& msg)
{
    CAfEntry *entry = CAfEntry::NewLC();
    ReadEntryL(*entry, msg);
    dataStorage.ActivitiesL(mInternalizedData, entry->ApplicationId());
    CleanupStack::PopAndDestroy(entry);
    ExternalizeL();
    WriteResponseL(msg);
}

// -----------------------------------------------------------------------------
/**
 * Handle getting application activity request
 * @param dataStorage - data storage
 * @param msg - requested message
 */
void CAfStorageAsyncTask::ApplicationActivityL(CAfStorage& dataStorage, 
                                               const RMessage2& msg)
{
    CAfEntry *src(CAfEntry::NewLC()) , *dst(0);
    ReadEntryL(*src, msg);
    dataStorage.ActivityL(dst, *src);
    CleanupStack::PushL(dst);
    ExternalizeL(*dst);
    CleanupStack::PopAndDestroy(dst);
    CleanupStack::PopAndDestroy(src);
    WriteResponseL(msg);
}

// -----------------------------------------------------------------------------
void CAfStorageAsyncTask::ExternalizeL()
{
    mExternalizedData << mInternalizedData;
    mInternalizedData.ResetAndDestroy();
}

// -----------------------------------------------------------------------------
/**
 * Externalzie entry into output buffer
 * @param entry - source entry
 */
void CAfStorageAsyncTask::ExternalizeL(const CAfEntry &entry)
{
    CAfEntry::ReallocL(mExternalizedData, entry.Size());
    RDesWriteStream writer(mExternalizedData);
    CleanupClosePushL(writer);
    writer << entry;
    CleanupStack::PopAndDestroy(&writer);
}

// -----------------------------------------------------------------------------
/**
 * Write response data into request message
 * @param msg - destination message
 */
void CAfStorageAsyncTask::WriteResponseL(const RMessage2& msg)
{
    if (EFalse == msg.IsNull()) {
        msg.WriteL(1, 
                   TPckgBuf<TInt>(mExternalizedData.Length()));//write data size
        msg.WriteL(2, 
                   TPckgBuf<CBase*>(this));//task identyfier
    }
}

// -----------------------------------------------------------------------------
/**
 * Returns ETrue if task is related with session argument
 */

TBool CAfStorageAsyncTask::IsSessionTask(const CSession2* /*session*/)
{
	return EFalse;
}
