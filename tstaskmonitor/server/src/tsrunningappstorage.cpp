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
#include "tsrunningappstorage.h"
#include "tsfswdatalist.h"
#include "tsfswentry.h"
#include <s32strm.h>
#include <s32mem.h>
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CRunningAppStorage::CRunningAppStorage(MHsDataObserver& observer)
: mObserver(observer)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CRunningAppStorage::~CRunningAppStorage()
{
    mData.Close();
    delete mEngine;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CRunningAppStorage* CRunningAppStorage::NewL(MHsDataObserver& observer,
                                             MTsResourceManager& resources,
                                             MTsWindowGroupsMonitor &wsMonitor)
{
    CRunningAppStorage * self = new (ELeave)CRunningAppStorage(observer);
    CleanupStack::PushL(self);
    self->ConstructL(resources, wsMonitor);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CRunningAppStorage::ConstructL(MTsResourceManager& resources, 
                                    MTsWindowGroupsMonitor &wsMonitor)
{
    mEngine = CTsFswDataList::NewL(resources, wsMonitor, *this) ;
    RArray<RWsSession::TWindowGroupChainInfo> wgList;
    CleanupClosePushL(wgList);
    User::LeaveIfError(resources.WsSession().WindowGroupList(&wgList));
    mEngine->HandleWindowGroupChanged(resources, wgList.Array());
    CleanupStack::PopAndDestroy(&wgList);
    DataChangedL();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
const TDesC8& CRunningAppStorage::Data() const
{
    return mData;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CRunningAppStorage::DataChanged() 
{
    TRAPD( err, DataChangedL() );
    if ( err == KErrNone)
    {
        mObserver.DataChanged();
    }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CRunningAppStorage::DataChangedL()
{
    RTsFswArray taskList = mEngine->FswDataL();
    CTsFswEntry* entry = CTsFswEntry::NewLC();
    // Create a dynamic flat buffer to hold this object’s member data
    
    const TInt KExpandSize = 256; // "Granularity" of dynamic buffer
    CBufFlat* buf = CBufFlat::NewL(KExpandSize);
    CleanupStack::PushL(buf);
    RBufWriteStream stream(*buf); 
    CleanupClosePushL(stream);
    entry->ExternalizeArrayL(stream, taskList);
    CleanupStack::PopAndDestroy(&stream);
    mData.Close();
    mData.CreateL( buf->Size() );
    buf->Read(0, mData, buf->Size());
    

    CleanupStack::PopAndDestroy(buf);
    CleanupStack::PopAndDestroy(entry);  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CRunningAppStorage::UpdateL(TInt key, const CFbsBitmap& data, TInt /*param*/, TInt priority)
    {
    UpdatePriority newpriority = static_cast<UpdatePriority>(priority);
    mEngine->SetScreenshotL(&data, newpriority, key);
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CRunningAppStorage::RemoveL(TInt key, TInt /*param*/)
    {
    mEngine->RemoveScreenshotL(key);
    }
