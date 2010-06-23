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
#include "tsentrykeygenerator.h"
#include <mmf/common/mmfcontrollerpluginresolver.h> // for CleanupResetAndDestroyPushL
#include <apgwgnam.h>
#include <bitstd.h>
#include <AknIconUtils.h> // avkon
#include <apgicnfl.h> // fbsbitmap
#include <AknIconSrvClient.h> 
#include <fbs.h>
#include <apgwgnam.h>


// size for the created app icons
const TInt KAppIconWidth = 128;
const TInt KAppIconHeight = 128;

//uids to be hidden
const TUid KHsApplicationUid = { 0x20022F35 };

const TInt KMaxLookupSize(75);

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CTsFswDataList::NewL
// --------------------------------------------------------------------------
//
CTsFswDataList* CTsFswDataList::NewL(MTsResourceManager& resources,
                                     MTsWindowGroupsMonitor &monitor, 
                                     MHsDataObserver& observer)
{
    CTsFswDataList* self = new (ELeave) CTsFswDataList(resources, monitor, observer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// --------------------------------------------------------------------------
// CTsFswDataList::CTsFswDataList
// --------------------------------------------------------------------------
//
CTsFswDataList::CTsFswDataList(MTsResourceManager& resources,
                               MTsWindowGroupsMonitor &monitor, 
                               MHsDataObserver& observer) 
:
    CTsWindowGroupsObserver(monitor),
    mResources(resources),
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
    BaseConstructL();
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
void CTsFswDataList::HandleWindowGroupChanged(MTsResourceManager &, 
                                              const TArray<RWsSession::TWindowGroupChainInfo> &wgList)
{
    TRAP_IGNORE(RTsFswArray newAppsList;
                CleanupResetAndDestroyPushL(newAppsList);
                CollectAppsL(newAppsList, wgList);
                FitDataToList(newAppsList);
                CleanupStack::PopAndDestroy(&newAppsList));
    
}

// --------------------------------------------------------------------------
// CTsFswDataList::
// --------------------------------------------------------------------------
//
void CTsFswDataList::CollectAppsL(RTsFswArray& appsList, 
                                  const TArray<RWsSession::TWindowGroupChainInfo> &wgList)
{
    TInt offset(KErrNotFound);
    for (TInt i = 0; i < wgList.Count(); ++i) {
        TTsEntryKey key = TsEntryKeyGeneraror::GenerateL(wgList[i].iId, wgList);
        //skip this entry if it is already on list
        if (FindEntry(appsList, key) >= 0) {
            continue;
        }

        // get window group name
        TInt wgId = key.mParentId;
        CApaWindowGroupName* windowName = CApaWindowGroupName::NewLC(mResources.WsSession(), wgId);
        TUid appUid = windowName->AppUid();
        
        //Check hidden applications
        if (KErrNotFound != (offset = mHiddenUids.Find(appUid))) {
            UpdateLookupTableL(mHiddenUids, offset);
        } else if (KErrNotFound != (offset = mAllowedUids.Find(appUid))) {
            UpdateLookupTableL(mAllowedUids, offset);
            AddEntryL(key, appUid, windowName, appsList);
        } else if(VerifyApplicationL(appUid)) {
            AddEntryL(key, appUid, windowName, appsList);
        }
        CleanupStack::PopAndDestroy(windowName);
    }
    CompressLookupTable(mHiddenUids);
    CompressLookupTable(mAllowedUids);
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
    mResources.ApaSession().GetAppInfo(info, appUid);
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
            TInt err = mResources.WsSession().GetWindowGroupClientThreadId(wgId, threadId);
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
void CTsFswDataList::FitDataToList(RTsFswArray& listToFit)
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
    if (changed || orderChanged) {
        mObserver.DataChanged();
    }
    allKeys.Close();
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
    TInt err = mResources.ApaSession().GetAppIcon(aAppUid, size, *apaMaskedBitmap);
    TInt iconsCount(0);
    if (err == KErrNone) {
        err = mResources.ApaSession().NumberOfOwnDefinedIcons(aAppUid, iconsCount);
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
        TInt err = mResources.ApaSession().GetAppIcon(aAppUid, fileNameFromApparc);
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
void CTsFswDataList::SetScreenshotL(const CFbsBitmap* bitmap, UpdatePriority priority, TInt wgId)
{
    TInt pos = FindEntry(mData, GenerateKeyL(wgId));
    User::LeaveIfError(pos);
    mData[pos]->SetScreenshotL(bitmap, priority);
}

// --------------------------------------------------------------------------
// CTsFswDataList::RemoveScreenshotL
// --------------------------------------------------------------------------
//
void CTsFswDataList::RemoveScreenshotL(TInt wgId)
{
    
    TInt pos = FindEntry(mData, GenerateKeyL(wgId));
    User::LeaveIfError(pos);
    mData[pos]->RemoveScreenshotL();
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
TBool CTsFswDataList::VerifyApplicationL(TUid uid)
{
    TBool retVal(EFalse);
    TApaAppInfo appInfo;
    TApaAppCapabilityBuf appCap;

    User::LeaveIfError(mResources.ApaSession().GetAllApps(0));
    // for every application get uid, hidden and missing attribute
    // and add to aArray.
    while (KErrNone == mResources.ApaSession().GetNextApp(appInfo)) {
        User::LeaveIfError(mResources.ApaSession().GetAppCapability(appCap, appInfo.iUid));
        if(!appCap().iAppIsHidden) {
            if (uid == appInfo.iUid) {
                retVal = ETrue;
                mAllowedUids.InsertL(appInfo.iUid, mAllowedUids.Count());
            }
        } else if(KErrNotFound == mHiddenUids.Find(appInfo.iUid)) {
            mHiddenUids.InsertL(appInfo.iUid, mHiddenUids.Count());
        }
    }
    if (EFalse == retVal && KErrNotFound == mHiddenUids.Find(uid)) {
        mHiddenUids.InsertL(uid, mHiddenUids.Count());
    }
    return retVal;
}

// --------------------------------------------------------------------------
// CTsFswDataList::GenerateKeyL(TInt)
// --------------------------------------------------------------------------
//
TTsEntryKey CTsFswDataList::GenerateKeyL(TInt wgId)
{
    RArray<RWsSession::TWindowGroupChainInfo> allWgIds;
   CleanupClosePushL(allWgIds);
   User::LeaveIfError(mResources.WsSession().WindowGroupList(0, &allWgIds));
   const TTsEntryKey key = TsEntryKeyGeneraror::GenerateL(wgId, allWgIds.Array());
   CleanupStack::PopAndDestroy(&allWgIds);
   return key;
}

// --------------------------------------------------------------------------
// CTsFswDataList::CompressLookupTable(RArray<TUid> &)
// --------------------------------------------------------------------------
//
void CTsFswDataList::CompressLookupTable(RArray<TUid> &array)
{
    while(KMaxLookupSize < array.Count()) {
        array.Remove(0);
    }
}

// --------------------------------------------------------------------------
// CTsFswDataList::UpdateLookupTableL(RArray<TUid> &, TInt)
// --------------------------------------------------------------------------
//
void CTsFswDataList::UpdateLookupTableL(RArray<TUid> &array, TInt offset)
{
    const TUid uid(array[offset]);
    array.Remove(offset);
    array.InsertL(uid, array.Count());
}
// end of file
