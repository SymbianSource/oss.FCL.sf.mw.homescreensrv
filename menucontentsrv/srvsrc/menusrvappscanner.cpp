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
#include <WidgetRegistryClient.h>
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

_LIT( KMenuOne, "1" );
_LIT( KMenuAttrMmcId, "mmc_id" );
_LIT( KMenuMassStorage, "mass_storage" );
_LIT( KMenuMmcHistoryFname, "mmchistory" );
_LIT( KZDrive, "Z" );

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
CMenuSrvAppScanner* CMenuSrvAppScanner::NewL( CMenuEng& aEng )
    {
    CMenuSrvAppScanner* scanner = new (ELeave) CMenuSrvAppScanner( aEng );
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
    iApaLsSession.Close();
    delete iMcsSatNotifier;
    delete iNotifier;
    iEng.DequeueOperation( *this );
    delete iMmcHistory;
    iFs.Close();
	iWidgetSession.Close();

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
CMenuSrvAppScanner::CMenuSrvAppScanner( CMenuEng& aEng )
: CActive( CActive::EPriorityStandard ), iEng( aEng )
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
    User::LeaveIfError( iApaLsSession.Connect() );
    User::LeaveIfError( iApaLsSession.GetAllApps() ); // This is async.
	User::LeaveIfError( iWidgetSession.Connect() );

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

    iApaLsSession.RegisterListPopulationCompleteObserver( iStatus );
    SetActive();
    iOpStatus = ETrue;
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
    iEng.QueueOperationL( *this );
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::DoCancel
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::DoCancel()
    {
    iApaLsSession.CancelListPopulationCompleteObserver();
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
		RArray<TMenuItem>& aMcsItems, const TAppAtributes& aApaItem,
		TUint aMmcId, TBool isLegacy)
    {
    RArray<TMenuItem> mcsUidItems;
    CleanupClosePushL( mcsUidItems );
    TInt appuid = aApaItem.GetUid();
    GetMcsAppItemsL( isLegacy, appuid, mcsUidItems );
    TBool isApaItemHidden( aApaItem.IsHidden() );

    // This app is not in the menu, add it now.
    // We don't add hidden items, there are too many of them!
    // do not display Menu app
    if ( !mcsUidItems.Count()
    		&& !isApaItemHidden
    		&& appuid !=  KMmUid3.iUid  )
        {
        if( appuid == KSatUid.iUid )
            {
            if( CMcsSatHandler::CheckVisibility() )
             	{
              	AddAppItemL( appuid, aMmcId );
               	}
            }
        else
          	{
          	AddAppItemL( appuid, aMmcId );
            GetMcsAppItemsL( isLegacy, appuid, mcsUidItems );
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
            MenuSrvUtil::UidToStringL( appuid, uidString, EFalse, EHex );
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
       	HandleMmcAttrUpdateL( item, appuid, aMmcId );
		HandleNativeAttrUpdateL( item, appuid );

        // "hidden" flag handling.
        HandleHiddenFlagUpdateL( item, appuid, isApaItemHidden );

        // "missing" flag handling
        HandleMissingFlagUpdateL( item );

        //"lock_delete" flag handling
        HandleLockDeleteFlagUpdateL(item, appuid);
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
		TInt & aAppUid, TBool aApaItemHidden )
	{
	TBool itemHidden = (0 != (aItem.Flags() & TMenuItem::EHidden));
	if (aAppUid == KSatUid.iUid)
		{
		if (itemHidden == CMcsSatHandler::CheckVisibility())
			{
			iEng.ModifiableObjectL(aItem.Id(), RMenuNotifier::EItemsAddedRemoved).
				SetFlags( TMenuItem::EHidden, !CMcsSatHandler::CheckVisibility());
			}
		}
	else if (itemHidden != aApaItemHidden)
		{
		iEng.ModifiableObjectL(aItem.Id(), RMenuNotifier::EItemsAddedRemoved).
			SetFlags( TMenuItem::EHidden, aApaItemHidden );
		}
	}

// ---------------------------------------------------------
// CMenuSrvAppScanner::HandleNativeAttrUpdateL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::HandleNativeAttrUpdateL(
		const TMenuItem& aItem, TInt aUid )
	{
    //we need this to delete uninstalled java app item
	if( ( IsMidlet( TUid::Uid( aUid ) )
	        || iWidgetSession.IsWidget( TUid::Uid( aUid ) ) )
    		&& iEng.ObjectL( aItem.Id() ).GetNative() )
    	{
    	iEng.ModifiableObjectL( aItem.Id(),
    			RMenuNotifier::EItemsNone ).SetNative( EFalse );
    	}
	}


// ---------------------------------------------------------
// CMenuSrvAppScanner::HandleMmcAttrUpdateL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::HandleMmcAttrUpdateL(
		const TMenuItem& aItem, TInt aUid, TUint aMmcId )
	{
    if ( IsMmcApp( TUid::Uid( aUid ) ) )
    	{
    	//app is instaled on mmc - KMenuAttrMmcId attribute update
    	TBuf<KUidChars> uidString;
        uidString.Format( KHexFormat, aMmcId );
        iEng.ModifiableObjectL( aItem.Id(), RMenuNotifier::EItemsAddedRemoved ).
			SetAttributeL( KMenuAttrMmcId, uidString, EFalse );
    	}
    else if ( IsMassStorageApp( TUid::Uid( aUid ) )
    		&& IsMidlet( TUid::Uid( aUid ) ) )
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
    RArray<TAppAtributes> apaAndCrItems;
    CleanupClosePushL( apaAndCrItems );
    GetApaItemsL( apaAndCrItems );
    GetCrItemsL( apaAndCrItems );
    TBool isLegacy = iEng.GetOnceLegacyFormat();


    //for every item in apaAndCrItems array
    for ( TInt i = ( apaAndCrItems.Count() - 1 ); i >= 0 ; i-- )
        {
        // if there was leave for any item we ignore it
        // and proceed to the next one
        TRAP_IGNORE(UpdateApplicationItemL(
        		mcsItems, apaAndCrItems[i], currentMmcId, isLegacy));
        }
    // Here the big list contains menu items that refer to missing apps.
    HandleMissingItemsL( mcsItems );
    CleanupStack::PopAndDestroy( &apaAndCrItems );
    CleanupStack::PopAndDestroy( &mcsItems );
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::HandleLockDeleteFlagUpdateL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::HandleLockDeleteFlagUpdateL(
		const TMenuItem& aItem, TInt aUid )
	{
	TBool isHidden = ( ( iEng.ObjectL( aItem.Id() )
							.Flags() & TMenuItem::EHidden ) != 0 );
	if ( !isHidden && IsInRomL( aUid ) )
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
// CMenuSrvAppScanner::IsInRomL
// ---------------------------------------------------------
//
TBool CMenuSrvAppScanner::IsInRomL( TInt aUid )
    {
    TBool inROM = EFalse;
    TApaAppInfo appInfo;
    if( iApaLsSession.GetAppInfo( appInfo, TUid::Uid( aUid ) ) == KErrNone )
        {
        RBuf path;
        CleanupClosePushL( path );
        path.CreateL( appInfo.iFullName );
        path.Replace( 0, 1, KZDrive );
        if( BaflUtils::FileExists( iFs, path ) )
            {
            inROM = ETrue;
            }
        CleanupStack::PopAndDestroy( &path );
        }
    return inROM;
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::GetCrItemsL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::GetCrItemsL( RArray<TAppAtributes>& aArray )
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
                                        RArray<TAppAtributes>& aArray )
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
void CMenuSrvAppScanner::SetHidden( const TDesC& aHiddenAppUid,
                                            RArray<TAppAtributes>& aArray )
    {
    TLex appUidDescriptor(aHiddenAppUid);
    TUint hiddenAppUid( 0 );
    appUidDescriptor.Val( hiddenAppUid, EHex );
    if ( hiddenAppUid )
       {
       //all applications in descriptor are hidden
       TAppAtributes appAtributes( hiddenAppUid,  ETrue );
       TInt id = aArray.Find(appAtributes, TAppAtributes::MatchItems );
       if ( id != KErrNotFound )
           {
           aArray[id].SetHidden( ETrue );
           }
       }
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::GetApaItemsL
// ---------------------------------------------------------
//
void CMenuSrvAppScanner::GetApaItemsL( RArray<TAppAtributes>& aArray )
    {
    TApaAppInfo appInfo;
    TApaAppCapabilityBuf appCap;
    TInt screenNumber;

    User::LeaveIfError( iApaLsSession.GetAllApps() );
    // for every application get uid, hidden and missing attribute
    // and add to aArray.
    while ( KErrNone == iApaLsSession.GetNextApp( appInfo ) )
        {
        User::LeaveIfError( iApaLsSession.GetAppCapability
            ( appCap, appInfo.iUid ) );
        User::LeaveIfError( iApaLsSession.GetDefaultScreenNumber
            ( screenNumber, appInfo.iUid ) );
        // "Hidden" status according to AppArc.
        TBool appHidden = appCap().iAppIsHidden || screenNumber != 0;

        TAppAtributes appAtributes( appInfo.iUid.iUid,  appHidden );
        aArray.AppendL( appAtributes );
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
    if((aKey ==KMenuHideApplication )||(aKey == KMenuHideCPApplication))
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
TInt CMenuSrvAppScanner::CreateInstallFolderL( TUint aUid )
	{
    // Find a folder for this app.
    TInt folder(0);

    TApaAppCapabilityBuf capability;
    User::LeaveIfError(iApaLsSession.GetAppCapability( capability, TUid::Uid( aUid )));

    if ( capability().iGroupName.Length() )
        {
        // appgroup_name is defined for this app. Find or create folder.
        AppGroupFolderL( capability().iGroupName, folder );
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
        TUint aUid,
        TUint aCurrentMmcId )
    {
    TInt folder = CreateInstallFolderL( aUid );
    // Now add the app item.
    CMenuEngObject* object = iEng.NewObjectL( KMenuTypeApp() );
    CleanupStack::PushL( object );
    TBuf<KUidChars> uidString;
    _LIT(KHexFormat, "0x%X");
    uidString.Format( KHexFormat, aUid );

    object->SetAttributeL( KMenuAttrUid(), uidString, EFalse );
    if ( aCurrentMmcId && IsMmcApp( TUid::Uid( aUid ) ) )
        {
        // This app is on the MMC, set the "mmc_id" attribute.
        uidString.Format( KHexFormat, aCurrentMmcId );
        object->SetAttributeL( KMenuAttrMmcId, uidString, EFalse );
        }
    if( IsMidlet( TUid::Uid( aUid ) ) 
	    || iWidgetSession.IsWidget( TUid::Uid( aUid ) ) )
    	{
    	//we need this to delete uninstalled java or wrt widget app item
    	object->SetNative( EFalse );
    	if ( !IsMmcApp( TUid::Uid( aUid ) )
    			&& IsMassStorageApp( TUid::Uid( aUid ) ) )
    		{
    		object->SetAttributeL( KMenuAttrMmcId, KMenuMassStorage, EFalse );
    		}
    	}
    EnsureFolderWritableL( folder );
    iEng.AddL( *object, folder, 0 );
    CleanupStack::Pop( object );
    }

// ---------------------------------------------------------------------------
// CMenuSrvAppScanner::IsMidletL
// Checks if given app is midlet by reading the apptype uid (2nd uid)
// ---------------------------------------------------------------------------
//
TBool CMenuSrvAppScanner::IsMidlet( const TUid aAppUid )
	{
    const TUid KMidletType = { 0x10210E26 };
    TUid typeuid = KNullUid;

    if ( KErrNone == iApaLsSession.GetAppType( typeuid, aAppUid  ) )
        {
        if ( typeuid == KMidletType )
            {
            return ETrue;
            }
        }
    return EFalse;
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
void CMenuSrvAppScanner::HandleMissingItemsL
( const RArray<TMenuItem>& aItems )
    {
    for ( TInt i = 0; i < aItems.Count(); i++ )
        {
        const TInt id = aItems[i].Id();
        TUint mmcId = 0;
        TPtrC val;
        TBool dummy;
        if( iEng.ObjectL( id ).FindAttribute( KMenuAttrMmcId(), val, dummy ) )
            {
            MenuUtils::GetTUint( val, mmcId );
			if ( mmcId && KErrNotFound != iMmcHistory->Find( mmcId ) )
				{
				// This item is on an MMC which is currently in the MMC history.
				// Set it "missing" but keep it.
				SetObjectFlagsL( ETrue, aItems[i], TMenuItem::EMissing,
						RMenuNotifier::EItemsAddedRemoved );
				}
			else if ( val == KMenuMassStorage()
						&& IsDriveInUse( DriveInfo::EDefaultMassStorage ) )
				{
				SetObjectFlagsL( ETrue, aItems[i], TMenuItem::EMissing,
						RMenuNotifier::EItemsAddedRemoved );
				}
			else
				{
				iEng.RemoveL( id );
				}
            }
        else
            {
            // This item is not on MMC or its MMC has been purged from the MMC
            // history. Hide the item.
            SetObjectFlagsL( EFalse, aItems[i], TMenuItem::ELockDelete );

			if( iEng.ObjectL( id ).GetNative() )
            	{
            	SetObjectFlagsL( ETrue, aItems[i], TMenuItem::EHidden,
						RMenuNotifier::EItemsAddedRemoved );
            	}
            else
            	{
               	iEng.RemoveL( id );
            	}
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
		const TUid aUid,
		const DriveInfo::TDefaultDrives& aDefaultDrive ) const
    {
    TBool ret( EFalse );
    TApaAppInfo info;
    TInt err = iApaLsSession.GetAppInfo( info, aUid );
    if ( !err && IsFileInDrive( info.iFullName, aDefaultDrive ) )
    	{
    	ret = ETrue;
    	}
    return ret;
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::IsMmcApp
// ---------------------------------------------------------
//
TBool CMenuSrvAppScanner::IsMmcApp( const TUid aUid ) const
    {
    return IsAppInDrive( aUid, DriveInfo::EDefaultRemovableMassStorage );
    }

// ---------------------------------------------------------
// CMenuSrvAppScanner::IsMassStorageApp
// ---------------------------------------------------------
//
TBool CMenuSrvAppScanner::IsMassStorageApp( const TUid aUid ) const
    {
    return IsAppInDrive( aUid, DriveInfo::EDefaultMassStorage );
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

// ==================== MEMBER FUNCTIONS ====================

// ---------------------------------------------------------
// TAppAtributes::TAppAtributes
// ---------------------------------------------------------
//
TAppAtributes::TAppAtributes ( TUint aUid, TBool aHidden/*, TBool aMissing*/)
    {
    iUid = aUid;
    iHidden = aHidden;
    /*iMissing = aMissing;*/
    }

// ---------------------------------------------------------
// TAppAtributes::GetUid
// ---------------------------------------------------------
//
TUint TAppAtributes::GetUid() const
    {
    return iUid;
    }

// ---------------------------------------------------------
// TAppAtributes::IsHidden
// ---------------------------------------------------------
//
TBool TAppAtributes::IsHidden() const
    {
    return iHidden;
    }
// ---------------------------------------------------------
// TAppAtributes::SetHidden
// ---------------------------------------------------------
//
void TAppAtributes::SetHidden( TBool aHidden )
    {
    iHidden = aHidden;
    }

// ---------------------------------------------------------
// TAppAtributes::MatchItems
// ---------------------------------------------------------
//
TBool TAppAtributes::MatchItems(const TAppAtributes& item1,
                                        const TAppAtributes& item2)
    {
    TBool result( EFalse );
    TUint uid1(item1.GetUid());
    TUint uid2(item2.GetUid());
    if ( uid1 == uid2 )
        {
        result = ETrue;
        }
    return result;
    }



//  End of File
