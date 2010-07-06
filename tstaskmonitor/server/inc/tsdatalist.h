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
* Description:  Non-closeable app list class
 *
*/


#ifndef TSDATALIST_H
#define TSDATALIST_H

#include <e32base.h>
#include <tswindowgroupsobserver.h>
#include "tsentry.h"
#include "tsdatastorage.h"
#include "tsdataobserver.h"

class CApaWindowGroupName;
class CFbsBitmap;

NONSHARABLE_CLASS( CTsDataList ) : public CTsWindowGroupsObserver,
                                   public MTsDataStorage
    {
public:
    static CTsDataList* NewL(MTsResourceManager& resources, 
                             MTsWindowGroupsMonitor &monitor, 
                             MHsDataObserver& observer);

    ~CTsDataList();

private:
    CTsDataList(MTsResourceManager& resources,
                MTsWindowGroupsMonitor &monitor, 
                MHsDataObserver& observer);

    void ConstructL();

public:
    const RTsFswArray& Data() const;

    void HandleWindowGroupChanged(MTsResourceManager &,
                                  const TArray<RWsSession::TWindowGroupChainInfo> &);

    TBool IsHiddenUid( TUid uid );

    void UpdateL(TInt key,const CFbsBitmap& data, TInt param, TInt priority);
    
    void UpdateL(TInt key, const Visibility& data, TInt param);

    void RemoveL(TInt key, TInt param);

private:
    void CollectAppsL(RTsFswArray& appsList,
                      const TArray<RWsSession::TWindowGroupChainInfo> &wgList);

    void AddEntryL(const TTsEntryKey& key, 
                   const TUid& appUid,
                   CApaWindowGroupName* wgName,
                   RTsFswArray& newList );

    TBool ConsiderOldDataL( const TTsEntryKey& key );

    HBufC* FindAppNameLC(CApaWindowGroupName* windowName,
                         const TUid& appUid,
                         TInt wgId );

    void FitDataToList( RTsFswArray& listToFit);

    TBool CheckIfExists(const CTsEntry& entry,
                        const RTsFswArray& newList) const;

    void GetAppIconL(const TUid& appUid,
                     CFbsBitmap*& bitmapArg, 
                     CFbsBitmap*& maskArg);

    TInt FindEntry(const RTsFswArray& list, const TTsEntryKey& key ) const;

    TBool EstablishOrder(const RArray<TTsEntryKey>& keyList);

    TBool VerifyApplicationL(TUid);

    TTsEntryKey GenerateKeyL(TInt);

    void CompressLookupTable(RArray<TUid> &array);

    void UpdateLookupTableL(RArray<TUid> &array, TInt offset);

private:
    /**
     * Resource manager. Not own
     */
    MTsResourceManager& mResources;
    
    MHsDataObserver &mObserver;
    
    RTsFswArray mData; // current fsw content, i.e. the task list

    // list of hidden uids
    RArray<TUid> mHiddenUids;
    
    // list of allowed uids
    RArray<TUid> mAllowedUids;
    };

#endif //TSDATALIST_H
