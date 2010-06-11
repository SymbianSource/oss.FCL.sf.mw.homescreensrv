/*
 * Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Task list entry
 *
 */

#define __E32SVR_H__
#include "tsfswentry.h"
#include "tsdataobserver.h"
#include <s32strm.h>
#include <fbs.h>

// --------------------------------------------------------------------------
// CTsFswEntry::NewL
// --------------------------------------------------------------------------
//
CTsFswEntry* CTsFswEntry::NewL()
{
    CTsFswEntry* self = NewLC();
    CleanupStack::Pop(self);
    return self;
}

// --------------------------------------------------------------------------
// CTsFswEntry::NewLC
// --------------------------------------------------------------------------
//
CTsFswEntry* CTsFswEntry::NewLC()
{
    CTsFswEntry* self = new (ELeave) CTsFswEntry;
    CleanupStack::PushL(self);
    return self;
}

// --------------------------------------------------------------------------
// CTsFswEntry::NewL
// --------------------------------------------------------------------------
//
CTsFswEntry* CTsFswEntry::NewL(const TTsEntryKey& key, MHsDataObserver *observer)
{
    CTsFswEntry* self = NewLC(key, observer);
    CleanupStack::Pop(self);
    return self;
}

// --------------------------------------------------------------------------
// CTsFswEntry::NewLC
// --------------------------------------------------------------------------
//
CTsFswEntry* CTsFswEntry::NewLC(const TTsEntryKey& key, MHsDataObserver* observer)
{
    CTsFswEntry* self = new (ELeave) CTsFswEntry(key, observer);
    CleanupStack::PushL(self);
    return self;
}

// --------------------------------------------------------------------------
// CTsFswEntry::~CTsFswEntry
// --------------------------------------------------------------------------
//
CTsFswEntry::~CTsFswEntry()
{
    delete mAppName;
    delete mAppIconBitmap;
    delete mAppIconMask;
    delete mScreenshot;
    delete mImgTool;
}

// --------------------------------------------------------------------------
// CTsFswEntry::CTsFswEntry
// --------------------------------------------------------------------------
//
CTsFswEntry::CTsFswEntry() :
    mPriority(Low)
{
    mKey = TTsEntryKey();
}

// --------------------------------------------------------------------------
// CTsFswEntry::CTsFswEntry
// --------------------------------------------------------------------------
//
CTsFswEntry::CTsFswEntry(const TTsEntryKey& key, MHsDataObserver *observer) :
    mKey(key), mPriority(Low), mObserver(observer)
{
    mWgId = key.mParentId;
}

// --------------------------------------------------------------------------
// CTsFswEntry::SetWgId
// --------------------------------------------------------------------------
//
void CTsFswEntry::SetWgId(TInt wgId)
{
    mWgId = wgId;
    mKey = TTsEntryKey(mWgId);
}

// --------------------------------------------------------------------------
// CTsFswEntry::SetAppUid
// --------------------------------------------------------------------------
//
void CTsFswEntry::SetAppUid(const TUid& uid)
{
    mAppUid = uid;
}

// --------------------------------------------------------------------------
// CTsFswEntry::SetAppNameL
// --------------------------------------------------------------------------
//
void CTsFswEntry::SetAppNameL(const TDesC& appName)
{
    delete mAppName;
    mAppName = 0;
    mAppName = appName.AllocL();
}

// --------------------------------------------------------------------------
// CTsFswEntry::SetSystemApp
// --------------------------------------------------------------------------
//
void CTsFswEntry::SetCloseableApp(TBool value)
{
    mCloseableApp = value;
}

// --------------------------------------------------------------------------
// CTsFswEntry::SetAppIconHandles
// --------------------------------------------------------------------------
//
void CTsFswEntry::SetAppIcon(CFbsBitmap* bitmap, CFbsBitmap* mask)
{
    mAppIconBitmap = bitmap;
    mAppIconMask = mask;
}

// --------------------------------------------------------------------------
// CTsFswEntry::WgId
// --------------------------------------------------------------------------
//
TInt CTsFswEntry::WgId() const
{
    return mWgId;
}

// --------------------------------------------------------------------------
// CTsFswEntry::AppUid
// --------------------------------------------------------------------------
//
TUid CTsFswEntry::AppUid() const
{
    return mAppUid;
}

// --------------------------------------------------------------------------
// CTsFswEntry::AppName
// --------------------------------------------------------------------------
//
const TDesC& CTsFswEntry::AppName() const
{
    return mAppName ? *mAppName : KNullDesC();
}

// --------------------------------------------------------------------------
// CTsFswEntry::SystemApp
// --------------------------------------------------------------------------
//
TBool CTsFswEntry::CloseableApp() const
{
    return mCloseableApp;
}

// --------------------------------------------------------------------------
// CTsFswEntry::AppIconBitmapHandle
// --------------------------------------------------------------------------
//
CFbsBitmap* CTsFswEntry::AppIconBitmap() const
{
    return mAppIconBitmap;
}

// --------------------------------------------------------------------------
// CTsFswEntry::AppIconMaskHandle
// --------------------------------------------------------------------------
//
CFbsBitmap* CTsFswEntry::AppIconMask() const
{
    return mAppIconMask;
}

