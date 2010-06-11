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


#ifndef TSFSWDATALIST_H
#define TSFSWDATALIST_H

#include <e32base.h>
#include <tswindowgroupsobserver.h>
#include "tsfswentry.h"
#include "tsdataobserver.h"

class CApaWindowGroupName;
class CFbsBitmap;

/**
 * Engine's data list.
 */
NONSHARABLE_CLASS( CTsFswDataList ) : public CTsWindowGroupsObserver
    {
public:
    /**
     * Two-phased constructor.
     */
    static CTsFswDataList* NewL(MTsResourceManager& resources, 
                                MTsWindowGroupsMonitor &monitor, 
                                MHsDataObserver& observer);

    /*
     * Destructor
     */
    ~CTsFswDataList();

private:
    /**
     * Constructor.
     */
    CTsFswDataList(MTsResourceManager& resources,
                   MTsWindowGroupsMonitor &monitor, 
                   MHsDataObserver& observer);

    /**
     * Performs 2nd phase construction.
     */
    void ConstructL();

public:
    /**
     * Returns a reference to the current content.
     * Also performs sanity checks, e.g. associates application icons
     * when no screenshot has been received.
     * @return  ref to content array
     */
    const RTsFswArray& FswDataL();

    /**
     * Interface implementation
     * @see MTsWindowGroupsObserver HandleWindowGroupChanged
     */
    void HandleWindowGroupChanged(MTsResourceManager &,
                                  const TArray<RWsSession::TWindowGroupChainInfo> &);
    
    /**
     * Checks if given uid is on hidden list
     * @param   aUid uid to be checked
     * @return  ETrue if uid is on hidden list
     */    
    TBool IsHiddenUid( TUid uid );

    /**
     * Set screenshot 
     */
    void SetScreenshotL(const CFbsBitmap* bitmap, UpdatePriority priority, TInt wgId);
    
    /**
     * Removes screenshot 
     */    
    void RemoveScreenshotL(TInt wgId);
    
private:
    /**
     * Adds running apps to the list.
     * @param   appsList    array to add to
     */
    void CollectAppsL(RTsFswArray& appsList,
                      const TArray<RWsSession::TWindowGroupChainInfo> &wgList);

    /**
     * Called from CollectTasksL for each entry in the task list.
     * @param   key       entry key
     * @param   appUid     application uid
     * @param   wgName     window group name or NULL
     * @param   newList    list to add to
     */
    void AddEntryL(const TTsEntryKey& key, 
                   const TUid& appUid,
                   CApaWindowGroupName* wgName,
                   RTsFswArray& newList );

    /**
     * Checks if there is an entry for same app in the content list.
     * If yes then it takes some of the data for the entry that
     * will correspond to the same app in the refreshed content list.
     * @param   key      new key in content list
     * @return  ETrue if app was found
     */
    TBool ConsiderOldDataL( const TTsEntryKey& key );

    /**
     * Finds out the application name.
     * @param   windowName window group name or NULL
     * @param   appUId     application uid
     * @param   wgId       window group id
     * @return  application name, ownership transferred to caller
     */
    HBufC* FindAppNameLC(CApaWindowGroupName* windowName,
                         const TUid& appUid,
                         TInt wgId );


    /**
     * Fit existing class contained data list into give one.
     * Data is being changed with application type consideration that is based 
     * on aConsiderWidgets param. 
     * Function removes or add entries into data depend on given list.
     * @param   listToFit          list with actual data  
     */
    void FitDataToList( RTsFswArray& listToFit);

    /**
     * Checks if there is an entry for same app in the given list.
     * @param   entry      entry
     * @param   newList    ref to list
     * @return  ETrue if app was found
     */
    TBool CheckIfExists( const CTsFswEntry& entry,
            const RTsFswArray& newList ) const;
            
    /**
     * Retrieves the bitmap/mask for the icon of the given app.
     * @param   appUid application uid
     * @param   bitmapArg bitmap ptr, ownership transferred to caller, or NULL
     * @param   maskArg   mask ptr, ownership transferred to caller, or NULL
     */
    void GetAppIconL(const TUid& appUid,
                     CFbsBitmap*& bitmapArg, 
                     CFbsBitmap*& maskArg);

    /**
     * Finds entry in array
     * @param   list list to find
     * @param   key finding key
     * @return   position or KErrNotFound
     */
    TInt FindEntry(const RTsFswArray& list, const TTsEntryKey& key ) const;

    /**
     * Establish entry order accridung to aKeyList, all keys MUST be in iData
     * @param   keyList reference key list
     * @return   ETrue if changes occured
     */
    TBool EstablishOrder(const RArray<TTsEntryKey>& keyList);
    
    /**
     * Gets allowed uids, tries to filter non GUI application 
     */
    TBool VerifyApplicationL(TUid);
    
    /**
     * Function generate task key using window group id
     * @param wgId - window group id of running application
     * @param entry key 
     */
    TTsEntryKey GenerateKeyL(TInt);
    
    /**
     * Analyse and compress lookup table id needed
     * @param array - lookup table that has to be checked
     * 
     */
    void CompressLookupTable(RArray<TUid> &array);
    
    /**
     * Change priority of an item in the lookup table
     * @param array - look up table
     * @param offset - index of an item in the table
     */
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

#endif //TSFSWDATALIST_H
