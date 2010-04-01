/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Shrortcut settings plug-in model.
*
*/


#include <centralrepository.h>
#include <eikenv.h>
#include <menu2internalcrkeys.h>

#include "hs_app_caiscutsettings.h"
#include "hs_app_caiscutsettingsmodel.h"
#include "hs_app_caiscutsettingscontainer.h"
#include "hs_app_caiscutsettingsitem.h"
#include "hs_app_aiscutpluginprivatecrkeys.h"
#include "aiscutdefs.h"
#include "hs_app_aiscutappuidparser.h"
#include "hs_app_aiscutrepositorywatcher.h"

#include "debug.h"

LOCAL_C TInt CompareKey(const TUint32& aLeft, const TUint32& aRight)
{
    TBool softkey1 = (0 != ((aLeft  & 0xFFFF) >= EAiScutSoftKeyLeft));
    TBool softkey2 = (0 != ((aRight & 0xFFFF) >= EAiScutSoftKeyLeft));
    TUint32 left  = aLeft  & (KScutBitMaskThemeDefault & KScutBitMaskLocked);
    TUint32 right = aRight & (KScutBitMaskThemeDefault & KScutBitMaskLocked);

    // Softkey shortcuts are always sorted to be smallest.
    if (!(softkey1 && softkey2))
    {
        if (softkey1)
        {
            return -1;
        }
        else if (softkey2)
        {
            return 1;
        }
    }

    if (left < right)
    {
        return -1;
    }
    else if (left > right)
    {
        return 1;
    }

    return 0;
}



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutSettingsModel* CAiScutSettingsModel::NewL(CAiScutSettings& aPlugin,
    CCoeEnv* aEnv)
{
    CAiScutSettingsModel* self = new (ELeave) CAiScutSettingsModel(aPlugin, aEnv);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutSettingsModel::CAiScutSettingsModel(CAiScutSettings& aPlugin, CCoeEnv* aEnv) 
    : iPlugin(aPlugin), iEnv(aEnv)
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutSettingsModel::~CAiScutSettingsModel()
{
    ActivateObservers(EFalse);

    delete iSettingsNotifier;
    delete iRepository;

    delete iHiddenAppsNotifier;
    delete iHiddenAppsRepository;

    iHiddenApps.Close();

    delete iAppList;
    delete iBkmList;

    iSettings.ResetAndDestroy();
    iKeys.Reset();
    delete iFavItemList;
    iBookmarkDb.Close();
    iBookmarkSess.Close();
    iApaSession.Close();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutSettingsModel::ConstructL()
{
    User::LeaveIfError(iApaSession.Connect());
    User::LeaveIfError(iBookmarkSess.Connect());
    User::LeaveIfError(iBookmarkDb.Open(iBookmarkSess, KBrowserBookmarks));

    iRepository = CRepository::NewL(KCRUidShortcutItems);

    iHiddenAppsRepository = CRepository::NewL(KCRUidMenu);

    // Create cenrep key observer for monitoring settings changes
    // when theme is changed.
    iSettingsNotifier = CAiScutRepositoryWatcher::NewL(
        KCRUidShortcutItems,
        TCallBack(HandleShortcutsChanged, this),
        iRepository);

    iSettingsNotifier->StartListeningL();

    iHiddenAppsNotifier = CAiScutRepositoryWatcher::NewL(
        KCRUidMenu,
        KMenuHideApplication,
        CCenRepNotifyHandler::EStringKey,
        TCallBack(HandleHiddenAppsChanged, this),
        iHiddenAppsRepository);

    iHiddenAppsNotifier->StartListeningL();

    GetHiddenAppsL();

    iAppList = CAiScutSettingsAppList::NewL(iEnv, *this);
    iAppList->StartL();

    iBkmList = CAiScutSettingsBkmList::NewL(iEnv, this, *this);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutSettingsModel::SetSettingsKeys(RArray<TUint32>& aKeys)
    {
    iKeys.Reset();
    
    for( TInt i = 0; i < aKeys.Count(); i++ )
        {
        TUint32 value( aKeys[i] );
        
        iKeys.Append( value );
        }        
    }
    
// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutSettingsModel::UpdateSettingsL()
{
    if( !iPlugin.Activated() )
        {
        return;
        }
        
    iSettings.ResetAndDestroy();

    TInt keyCount( iKeys.Count() );

    if( keyCount > 0 )
        {
        for( TInt i = 0; i < keyCount; i++ )
            {
            CAiScutSettingsItem* setting = CAiScutSettingsItem::NewLC(
                *this, i, iKeys[i] );

            setting->ReadL( iRepository );

            User::LeaveIfError( iSettings.Append( setting ) );
            CleanupStack::Pop( setting );           
            }
                
        return;
        }
        
    RArray<TUint32> defaultKeys;
    CleanupClosePushL(defaultKeys);
    
    // Find the default keys and user defined keys.
    iRepository->FindL(
        KScutCenRepKeyThemeDefault, KScutCenRepKeyMask, defaultKeys);        

    TInt i;
    TUint32 defaultKey;
    TUint32 userKey;
    TInt count;
    TInt lockedKeys  = 0;
    TInt visibleKeys = 0;

    TLinearOrder<TUint32> order(CompareKey);
    defaultKeys.Sort(order);

    count = defaultKeys.Count();
    for (i = 0; i < count; ++i)
    {
        defaultKey = defaultKeys[i];
        userKey = defaultKey & KScutBitMaskThemeDefault;
        // We show only actual shortcuts
        if ( (! ( defaultKey & KScutFlagBitIconOverride )) && 
             (! ( defaultKey & KScutFlagBitToolbarShortcut )))
            {                               
    		__PRINT( __DBG_FORMAT( "XAI:   %d. key = 0x%x"), i+1, 
    			(defaultKey & (KScutBitMaskThemeDefault & KScutBitMaskLocked)));

            if (!(userKey & KScutFlagBitOptionallyVisible))
            {
                visibleKeys++;
            }

            TBool locked = (0 != (userKey & KScutFlagBitLocked));

            if (locked)
            {
                lockedKeys++;
            }
            else
            {
                CAiScutSettingsItem* setting = CAiScutSettingsItem::NewLC(
                    *this, visibleKeys-1, defaultKey);

                setting->ReadL(iRepository);

                User::LeaveIfError(iSettings.Append(setting));
                CleanupStack::Pop(setting);
            }
        }

    }

	__PRINT( __DBG_FORMAT("XAI:   %d keys, %d locked"), count, lockedKeys);

    CleanupStack::PopAndDestroy(&defaultKeys);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutSettingsModel::UpdateSettingsContainerL()
{
    if (iContainer)
    {
        if (iContainer->IsChangeDialogShowing())
        {
            iContainer->CloseChangeDialog();
        }
    }

    UpdateSettingsL();

    if (iContainer)
    {
        iContainer->ResetCurrentListL(0);
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutSettingsModel::GetHiddenAppsL()
{
    HBufC* buffer = HBufC::NewLC(
        NCentralRepositoryConstants::KMaxUnicodeStringLength);
    TPtr bufferPtr = buffer->Des();

    iHiddenAppsRepository->Get(KMenuHideApplication, bufferPtr);

	__PRINT( __DBG_FORMAT("XAI: CAiScutSettingsModel::GetHiddenAppsL '%S' "), buffer);

    iHiddenApps.Reset();
    TAiScutAppUidParser uidParser(bufferPtr, iHiddenApps);
    uidParser.ParseL();

    CleanupStack::PopAndDestroy(buffer);
}

// ---------------------------------------------------------------------------
// From MDesCArray
// Returns the number of descriptor elements in a descriptor array.
// ---------------------------------------------------------------------------
//
TInt CAiScutSettingsModel::MdcaCount() const
{
    return iSettings.Count();
}

// ---------------------------------------------------------------------------
// From MDesCArray
// Indexes into a descriptor array.
// ---------------------------------------------------------------------------
//
TPtrC CAiScutSettingsModel::MdcaPoint(TInt aIndex) const
{
    if (aIndex < 0 || aIndex >= iSettings.Count())
    {
        TPtrC ret(KNullDesC);
        return ret;
    }

    return iSettings[aIndex]->ListBoxLine();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CAiScutSettingsModel::HandleShortcutsChanged(TAny* aPtr)
{
	__PRINTS( "XAI: CAiScutSettingsModel::HandleShortcutsChanged");

    if (aPtr)
        {
        CAiScutSettingsModel* self = static_cast<CAiScutSettingsModel*>(aPtr);
        TRAP_IGNORE(self->UpdateSettingsContainerL());
        }

    return KErrNone;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CAiScutSettingsModel::HandleHiddenAppsChanged(TAny* aPtr)
{
	__PRINTS("XAI: CAiScutSettingsModel::HandleHiddenAppsChanged");

    if (aPtr)
        {
        CAiScutSettingsModel* self = static_cast<CAiScutSettingsModel*>(aPtr);

        TRAP_IGNORE(
            self->iAppList->StartL();
            self->GetHiddenAppsL();
            self->UpdateSettingsContainerL() ) // TRAP_IGNORE
        }

    return KErrNone;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TBool CAiScutSettingsModel::IsHidden(const TUid& aAppUid) const
{
    if (iHiddenApps.Find(aAppUid) == KErrNotFound)
    {
        return EFalse;
    }

	__PRINT( __DBG_FORMAT( "XAI: CAiScutSettingsModel::IsHidden (0x%x) ETrue"), aAppUid);

    return ETrue;
}

// ---------------------------------------------------------------------------
// From MAiScutListObserver
// Callback for application list events.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsModel::HandleScutListEventL(TScutListEvent aEvent,
    TBool /*aAdded*/)
{
    switch (aEvent)
    {
    case MAiScutListObserver::EAppListReady:
        if (iContainer)
        {
            iContainer->HideWaitNoteDialog();
        }
        break;

    case MAiScutListObserver::EAppListUpdated:
    case MAiScutListObserver::EBkmListUpdated:
        UpdateSettingsContainerL();
        break;

    default:
        break;
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutSettingsModel::ActivateObservers(TBool aActivate)
{
    TRAP_IGNORE(
        if (iAppList)
        {
            iAppList->SetObservingL(aActivate);
        }
        if (iBkmList)
        {
            iBkmList->SetObservingL(aActivate);
        }
    )
}

// ---------------------------------------------------------------------------
// Returns a setting item for the given index.
// ---------------------------------------------------------------------------
//
CAiScutSettingsItem* CAiScutSettingsModel::Item(TInt aIndex) const
{
    if (aIndex >= 0 && aIndex < iSettings.Count())
    {
        return iSettings[aIndex];
    }

    return NULL;
}

// ---------------------------------------------------------------------------
// Utility function to retrieve a bookmark caption from an uid.
// ---------------------------------------------------------------------------
//
TInt CAiScutSettingsModel::GetBkmCaptionL(const TUid aUid, TDes& aCaption)
{
    CFavouritesItem* favItem = CFavouritesItem::NewLC();
    TInt err = iBookmarkDb.Get(aUid.iUid, *favItem);
    if (err == KErrNone)
    {
        aCaption.Copy(favItem->Name());
    }

    CleanupStack::PopAndDestroy(favItem);

    return err;
}

// ---------------------------------------------------------------------------
// Utility function to retrieve an appliation caption from an uid.
// ---------------------------------------------------------------------------
//
TInt CAiScutSettingsModel::GetAppCaption(const TUid aUid, TDes& aCaption)
{
    TInt err = KErrNone;
    TApaAppInfo appInfo;

    err = iApaSession.GetAppInfo(appInfo, aUid);
    if (err == KErrNone)
    {
        aCaption.Copy(appInfo.iCaption);
    }

    return err;
}

// ---------------------------------------------------------------------------
// Returns a pointer to the coe environment.
// ---------------------------------------------------------------------------
//
CCoeEnv* CAiScutSettingsModel::Env()
{
    return iEnv;
}

// ---------------------------------------------------------------------------
// Handles saving a setting item to central repository.
// ---------------------------------------------------------------------------
//
void CAiScutSettingsModel::SaveItemL(CAiScutSettingsItem& aItem)
{
    // Stop monitoring own settings changes.
    iSettingsNotifier->StopListening();

    TInt err = aItem.Save(iRepository);

	__PRINT( __DBG_FORMAT( "XAI: CAiScutSettingsModel::SaveItemL key = 0x%x, err = %d"), 
		aItem.Key(), err);
    UpdateSettingsL();
    iSettingsNotifier->StartListeningL();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutSettingsModel::SetContainer(CAiScutSettingsContainer* aContainer)
{
    iContainer = aContainer;
}

// -----------------------------------------------------------------------------
// Using the Favorites dB, get the bookmarked pages
// -----------------------------------------------------------------------------
//
void CAiScutSettingsModel::ReadBookmarksL()
{
    // Get bookmarks
    delete iFavItemList;
    iFavItemList = NULL;

    iFavItemList = new (ELeave) CFavouritesItemList();
    TInt err = iBookmarkDb.GetAll(
        *iFavItemList, KFavouritesNullUid, CFavouritesItem::EItem);
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TInt CAiScutSettingsModel::BookmarkCount() const
{
    if (iFavItemList)
    {
        return iFavItemList->Count();
    }
    else
    {
        return 0;
    }
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CFavouritesItem* CAiScutSettingsModel::GetBookmark(TInt aIndex)
{
    if (aIndex >= 0 && aIndex < iFavItemList->Count())
    {
    	    
    	if (iFavItemList)
    	{
        	return iFavItemList->At(aIndex);
    	}
    	else
    	{
        	return NULL;
    	}
    }
    else
    {
    	return NULL;	
    }
}

// ---------------------------------------------------------------------------
// Returns reference to the bookmark database owned by the engine.
// ---------------------------------------------------------------------------
//
RFavouritesDb& CAiScutSettingsModel::FavouritesDb()
{
    return iBookmarkDb;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutSettingsAppList* CAiScutSettingsModel::AppList()
{
    return iAppList;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutSettingsBkmList* CAiScutSettingsModel::BkmList()
{
    return iBkmList;
}

// End of File.
