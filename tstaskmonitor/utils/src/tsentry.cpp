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
#include <s32strm.h>
#include <fbs.h>
#include "tsentry.h"
#include "tsdataobserver.h"


// --------------------------------------------------------------------------
// CTsFswEntry::NewL
// --------------------------------------------------------------------------
//
CTsEntry* CTsEntry::NewL()
{
    CTsEntry* self = NewLC();
    CleanupStack::Pop(self);
    return self;
}

// --------------------------------------------------------------------------
// CTsFswEntry::NewLC
// --------------------------------------------------------------------------
//
CTsEntry* CTsEntry::NewLC()
{
    CTsEntry* self = new (ELeave) CTsEntry;
    CleanupStack::PushL(self);
    return self;
}

// --------------------------------------------------------------------------
// CTsFswEntry::NewL
// --------------------------------------------------------------------------
//
CTsEntry* CTsEntry::NewL(const TTsEntryKey& key, MHsDataObserver *observer)
{
    CTsEntry* self = NewLC(key, observer);
    CleanupStack::Pop(self);
    return self;
}

// --------------------------------------------------------------------------
// CTsFswEntry::NewLC
// --------------------------------------------------------------------------
//
CTsEntry* CTsEntry::NewLC(const TTsEntryKey& key, MHsDataObserver* observer)
{
    CTsEntry* self = new (ELeave) CTsEntry(key, observer);
    CleanupStack::PushL(self);
    return self;
}

// --------------------------------------------------------------------------
// CTsFswEntry::~CTsFswEntry
// --------------------------------------------------------------------------
//
CTsEntry::~CTsEntry()
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
CTsEntry::CTsEntry() :
    mVisibility(Visible),
    mPriority(Low)
{
    mKey = TTsEntryKey();
}

// --------------------------------------------------------------------------
// CTsFswEntry::CTsFswEntry
// --------------------------------------------------------------------------
//
CTsEntry::CTsEntry(const TTsEntryKey& key, MHsDataObserver *observer) 
:
    mVisibility(Visible),
    mKey(key), 
    mPriority(Low), 
    mObserver(observer)
{
}

// --------------------------------------------------------------------------
// CTsFswEntry::SetAppUid
// --------------------------------------------------------------------------
//
void CTsEntry::SetAppUid(const TUid& uid)
{
    mAppUid = uid;
}

// --------------------------------------------------------------------------
// CTsFswEntry::SetAppNameL
// --------------------------------------------------------------------------
//
void CTsEntry::SetAppNameL(const TDesC& appName)
{
    delete mAppName;
    mAppName = 0;
    mAppName = appName.AllocL();
}

// --------------------------------------------------------------------------
// CTsFswEntry::SetSystemApp
// --------------------------------------------------------------------------
//
void CTsEntry::SetCloseableApp(TBool value)
{
    mCloseableApp = value;
}

// --------------------------------------------------------------------------
// CTsFswEntry::SetAppIconHandles
// --------------------------------------------------------------------------
//
void CTsEntry::SetAppIcon(CFbsBitmap* bitmap, CFbsBitmap* mask)
{
    mAppIconBitmap = bitmap;
    mAppIconMask = mask;
}

// --------------------------------------------------------------------------
/**
 * Application uid.
 */
TUid CTsEntry::AppUid() const
{
    return mAppUid;
}

// --------------------------------------------------------------------------
/**
 * Application name.
 */
const TDesC& CTsEntry::AppName() const
{
    return mAppName ? *mAppName : KNullDesC();
}

// --------------------------------------------------------------------------
/**
 * Retrieve entry visibilit status
 */
Visibility CTsEntry::GetVisibility() const
{
    return mVisibility;
}

// --------------------------------------------------------------------------
/**
 * Change entry visibility status
 * @param visibility - new visibility status
 */
void CTsEntry::SetVisibility(Visibility visibility)
{
    mVisibility = visibility;
}

// --------------------------------------------------------------------------
/**
 * ETrue if the application is closeable
 */
TBool CTsEntry::CloseableApp() const
{
    return mCloseableApp;
}

// --------------------------------------------------------------------------
/**
 * Application icon bitmap
 */
CFbsBitmap* CTsEntry::AppIconBitmap() const
{
    return mAppIconBitmap;
}

// --------------------------------------------------------------------------
/**
 * Application icon mask.
 */
CFbsBitmap* CTsEntry::AppIconMask() const
{
    return mAppIconMask;
}

// --------------------------------------------------------------------------
/**
 * Entry's key
 */
const TTsEntryKey& CTsEntry::Key() const
{
    return mKey;
}
// --------------------------------------------------------------------------
// CTsFswEntry::ExternalizeL
// --------------------------------------------------------------------------
//
void CTsEntry::ExternalizeL(RWriteStream& stream) const
{
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
void CTsEntry::InternalizeL(RReadStream& stream)
{
    
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
void CTsEntry::ExternalizeArrayL(RWriteStream& stream, const RTsFswArray& array)
{
    TInt iter(0);
    RArray<TInt> visibleItems(array.Count() ? array.Count() : 1);
    CleanupClosePushL(visibleItems);
    for (iter = 0; iter < array.Count(); ++iter) {
        if (Visible == array[iter]->GetVisibility()) {
            visibleItems.AppendL(iter);
        }
    }
    stream.WriteInt32L(visibleItems.Count());
    for (iter = 0; iter < visibleItems.Count(); ++iter) {
        array[visibleItems[iter]]->ExternalizeL(stream);
    }
    CleanupStack::PopAndDestroy(&visibleItems);
}

// --------------------------------------------------------------------------
// CTsFswEntry::InternalizeArrayL
// --------------------------------------------------------------------------
//
void CTsEntry::InternalizeArrayL(RReadStream& stream, RTsFswArray& array)
{
    array.ResetAndDestroy();
    TInt count = stream.ReadInt32L();
    for (TInt i = 0; i < count; ++i) {
        CTsEntry* entry = CTsEntry::NewLC();
        entry->InternalizeL(stream);
        array.AppendL(entry);
        CleanupStack::Pop(entry);
    }
}

// --------------------------------------------------------------------------
/**
 * Application type.
 */
TAppType CTsEntry::Type() const
{
    return EApp;
}

// --------------------------------------------------------------------------
// CTsFswEntry::SetScreenshot
// --------------------------------------------------------------------------
//
void CTsEntry::SetScreenshotL(const CFbsBitmap& bitmapArg, UpdatePriority priority)
{
    TInt currentPriority = static_cast<TInt> (mPriority);
    TInt newPriority = static_cast<TInt> (priority);
    if(newPriority <currentPriority) {
        User::Leave(KErrAccessDenied);
    }
    
    CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    User::LeaveIfError(bitmap->Duplicate(bitmapArg.Handle()));
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
void CTsEntry::RemoveScreenshotL()
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
/**
 * Application screenshot.
 */
CFbsBitmap* CTsEntry::Screenshot() const
{
    return mScreenshot;
}

// --------------------------------------------------------------------------
/**
 * Priority.
 */
UpdatePriority CTsEntry::Priority() const
{
    return mPriority;
}

// --------------------------------------------------------------------------
// CTsFswEntry::Priority
// --------------------------------------------------------------------------
//
void CTsEntry::ImageReadyCallBack(TInt error, const CFbsBitmap *bitmap)
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
