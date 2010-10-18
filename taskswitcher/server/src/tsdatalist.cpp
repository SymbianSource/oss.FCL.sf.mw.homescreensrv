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
#include <fbs.h>
#include <apgcli.h>

#include "tsutils.h"
#include "tsdatalist.h"
#include "tsscreenshotmsg.h"
#include "tsunregscreenshotmsg.h"
#include "tsresourcemanager.h"
#include "tsrunningapp.h"
#include "tsrunningappstorage.h"
#include "tsiconprovider.h"
#include "tsidlist.h"

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
/**
 * Two-phased constructor.
 */
CTsDataList* CTsDataList::NewL( MTsResourceManager& aResources,
                                MTsDataObserver& aObserver,
                                TsEnv& aEnv )
    {
    CTsDataList* self = new (ELeave) CTsDataList( aResources, 
                                                  aObserver,
                                                  aEnv);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
/**
 * Constructor.
 */
CTsDataList::CTsDataList(MTsResourceManager& aResources,
                         MTsDataObserver& aObserver,
                         TsEnv& aEnv) 
:
    CTsWindowGroupsObserverBase( aResources.WsMonitor() ),
    iResources( aResources ),
    iObserver( aObserver ),
    iEnv( aEnv )
    {
    }

// --------------------------------------------------------------------------
/*
 * Destructor
 */
CTsDataList::~CTsDataList()
    {
    iData.ResetAndDestroy();
    iVisibleData.Close();
    }

// --------------------------------------------------------------------------
/**
 * Performs 2nd phase construction.
 */
void CTsDataList::ConstructL()
    {
    BaseConstructL();
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
    return iVisibleData;
    }

// --------------------------------------------------------------------------
/**
 * Interface implementation
 * @see MTsWindowGroupsObserver HandleWindowGroupChanged
 */
void CTsDataList::HandleWindowGroupChanged(
                      MTsResourceManager &/*aResources*/, 
                      const MTsRunningApplicationStorage& aStorage)
    {
    TRAP_IGNORE(RDebug::Print(_L("[TaskSwitcher] processing started"));
                RTsFswArray newAppsList;
                TaskSwitcher::CleanupResetAndDestroyPushL(newAppsList);
                CollectAppsL(newAppsList, aStorage);
                RDebug::Print(_L("[TaskSwitcher] gathered app info"));
                FitDataToListL(newAppsList);
                CleanupStack::PopAndDestroy(&newAppsList);
                RDebug::Print(_L("[TaskSwitcher] processing finished"));
                );
    }

// --------------------------------------------------------------------------
/**
 * Adds running apps to the list.
 * @param aAppsList array to add to
 */
void CTsDataList::CollectAppsL(RTsFswArray& aAppsList, 
                               const MTsRunningApplicationStorage& aStorage)
    {
    for( TInt i(0); i < aStorage.Count(); ++i )
        {
        TTsEntryKey key;
        TInt err = aStorage.GenerateKey(key, aStorage[i].WindowGroupId());
        //skip this entry if it is already on list or generate key failed
        if( err!=KErrNone || FindEntry( aAppsList, key ) >= 0 ) 
            {
            continue;
            }

        // get screen number (-1=console, 0=main screen, 1=cover ui)
        TInt appScreen = 0;
        TInt scrNumErr = 
            iResources.ApaSession().GetDefaultScreenNumber( appScreen, 
                                                            aStorage[i].Uid() );
        
        if( aStorage[i].Uid().iUid && 
            MTsRunningApplication::None == aStorage[i].HideMode()&& 
            (appScreen == 0 || appScreen == -1) && 
            scrNumErr == KErrNone )
            {
            AddEntryL(key, aStorage[i], aAppsList );
            }
        }
    }

// --------------------------------------------------------------------------
/**
 * Called from CollectTasksL for each entry in the task list.
 * @param   aRunningApp  running application entry
 * @param   aNewList   list to add to
 */
void CTsDataList::AddEntryL( const TTsEntryKey aKey, 
                             const MTsRunningApplication& aRunningApp, 
                             RTsFswArray& aNewList )
    {
    CTsRunningAppEntry* entry = CTsRunningAppEntry::NewLC(aKey,
                                                          aRunningApp,
                                                          iResources,
                                                          iObserver, 
                                                          &iEnv );
    // add to new list, ownership is transferred
    aNewList.AppendL( entry );
    CleanupStack::Pop( entry );
    }

// --------------------------------------------------------------------------
/**
 * Checks if there is an entry for same app in the content list.
 * If yes then it takes some of the data for the entry that
 * will correspond to the same app in the refreshed content list.
 * @param   aKey      new key in content list
 * @return  ETrue if app was found
 */
TBool CTsDataList::ConsiderOldData( const TTsEntryKey& aKey )
    {
    for(TInt entryIdx = 0, oldCount = iData.Count(); entryIdx < oldCount; ++entryIdx) 
        {
        if (iData[entryIdx]->Key().Key() == aKey.Key()) 
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
/**
 * Fit existing class contained data list into give one.
 * Data is being changed with application type consideration that is based 
 * on aConsiderWidgets param. 
 * Function removes or add entries into data depend on given list.
 * @param   aListToFit          list with actual data  
 */
void CTsDataList::FitDataToListL( RTsFswArray& aListToFit )
    {
    TBool changed = EFalse;
    TInt dataCount = iData.Count();
    
    //remove items that dont't exists in newly collected list
    for (TInt i = dataCount - 1; i >= 0; --i) 
        {
        if( !CheckIfExistsL( *iData[i], aListToFit ) )
            {
            delete iData[i];
            iData.Remove( i );
            changed = ETrue;
            }
        }
    RArray<TTsEntryKey> allKeys;
    
    //add new items at start
    for(TInt i = aListToFit.Count() - 1; i >= 0; --i)
        {        
        User::LeaveIfError( allKeys.Insert(aListToFit[i]->Key(), 0) );
        if( !CheckIfExistsL( *aListToFit[i], iData ) ) 
            {
            HideEntryIfNotAllowed( aListToFit[i] );
            User::LeaveIfError( iData.Insert( aListToFit[i], 0 ) );
            iData[0]->RefreshTimestamp();
            aListToFit[i] = 0;
            changed = ETrue;
            }
        }
    //establish order
    TBool orderChanged = EstablishOrderL( allKeys );
    //update entries data
    TBool dataChanged = UpdateEntryData( aListToFit );
    RebuildVisibleDataListL();
    if( changed || orderChanged || dataChanged )
        {
        iObserver.DataChanged();
        }
    allKeys.Close();
    }

// --------------------------------------------------------------------------
/**
 * Checks if there is an entry for same app in the given list.
 * @param   aEntry      entry
 * @param   aList    ref to list
 * @return  ETrue if app was found
 */

TBool CTsDataList::CheckIfExistsL( const MTsEntry& aEntry, 
                                  const RTsFswArray& aList ) const
    {
    return 0 <= FindEntry( aList, aEntry.Key() );
    }

// --------------------------------------------------------------------------
TBool CTsDataList::IsSupported( TInt aFunction ) const
    {
    return ( RegisterScreenshotMessage == aFunction ||
             UnregisterScreenshotMessage == aFunction ||
             WindowGroupToBackgroundMessage == aFunction );
    }

// --------------------------------------------------------------------------
void CTsDataList::HandleDataL( TInt aFunction, RReadStream& aDataStream )
    {
    switch( aFunction )
        {
        case RegisterScreenshotMessage:
            RegisterScreenshotL( aDataStream );
            break;
        case UnregisterScreenshotMessage:
            UnregisterScreenshotL( aDataStream );
            break;
        case WindowGroupToBackgroundMessage:
            UpdateTaskTimestampL( aDataStream );
            break;
        }
    }

// --------------------------------------------------------------------------
MTsEntry& CTsDataList::FindL(TTsEntryKey& aKey)
    {
    MTsEntry* entry(0);
    for(TInt offset(0); 0 == entry && offset < iData.Count(); ++offset)
        {
        if(iData[offset]->Key().Key() == aKey.Key())
            {
            entry = iData[offset];
            }
        }
    User::LeaveIfNull(entry);
    return *entry;
    }

// --------------------------------------------------------------------------
void CTsDataList::RegisterScreenshotL( RReadStream& aDataStream )
    {
    CTsScreenshotMsg* msg = CTsScreenshotMsg::NewLC( aDataStream );
    const TInt pos = FindEntry(iData, GenerateKeyL(msg->WindowGroupId() ) );
    User::LeaveIfError(pos);
    iData[pos]->SetScreenshotL( msg->Screenshot(), msg->Priority(), msg->Rotation() );
    CleanupStack::PopAndDestroy(msg);
    }
// --------------------------------------------------------------------------
void CTsDataList::UnregisterScreenshotL(RReadStream& aDataStream)
{
    CTsUnregisterScreenshotMsg* msg = 
        CTsUnregisterScreenshotMsg::NewLC(aDataStream);
    const TInt pos = FindEntry( iData, GenerateKeyL(msg->windowGroupId() ) );
    User::LeaveIfError(pos);
    iData[pos]->RemoveScreenshotL();
    CleanupStack::PopAndDestroy(msg);
}

// --------------------------------------------------------------------------
void CTsDataList::UpdateTaskTimestampL( RReadStream& aDataStream )
    {
    const TInt wgId = aDataStream.ReadInt32L();
    const TInt pos = FindEntry( iData, GenerateKeyL( wgId ) );
    User::LeaveIfError( pos );

    iData[pos]->RefreshTimestamp();

    iObserver.DataChanged();
    }

// --------------------------------------------------------------------------
/**
 * Finds entry in array
 * @param   aList list to find
 * @param   aKey finding key
 * @return   position or KErrNotFound
 */
TInt CTsDataList::FindEntry( const RTsFswArray& aList, 
                             const TTsEntryKey& aKey ) const
    {
    TInt pos(KErrNotFound);
    for(TInt entryIdx = 0; 
        entryIdx < aList.Count() && KErrNotFound == pos; 
        ++entryIdx)
        {
        if (aList[entryIdx]->Key().Key() == aKey.Key())
            {
            pos = entryIdx;
            }
        }
    return pos;
    }

// --------------------------------------------------------------------------
/**
 * Establish entry order accridung to aKeyList, all keys MUST be in iData
 * @param   aKeyList reference key list
 * @return   ETrue if changes occured
 */
TBool CTsDataList::EstablishOrderL( const RArray<TTsEntryKey>& aKeyList )
    {
    TBool changed( EFalse );
    TInt lastChangedItem( KErrNotFound );
    __ASSERT_ALWAYS( iData.Count() == aKeyList.Count(), User::Panic(_L("EstablishOrder 1"), KErrBadHandle) );
    for (TInt i = 0; i < aKeyList.Count(); i++)
        {
        const TTsEntryKey& currentdataKey = iData[i]->Key();
        const TTsEntryKey& referenceKey = aKeyList[i];
        if( !(currentdataKey == referenceKey) )
            {
            TInt foundPos = FindEntry( iData, referenceKey );
            __ASSERT_ALWAYS(foundPos>=0,  User::Panic(_L("EstablishOrder 2"), KErrBadHandle));
            CTsRunningAppEntry* entry = iData[foundPos];
            iData.Remove( foundPos );
            iData.Insert( entry, i );
            changed = ETrue;
            lastChangedItem = i;
            }
        }
    for (TInt i = lastChangedItem; i >= 0; --i)
        {
        iData[i]->RefreshTimestamp();
        }
    return changed;
    }

// --------------------------------------------------------------------------
/**
 * Updates entry data on mData according to list
 * @param   aList reference entires list
 * @return   ETrue if changes occured
 */
TBool CTsDataList::UpdateEntryData( const RTsFswArray& aList )
{
    __ASSERT_ALWAYS( iData.Count() == aList.Count(), 
                     User::Panic(_L("UpdateEntryData 1"), KErrBadHandle) );
    TBool changed( EFalse );
    for( TInt i=0; i<aList.Count(); i++ )
        {
        if(aList[i]) 
            {
            __ASSERT_ALWAYS(iData[i]->Key() == aList[i]->Key(), 
                            User::Panic(_L("UpdateEntryData 2"), KErrBadHandle));
            if( iData[i]->IsClosable() != aList[i]->IsClosable() )
                {
                iData[i]->SetCloseableApp(aList[i]->IsClosable());
                changed = ETrue;
                }
            }
        }
    return changed;
}

// --------------------------------------------------------------------------
/**
 * Function generate task key using window group id
 * @param wgId - window group id of running application
 * @param entry key 
 */
TTsEntryKey CTsDataList::GenerateKeyL( TInt aWgId )
    {
    TTsEntryKey retVal;
    iResources.WsMonitor().Storage().GenerateKey(retVal, aWgId);
    return retVal;
    /*RArray<RWsSession::TWindowGroupChainInfo> allWgIds;
    CleanupClosePushL( allWgIds );
    User::LeaveIfError( iResources.WsSession().WindowGroupList( 0, &allWgIds ) );
    TTsEntryKey key;
    User::LeaveIfError(TsEntryKeyGeneraror::Generate(key, aWgId, allWgIds.Array()));
    CleanupStack::PopAndDestroy( &allWgIds );
    return key;*/
    }

// --------------------------------------------------------------------------
/**
 * Hides entrie if exist on mHiddenUids
 * @param   entry
 */
void CTsDataList::HideEntryIfNotAllowed( CTsRunningAppEntry* aEntry )
    {
    if( iResources.ApplicationsBlackList().IsPresent(aEntry->Uid().iUid) )
        {
        aEntry->SetHidden(ETrue);
        }
    }

// --------------------------------------------------------------------------
void CTsDataList::RebuildVisibleDataListL()
    {
    iVisibleData.Reset();
    RArray<TInt> visibleItems(iData.Count() ? iData.Count() : 1);
    CleanupClosePushL(visibleItems);
    for( TInt iter = 0; iter < iData.Count(); ++iter )
        {
        if( MTsRunningApplication::None == iData[iter]->HideMode() )
            {
            visibleItems.AppendL(iter);
            }
        }
    for( TInt iter = 0; iter < visibleItems.Count(); ++iter ) 
        {
        iVisibleData.AppendL( iData[visibleItems[iter]] );
        }
    CleanupStack::PopAndDestroy(&visibleItems);
    }
// end of file
