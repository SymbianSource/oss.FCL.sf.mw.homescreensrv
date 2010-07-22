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

#include "tsdatalist.h"
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
/**
 * Two-phased constructor.
 */
CTsDataList* CTsDataList::NewL(MTsResourceManager& resources,
                                     MTsWindowGroupsMonitor &monitor, 
                                     MHsDataObserver& observer)
{
    CTsDataList* self = new (ELeave) CTsDataList(resources, monitor, observer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// --------------------------------------------------------------------------
/**
 * Constructor.
 */
CTsDataList::CTsDataList(MTsResourceManager& resources,
                         MTsWindowGroupsMonitor &monitor, 
                         MHsDataObserver& observer) 
:
    CTsWindowGroupsObserver(monitor),
    mResources(resources),
    mObserver(observer)
{
}

// --------------------------------------------------------------------------
/*
 * Destructor
 */
CTsDataList::~CTsDataList()
{
    mData.ResetAndDestroy();
    mHiddenUids.Close();
    mAllowedUids.Close();
    RFbsSession::Disconnect();
    RAknIconSrvClient::Disconnect();
}

// --------------------------------------------------------------------------
/**
 * Performs 2nd phase construction.
 */
void CTsDataList::ConstructL()
{
    BaseConstructL();
    mHiddenUids.AppendL(KHsApplicationUid);
    User::LeaveIfError(RFbsSession::Connect());
    RAknIconSrvClient::Connect();
}

// --------------------------------------------------------------------------
/**
 * Returns a reference to the current content.
 * Also performs sanity checks, e.g. associates application icons
 * when no screenshot has been received.
 * @return  ref to content array
 */
const RTsFswArray& CTsDataList::Data() const
{
    return mData;
}

// --------------------------------------------------------------------------
/**
 * Interface implementation
 * @see MTsWindowGroupsObserver HandleWindowGroupChanged
 */
void CTsDataList::HandleWindowGroupChanged(MTsResourceManager &, 
                                              const TArray<RWsSession::TWindowGroupChainInfo> &wgList)
{
    TRAP_IGNORE(RTsFswArray newAppsList;
                CleanupResetAndDestroyPushL(newAppsList);
                CollectAppsL(newAppsList, wgList);
                FitDataToList(newAppsList);
                CleanupStack::PopAndDestroy(&newAppsList));
    
}

// --------------------------------------------------------------------------
/**
 * Adds running apps to the list.
 * @param   appsList    array to add to
 */
void CTsDataList::CollectAppsL(RTsFswArray& appsList, 
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
        CApaWindowGroupName* windowName = CApaWindowGroupName::NewLC(mResources.WsSession(), key.WindowGroupId());
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
/**
 * Called from CollectTasksL for each entry in the task list.
 * @param   key       entry key
 * @param   appUid     application uid
 * @param   wgName     window group name or NULL
 * @param   newList    list to add to
 */
void CTsDataList::AddEntryL(const TTsEntryKey& key, const TUid& appUid,
    CApaWindowGroupName* wgName, RTsFswArray& newList)
{
    CTsEntry* entry = CTsEntry::NewLC(key, &mObserver);

    // check if present in old list and if yes then take some of the old data
    TBool found = ConsiderOldDataL(key);

    // if not present previously then find out app name
    // and check if screenshot is already available
    if (!found) {
        entry->SetAppUid(appUid);
        HBufC* name = FindAppNameLC(wgName, appUid, key.WindowGroupId());
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
/**
 * Checks if there is an entry for same app in the content list.
 * If yes then it takes some of the data for the entry that
 * will correspond to the same app in the refreshed content list.
 * @param   key      new key in content list
 * @return  ETrue if app was found
 */
TBool CTsDataList::ConsiderOldDataL(const TTsEntryKey& key)
{
    for (TInt entryIdx = 0, oldCount = mData.Count(); entryIdx < oldCount; ++entryIdx) {
        if (mData[entryIdx]->Key() == key) {
            return ETrue;
        }
    }
    return EFalse;
}

// --------------------------------------------------------------------------
/**
 * Finds out the application name.
 * @param   windowName window group name or NULL
 * @param   appUId     application uid
 * @param   wgId       window group id
 * @return  application name, ownership transferred to caller
 */
HBufC* CTsDataList::FindAppNameLC(CApaWindowGroupName* windowName, const TUid& appUid, TInt wgId)
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
/**
 * Fit existing class contained data list into give one.
 * Data is being changed with application type consideration that is based 
 * on aConsiderWidgets param. 
 * Function removes or add entries into data depend on given list.
 * @param   listToFit          list with actual data  
 */
void CTsDataList::FitDataToList(RTsFswArray& listToFit)
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
/**
 * Checks if there is an entry for same app in the given list.
 * @param   entry      entry
 * @param   newList    ref to list
 * @return  ETrue if app was found
 */

TBool CTsDataList::CheckIfExists(const CTsEntry& entry, const RTsFswArray& list) const
{
    TBool exists(EFalse);
    TInt pos = FindEntry(list, entry.Key());
    if (pos >= 0) {
        exists = ETrue;
    }
    return exists;
}

// --------------------------------------------------------------------------
/**
 * Retrieves the bitmap/mask for the icon of the given app.
 * @param   appUid application uid
 * @param   bitmapArg bitmap ptr, ownership transferred to caller, or NULL
 * @param   maskArg   mask ptr, ownership transferred to caller, or NULL
 */
void CTsDataList::GetAppIconL(const TUid& aAppUid, CFbsBitmap*& bitmapArg, CFbsBitmap*& maskArg)
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
/**
 * Checks if given uid is on hidden list
 * @param   aUid uid to be checked
 * @return  ETrue if uid is on hidden list
 */
TBool CTsDataList::IsHiddenUid(TUid uid)
{
    return mHiddenUids.Find(uid) >= 0 ? ETrue : EFalse;
}

// --------------------------------------------------------------------------
/**
 * Finds entry in array
 * @param   list list to find
 * @param   key finding key
 * @return   position or KErrNotFound
 */
TInt CTsDataList::FindEntry(const RTsFswArray& list, const TTsEntryKey& key) const
{
    TInt pos(KErrNotFound);
    for (TInt entryIdx = 0; entryIdx < list.Count(); ++entryIdx) {
        if (list[entryIdx]->Key() == key) {
            pos = entryIdx;
            break;
        }
    }
    return pos;
}
// --------------------------------------------------------------------------
/**
 * Set screenshot 
 */
void CTsDataList::UpdateL(TInt key, const CFbsBitmap& data, TInt /*param*/, TInt priority)
{
    const TInt pos = FindEntry(mData, GenerateKeyL(key));
    User::LeaveIfError(pos);
    mData[pos]->SetScreenshotL(data, static_cast<UpdatePriority>(priority));
}

// --------------------------------------------------------------------------
/**
 * Change visibility status 
 */
void CTsDataList::UpdateL(TInt key, const Visibility& data, TInt /*param*/)
{
    const TInt pos = FindEntry(mData, GenerateKeyL(key));
    User::LeaveIfError(pos);
    data == mData[pos]->GetVisibility() ? User::Leave(KErrInUse) : 
                                          mData[pos]->SetVisibility(data);
    mObserver.DataChanged();
}

// --------------------------------------------------------------------------
/**
 * Removes screenshot 
 */
void CTsDataList::RemoveL(TInt key, TInt /*param*/)
{
    TInt pos = FindEntry(mData, GenerateKeyL(key));
    User::LeaveIfError(pos);
    mData[pos]->RemoveScreenshotL();
}

// --------------------------------------------------------------------------
/**
 * Establish entry order accridung to aKeyList, all keys MUST be in iData
 * @param   keyList reference key list
 * @return   ETrue if changes occured
 */
TBool CTsDataList::EstablishOrder(const RArray<TTsEntryKey>& keyList)
{
    TBool changed(EFalse);
    __ASSERT_ALWAYS(mData.Count() == keyList.Count(), User::Panic(_L("EstablishOrder 1"), KErrBadHandle) );
    for (TInt i = 0; i < keyList.Count(); i++) {
        const TTsEntryKey& currentdataKey = mData[i]->Key();
        const TTsEntryKey& referenceKey = keyList[i];
        if (!(currentdataKey == referenceKey)) {
            TInt foundPos = FindEntry(mData, referenceKey);
            __ASSERT_ALWAYS(foundPos>=0, User::Panic(_L("EstablishOrder 2"), KErrBadHandle) );
            CTsEntry* entry = mData[foundPos];
            mData.Remove(foundPos);
            mData.Insert(entry, i);
            changed = ETrue;
        }
    }
    return changed;
}

// --------------------------------------------------------------------------
/**
 * Gets allowed uids, tries to filter non GUI application 
 */
TBool CTsDataList::VerifyApplicationL(TUid uid)
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
/**
 * Function generate task key using window group id
 * @param wgId - window group id of running application
 * @param entry key 
 */
TTsEntryKey CTsDataList::GenerateKeyL(TInt wgId)
{
    RArray<RWsSession::TWindowGroupChainInfo> allWgIds;
   CleanupClosePushL(allWgIds);
   User::LeaveIfError(mResources.WsSession().WindowGroupList(0, &allWgIds));
   const TTsEntryKey key(TsEntryKeyGeneraror::GenerateL(wgId, allWgIds.Array()));
   CleanupStack::PopAndDestroy(&allWgIds);
   return key;
}

// --------------------------------------------------------------------------
/**
 * Analyse and compress lookup table id needed
 * @param array - lookup table that has to be checked
 * 
 */
void CTsDataList::CompressLookupTable(RArray<TUid> &array)
{
    while(KMaxLookupSize < array.Count()) {
        array.Remove(0);
    }
}

// --------------------------------------------------------------------------
/**
 * Change priority of an item in the lookup table
 * @param array - look up table
 * @param offset - index of an item in the table
 */
void CTsDataList::UpdateLookupTableL(RArray<TUid> &array, TInt offset)
{
    const TUid uid(array[offset]);
    array.Remove(offset);
    array.InsertL(uid, array.Count());
}
// end of file
