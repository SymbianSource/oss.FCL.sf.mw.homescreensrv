/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
#include <mcsmenuutils.h>
#include <e32property.h>
#include <widgetregistryclient.h>
#include "menusrvappscanner.h"
#include "menusrvmmchistory.h"
#include "menueng.h"
#include "menuengobject.h"
#include "pathinfo.h"
#include "menulogger.h"
#include "centralrepository.h"
#include "mcssathandler.h"
#include "menusrvengutils.h"
#include "bautils.h"
#include "menuutil.h"

_LIT( KMenuOne, "1" );
_LIT( KMenuAttrMmcId, "mmc_id" );
_LIT( KMenuMassStorage, "mass_storage" );
_LIT( KMenuPredefinedWidget, "predefined_widget" );
_LIT( KMenuMmcHistoryFname, "mmchistory" );

// ==================== LOCAL FUNCTIONS ====================

/**
* Identity function to search in an array of TMenuItems.
* Identity is the ID.
* @param aLeft Search term.
* @param aRight Array item.
* @return ETrue if ID-s match.
*/
LOCAL_C TBool IdMatch( const TMenuItem& aLeft, const TMenuItem& aRight )
    {
    return aLeft.Id() == aRight.Id();
    }

// ==================== MEMBER FUNCTIONS ====================

