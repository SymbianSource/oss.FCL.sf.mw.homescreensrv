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
#include "tsfswengine.h"
#include "tsfswmonitor.h"
#include "tsfswentry.h"
#include <s32strm.h>
#include <S32MEM.H>
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
    delete mMonitor;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CRunningAppStorage* CRunningAppStorage::NewL(MHsDataObserver& observer)
{
    CRunningAppStorage * self = new (ELeave)CRunningAppStorage(observer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CRunningAppStorage::ConstructL()
{
    mEngine = CTsFswEngine::NewL(*this) ;
    mMonitor = CTsFswMonitor::NewL(*mEngine);
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
    
    int a=8;
    if( a==9)
    {
        CActiveScheduler::Stop();
    }
}
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
