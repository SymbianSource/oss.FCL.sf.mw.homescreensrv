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
 * Description:  File containing application list classes
 *
 */

//INCLUDES:

#include "tsfswdatalist.h"
#include "tsfswengine.h"
#include "tsentrykeygenerator.h"
#include <mmf/common/mmfcontrollerpluginresolver.h> // for CleanupResetAndDestroyPushL
#include <apgwgnam.h>
#include <bitstd.h>
#include <AknIconUtils.h> // avkon
#include <apgicnfl.h> // fbsbitmap
#include <AknIconSrvClient.h> 
#include <fbs.h>
#include <APGWGNAM.H>


// size for the created app icons
const TInt KAppIconWidth = 128;
const TInt KAppIconHeight = 128;

//uids to be hidden
const TUid KHsApplicationUid = { 0x20022F35 };

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CTsFswDataList::NewL
// --------------------------------------------------------------------------
//
CTsFswDataList* CTsFswDataList::NewL(MHsDataObserver &observer)
{
    CTsFswDataList* self = new (ELeave) CTsFswDataList(observer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// --------------------------------------------------------------------------
// CTsFswDataList::CTsFswDataList
// --------------------------------------------------------------------------
//
CTsFswDataList::CTsFswDataList(MHsDataObserver& observer) :
    mObserver(observer)
{
}

// --------------------------------------------------------------------------
// CTsFswDataList::~CTsFswDataList
// --------------------------------------------------------------------------
//
CTsFswDataList::~CTsFswDataList()
{
    mData.ResetAndDestroy();
    mAppArcSession.Close();
    mWsSession.Close();
    mHiddenUids.Close();
    mAllowedUids.Close();
    RFbsSession::Disconnect();
    RAknIconSrvClient::Disconnect();
}

// --------------------------------------------------------------------------
// CTsFswDataList::ConstructL
// --------------------------------------------------------------------------
//
void CTsFswDataList::ConstructL()
{
    User::LeaveIfError(mWsSession.Connect());
    User::LeaveIfError(mAppArcSession.Connect());
    mHiddenUids.AppendL(KHsApplicationUid);
    User::LeaveIfError(RFbsSession::Connect());
    RAknIconSrvClient::Connect();
}

// --------------------------------------------------------------------------
// CTsFswDataList::FswDataL
// --------------------------------------------------------------------------
//
const RTsFswArray& CTsFswDataList::FswDataL()
{
    return mData;
}

// --------------------------------------------------------------------------
// CTsFswDataList::CollectTasksL
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::CollectTasksL()
{
    TBool changed = EFalse;
    RTsFswArray newAppsList;
    CleanupResetAndDestroyPushL(newAppsList);
    CollectAppsL(newAppsList);
    changed |= FitDataToListL(newAppsList);
    CleanupStack::PopAndDestroy(&newAppsList);
    return changed;
}

// --------------------------------------------------------------------------
// CTsFswDataList::
// --------------------------------------------------------------------------
//
void CTsFswDataList::CollectAppsL(RTsFswArray& appsList)
{
    // gets allowed uids - running apps without hidden uids, null uids, filtered uid
    GetAllowedUidsL();

    // get all window groups
    RArray<RWsSession::TWindowGroupChainInfo> allWgIds;
    CleanupClosePushL(allWgIds);
    User::LeaveIfError(mWsSession.WindowGroupList(0, &allWgIds));

    TInt count = allWgIds.Count();
    for (TInt i = 0; i < count; ++i) {
        TTsEntryKey key = TsEntryKeyGeneraror::GenerateL(allWgIds[i].iId, allWgIds.Array());
        //skip this entry if it is already on list
        if (FindEntry(appsList, key) >= 0) {
            continue;
        }

        // get window group name
        TInt wgId = key.mParentId;
        CApaWindowGroupName* windowName = CApaWindowGroupName::NewLC(mWsSession, wgId);
        TUid appUid = windowName->AppUid();

        // add item to task list if uid is allowed
        if (mAllowedUids.Find(appUid) >= 0) {
            AddEntryL(key, appUid, windowName, appsList);
        }
        CleanupStack::PopAndDestroy(windowName);
    }
    CleanupStack::PopAndDestroy(&allWgIds);
}

// --------------------------------------------------------------------------
// CTsFswDataList::AddEntryL
// --------------------------------------------------------------------------
//
void CTsFswDataList::AddEntryL(const TTsEntryKey& key, const TUid& appUid,
    CApaWindowGroupName* wgName, RTsFswArray& newList)
{
    CTsFswEntry* entry = CTsFswEntry::NewLC(key, &mObserver);

    // check if present in old list and if yes then take some of the old data
    TBool found = ConsiderOldDataL(key);

    // if not present previously then find out app name
    // and check if screenshot is already available
    if (!found) {
        entry->SetAppUid(appUid);
        HBufC* name = FindAppNameLC(wgName, appUid, key.mParentId);
        if (name) {
            entry->SetAppNameL(*name);
        }
        CleanupStack::PopAndDestroy(name);

        if (wgName) {
            entry->SetCloseableApp(!wgName->IsSystem());
        }
        CFbsBitmap* iconBitmap = NULL;
        CFbsBitmap* iconMask = NULL;
        GetAppIconL(appUid, iconBitmap, iconMask);
        //transfer ownership to entry
        entry->SetAppIcon(iconBitmap, iconMask);
    }

    // add to new list, ownership is transferred
    newList.AppendL(entry);
    CleanupStack::Pop(entry);
}

// --------------------------------------------------------------------------
// CTsFswDataList::ConsiderOldDataL
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::ConsiderOldDataL(const TTsEntryKey& key)
{
    for (TInt entryIdx = 0, oldCount = mData.Count(); entryIdx < oldCount; ++entryIdx) {
        if (mData[entryIdx]->Key() == key) {
            return ETrue;
        }
    }
    return EFalse;
}

// --------------------------------------------------------------------------
// CTsFswDataList::FindAppNameLC
// --------------------------------------------------------------------------
//
HBufC* CTsFswDataList::FindAppNameLC(CApaWindowGroupName* windowName, const TUid& appUid, TInt wgId)
{
    //Retrieve the app name
    TApaAppInfo info;
    mAppArcSession.GetAppInfo(info, appUid);
    TPtrC caption = info.iShortCaption;

    HBufC* tempName = 0;
    if (!caption.Length() && windowName) // if not set - use thread name instead
    {
        if (windowName->Caption().Length()) {
            tempName = windowName->Caption().AllocL();
            //put on cleanupstack after the if
        }
        else {
            TThreadId threadId;
            TInt err = mWsSession.GetWindowGroupClientThreadId(wgId, threadId);
            if (err == KErrNone) {
                RThread thread;
                CleanupClosePushL(thread);
                err = thread.Open(threadId);
                if (err == KErrNone) {
                    tempName = thread.Name().AllocL(); // codescanner::forgottoputptroncleanupstack
                    // tempName put on cleanupstack after the if
                }
                CleanupStack::PopAndDestroy(&thread);
            }
        }
    }
    else {
        tempName = caption.AllocL();
        //put on cleanupstack after the if
    }
    CleanupStack::PushL(tempName);
    return tempName;
}

// --------------------------------------------------------------------------
// CTsFswDataList::FitDataToListL
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::FitDataToListL(RTsFswArray& listToFit)
{
    TBool changed = EFalse;
    TInt listCount = listToFit.Count();
    TInt dataCount = mData.Count();

    //remove items that dont't exists in newly collected list      
    for (TInt i = dataCount - 1; i >= 0; --i) {
        if (!CheckIfExists(*mData[i], listToFit)) {
            delete mData[i];
            mData.Remove(i);
            changed = ETrue;
        }
    }
    RArray<TTsEntryKey> allKeys;

    //add new item at start
    for (TInt i = listToFit.Count() - 1; i >= 0; --i) {
        allKeys.Insert(listToFit[i]->Key(), 0);
        if (!CheckIfExists(*listToFit[i], mData)) {
            mData.Insert(listToFit[i], 0);
            listToFit[i] = 0;
            changed = ETrue;
        }
    }
    //establish order
    TBool orderChanged = EstablishOrder(allKeys);
    changed = changed || orderChanged;
    allKeys.Close();
    return changed;
}

// --------------------------------------------------------------------------
// CTsFswDataList::CheckIfExists
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::CheckIfExists(const CTsFswEntry& entry, const RTsFswArray& list) const
{
    TBool exists(EFalse);
    TTsEntryKey key;
    TInt pos = FindEntry(list, entry.Key());
    if (pos >= 0) {
        exists = ETrue;
    }
    return exists;
}

// --------------------------------------------------------------------------
// CTsFswDataList::GetAppIconL
// --------------------------------------------------------------------------
//
void CTsFswDataList::GetAppIconL(const TUid& aAppUid, CFbsBitmap*& bitmapArg, CFbsBitmap*& maskArg)
{
    bitmapArg = maskArg = NULL;

    TSize size(KAppIconWidth, KAppIconHeight);
    CApaMaskedBitmap* apaMaskedBitmap = CApaMaskedBitmap::NewLC();
    TInt err = mAppArcSession.GetAppIcon(aAppUid, size, *apaMaskedBitmap);
    TInt iconsCount(0);
    if (err == KErrNone) {
        err = mAppArcSession.NumberOfOwnDefinedIcons(aAppUid, iconsCount);
    }

    if ((err == KErrNone) && (iconsCount > 0)) {
        bitmapArg = static_cast<CFbsBitmap*> (apaMaskedBitmap);
        TInt maskHandle = apaMaskedBitmap->Mask()->Handle();
        maskArg = new (ELeave) CFbsBitmap;
        maskArg->Duplicate(maskHandle);
        CleanupStack::Pop(apaMaskedBitmap);
    }
    else {
        CleanupStack::PopAndDestroy(apaMaskedBitmap);
        HBufC* fileNameFromApparc = NULL;
        TInt err = mAppArcSession.GetAppIcon(aAppUid, fileNameFromApparc);
        if (err == KErrNone) {
            CleanupStack::PushL(fileNameFromApparc);
            CFbsBitmap *bitamp(0);
            CFbsBitmap *mask(0);
            TInt bitmapIndex = 0;
            TInt maskIndex = 1;
            // it will change bitmap ids if it is mif (checking inside)
            AknIconUtils::ValidateLogicalAppIconId(*fileNameFromApparc, bitmapIndex, maskIndex);
            AknIconUtils::CreateIconLC(bitamp, mask, fileNameFromApparc->Des(), bitmapIndex,
                maskIndex);

            if (AknIconUtils::IsMifFile(*fileNameFromApparc)) {
                AknIconUtils::DisableCompression(bitamp);
                AknIconUtils::SetSize(bitamp, TSize(KAppIconWidth, KAppIconHeight),
                    EAspectRatioPreservedAndUnusedSpaceRemoved);
                // bitmap and icon, AknsUtils::CreateIconLC doesn't specify the order
                CleanupStack::Pop(2);
                bitmapArg = bitamp;
                maskArg = mask;
            }
            else {
                CleanupStack::PopAndDestroy(2);
            }
            CleanupStack::PopAndDestroy(fileNameFromApparc);
        }
    }
}

// --------------------------------------------------------------------------
// CTsFswDataList::GetAppIconL
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::IsHiddenUid(TUid uid)
{
    TInt result = mHiddenUids.Find(uid);
    return result >= 0 ? ETrue : EFalse;
}

// --------------------------------------------------------------------------
// CTsFswDataList::FindEntry
// --------------------------------------------------------------------------
//
TInt CTsFswDataList::FindEntry(const RTsFswArray& list, const TTsEntryKey& key) const
{
    TInt pos(KErrNotFound);
    TInt dataCount = list.Count();
    for (TInt entryIdx = 0; entryIdx < dataCount; ++entryIdx) {
        if (list[entryIdx]->Key() == key) {
            pos = entryIdx;
            break;
        }
    }
    return pos;
}

// --------------------------------------------------------------------------
// CTsFswDataList::SetScreenshotL
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::SetScreenshotL(const CFbsBitmap* bitmap, UpdatePriority priority, TInt wgId)
{
    RArray<RWsSession::TWindowGroupChainInfo> allWgIds;
    CleanupClosePushL(allWgIds);
    User::LeaveIfError(mWsSession.WindowGroupList(0, &allWgIds));
    TTsEntryKey key = TsEntryKeyGeneraror::GenerateL(wgId, allWgIds.Array());
    TInt pos = FindEntry(mData, key);
    TBool updated(EFalse);
    if (pos >= 0) {
        updated = mData[pos]->SetScreenshotL(bitmap, priority);
    }
    else {
        User::Leave(KErrNotFound);
    }
    CleanupStack::PopAndDestroy(&allWgIds);
    return updated;
}

// --------------------------------------------------------------------------
// CTsFswDataList::RemoveScreenshotL
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::RemoveScreenshotL(TInt wgId)
{
    RArray<RWsSession::TWindowGroupChainInfo> allWgIds;
    CleanupClosePushL(allWgIds);
    User::LeaveIfError(mWsSession.WindowGroupList(0, &allWgIds));
    TTsEntryKey key = TsEntryKeyGeneraror::GenerateL(wgId, allWgIds.Array());
    TInt pos = FindEntry(mData, key);
    TBool updated(EFalse);
    if (pos >= 0) {
        updated = mData[pos]->RemoveScreenshot();
    }
    else {
        User::Leave(KErrNotFound);
    }
    CleanupStack::PopAndDestroy(&allWgIds);
    return updated;
}

// --------------------------------------------------------------------------
// CTsFswDataList::EstablishOrder
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::EstablishOrder(const RArray<TTsEntryKey>& keyList)
{
    TBool changed(EFalse);
    __ASSERT_ALWAYS(mData.Count() == keyList.Count(), User::Panic(_L("EstablishOrder 1"), KErrBadHandle) );
    for (TInt i = 0; i < keyList.Count(); i++) {
        const TTsEntryKey& currentdataKey = mData[i]->Key();
        const TTsEntryKey& referenceKey = keyList[i];
        if (!(currentdataKey == referenceKey)) {
            TInt foundPos = FindEntry(mData, referenceKey);
            __ASSERT_ALWAYS(foundPos>=0, User::Panic(_L("EstablishOrder 2"), KErrBadHandle) );
            CTsFswEntry* entry = mData[foundPos];
            mData.Remove(foundPos);
            mData.Insert(entry, i);
            changed = ETrue;
        }
    }
    return changed;
}

// --------------------------------------------------------------------------
// CTsFswDataList::GetAllowedUidsL
// --------------------------------------------------------------------------
//
void CTsFswDataList::GetAllowedUidsL()
{
    mAllowedUids.Close();
    TApaAppInfo* appInfo = new (ELeave) TApaAppInfo();
    CleanupStack::PushL(appInfo);
    TApaAppCapabilityBuf appCap;

    User::LeaveIfError(mAppArcSession.GetAllApps(0));
    // for every application get uid, hidden and missing attribute
    // and add to aArray.
    while (KErrNone == mAppArcSession.GetNextApp(*appInfo)) {
        TUid uid = appInfo->iUid;
        User::LeaveIfError(mAppArcSession.GetAppCapability(appCap, uid));
        // do not add hidden app and with null uid.
        if (!appCap().iAppIsHidden && !IsHiddenUid(uid) && uid.iUid) {
            mAllowedUids.AppendL(uid);
        }
    }
    CleanupStack::PopAndDestroy(appInfo);
}

// end of file