// ---------------------------------------------------------
// CMenuSrvAppScanner::NewL
// ---------------------------------------------------------
//
CMenuSrvAppScanner* CMenuSrvAppScanner::NewL(
        CMenuEng& aEng, CMenuSrvEngUtils& aSrvEngUtils  )
    {
    CMenuSrvAppScanner* scanner = new (ELeave) CMenuSrvAppScanner(
            aEng, aSrvEngUtils );
    CleanupStack::PushL( scanner );
    scanner->ConstructL();
    CleanupStack::Pop( scanner );
    return scanner;
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::~CMenuSrvAppScanner
// ---------------------------------------------------------
//
CMenuSrvAppScanner::~CMenuSrvAppScanner()
    {
    Cancel();
    delete iMcsSatNotifier;
    delete iNotifier;
    iEng.DequeueOperation( *this );
    delete iMmcHistory;
    iFs.Close();

    if( iCenRepNotifyHandlerHiddenApps )
        {
        iCenRepNotifyHandlerHiddenApps->StopListening();
        }
    delete iCenRepNotifyHandlerHiddenApps;

    if( iCenRepNotifyHandlerCPHiddenApps )
        {
        iCenRepNotifyHandlerCPHiddenApps->StopListening();
        }
    delete iCenRepNotifyHandlerCPHiddenApps;

    if( iCenRepNotifyHandlerCPHiddenFolders )
        {
        iCenRepNotifyHandlerCPHiddenFolders->StopListening();
        }
    delete iCenRepNotifyHandlerCPHiddenFolders;
  	delete iCenRepSession;
  	delete iFreeSpaceObserver;

    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::CMenuSrvAppScanner
// ---------------------------------------------------------
//
CMenuSrvAppScanner::CMenuSrvAppScanner(
        CMenuEng& aEng, CMenuSrvEngUtils& aSrvEngUtils )
: CActive( CActive::EPriorityStandard ), iEng( aEng ),
  iSrvEngUtils( aSrvEngUtils )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::ConstructL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::ConstructL()
    {
    iOpStatus = EFalse;
    User::LeaveIfError( iFs.Connect() );
    TFileName path;
    User::LeaveIfError( iFs.PrivatePath( path ) );
    TUint attribute;
    if( iFs.Att( path, attribute) == KErrNotFound )
    	{
        TInt mdRes = iFs.MkDirAll( path );
        if ( mdRes != KErrNone )
        	{
        	User::Leave( mdRes );
        	}
    	}

    iMmcHistory = new (ELeave) CMenuSrvMmcHistory();
    iMmcHistory->LoadL( iFs, KMenuMmcHistoryFname() );
    // The notifier has its own session to apparc, instead of taking
    // it as argument... :(
    iNotifier = CApaAppListNotifier::NewL( this, CActive::EPriorityStandard );

   	iCenRepSession = CRepository::NewL( KCRUidMenu );

	iCenRepNotifyHandlerHiddenApps = CCenRepNotifyHandler::NewL(
	    *this, *iCenRepSession,
		CCenRepNotifyHandler::EStringKey, KMenuHideApplication );
	iCenRepNotifyHandlerCPHiddenApps = CCenRepNotifyHandler::NewL(
	    *this, *iCenRepSession,
		CCenRepNotifyHandler::EStringKey, KMenuHideCPApplication );

	iCenRepNotifyHandlerCPHiddenFolders = CCenRepNotifyHandler::NewL(
	    *this, *iCenRepSession,
		CCenRepNotifyHandler::EStringKey, KMenuHideCPFolder );

	iCenRepNotifyHandlerHiddenApps->StartListeningL();
	iCenRepNotifyHandlerCPHiddenApps->StartListeningL();
	iCenRepNotifyHandlerCPHiddenFolders->StartListeningL();

	iMcsSatNotifier = CMcsSatNotifier::NewL(
	                this, KCRUidMenu, KMenuShowSatUI );

	iFreeSpaceObserver = CMcsFreeSpaceObserver::NewL( *this );

    //SetActive();
	ScheduleScan();
    //iOpStatus = ETrue;
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::RunL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::RunL()
    {
    User::LeaveIfError( iStatus.Int() ); // Handle errors in RunL.
    // AppArc app scan complete, we have the app list.
    // Now queue this operation to be executed by the Engine.
    if ( !MenuUtil::BackupInProgressL() )
        {
        iEng.QueueOperationL( *this );
        }
    else
        {
        iOpStatus = EFalse;
        }
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::DoCancel
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::DoCancel()
    {
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::RunError
// ---------------------------------------------------------
//
TInt CMenuSrvAppScanner::RunError( TInt /*aError*/ )
    {
    // Ignore the error (what else could we do?).
    // When next AppArc update occurs, we will run again.
    return KErrNone;
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::UpdateApplicationWithHideIfInstalledItemsL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::UpdateApplicationWithHideIfInstalledItemsL()
    {
    TInt root;
    iEng.RootFolderL( root );

    TMenuSrvAttrExistsFilter appFilter;
    appFilter.SetAttr( KMenuAttrHideIfInstalledUid() );

    RArray<TMenuItem> items;
    CleanupClosePushL( items );
    iEng.GetItemsL( items, root, &appFilter, ETrue );

    for (TInt i=0; i < items.Count(); i++)
        {
        TBool wasHidden;
        TPtrC uidTmp;

        GetHiddenFlagAndUidL( items[i].Id(), wasHidden, uidTmp );

        HideItemIfPresentL( root, items[i].Id(), uidTmp, wasHidden );
        }
    CleanupStack::PopAndDestroy( &items );
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::GetHiddenFlagAndUidL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::GetHiddenFlagAndUidL( TInt aId, TBool& aHidden, TPtrC& aUidStr )
    {
    const CMenuEngObject& object = iEng.ObjectL( aId );
    aHidden = (0 != (object.Flags() & TMenuItem::EHidden));

    TBool tmp;
    object.FindAttribute( KMenuAttrHideIfInstalledUid(), aUidStr, tmp);
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::HideAppIfExists
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::HideItemIfPresentL( TInt aFolder, TInt aId, const TDesC& aUid, TBool aWasHidden )
    {
    TBool hideItem = EFalse;
    TMenuSrvAttrFilter uidAppFilter;
    uidAppFilter.SetAttr( KMenuAttrUid(), aUid );

    RArray<TMenuItem> appItems;
    CleanupClosePushL( appItems );
    iEng.GetItemsL( appItems, aFolder, &uidAppFilter, ETrue );

    for (TInt i=0; i < appItems.Count(); i++)
        {
        if(appItems[i].Type() != KMenuTypeLink())
        	{
            const TMenuItem& item = appItems[i];

            TBool itemHidden = (0 != (item.Flags() & TMenuItem::EHidden));
            TBool itemMissing = (0 != (item.Flags() & TMenuItem::EMissing));
            if ( !itemHidden && !itemMissing )
                {
                hideItem = ETrue;
                }
        	}
        }

    SetHiddenFlagL( aId, aWasHidden, hideItem );

    CleanupStack::PopAndDestroy( &appItems );
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::SetHiddenFlagL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::SetHiddenFlagL( TInt aId, TBool aWasHidden, TBool aHide )
    {
    if ( aWasHidden ^ aHide )
        {
        iEng.ModifiableObjectL( aId, RMenuNotifier::EItemsAddedRemoved ).
            SetFlags( TMenuItem::EHidden, aHide );
        }
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::ValidateLinkUidL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::ValidateLinkUidL( TInt aFolder, TInt aId, const TDesC& aUidStr )
    {
    TMenuSrvAttrFilter uidAppFilter;
    uidAppFilter.SetAttr( KMenuAttrUid(), aUidStr );

    RArray<TMenuItem> appItems;
    CleanupClosePushL( appItems );
    iEng.GetItemsL( appItems, aFolder, &uidAppFilter, ETrue );

    TBool showItem = EFalse;
    TBool exists = EFalse;
    TBool itemMissing = EFalse;
    for (TInt i=0; i < appItems.Count(); i++)
        {
        if(appItems[i].Id() != aId)
        	{
            exists = ETrue;
            const TMenuItem& item = appItems[i];

            TBool itemHidden = (0 != (item.Flags() & TMenuItem::EHidden));
            itemMissing = (0 != (item.Flags() & TMenuItem::EMissing));
            if ( !itemHidden && !itemMissing )
                {
                showItem = ETrue;
                }
        	}
        }

    UpdateLinkL( aId, exists, showItem, itemMissing );

    CleanupStack::PopAndDestroy( &appItems );
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::UpdateLinkL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::UpdateLinkL(
    TInt aId,
    TBool aExists,
    TBool aShowItem,
    TBool aMissingItem )
    {
    if (!aExists)
        {
        iEng.ModifiableObjectL( aId ).SetFlags( TMenuItem::ELockDelete, EFalse );
        iEng.RemoveL( aId );
        }
    else if ( !aMissingItem )
        {
        const CMenuEngObject& object = iEng.ObjectL( aId );
        TBool itemHidden = (0 != (object.Flags() & TMenuItem::EHidden));
        TBool itemMissing = (0 != (object.Flags() & TMenuItem::EMissing));
        if ( itemHidden == aShowItem )
            {
            iEng.ModifiableObjectL( aId, RMenuNotifier::EItemsAddedRemoved  ).
            SetFlags( TMenuItem::EHidden, !aShowItem );
            }
        if ( itemMissing != aMissingItem )
            {
            iEng.ModifiableObjectL( aId, RMenuNotifier::EItemsAddedRemoved  ).
            SetFlags( TMenuItem::EMissing, aMissingItem );
            }
        }
    else
        {
        const CMenuEngObject& object = iEng.ObjectL( aId );
        TBool itemMissing = (0 != (object.Flags() & TMenuItem::EMissing));
        if ( itemMissing != aMissingItem )
            {
            iEng.ModifiableObjectL( aId, RMenuNotifier::EItemsAddedRemoved  ).
            SetFlags( TMenuItem::EMissing, aMissingItem );
            }
        }
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::ValidateLinkRefIdL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::ValidateLinkRefIdL( TInt aId, const TDesC& refIdStr )
    {
    TUint refItemId;
    MenuUtils::GetTUint( refIdStr, refItemId );

    TMenuItem refItem;

    TBool showItem = EFalse;
    TBool exists = EFalse;

    TRAPD( err, iEng.GetItemL( refItemId, refItem ) );

    if (err == KErrNone)
        {
        exists = ETrue;
        }

    TBool itemHidden = (0 != (refItem.Flags() & TMenuItem::EHidden));
    TBool itemMissing = (0 != (refItem.Flags() & TMenuItem::EMissing));
    if ( !itemHidden  && !itemMissing )
        {
        showItem = ETrue;
        }

    UpdateLinkL( aId, exists, showItem, itemMissing );
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::ValidateLinkL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::ValidateLinkL( TInt aFolder, TInt aId )
    {
    const CMenuEngObject& object = iEng.ObjectL( aId );

    TBool tmp;
    TPtrC refIdStr;
    TPtrC uidStr;
    if ( object.FindAttribute( KMenuAttrRefId(), refIdStr, tmp) )
        {
        ValidateLinkRefIdL( aId, refIdStr );
        }
    else if ( object.FindAttribute( KMenuAttrUid(), uidStr, tmp) )
        {
        ValidateLinkUidL( aFolder, aId, uidStr );
        }
    else
        {
        UpdateLinkL( aId, EFalse, EFalse, EFalse );
        }
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::UpdateLinkItemsL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::UpdateLinkItemsL()
    {
    TInt root;
    iEng.RootFolderL( root );

    TMenuSrvTypeFilter linkFilter;
    linkFilter.SetType( KMenuTypeLink() );

    RArray<TMenuItem> items;
    CleanupClosePushL( items );
    iEng.GetItemsL( items, root, &linkFilter, ETrue );

    for (TInt i=0; i < items.Count(); i++)
        {
        ValidateLinkL( root, items[i].Id() );
        }
    CleanupStack::PopAndDestroy( &items );
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::HandleFreeSpaceEventL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::HandleFreeSpaceEventL()
    {
    if ( !IsActive() && !iOpStatus )
        {
        iOpStatus = ETrue;
        TRequestStatus* ownStatus = &iStatus;
        *ownStatus = KRequestPending;
        SetActive();
        User::RequestComplete( ownStatus, KErrNone );
        }
    }
// ---------------------------------------------------------
// CMenuSrvAppScanner::RunMenuEngOperationL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::RunMenuEngOperationL()
    {
    UpdateApplicationItemsL();
    UpdateApplicationWithHideIfInstalledItemsL();
    UpdateLinkItemsL();
    iOpStatus = EFalse;
    }


// ---------------------------------------------------------
// CMenuSrvAppScanner::UpdateApplicationItemL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::UpdateApplicationItemL(
		RArray<TMenuItem>& aMcsItems, const CMenuSrvAppAttributes& aApaItem,
		TUint aMmcId, TBool isLegacy)
    {
    RArray<TMenuItem> mcsUidItems;
    CleanupClosePushL( mcsUidItems );
    TUid appuid = aApaItem.GetUid();
    GetMcsAppItemsL( isLegacy, appuid.iUid, mcsUidItems );
    TBool isApaItemHidden( aApaItem.IsHidden() );

    // This app is not in the menu, add it now.
    // We don't add hidden items, there are too many of them!
    // do not display Menu app
    if ( !mcsUidItems.Count()
    		&& !isApaItemHidden
    		&& appuid !=  KMmUid3  )
        {
        if( appuid == KSatUid )
            {
            if( CMcsSatHandler::CheckVisibility() )
             	{
              	AddAppItemL( aApaItem, aMmcId );
               	}
            }
        else
          	{
          	AddAppItemL( aApaItem, aMmcId );
            GetMcsAppItemsL( isLegacy, appuid.iUid, mcsUidItems );
           	}
        }//if

    // if there are any items with legacy UID format
    // update them to new uid format
    else if ( mcsUidItems.Count() && isLegacy )
        {
        //for every item with matching UID
        for (TInt j=0; j < mcsUidItems.Count(); j++)
     		{
       		CMenuEngObject& object =
      		                iEng.ModifiableObjectL( mcsUidItems[j].Id() );
            TBuf<KUidChars> uidString;
            MenuSrvUtil::UidToStringL( appuid.iUid, uidString, EFalse, EHex );
       	    object.SetAttributeL( KMenuAttrUid(), uidString, EFalse );
       		}//for
        }//else if
    // "hidden", "missing" and "lock_delete"  flags update
    for ( TInt j = 0; j < mcsUidItems.Count(); j++ )
        {
        const TMenuItem& item = mcsUidItems[j];

        //we need to handle first run of appscanner,
        //there might be some incorrect data in content xml file
        //if this will have impact on performance we may run this methods only at start up
       	HandleMmcAttrUpdateL( item, aApaItem, aMmcId );
		HandleNativeAttrUpdateL( item, aApaItem );

        // "hidden" flag handling.
        HandleHiddenFlagUpdateL( item, aApaItem );

        // "missing" flag handling
        HandleMissingFlagUpdateL( item );

        //"lock_delete" flag handling
        HandleLockDeleteFlagUpdateL(item, aApaItem );
        // if item was just added to MCS  it is not present in aMcsItems
        // so we cannot remove it
        TInt index = aMcsItems.Find( item, TIdentityRelation<TMenuItem>( IdMatch ) );
        if ( index != KErrNotFound )
        	{
        	aMcsItems.Remove( index );
        	}
        }//for
    CleanupStack::PopAndDestroy( &mcsUidItems );
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::HandleHiddenFlagUpdateL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::HandleHiddenFlagUpdateL( const TMenuItem & aItem,
        const CMenuSrvAppAttributes& aApaItem )
	{
	TBool itemHidden = (0 != (aItem.Flags() & TMenuItem::EHidden));
	if ( aApaItem.GetUid() == KSatUid )
		{
		if (itemHidden == CMcsSatHandler::CheckVisibility())
			{
			iEng.ModifiableObjectL(aItem.Id(), RMenuNotifier::EItemsAddedRemoved).
				SetFlags( TMenuItem::EHidden, !CMcsSatHandler::CheckVisibility());
			}
		}
	else if ( itemHidden != aApaItem.IsHidden() )
		{
		iEng.ModifiableObjectL(aItem.Id(), RMenuNotifier::EItemsAddedRemoved).
			SetFlags( TMenuItem::EHidden, aApaItem.IsHidden() );
		}
	}

// ---------------------------------------------------------
// CMenuSrvAppScanner::HandleNativeAttrUpdateL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::HandleNativeAttrUpdateL(
		const TMenuItem& aItem, const CMenuSrvAppAttributes& aApaItem )
	{
    //we need this to delete uninstalled java app item
    if( aApaItem.GetAppType() != iEng.ObjectL( aItem.Id() ).GetAppType() )
    	{
    	iEng.ModifiableObjectL( aItem.Id(),
    			RMenuNotifier::EItemsNone ).SetAppType( aApaItem.GetAppType() );
    	}
	}


// ---------------------------------------------------------
// CMenuSrvAppScanner::HandleMmcAttrUpdateL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::HandleMmcAttrUpdateL(
		const TMenuItem& aItem, const CMenuSrvAppAttributes& aApaItem, TUint aMmcId )
	{
    if ( IsInMmc( aApaItem ) )
    	{
    	//app is instaled on mmc - KMenuAttrMmcId attribute update
    	TBuf<KUidChars> uidString;
        uidString.Format( KHexFormat, aMmcId );
        iEng.ModifiableObjectL( aItem.Id(), RMenuNotifier::EItemsAddedRemoved ).
			SetAttributeL( KMenuAttrMmcId, uidString, EFalse );
    	}
    else if ( IsInMassStorage( aApaItem )
    		&& aApaItem.GetAppType() == CMenuEngObject::EWidgetApp )
    	{
    	//its java app installed on mass storage, we need to leave it in xml
    	//in case of connecting usb in mass storage mode
        iEng.ModifiableObjectL( aItem.Id(), RMenuNotifier::EItemsAddedRemoved ).
			SetAttributeL( KMenuAttrMmcId, KMenuMassStorage, EFalse );
    	}
    else
    	{
    	//its installed on c: drive - remove attribute
		iEng.ModifiableObjectL( aItem.Id(), RMenuNotifier::EItemsAddedRemoved ).
			RemoveAttribute( KMenuAttrMmcId );
    	}
	}
// ---------------------------------------------------------
// CMenuSrvAppScanner::UpdateApplicationItemsL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::UpdateApplicationItemsL()
    {
    TUint currentMmcId = UpdateMmcHistoryL();
    // get all MCS items
    RArray<TMenuItem> mcsItems;
    CleanupClosePushL( mcsItems );
    GetMcsAppItemsL( mcsItems );
    iSrvEngUtils.ReloadApaItemsL();
    GetCrItemsL( iSrvEngUtils.GetAppItemsL() );
    TBool isLegacy = iEng.GetOnceLegacyFormat();


    //for every item in apaAndCrItems array
    for ( TInt i = ( iSrvEngUtils.GetAppItemsL().Count() - 1 ); i >= 0 ; i-- )
        {
        // if there was leave for any item we ignore it
        // and proceed to the next one
        TRAP_IGNORE(UpdateApplicationItemL(
        		mcsItems, *iSrvEngUtils.GetAppItemsL()[i], currentMmcId, isLegacy));
        }
    // Here the big list contains menu items that refer to missing apps.
    HandleMissingItemsL( mcsItems );
    CleanupStack::PopAndDestroy( &mcsItems );
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::HandleLockDeleteFlagUpdateL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::HandleLockDeleteFlagUpdateL(
		const TMenuItem& aItem, const CMenuSrvAppAttributes& aApaItem )
	{
	TBool isHidden = ( ( iEng.ObjectL( aItem.Id() )
							.Flags() & TMenuItem::EHidden ) != 0 );
	if ( !isHidden && IsInRomL( aApaItem ) )
		{
		if ( ( aItem.Flags() & TMenuItem::ELockDelete ) == 0 )
			{
			iEng.ModifiableObjectL(aItem.Id())
					.SetFlags( TMenuItem::ELockDelete, ETrue );
			}
		}
	else
		{
		if ( ( aItem.Flags() & TMenuItem::ELockDelete ) != 0 )
			{
			iEng.ModifiableObjectL(aItem.Id())
					.SetFlags( TMenuItem::ELockDelete, EFalse );
			}
		}
	}

// ---------------------------------------------------------
// CMenuSrvAppScanner::HandleMissingFlagUpdateL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::HandleMissingFlagUpdateL( const TMenuItem& aItem )
	{
	if ( aItem.Flags() & TMenuItem::EMissing )
        {
        CLOG_WRITE_FORMAT8( "Unsetting flag EMissing on %d", item.Id() );
        //application found so we unset "missing" flag
        iEng.ModifiableObjectL( aItem.Id(), RMenuNotifier::EItemsAddedRemoved ).
            SetFlags( TMenuItem::EMissing, EFalse );
        }
	}

// ---------------------------------------------------------
// CMenuSrvAppScanner::GetCrItemsL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::GetCrItemsL( RPointerArray<CMenuSrvAppAttributes>& aArray )
    {
 	TBuf<KCenRepBufferSize> buf;
	iCenRepSession->Get( KMenuHideCPApplication, buf );
	ParseUidsL( buf, aArray );// parses UIDs from buf and appends them to array
	iCenRepSession->Get( KMenuHideApplication, buf );
	ParseUidsL( buf, aArray );// parses UIDs from buf and appends them to array
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::ParseUidsL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::ParseUidsL( const TDesC& aHiddenApplications,
                                        RPointerArray<CMenuSrvAppAttributes>& aArray )
    {
    TLex input( aHiddenApplications );
    TLexMark startMark;
    input.Mark( startMark );
    TBool notEmpty = EFalse;
    while ( !input.Eos() )
        {
        if( input.Peek() == ',')
            {
            SetHidden(input.MarkedToken( startMark ), aArray);
            input.Inc();
            input.Mark( startMark );
            }
        input.Inc();
        notEmpty = ETrue;
        }
	if ( notEmpty )
		{
		SetHidden(input.MarkedToken( startMark ), aArray);
		}


    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::SetHidden
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::SetHidden(
        const TDesC& aHiddenAppUid, RPointerArray<CMenuSrvAppAttributes>& aArray )
    {
    TLex appUidDescriptor(aHiddenAppUid);
    TUint hiddenAppUid( 0 );
    appUidDescriptor.Val( hiddenAppUid, EHex );
    if ( hiddenAppUid )
        {
        TApaAppCapabilityBuf capability;
        TApaAppInfo appInfo;
        appInfo.iUid = TUid::Uid( hiddenAppUid );
        CMenuSrvAppAttributes* attribute = CMenuSrvAppAttributes::NewLC( capability, appInfo );
        TInt index = aArray.Find( attribute,
                TIdentityRelation<CMenuSrvAppAttributes>( CMenuSrvAppAttributes::MatchItems ) );
        if( index >= 0 && !aArray[index]->IsHidden() )
            {
            aArray[index]->SetHidden( ETrue );
            }
        CleanupStack::PopAndDestroy( attribute );
        }
    }


// ---------------------------------------------------------
// CMenuSrvAppScanner::GetMcsItemsL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::GetMcsAppItemsL( RArray<TMenuItem>& aArray )
    {
    TInt root;
    iEng.RootFolderL( root );
    TMenuSrvTypeFilter appFilter;
    appFilter.SetType( KMenuTypeApp() );
    iEng.GetItemsL( aArray, root, &appFilter, ETrue );
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::GetMcsItemsL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::GetMcsAppItemsL( TBool aLegacy,
                       const TUint aUid, RArray<TMenuItem>& aArray )
    {
    TInt root;
    iEng.RootFolderL( root );
    TMenuSrvTypeAttrFilter appFilter;
    appFilter.SetType( KMenuTypeApp() );
    TBuf<KUidChars> uidString;
    MenuSrvUtil::UidToStringL( aUid, uidString, aLegacy, EHex );
    appFilter.SetAttr( KMenuAttrUid(), uidString );
    iEng.GetItemsL( aArray, root, &appFilter, ETrue );
    //if not found in mcs try with UID in decimal format
    if (!aArray.Count())
        {
        MenuSrvUtil::UidToStringL( aUid, uidString, aLegacy, EDecimal );
        appFilter.SetAttr( KMenuAttrUid(), uidString );
        iEng.GetItemsL( aArray, root, &appFilter, ETrue );
        }
    }


// ---------------------------------------------------------
// CMenuSrvAppScanner::CompletedMenuEngOperation
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::CompletedMenuEngOperation( TInt /*aErr*/ )
    {
    iOpStatus = EFalse;
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::HandleAppListEvent
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::HandleAppListEvent( TInt /*aEvent*/ )
    {
    // We only have one event, EAppListChanged.
    // Call back RunL async, to requeue and initiate rescan.
    if ( !IsActive()  && !iOpStatus)
        {
        iOpStatus = ETrue;
        TRequestStatus* ownStatus = &iStatus;
        *ownStatus = KRequestPending;
        SetActive();
        User::RequestComplete( ownStatus, KErrNone );
        }
    }

// -----------------------------------------------------------------------------
// CMenuSrvAppScanner::HandleNotifyString
// -----------------------------------------------------------------------------
//
void CMenuSrvAppScanner::HandleNotifyString( TUint32 aKey,
        const TDesC16& /*aNewValue*/ )
    {
    if((aKey == KMenuHideApplication )||(aKey == KMenuHideCPApplication))
        {
        if ( !IsActive() && !iOpStatus )
            {
            iOpStatus = ETrue;
            TRequestStatus* ownStatus = &iStatus;
            *ownStatus = KRequestPending;
            SetActive();
            User::RequestComplete( ownStatus, KErrNone );
            }
        }
    }

// -----------------------------------------------------------------------------
// CMenuSrvAppScanner::SATChangeL
// -----------------------------------------------------------------------------
//
void CMenuSrvAppScanner::SATChangeL()
    {
    ScheduleScan();
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::ScheduleScan
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::ScheduleScan()
	{
	if ( !IsActive() && !iOpStatus )
		{
		iOpStatus = ETrue;
		TRequestStatus* ownStatus = &iStatus;
		*ownStatus = KRequestPending;
		SetActive();
		User::RequestComplete( ownStatus, KErrNone );
		}
	}

// ---------------------------------------------------------
// CMenuSrvAppScanner::CreateInstallFolderL
// ---------------------------------------------------------
//
TInt CMenuSrvAppScanner::CreateInstallFolderL( const CMenuSrvAppAttributes& aApaItem )
	{
    // Find a folder for this app.
    TInt folder(0);
    if ( aApaItem.GetGroupName().Length() )
        {
        // appgroup_name is defined for this app. Find or create folder.
        AppGroupFolderL( aApaItem.GetGroupName(), folder );
        }
    else
        {
        // No appgroup_name, use default folder, if any.
        DefaultFolderL( folder );
        }
    if ( !folder )
        {
        // Last resort: it goes to the root.
        iEng.RootFolderL( folder );
        }
    return folder;
	}

// ---------------------------------------------------------
// CMenuSrvAppScanner::AddAppItemL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::AddAppItemL(
        const CMenuSrvAppAttributes& aApaItem,
        TUint aCurrentMmcId )
    {
    TInt folder = CreateInstallFolderL( aApaItem );
    // Now add the app item.
    CMenuEngObject* object = iEng.NewObjectL( KMenuTypeApp() );
    CleanupStack::PushL( object );
    TBuf<KUidChars> uidString;
    _LIT(KHexFormat, "0x%X");
    uidString.Format( KHexFormat, aApaItem.GetUid().iUid );

    object->SetAttributeL( KMenuAttrUid(), uidString, EFalse );
    if ( aCurrentMmcId && IsInMmc( aApaItem ) )
        {
        // This app is on the MMC, set the "mmc_id" attribute.
        uidString.Format( KHexFormat, aCurrentMmcId );
        object->SetAttributeL( KMenuAttrMmcId, uidString, EFalse );
        }
    if( aApaItem.GetAppType() == CMenuEngObject::EWidgetApp )
    	{
    	//we need this to delete uninstalled java or wrt widget app item
    	if ( !IsInMmc( aApaItem )
    			&& IsInMassStorage( aApaItem ) )
    		{
    		object->SetAttributeL( KMenuAttrMmcId, KMenuMassStorage, EFalse );
    		}
    	}
    object->SetAppType( aApaItem.GetAppType() );
    EnsureFolderWritableL( folder );
    iEng.AddL( *object, folder, 0 );
    CleanupStack::Pop( object );
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::AppGroupFolderL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::AppGroupFolderL( const TDesC& aAppGroupName, TInt& aFolderId )
    {
    __ASSERT_DEBUG( aAppGroupName.Length(), User::Invariant() );

    TInt folder = 0;
    TInt defaultFolderId( 0 );
    iEng.RootFolderL( defaultFolderId );
    const CMenuEngObject& rootObject = iEng.ObjectL( defaultFolderId );
    TPtrC groupName;
    TBool localized;

    if ( rootObject.FindAttribute( KMenuAttrAppGroupName, groupName, localized ) &&
    	groupName.Compare( aAppGroupName )==0 )
        {
        folder = defaultFolderId;
        }
    else
    	{
        TMenuSrvTypeAttrFilter filter;
        filter.SetType( KMenuTypeFolder() );
        filter.SetAttr( KMenuAttrAppGroupName(), aAppGroupName );
        RArray<TMenuItem> items;
        CleanupClosePushL( items );

        iEng.GetItemsL( items, defaultFolderId, &filter, ETrue );
        if ( items.Count() )
            {
            folder = items[0].Id();
            }
        CleanupStack::PopAndDestroy( &items );
    	}
    if ( !folder )
        {
        // No such folder, create it now.
        CMenuEngObject* object = iEng.NewObjectL( KMenuTypeFolder() );
        CleanupStack::PushL( object );
        object->SetAttributeL
            ( KMenuAttrAppGroupName(), aAppGroupName, EFalse );
        EnsureFolderWritableL( defaultFolderId );
        iEng.AddL( *object, defaultFolderId, 0 );

        CleanupStack::Pop( object );
        folder = object->Id();
        }
    __ASSERT_DEBUG( folder, User::Invariant() );
    aFolderId = folder;
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::DefaultFolderL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::DefaultFolderL( TInt& aFolderId )
    {
    TInt folder = 0;
    TMenuSrvTypeAttrFilter filter;
    filter.SetType( KMenuTypeFolder() );
    filter.SetAttr( KMenuAttrDefault(), KMenuOne() );
    RArray<TMenuItem> items;
    CleanupClosePushL( items );
    TInt rootId;
    iEng.RootFolderL( rootId );
    iEng.GetItemsL( items, rootId, &filter, ETrue );
    if ( items.Count() )
        {
        folder = items[0].Id();
        }
    CleanupStack::PopAndDestroy( &items );
    aFolderId = folder;
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::EnsureFolderWritableL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::EnsureFolderWritableL( TInt aFolder )
    {
    // This folder should not be read-only! Remove the protection.
    // Otherwise we can't add the items to it.
    if ( iEng.ObjectL( aFolder ).Flags() & TMenuItem::ELockMoveInto )
        {
        iEng.ModifiableObjectL( aFolder ).
            SetFlags( TMenuItem::ELockMoveInto, EFalse );
        }
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::HandleMissingItemsL
// ---------------------------------------------------------
//
TMenuItem::TFlags CMenuSrvAppScanner::GetProperFlagL( const TMenuItem& aItem )
    {
    TMenuItem::TFlags flags = TMenuItem::ENoFlag;

    TUint mmcId = 0;
    TPtrC val;
    TBool dummy;
    if( iEng.ObjectL( aItem.Id() ).FindAttribute( KMenuAttrMmcId(), val, dummy ) )
        {
        MenuUtils::GetTUint( val, mmcId );
        if ( mmcId && KErrNotFound != iMmcHistory->Find( mmcId ) )
            {
            // This item is on an MMC which is currently in the MMC history.
            // Set it "missing" but keep it.
            flags = TMenuItem::EMissing;
            }
        else if ( val == KMenuMassStorage()
                    && IsDriveInUse( DriveInfo::EDefaultMassStorage ) )
            {
            flags = TMenuItem::EMissing;
            }
        else if ( iEng.ObjectL( aItem.Id() ).FindAttribute(
                KMenuPredefinedWidget(), val, dummy ) )
            {
            flags = TMenuItem::EMissing;
            }
        }
    else if( iEng.ObjectL( aItem.Id() ).GetAppType()
                != CMenuEngObject::EWidgetApp
            || iEng.ObjectL( aItem.Id() ).FindAttribute(
                    KMenuPredefinedWidget(), val, dummy ) )
        {
        flags = TMenuItem::EHidden;
        }
    return flags;
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::HandleMissingItemsL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::HandleMissingItemsL(
        const RArray<TMenuItem>& aItems )
    {
    for ( TInt i = 0; i < aItems.Count(); i++ )
        {
        TMenuItem::TFlags flags = GetProperFlagL( aItems[i] );
        if( flags == TMenuItem::ENoFlag )
            {
            iEng.RemoveL( aItems[i].Id() );
            }
        else
            {
            SetObjectFlagsL( ETrue, aItems[i], flags,
                    RMenuNotifier::EItemsAddedRemoved );
            }
        }
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::SetObjectFlagsL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::SetObjectFlagsL( TBool aFlagValue, const TMenuItem& aItem,
		const TMenuItem::TFlags& aFlag, const RMenuNotifier::TEvent& aEvent  )
	{
	TBool itemFlagPresent = (0 != (aItem.Flags() & aFlag));
	if( aFlagValue != itemFlagPresent )
		{
		iEng.ModifiableObjectL( aItem.Id(), aEvent ).
			 SetFlags( aFlag, aFlagValue );
		}
	}

// ---------------------------------------------------------
// CMenuSrvAppScanner::UpdateMmcHistoryL
// ---------------------------------------------------------
//
TUint CMenuSrvAppScanner::UpdateMmcHistoryL()
    {
    TUint mmcId = CurrentMmcId();
    if( mmcId )
    	{
    	iMmcHistory->InsertL( mmcId );
    	iMmcHistory->SaveL( iFs, KMenuMmcHistoryFname() );
    	}
    return mmcId;
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::CurrentMmcId
// ---------------------------------------------------------
//
TUint CMenuSrvAppScanner::CurrentMmcId() const
    {
    // Get mmc id. Errors are ignored.
    TUint mmcId = 0;
    TInt mmcDrive;
    TInt err;
    err = DriveInfo::GetDefaultDrive(
    	    DriveInfo::EDefaultRemovableMassStorage, mmcDrive );
    if ( !err )
        {
        TVolumeInfo volumeInfo;
        err = iFs.Volume( volumeInfo, mmcDrive );
        if( !err )
            {
            mmcId = volumeInfo.iUniqueID;
            }
        }
    return mmcId;
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::IsFileInDrive
// ---------------------------------------------------------
//
TBool CMenuSrvAppScanner::IsFileInDrive(
			const TDesC& aFileName,
			const DriveInfo::TDefaultDrives& aDefaultDrive ) const
    {
    if ( aFileName.Length() )
        {
        TInt mmcDrive;
        TInt err = DriveInfo::GetDefaultDrive(
						aDefaultDrive, mmcDrive );
        if ( !err )
            {
            TInt fileDrive;
            err = RFs::CharToDrive( aFileName[0], fileDrive );
            if ( !err && fileDrive == mmcDrive )
                {
                return ETrue;
                }
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::IsAppInDrive
// ---------------------------------------------------------
//
TBool CMenuSrvAppScanner::IsAppInDrive(
        const CMenuSrvAppAttributes& aApaItem,
		const DriveInfo::TDefaultDrives& aDefaultDrive ) const
    {
    TBool ret( EFalse );
    if ( IsFileInDrive( aApaItem.GetFullName(), aDefaultDrive ) )
    	{
    	ret = ETrue;
    	}
    return ret;
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::IsInMmc
// ---------------------------------------------------------
//
TBool CMenuSrvAppScanner::IsInMmc( const CMenuSrvAppAttributes& aApaItem ) const
    {
    return IsAppInDrive( aApaItem, DriveInfo::EDefaultRemovableMassStorage );
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::IsInMassStorage
// ---------------------------------------------------------
//
TBool CMenuSrvAppScanner::IsInMassStorage( const CMenuSrvAppAttributes& aApaItem ) const
    {
    return IsAppInDrive( aApaItem, DriveInfo::EDefaultMassStorage );
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::IsInRomL
// ---------------------------------------------------------
//
TBool CMenuSrvAppScanner::IsInRomL( const CMenuSrvAppAttributes& aApaItem ) const
    {
    return IsAppInDrive( aApaItem, DriveInfo::EDefaultRom );
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::IsDriveInUse
// ---------------------------------------------------------
//
TBool CMenuSrvAppScanner::IsDriveInUse(
		const DriveInfo::TDefaultDrives& aDefaultDrive )
    {
    TBool inUse( EFalse );
    TInt drive;

    TInt err = DriveInfo::GetDefaultDrive( aDefaultDrive, drive );
    if( err == KErrNone )
		{
		TUint status;
		err =  DriveInfo::GetDriveStatus(  iFs, drive, status );
		if( err == KErrNone
				&& ( status & DriveInfo::EDriveInUse ) )
			{
			inUse = ETrue;
			}
		}

	return inUse;
    }

//  End of File
