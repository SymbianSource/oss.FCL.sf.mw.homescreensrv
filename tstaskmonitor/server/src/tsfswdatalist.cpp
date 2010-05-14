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
#define __E32SVR_H__
#include "tsfswdatalist.h"
#include "tsfswengine.h"
#include <mmf/common/mmfcontrollerpluginresolver.h> // for CleanupResetAndDestroyPushL
#include <featmgr.h>
#include <apgwgnam.h>
#include <AknsUtils.h>
#include <gulicon.h>
#include <e32debug.h>
#include <bitstd.h>
#include <AknIconUtils.h> // avkon
#include <APGICNFL.H> // fbsbitmap
#include <akniconsrvclient.h> 

// size for the created app icons
const TInt KAppIconWidth = 128;
const TInt KAppIconHeight = 128;

//uids to be hidden
const TUid KHsApplicationUid = {0x20022F35};
const TUid KTelephoneUid = {0x100058B3};

// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// CTsFswDataList::NewL
// --------------------------------------------------------------------------
//
CTsFswDataList* CTsFswDataList::NewL(CTsFswEngine& aEngine)
    {
    CTsFswDataList* self = new (ELeave) CTsFswDataList(aEngine);
    CleanupStack::PushL (self );
    self->ConstructL ( );
    CleanupStack::Pop ( self );
    return self;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::CTsFswDataList
// --------------------------------------------------------------------------
//
CTsFswDataList::CTsFswDataList(CTsFswEngine& aEngine) :
    iEngine(aEngine)
    {
    }

// --------------------------------------------------------------------------
// CTsFswDataList::~CTsFswDataList
// --------------------------------------------------------------------------
//
CTsFswDataList::~CTsFswDataList( )
    {
    iData.ResetAndDestroy();
    iAppArcSession.Close();
    iWsSession.Close();
    iHiddenUids.Close();
    RFbsSession::Disconnect();
    RAknIconSrvClient::Disconnect();
    }

// --------------------------------------------------------------------------
// CTsFswDataList::ConstructL
// --------------------------------------------------------------------------
//
void CTsFswDataList::ConstructL( )
    {
    User::LeaveIfError( iWsSession.Connect() );
    User::LeaveIfError( iAppArcSession.Connect() );
    iHiddenUids.AppendL( KHsApplicationUid );
    iHiddenUids.AppendL( KTelephoneUid );
    User::LeaveIfError(RFbsSession::Connect());
    RAknIconSrvClient::Connect();
    }

// --------------------------------------------------------------------------
// CTsFswDataList::FswDataL
// --------------------------------------------------------------------------
//
const RTsFswArray& CTsFswDataList::FswDataL()
    {
    // check the dirty flag and refresh if needed
    if ( iTaskListDirty )
        {
        CollectTasksL();
        // dirty flag is cleared in the above call
        }

    return iData;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::CollectTasksL
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::CollectTasksL()
    {
    // clear dirty flag
    iTaskListDirty = EFalse;
    TBool changed = EFalse;
    
    RTsFswArray newAppsList;
    
    CleanupResetAndDestroyPushL(newAppsList);
    
    CollectAppsL(newAppsList);
    
    changed |= FitDataToListL(newAppsList, EFalse);
    
    CleanupStack::PopAndDestroy( &newAppsList );
    
    return changed;
    }


// --------------------------------------------------------------------------
// CTsFswDataList::SetDirty()
// --------------------------------------------------------------------------
//
void CTsFswDataList::SetDirty()
    {
    iTaskListDirty = ETrue;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::SetAppDataRefreshNeeded()
// --------------------------------------------------------------------------
//
void CTsFswDataList::SetAppDataRefreshNeeded()
    {
    iAppDataRefreshNeeded = ETrue;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::MoveEntryAtStart
// --------------------------------------------------------------------------
//
void CTsFswDataList::MoveEntryAtStart(TInt aAppId, TBool /*aWidget*/)
    {
    TInt appId(0);
    //check embeded case

    TInt wgId(0);
    CApaWindowGroupName::FindByAppUid(TUid::Uid(aAppId), iWsSession, wgId);
    TInt parentWgId = FindMostTopParentWgId( wgId );
    if( parentWgId != KErrNotFound )
        {
        TUid appUid = TUid::Uid(0);
        AppUidForWgId(parentWgId, appUid); 
        appId = appUid.iUid;
        }

    
    if( !appId )
    	{
		appId = aAppId;
    	}
    
    for ( TInt i = 0; i < iData.Count(); ++i )
       {
       if( iData[i]->AppUid().iUid == appId )
           {
           CTsFswEntry* entry = iData[i];
           iData.Remove(i);
           iData.Insert(entry, 0);
           iOrderChanged = ETrue;
           break;
           }
       }
    }

// --------------------------------------------------------------------------
// CTsFswDataList::
// --------------------------------------------------------------------------
//
void CTsFswDataList::CollectAppsL(RTsFswArray& aAppsList)
    {

    
    // update app data if needed
    // (usually on startup and when new apps might have been installed)
    if ( iAppDataRefreshNeeded )
        {

        iAppArcSession.GetAllApps();
        iAppDataRefreshNeeded = EFalse;
        }

    // get all window groups
    RArray<RWsSession::TWindowGroupChainInfo> allWgIds;
    CleanupClosePushL( allWgIds );
    User::LeaveIfError( iWsSession.WindowGroupList( 0, &allWgIds ) );

    TInt count = allWgIds.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        // ignore uninteresting entries (e.g. embedded apps)
        if ( allWgIds[i].iParentId > 0 )
            {
            continue;
            }
        
        // get window group name
        TInt wgId = allWgIds[i].iId;
        CApaWindowGroupName* windowName =
            CApaWindowGroupName::NewLC( iWsSession, wgId );
        TUid appUid = windowName->AppUid();
        
        // ignore entries with null uid
        if ( !appUid.iUid )
            {
            CleanupStack::PopAndDestroy( windowName );
            continue;
            }

        // add item to task list if it is not hidden
        if ( !windowName->Hidden() && !IsHiddenUid(appUid) )
            {
            AddEntryL(wgId, appUid, windowName, aAppsList, EFalse);
            }
        CleanupStack::PopAndDestroy( windowName );
        }
    CleanupStack::PopAndDestroy( &allWgIds );
    }

// --------------------------------------------------------------------------
// CTsFswDataList::AddEntryL
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::AddEntryL( TInt aWgId, const TUid& aAppUid,
        CApaWindowGroupName* aWgName, RTsFswArray& aNewList,
        TBool /*aIsWidget*/ )
    {
    TBool changed = EFalse;
    CTsFswEntry* entry = CTsFswEntry::NewLC();
    entry->SetWgId( aWgId );
    entry->SetAppUid( aAppUid );

    // check if present in old list and if yes then take some of the old data
    TBool found = ConsiderOldDataL( *entry, aAppUid, changed, aNewList );

    // if not present previously then find out app name
    // and check if screenshot is already available
    if ( !found )
        {
        // when adding a new entry to the list it is changed for sure
        changed = ETrue;
        HBufC* name = FindAppNameLC( aWgName, aAppUid, aWgId );
        if ( name )
            {
            entry->SetAppNameL( *name );
            }
        CleanupStack::PopAndDestroy( name );

        if ( aWgName )
            {
            entry->SetCloseableApp( !aWgName->IsSystem() );
            }
        CFbsBitmap* iconBitmap = NULL;
        CFbsBitmap* iconMask = NULL;
        GetAppIconL(aAppUid, iconBitmap, iconMask);    
        //transfer ownership to entry
        entry->SetAppIcon( iconBitmap, iconMask );        
        }        

    // add to new list, ownership is transferred
    aNewList.AppendL( entry );
    CleanupStack::Pop( entry );
    return changed;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::ConsiderOldDataL
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::ConsiderOldDataL( CTsFswEntry& aEntry,
        const TUid& aAppUid,
        TBool& aChanged,
        RTsFswArray& aNewList )
    {
    for ( TInt entryIdx = 0, oldCount = iData.Count();
            entryIdx < oldCount; ++entryIdx )
        {
        // Enough to check appuid, no need to bother with wgid as the
        // screenshot handle is up-to-date or will be updated later anyway.
        if ( iData[entryIdx]->AppUid() == aAppUid )
            {
            iData[entryIdx]->SetWgId( aEntry.WgId());
            // if positions do not match then the list is different than before
            if ( entryIdx != aNewList.Count() )
                {
                aChanged = ETrue;
                }
            CTsFswEntry* oldEntry = iData[entryIdx];
            aEntry.SetAppNameL( oldEntry->AppName() );            
            aEntry.SetCloseableApp( oldEntry->CloseableApp() );
            return ETrue;
            }
        }
    return EFalse;
    }


// --------------------------------------------------------------------------
// CTsFswDataList::FindParentWgId
// --------------------------------------------------------------------------
//
TInt CTsFswDataList::FindParentWgId( TInt aWgId )
    {
    TInt parent( KErrNotFound );
    RArray<RWsSession::TWindowGroupChainInfo> allWgIds;
    // Ask for window group list from RWsSession
    TInt error = iWsSession.WindowGroupList( 0, &allWgIds );
    if ( !error )
        {
        TInt count( allWgIds.Count() );
        for ( TInt i( 0 ); i < count; i++ )
            {
            RWsSession::TWindowGroupChainInfo info = allWgIds[i];
            if ( info.iId == aWgId && info.iParentId > 0)
                {
                parent = info.iParentId;
                break;
                }
            }
        }
    allWgIds.Close();
    return parent;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::FindMostTopParentWgId
// --------------------------------------------------------------------------
//
TInt CTsFswDataList::FindMostTopParentWgId( TInt aWgId )
    {
    TInt parent( KErrNotFound );
    parent = FindParentWgId( aWgId );
    if( parent != KErrNotFound)
        {
        TInt topParent = FindMostTopParentWgId(parent);
        if( topParent != KErrNotFound )
            {
            parent = topParent;
            }
        }
    return parent;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::FindAppNameLC
// --------------------------------------------------------------------------
//
HBufC* CTsFswDataList::FindAppNameLC( CApaWindowGroupName* aWindowName,
                                  const TUid& aAppUid,
                                  TInt aWgId )
    {
    //Retrieve the app name
    TApaAppInfo info;
    iAppArcSession.GetAppInfo( info, aAppUid );
    TPtrC caption = info.iShortCaption;

    HBufC* tempName = 0;
    if ( !caption.Length() && aWindowName ) // if not set - use thread name instead
        {
        if ( aWindowName->Caption().Length() )
            {
            tempName = aWindowName->Caption().AllocL();
            //put on cleanupstack after the if
            }
        else
            {
            TThreadId threadId;
            TInt err = iWsSession.GetWindowGroupClientThreadId(
                    aWgId, threadId );
            if ( err == KErrNone )
                {
                RThread thread;
                CleanupClosePushL( thread );
                err = thread.Open ( threadId );
                if ( err==KErrNone )
                    {
                    tempName = thread.Name().AllocL(); // codescanner::forgottoputptroncleanupstack
                    // tempName put on cleanupstack after the if
                    }
                CleanupStack::PopAndDestroy( &thread );
                }
            }
        }
    else
        {
        tempName = caption.AllocL();
        //put on cleanupstack after the if
        }
    CleanupStack::PushL( tempName );
    return tempName;
    }


// --------------------------------------------------------------------------
// CTsFswDataList::FitDataToListL
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::FitDataToListL(RTsFswArray& aListToFit,
        TBool /*aConsiderWidgets*/)
    {
    TBool changed = EFalse;
    TInt listCount = aListToFit.Count();
    TInt dataCount = iData.Count();
    

    //remove items that dont't exists in newly collected list      
    for (TInt i = dataCount -1; i >= 0; --i)
          {
          if ( !CheckIfExists(*iData[i], aListToFit) )
              {
              delete iData[i];
              iData.Remove(i);
              changed = ETrue;
              }
          }
      
    //add new item at start
    //for (TInt i = 0; i < aListToFit.Count(); ++i)
    for (TInt i = aListToFit.Count()-1; i >= 0; --i)
        {
        if (!CheckIfExists(*aListToFit[i], iData))
            {
            iData.Insert(aListToFit[i], 0);
            aListToFit[i] = 0;
            changed = ETrue;
            }
        }
    changed = changed || iOrderChanged;
    iOrderChanged = EFalse;
    return changed;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::CheckIfExists
// --------------------------------------------------------------------------
//
TBool CTsFswDataList::CheckIfExists(const CTsFswEntry& aEntry,
        const RTsFswArray& aList) const
    {
    TBool exists(EFalse);
    TInt dataCount = aList.Count();
    for (TInt entryIdx = 0; entryIdx < dataCount; ++entryIdx)
        {
        if (aList[entryIdx]->AppUid() == aEntry.AppUid())
            {
            exists = ETrue;
            break;
            }
        }
    return exists;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::AppUidForWgId
// --------------------------------------------------------------------------
//
TInt CTsFswDataList::AppUidForWgId( TInt aWgId, TUid& aUid )
    {
	TRAPD(err, aUid = AppUidForWgIdL( aWgId ) );
	return err;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::AppUidForWgIdL
// --------------------------------------------------------------------------
//
TUid CTsFswDataList::AppUidForWgIdL( TInt aWgId )
    {
    CApaWindowGroupName* windowName =
        CApaWindowGroupName::NewLC( iWsSession, aWgId );
    TUid appUid = windowName->AppUid();
    CleanupStack::PopAndDestroy( windowName );
    return appUid;
    }

// --------------------------------------------------------------------------
// CTsFswDataList::GetAppIconL
// --------------------------------------------------------------------------
//
void CTsFswDataList::GetAppIconL( const TUid& aAppUid,
        CFbsBitmap*& aBitmap, CFbsBitmap*& aMask )
    {
    aBitmap = aMask = NULL;

    TSize size( KAppIconWidth, KAppIconHeight );
    CApaMaskedBitmap* apaMaskedBitmap = CApaMaskedBitmap::NewLC();
    TInt err = iAppArcSession.GetAppIcon( aAppUid, size, *apaMaskedBitmap );
    TInt iconsCount(0);
    if ( err == KErrNone )
        {
        err = iAppArcSession.NumberOfOwnDefinedIcons(aAppUid, iconsCount);
        }
    
    if ( (err == KErrNone) && (iconsCount>0) )
        {
        aBitmap = static_cast<CFbsBitmap*>(apaMaskedBitmap);
        TInt maskHandle = apaMaskedBitmap->Mask()->Handle();
        aMask = new (ELeave) CFbsBitmap;
        aMask->Duplicate( maskHandle );
        CleanupStack::Pop( apaMaskedBitmap );
        }
    else
        {
        CleanupStack::PopAndDestroy( apaMaskedBitmap );
        HBufC* fileNameFromApparc = NULL;
        TInt err = iAppArcSession.GetAppIcon(aAppUid,fileNameFromApparc);        
        if ( err == KErrNone ) 
            {
            CleanupStack::PushL(fileNameFromApparc);
            CFbsBitmap *bitamp(0);
            CFbsBitmap *mask(0);
            TInt bitmapIndex = 0;
            TInt maskIndex = 1;    
            // it will change bitmap ids if it is mif (checking inside)
            AknIconUtils::ValidateLogicalAppIconId( *fileNameFromApparc, bitmapIndex, maskIndex );
            AknIconUtils::CreateIconLC( bitamp, mask, fileNameFromApparc->Des(), bitmapIndex, maskIndex );

            if (AknIconUtils::IsMifFile(*fileNameFromApparc)) \
                {
                AknIconUtils::DisableCompression(bitamp);
                AknIconUtils::SetSize(bitamp, TSize(KAppIconWidth, KAppIconHeight), EAspectRatioPreservedAndUnusedSpaceRemoved);                                
                // bitmap and icon, AknsUtils::CreateIconLC doesn't specify the order
                CleanupStack::Pop(2);                  
                aBitmap = bitamp;
                aMask = mask;
                }
            else
                {
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
TBool CTsFswDataList::IsHiddenUid( TUid aUid )
    {
    TInt result = iHiddenUids.Find( aUid );
    return result >= 0 ? ETrue : EFalse;
    }

// end of file