// --------------------------------------------------------------------------
// CTsFswEntry::Key
// --------------------------------------------------------------------------
//
const TTsEntryKey& CTsFswEntry::Key() const
{
    return mKey;
}
// --------------------------------------------------------------------------
// CTsFswEntry::ExternalizeL
// --------------------------------------------------------------------------
//
void CTsFswEntry::ExternalizeL(RWriteStream& stream)
{
    stream.WriteInt32L(mWgId);
    stream.WriteInt32L(mAppUid.iUid);
    stream << AppName();
    stream.WriteInt32L(mCloseableApp);
    if (mAppIconBitmap) {
        stream.WriteInt32L(mAppIconBitmap->Handle());
    }
    else {
        stream.WriteInt32L(KErrNotFound);
    }

    if (mAppIconMask) {
        stream.WriteInt32L(mAppIconMask->Handle());
    }
    else {
        stream.WriteInt32L(KErrNotFound);
    }

    if (mScreenshot) {
        stream.WriteInt32L(mScreenshot->Handle());
    }
    else {
        stream.WriteInt32L(KErrNotFound);
    }

    mKey.ExternalizeL(stream);
    TInt currentPriority = static_cast<TInt> (mPriority);
    stream.WriteInt32L(currentPriority);
}

// --------------------------------------------------------------------------
// CTsFswEntry::InternalizeL
// --------------------------------------------------------------------------
//
void CTsFswEntry::InternalizeL(RReadStream& stream)
{
    mWgId = stream.ReadInt32L();
    mAppUid = TUid::Uid(stream.ReadInt32L());
    delete mAppName;
    mAppName = NULL;
    mAppName = HBufC::NewL(stream, KMaxTInt);
    mCloseableApp = stream.ReadInt32L();
    TInt appIconBitmapHandle = stream.ReadInt32L();
    TInt appIconMaskHandle = stream.ReadInt32L();
    TInt screenshotHandle = stream.ReadInt32L();
    if (appIconBitmapHandle != KErrNotFound) {
        mAppIconBitmap = new (ELeave) CFbsBitmap;
        mAppIconBitmap->Duplicate(appIconBitmapHandle);
    }
    if (appIconMaskHandle != KErrNotFound) {
        mAppIconMask = new (ELeave) CFbsBitmap;
        mAppIconMask->Duplicate(appIconMaskHandle);
    }
    if (screenshotHandle != KErrNotFound) {
        mScreenshot = new (ELeave) CFbsBitmap;
        mScreenshot->Duplicate(screenshotHandle);
    }
    mKey.InternalizeL(stream);
    TInt currentPriority = stream.ReadInt32L();
    mPriority = static_cast<UpdatePriority> (currentPriority);
}

// --------------------------------------------------------------------------
// CTsFswEntry::ExternalizeArrayL
// --------------------------------------------------------------------------
//
void CTsFswEntry::ExternalizeArrayL(RWriteStream& stream, const RTsFswArray& aArray)
{
    TInt count = aArray.Count();
    stream.WriteInt32L(count);
    for (TInt i = 0; i < count; ++i) {
        aArray[i]->ExternalizeL(stream);
    }
}

// --------------------------------------------------------------------------
// CTsFswEntry::InternalizeArrayL
// --------------------------------------------------------------------------
//
void CTsFswEntry::InternalizeArrayL(RReadStream& stream, RTsFswArray& aArray)
{
    aArray.ResetAndDestroy();
    TInt count = stream.ReadInt32L();
    for (TInt i = 0; i < count; ++i) {
        CTsFswEntry* entry = CTsFswEntry::NewLC();
        entry->InternalizeL(stream);
        aArray.AppendL(entry);
        CleanupStack::Pop(entry);
    }
}

// --------------------------------------------------------------------------
// CTsFswEntry::Type
// --------------------------------------------------------------------------
//
TAppType CTsFswEntry::Type() const
{
    return EApp;
}

// --------------------------------------------------------------------------
// CTsFswEntry::SetScreenshot
// --------------------------------------------------------------------------
//
void CTsFswEntry::SetScreenshotL(const CFbsBitmap* bitmapArg, UpdatePriority priority)
{
    TInt currentPriority = static_cast<TInt> (mPriority);
    TInt newPriority = static_cast<TInt> (priority);
    if(newPriority <currentPriority) {
        User::Leave(KErrAccessDenied);
    }
    
    CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    User::LeaveIfError(bitmap->Duplicate(bitmapArg->Handle()));
    CleanupStack::Pop(bitmap);

    mPriority = priority;
    delete mScreenshot;
    mScreenshot = bitmap;

    delete mImgTool;
    mImgTool = 0;

    mImgTool = CTsGraphicFileScalingHandler::NewL(*this, *mScreenshot, TSize(128, 128),
    CTsGraphicFileScalingHandler::EKeepAspectRatioByExpanding);
}

// --------------------------------------------------------------------------
// CTsFswEntry::RemoveScreenshot
// --------------------------------------------------------------------------
//
void CTsFswEntry::RemoveScreenshotL()
{
    if (!mScreenshot) {
        User::Leave(KErrNotFound);
    }
    delete mScreenshot;
    mScreenshot = NULL;
    mPriority = Low;
    if (mObserver) {
        mObserver->DataChanged();
    }
}

// --------------------------------------------------------------------------
// CTsFswEntry::Screenshot
// --------------------------------------------------------------------------
//
CFbsBitmap* CTsFswEntry::Screenshot() const
{
    return mScreenshot;
}

// --------------------------------------------------------------------------
// CTsFswEntry::Priority
// --------------------------------------------------------------------------
//
UpdatePriority CTsFswEntry::Priority() const
{
    return mPriority;
}

// --------------------------------------------------------------------------
// CTsFswEntry::Priority
// --------------------------------------------------------------------------
//
void CTsFswEntry::ImageReadyCallBack(TInt error, const CFbsBitmap *bitmap)
{
    if (KErrNone == error && 0 != bitmap) {
        mScreenshot->Reset();
        mScreenshot->Duplicate(bitmap->Handle());

        if (mObserver) {
            mObserver->DataChanged();
        }
    }
}

// end of file
