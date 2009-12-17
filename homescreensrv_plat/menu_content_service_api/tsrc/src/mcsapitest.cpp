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
// INCLUDE FILES

#include <ecom/ecom.h>
#include <Stiftestinterface.h>
#include "mcsapitest.h"
#include "mcsmenuopwatcher.h"
#include "mcsrequestnotification.h"
#include "mcsmenuhandlerplugin.h"
#include "mcsmenuhandlerplugin.hrh"

#include <mcsmenu.h>
#include <mcsmenuitem.h>
#include <mcsmenunotifier.h>
#include <mcsmenufilter.h>
#include <mcsmenuutils.h>
#include <mcsmenuiconutility.h>
#include <S32MEM.H>
#include <apgtask.h>

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMCSAPITest::Test_RMenuConstructorL(CStifItemParser& /*aItem*/ )
	{
	RMenu menu;
	CleanupClosePushL(menu);
	// Didn't panick during construction, so it's ok
	CleanupStack::PopAndDestroy(&menu);
	return KErrNone;
	}


// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMCSAPITest::Test_RMenuCloseL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);

	RMenu menu;
	menu.Close();
	result = KErrNone;
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMCSAPITest::Test_RMenuOpenL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);

	RMenu menu;
	//TRAPD (err, menu.OpenL(_L("sapitest_menudata")));
    TRAPD (err, SetupMenuL(menu); );
	result = err;
	menu.Close();

	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMCSAPITest::Test_RMenuRootFolderL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);

	RMenu menu;
	SetupMenuL(menu);
	CleanupClosePushL(menu);

	TInt root;
    TRAPD(err, root = menu.RootFolderL());

	if (err == KErrNone && root > 0)
		{
		result = KErrNone;
		}

	CleanupStack::PopAndDestroy(&menu);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMCSAPITest::Test_RMenuRootGetItemsL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);

	RMenu menu;
	RArray<TMenuItem> itemArray;
	const TInt KRootId = 1;

	CleanupClosePushL(menu);
	CleanupClosePushL(itemArray);

	SetupMenuL(menu);
    TRAPD(err, menu.GetItemsL(itemArray, KRootId));

	if (err == KErrNone && itemArray.Count() > 0)
		{
		result = KErrNone;
		}

	CleanupStack::PopAndDestroy(&itemArray);
	CleanupStack::PopAndDestroy(&menu);

	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMCSAPITest::Test_RMenuGetRunningAppsL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);

	RMenu menu;
	RArray<TUid> uidArray;

	CleanupClosePushL(menu);
	CleanupClosePushL(uidArray);
	SetupMenuL(menu);
    TRAPD(err, menu.GetRunningAppsL(uidArray));

	WaitL(1000000);

	if (err == KErrNone && uidArray.Count() > 0)
		{
		result = KErrNone;
		}

	CleanupStack::PopAndDestroy(&uidArray);
	CleanupStack::PopAndDestroy(&menu);

	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMCSAPITest::Test_RMenuRemoveL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);

	RMenu menu;
	const TInt KRootId = 1;

	CleanupClosePushL(menu);
	SetupMenuL(menu);

	CMCSMenuOpWatcher* watcher = CMCSMenuOpWatcher::NewL(ETrue);
	CleanupStack::PushL(watcher);

	CMenuOperation* oper = menu.RemoveL(101, watcher->iStatus) ;
	watcher->Watch( *oper);
	CActiveScheduler::Start();
	WaitL( 1000000);

	result = watcher->GetStatus();

	CleanupStack::PopAndDestroy(watcher);
	CleanupStack::PopAndDestroy(&menu);

	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMCSAPITest::Test_RMenuMoveToFolderL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);

	TInt moveId = 3;
	TInt moveBefore = 2;
	TInt folder = 90;

	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);

	RArray<TInt> items;
	CleanupClosePushL(items);
	items.Append(moveId);
	
	CMCSMenuOpWatcher* watcheriMCS = CMCSMenuOpWatcher::NewL();
	CleanupStack::PushL(watcheriMCS);
	
	CMenuOperation* operationiMCS = menu.MoveToFolderL( items, folder, moveBefore,
			watcheriMCS->iStatus );
	
	watcheriMCS->Watch( *operationiMCS);

	WaitL( 200000);
	result = watcheriMCS->GetStatus();

	CleanupStack::PopAndDestroy(watcheriMCS);
	CleanupStack::PopAndDestroy(&items);
	CleanupStack::PopAndDestroy(&menu);

	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMCSAPITest::Test_RMenuReorderL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);

	TInt moveId = 3;
	TInt moveBefore = 2;
	TInt revMoveBefore = 6;

	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);

	CMCSMenuOpWatcher* watcheriMCS = CMCSMenuOpWatcher::NewL();
	CleanupStack::PushL(watcheriMCS);
	CMenuOperation* operationiMCS = menu.ReorderL(moveId, moveBefore,
			watcheriMCS->iStatus);
	watcheriMCS->Watch( *operationiMCS);

	WaitL( 200000);
	TInt status = watcheriMCS->GetStatus();
	CleanupStack::PopAndDestroy(watcheriMCS);

	watcheriMCS = CMCSMenuOpWatcher::NewL();
	CleanupStack::PushL(watcheriMCS);
	operationiMCS = menu.ReorderL(moveId, revMoveBefore, watcheriMCS->iStatus);
	watcheriMCS->Watch( *operationiMCS);
	WaitL( 200000);

	result = status;
	CleanupStack::PopAndDestroy(watcheriMCS);
	CleanupStack::PopAndDestroy(&menu);

	return result;
	}

TInt CMCSAPITest::Test_RMenuResourceMarkL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);

	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);

	menu.ResourceCheck();
	// Didn't panicked till now, must be ok
	result = KErrNone;

	CleanupStack::PopAndDestroy(&menu);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMCSAPITest::Test_RMenuResourceCheckL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);

	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);

	menu.ResourceMark();
	// Didn't panicked till now, must be ok
	result = KErrNone;

	CleanupStack::PopAndDestroy(&menu);

	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMCSAPITest::Test_RMenuResourceCountL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);

	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);

	TInt res = menu.ResourceCount();
	if (res == 0)
		{
		result = KErrNone;
		}

	CleanupStack::PopAndDestroy(&menu);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CMCSAPITest::Test_RMenu__DbgSetAllocFailL(CStifItemParser& /*aItem*/ )
	{
	TInt result(KErrGeneral);

	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);
	
	menu.__DbgSetAllocFail(RAllocator::ERandom, 0);
	result = KErrNone;

	CleanupStack::PopAndDestroy(&menu);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_TMenuItemConstructorL(CStifItemParser& /*aItem*/ )
	{
	TMenuItem hdr;
	// Didn't panicked so it passed
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_TMenuItemId(CStifItemParser& /*aItem*/ )
	{
	TInt result(KErrGeneral);
	TMenuItem hdr;
	if (hdr.Id() == 0)
		{
		// Id was called with no problems
		result = KErrNone;
		}
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_TMenuItemParent(CStifItemParser& /*aItem*/ )
	{
	TInt result(KErrGeneral);
	TMenuItem hdr;
	if (hdr.Parent() == 0)
		{
		//  Parent was called with no problems
		result = KErrNone;
		}
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_TMenuItemFlags(CStifItemParser& /*aItem*/ )
	{
	TInt result(KErrGeneral);
	TMenuItem hdr;
	if (hdr.Flags() == 0)
		{
		// Flags was called with no problems
		result = KErrNone;
		}
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_TMenuItemType(CStifItemParser& /*aItem*/ )
	{
	TMenuItem hdr;
	hdr.Type();
	// Didn't panicked during Type call, so it passed
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_TMenuItemSetId(CStifItemParser& /*aItem*/ )
	{
	TInt result(KErrGeneral);
	TMenuItem hdr;
	hdr.SetId(1);
	if (hdr.Id() == 1)
		{
		result = KErrNone;
		}
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_TMenuItemSetParent(CStifItemParser& /*aItem*/ )
	{
	TInt result(KErrGeneral);
	TMenuItem hdr;
	hdr.SetParent(1);
	if (hdr.Parent() == 1)
		{
		result = KErrNone;
		}
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_TMenuItemSetFlags(CStifItemParser& /*aItem*/ )
	{
	TInt result(KErrGeneral);
	TMenuItem hdr;
	hdr.SetFlags(1);
	if (hdr.Flags() == 1)
		{
		result = KErrNone;
		}
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_TMenuItemSetType(CStifItemParser& /*aItem*/ )
	{
	TInt result(KErrGeneral);
	TMenuItem hdr;
	hdr.SetType(KMenuTypeUrl);
	if (hdr.Type() == KMenuTypeUrl)
		{
		result = KErrNone;
		}
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuItemCreateL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	TInt beforeId(2);
	TInt folderId(2);

	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);

	CMenuItem *item;
	TRAPD(err, item = CMenuItem::CreateL(menu, KMenuTypeUrl, folderId, beforeId));
	CleanupStack::PushL(item);
	if (err == KErrNone)
		{
		result = KErrNone;
		}
	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(&menu);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuItemOpenIdL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	TInt itemId(2);
	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);

	CMenuItem *item;
    TRAPD(err, item = CMenuItem::OpenL(menu, itemId));
	CleanupStack::PushL(item);
	if (err == KErrNone)
		{
		result = KErrNone;
		}
	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(&menu);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuItemOpenHdrL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);

	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);

	CMenuItem *item;
	TMenuItem* hdr = new (ELeave) TMenuItem();
	CleanupStack::PushL(hdr);
	hdr->SetId(85);
	hdr->SetParent(1);
	hdr->SetType(KMenuTypeUrl);


	TRAPD(err, item = CMenuItem::OpenL(menu, *hdr));
	CleanupStack::PushL(item);
	if (err == KErrNone)
		{
		result = KErrNone;
		}
	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(hdr);
	CleanupStack::PopAndDestroy(&menu);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuItemIdL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	TInt itemId(2);
	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);

	CMenuItem *item;
	TRAPD(err, item = CMenuItem::OpenL(menu, itemId))
	;
	CleanupStack::PushL(item);
	if (err == KErrNone && item->Id() == itemId)
		{
		result = KErrNone;
		}
	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(&menu);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuItemParentL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);
	TInt itemId(2);
	TInt parentId(1);

	CMenuItem *item;
	TRAPD(err, item = CMenuItem::OpenL(menu, itemId))
	;
	CleanupStack::PushL(item);
	if (err == KErrNone && item->Parent() == parentId)
		{
		result = KErrNone;
		}
	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(&menu);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuItemFlagsL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);
	TInt itemId(111);

	CMenuItem *item;
    TRAPD(err, item = CMenuItem::OpenL(menu, itemId));
	CleanupStack::PushL(item);
	if (err == KErrNone && (item->Flags() & TMenuItem::ELockDelete) != 0)
		{
		result = KErrNone;
		}
	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(&menu);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuItemSetFlagsL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);
	TInt itemId(111);

	CMenuItem *item;
	TRAPD(err, item = CMenuItem::OpenL(menu, itemId))
	;
	CleanupStack::PushL(item);
	if (err == KErrNone)
		{
		item->SetFlags(TMenuItem::ELockMoveInto, ETrue);
		if ((item->Flags() & TMenuItem::ELockMoveInto) != 0)
			{
			result = KErrNone;
			}
		}
	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(&menu);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuItemTypeL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);
	TInt itemId(111);

	CMenuItem *item;
	TRAPD(err, item = CMenuItem::OpenL(menu, itemId))
	;
	CleanupStack::PushL(item);
	if (err == KErrNone && item->Type() == KMenuTypeUrl)
		{
		result = KErrNone;
		}
	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(&menu);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuItemGetAttributeL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);
	TInt itemId(2);
	_LIT(KExpectedLongName, "long__");

	CMenuItem *item;
    TRAPD(err, item = CMenuItem::OpenL(menu, itemId));
	CleanupStack::PushL(item);
	if (err == KErrNone)
		{
		TBool attrExists(EFalse);
		TPtrC longName = item->GetAttributeL(KMenuAttrLongName, attrExists);
		if (attrExists && longName == KExpectedLongName)
			{
			result = KErrNone;
			}
		}
	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(&menu);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuItemGetAttributeListL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);
	TInt itemId(2);

	CMenuItem *item;
    TRAPD(err, item = CMenuItem::OpenL(menu, itemId));
	CleanupStack::PushL(item);
	if (err == KErrNone)
		{
		RArray<TAttributeName> attrList;
		CleanupClosePushL(attrList);
		item->GetAttributeListL(attrList);
		if (attrList.Count() > 0)
			{
			result = KErrNone;
			}
		CleanupStack::PopAndDestroy(&attrList);
		}
	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(&menu);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuItemRunningStatusL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);
	TInt itemId(2);

	CMenuItem *item;
    TRAPD(err, item = CMenuItem::OpenL(menu, itemId));
	CleanupStack::PushL(item);
	if (err == KErrNone)
		{
		result = KErrCancel;
		if (item->RunningStatusL() == EFalse)
			{
			result = KErrNone;
			}
		}
	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(&menu);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuItemSetAttributeL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);
	TInt itemId(2);
	_LIT(KNewLongName, "newlong__");

	CMenuItem *item;
    TRAPD(err, item = CMenuItem::OpenL(menu, itemId));
	CleanupStack::PushL(item);
	if (err == KErrNone)
		{
		item->SetAttributeL(KMenuAttrLongName, KNewLongName);
		SaveL(item);

		TBool attrExists(EFalse);
		TPtrC longName = item->GetAttributeL(KMenuAttrLongName, attrExists);
		if (attrExists && longName == KNewLongName)
			{
			result = KErrNone;
			}
		}
	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(&menu);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuItemRemoveAttributeL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	TInt itemId(2);
	_LIT(KNewLongName, "newlong__");
	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);

	CMenuItem *item;
    TRAPD(err, item = CMenuItem::OpenL(menu, itemId));
	CleanupStack::PushL(item);
	if (err == KErrNone)
		{
		item->RemoveAttributeL(KMenuAttrLongName);
		SaveL(item);
		TBool attrExists(EFalse);
		TPtrC longName = item->GetAttributeL(KMenuAttrLongName, attrExists);
		if (attrExists == EFalse)
			{
			result = KErrNone;
			}
		}
	CleanupStack::PopAndDestroy(item);
	CleanupStack::PopAndDestroy(&menu);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuItemSaveL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	TInt itemId(2);
	_LIT(KNewLongName, "newlong__");
	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);

	CMenuItem* menuitem = CMenuItem::OpenL(menu, itemId);
	CleanupStack::PushL(menuitem);

	menuitem->SetAttributeL(KMenuAttrLongName, KNewLongName);

	CMCSMenuOpWatcher* watcher = CMCSMenuOpWatcher::NewL(ETrue);
	CleanupStack::PushL(watcher);

	CMenuOperation* oper = menuitem->SaveL(watcher->iStatus);
	watcher->Watch( *oper);
	CActiveScheduler::Start();
	WaitL( 1000000);

	result = watcher->GetStatus();

	CleanupStack::PopAndDestroy(watcher);
	CleanupStack::PopAndDestroy(menuitem);
	CleanupStack::PopAndDestroy(&menu);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuItemHandleCommandL(CStifItemParser& /*aItem*/)
	{

	TInt result(KErrGeneral);
	TInt itemId(2);
	TInt uid = 0x10005901;
	RMenu menu;
	CleanupClosePushL(menu);
	SetupMenuL(menu);

	CMenuItem* menuitem = CMenuItem::OpenL(menu, itemId);
	CleanupStack::PushL(menuitem);

	CMCSMenuOpWatcher* watcher = CMCSMenuOpWatcher::NewL(ETrue);
	CleanupStack::PushL(watcher);

	CMenuOperation* oper = menuitem->HandleCommandL( KMenuCmdOpen, _L8("") , watcher->iStatus);
	watcher->Watch( *oper);
	CActiveScheduler::Start();
	WaitL( 1000000);

	result = watcher->GetStatus();
	
	// Close opened app
	RWsSession session;
	TInt error = session.Connect();
	TUid uidApp( TUid::Uid( uid )) ;
	TApaTaskList taskList( session );    
	TApaTask task = taskList.FindApp( uidApp ); 

	if( task.Exists() )
		{
		task.EndTask();
		}
	else
		{
		result = KErrNotFound;
		}

	WaitL( 1000000 );
	session.Close();
	WaitL( 1000000  );

	CleanupStack::PopAndDestroy(watcher);
	CleanupStack::PopAndDestroy(menuitem);
	CleanupStack::PopAndDestroy(&menu);
	return result;

	}

TInt CMCSAPITest::Test_RMenuNotifierConstructorL(CStifItemParser& /*aItem*/ )
	{
    TInt result ( KErrGeneral );
   
    RMenuNotifier notifier; // Construction
    CleanupClosePushL( notifier ); 
    CleanupStack::PopAndDestroy( &notifier );
    // Didn't panicked during construction so it's ok
    return KErrNone;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_RMenuNotifierOpenL(CStifItemParser& /*aItem*/ )
    {
    TInt result ( KErrGeneral );
    
    RMenu menu; 
    CleanupClosePushL( menu ); 
    SetupMenuL( menu );
    WaitL( 200000 );
   
    RMenuNotifier notifier;
    notifier.Open( menu );
    
    notifier.Close();
    CleanupStack::PopAndDestroy( &menu );
    result = KErrNone;
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_RMenuNotifierNotifyL(CStifItemParser& /*aItem*/ )
    {
    TInt result ( KErrGeneral );
    TInt folderId = 110;
    
    RMenu menu; 
    CleanupClosePushL( menu ); 
    SetupMenuL( menu );
    WaitL( 200000 );
    
    CMCSReqNotification * request = CMCSReqNotification::NewL( menu );
    CleanupStack::PushL( request );
    WaitL( 200000 );
    request->StartNotificationL( folderId, RMenuNotifier::EItemsReordered );
    WaitL( 200000 );

    if( request->GetStatus() == KErrNone )
		{
		result = KErrNone;
		}
    
    CleanupStack::PopAndDestroy( request );
    CleanupStack::PopAndDestroy( &menu );
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_RMenuNotifierCloseL(CStifItemParser& /*aItem*/ )
    {
    TInt result ( KErrGeneral );
    RMenuNotifier  iNotifier;
    iNotifier.Close();
    result = KErrNone;
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_RMenuNotifierCancelL(CStifItemParser& /*aItem*/ )
    {
    TInt result ( KErrGeneral );
    TInt folderId = 110;
    
    RMenu menu; 
    CleanupClosePushL( menu ); 
    SetupMenuL( menu );
    WaitL( 200000 );
    
    CMCSReqNotification * request = CMCSReqNotification::NewL( menu );
    CleanupStack::PushL( request );
    WaitL( 200000 );
    request->StartNotificationL( folderId, RMenuNotifier::EItemsReordered );
    WaitL( 200000 );

    if( request->GetStatus() == KErrNone )
		{
		result = KErrNone;
		}
    
    CleanupStack::PopAndDestroy( request );
    CleanupStack::PopAndDestroy( &menu );
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuFilterDestructorL(CStifItemParser& /*aItem*/ )
	{
    TInt result (KErrGeneral);
    CMenuFilter* filter;
    filter = CMenuFilter::NewL();
    
    // Check destructor
    delete filter;
    // Didn't panicked so it's ok
    return KErrNone;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuFilterNewL(CStifItemParser& /*aItem*/ )
    {
    TInt result (KErrGeneral);
    CMenuFilter* filter = NULL;
    TRAPD (err, filter = CMenuFilter::NewL());
    result = err;
    CleanupStack::PushL( filter );
    CleanupStack::PopAndDestroy( filter );  
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuFilterNewLC(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	CMenuFilter* filter= NULL;
    TRAPD (err, filter = CMenuFilter::NewLC();
    CleanupStack::PopAndDestroy( filter ));
	result = err;
	return result;
	}
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuFilterFlagsOn(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	CMenuFilter* filter = CMenuFilter::NewLC();
	TUint32 flag( 0);
	flag = filter->FlagsOn();
	if (flag ==KErrNone)
		{
		filter->SetFlags(TMenuItem::EHidden, 0);
		flag = filter->FlagsOn();
		if (flag == TMenuItem::EHidden)
			{
			result = KErrNone;
			}
		}
	CleanupStack::PopAndDestroy(filter);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuFilterFlagsOff(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	CMenuFilter* filter = CMenuFilter::NewLC();
	TUint32 flag( 0);
	flag = filter->FlagsOff();
	if (flag ==KErrNone)
		{
		filter->SetFlags( 0, TMenuItem::EHidden);
		flag = filter->FlagsOff();
		if (flag == TMenuItem::EHidden)
			{
			result = KErrNone;
			}
		}
	CleanupStack::PopAndDestroy(filter);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuFilterType(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	CMenuFilter* filter = CMenuFilter::NewLC();
	TPtrC type(filter->Type() );
	if (type == KNullDesC() )
		{
		filter->SetType( _L("test"));
		TPtrC type2(filter->Type() );
		if (type2 == _L("test"))
			{
			result = KErrNone;
			}
		}
	CleanupStack::PopAndDestroy(filter);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuFilterNumAttributes(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	CMenuFilter* filter = CMenuFilter::NewLC();
	TInt count(filter->NumAttributes() );
	if (count == KErrNone)
		{
		filter->HaveAttributeL( _L("test1"), _L("test1"));
		filter->HaveAttributeL( _L("test2"), _L("test2"));
		filter->HaveAttributeL( _L("test3"), _L("test3"));
		filter->HaveAttributeL( _L("test4"), _L("test4"));
		filter->HaveAttributeL( _L("test5"), _L("test5"));
		filter->HaveAttributeL( _L("test6"), _L("test6"));
		count = filter->NumAttributes();
		if (count == 6)
			{
			result = KErrNone;
			}
		}
	CleanupStack::PopAndDestroy(filter);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuFilterGetAttribute(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	CMenuFilter* filter = CMenuFilter::NewLC();
	TPtrC nameAttribute(KNullDesC);
	TPtrC valueAttribute(KNullDesC);
	filter->HaveAttributeL( _L("test"), _L("Valuetest"));
	if (filter->GetAttribute( 0, nameAttribute, valueAttribute)
			&& nameAttribute == _L("test") && valueAttribute == _L("Valuetest"))
		{
		result = KErrNone;
		}
	CleanupStack::PopAndDestroy(filter);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuFilterReset(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	CMenuFilter* filter = CMenuFilter::NewLC();
	filter->HaveAttributeL( _L("test"), _L("Valuetest"));
	filter->SetType( _L("test"));
	filter->SetFlags(TMenuItem::EHidden, TMenuItem::EHidden);
	filter->Reset();
	TPtrC nameAttribute(KNullDesC);
	TPtrC valueAttribute(KNullDesC);
	TPtrC type(filter->Type() );
	filter->HaveAttributeL( _L("test3"), _L("Valuetest3"));
	if (filter->GetAttribute( 0, nameAttribute, valueAttribute)
			&& filter->FlagsOn() == KErrNone && filter->FlagsOff() == KErrNone
			&& type == KNullDesC() && nameAttribute == _L("test3") && valueAttribute == _L("Valuetest3"))
		{
		result = KErrNone;
		}
	CleanupStack::PopAndDestroy(filter);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuFilterSetFlags(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	CMenuFilter* filter = CMenuFilter::NewLC();
	filter->SetFlags(TMenuItem::EHidden, TMenuItem::EHidden);
	if (filter->FlagsOn() == TMenuItem::EHidden && filter->FlagsOff()
			== TMenuItem::EHidden)
		{
		result = KErrNone;
		}
	CleanupStack::PopAndDestroy(filter);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuFilterSetType(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	CMenuFilter* filter = CMenuFilter::NewLC();
	filter->SetType( _L("test"));
	TPtrC type(filter->Type() );
	if (type == _L("test"))
		{
		result = KErrNone;
		}
	CleanupStack::PopAndDestroy(filter);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuFilterHaveAttributeL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	CMenuFilter* filter = CMenuFilter::NewLC();
	filter->HaveAttributeL( _L("test"), _L("Valuetest"));
	filter->HaveAttributeL( _L("test"), _L("Valuetest2"));
	TPtrC nameAttribute(KNullDesC);
	TPtrC valueAttribute(KNullDesC);
	if (filter->GetAttribute( 0, nameAttribute, valueAttribute)
			&& nameAttribute == _L("test") && valueAttribute == _L("Valuetest2"))
		{
		result = KErrNone;
		}
	CleanupStack::PopAndDestroy(filter);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuFilterDoNotHaveAttributeL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	CMenuFilter* filter = CMenuFilter::NewLC();
	filter->HaveAttributeL( _L("test"), _L("Valuetest"));
	filter->DoNotHaveAttributeL( _L("test"));
	TPtrC nameAttribute(KNullDesC);
	TPtrC valueAttribute(KNullDesC);
	if ( !filter->GetAttribute( 0, nameAttribute, valueAttribute)
			&& nameAttribute == _L("test") && valueAttribute == KNullDesC() )
		{
		result = KErrNone;
		}
	CleanupStack::PopAndDestroy(filter);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuFilterRemoveAttribute(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	CMenuFilter* filter = CMenuFilter::NewLC();
	filter->HaveAttributeL( _L("test"), _L("Valuetest"));
	filter->RemoveAttribute( _L("test"));
	TPtrC nameAttribute(KNullDesC);
	TPtrC valueAttribute(KNullDesC);
	filter->HaveAttributeL( _L("test2"), _L("Valuetest2"));
	if (filter->GetAttribute( 0, nameAttribute, valueAttribute)
			&& nameAttribute == _L("test2") && valueAttribute == _L("Valuetest2"))
		{
		result = KErrNone;
		}
	CleanupStack::PopAndDestroy(filter);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_McsIconUtilityGetItemIconL(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	RMenu menu;
	CleanupClosePushL(menu);
	RArray<TMenuItem> itemArray;
	CleanupClosePushL(itemArray);

	SetupMenuL(menu);
	CMenuFilter* filter = CMenuFilter::NewLC();
	filter->HaveAttributeL( _L("uid"), _L("0x1028311D"));
    TRAPD(err, menu.GetItemsL(itemArray, 29, filter));

	if (err == KErrNone && itemArray.Count() == 1)
		{
		CMenuItem* menuitem = CMenuItem::OpenL(menu, itemArray[0].Id() );
		CleanupStack::PushL(menuitem);
		CAknIcon* icon = MenuIconUtility::GetItemIconL( *menuitem);
		if (icon)
			{
			result = KErrNone;
			}
		delete icon;
		CleanupStack::PopAndDestroy(menuitem);
		}

	CleanupStack::PopAndDestroy(filter);
	CleanupStack::PopAndDestroy(&itemArray);
	CleanupStack::PopAndDestroy(&menu);
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_MenuUtilsGetTUint(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	TPtrC descTUint(_L("0x12345"));
	TUint intTUint(KErrNone);
	MenuUtils::GetTUint(descTUint, intTUint);
	if (intTUint == 74565)
		{
		result = KErrNone;
		}
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_MenuUtilsSetTUint(CStifItemParser& /*aItem*/)
	{
	TInt result(KErrGeneral);
	TBuf<8> descTUint;
	MenuUtils::SetTUint(descTUint, 12345);
	if (descTUint == _L("0x3039"))
		{
		result = KErrNone;
		}
	return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_MenuHandlerPluginNewL(CStifItemParser& /*aItem*/ )
    {
    TInt result (KErrGeneral);
    RMenu menu; 
    CleanupClosePushL( menu ); 
    SetupMenuL( menu );
    WaitL( 200000 );

    
    CMenuHandlerPlugin* plugin = CMenuHandlerPlugin::NewL( TUid::Uid(0x200113E1), menu );
    CleanupStack::PushL( plugin );
    if( plugin )
    	{
    	result = KErrNone;	
    	}
    
    CleanupStack::PopAndDestroy( plugin );
    CleanupStack::PopAndDestroy( &menu );
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_MenuHandlerPluginDeleteL(CStifItemParser& /*aItem*/ )
    {
    TInt result (KErrGeneral);
    RMenu menu; 
    CleanupClosePushL( menu ); 
    SetupMenuL( menu );
    WaitL( 200000 );

    CMenuHandlerPlugin* plugin = CMenuHandlerPlugin::NewL( TUid::Uid( 0x200113E1 ), menu );
    //CMenuHandlerPlugin* plugin;
    //TRAPD(err, plugin = CMenuHandlerPlugin::NewL( TUid::Uid( MENU_HANDLER_IF_UID ), menu ));
    delete plugin;
    plugin = NULL;
    result = KErrNone;	
    
    CleanupStack::PopAndDestroy( &menu );
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_MenuHandlerPluginConstructL(CStifItemParser& /*aItem*/ )
    {
    TInt result (KErrGeneral);
    RMenu menu;
    CleanupClosePushL( menu ); 
    SetupMenuL( menu );
    WaitL( 200000 );

    CMenuHandlerPlugin* plugin = new CMenuHandlerPlugin( menu );
    delete plugin;
    plugin = NULL;

    result = KErrNone;	

    CleanupStack::PopAndDestroy( &menu );
    return result;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuHandlerSupportsType(CStifItemParser& /*aItem*/ )
	{
    TInt result (KErrGeneral);
    RMenu menu; 
    CleanupClosePushL( menu ); 
    SetupMenuL( menu );
    WaitL( 200000 );

    CMenuHandlerPlugin* plugin = CMenuHandlerPlugin::NewL( TUid::Uid(0x200113E1), menu );
    CleanupStack::PushL( plugin );
    
    
    if( plugin && !plugin->SupportsType(KNullDesC))
    	{
    	result = KErrNone;	
    	}
    
    CleanupStack::PopAndDestroy( plugin );
    CleanupStack::PopAndDestroy( &menu );
    return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuHandlerHandleCommandL(CStifItemParser& /*aItem*/ )
	{
    TInt result (KErrGeneral);
    TInt itemId(2);
    RMenu menu; 
    CleanupClosePushL( menu ); 
    SetupMenuL( menu );
    WaitL( 200000 );
    
    CMenuItem *item;
    TRAP_IGNORE(item = CMenuItem::OpenL(menu, itemId));
    CleanupStack::PushL(item);
    WaitL( 200000 );

    CMenuHandlerPlugin* plugin = CMenuHandlerPlugin::NewL( TUid::Uid(0x200113E0), menu );
    CleanupStack::PushL( plugin );
    
    
    if( plugin && plugin->SupportsType(KMenuTypeApp))
    	{
    	CMCSMenuOpWatcher* watcher =  CMCSMenuOpWatcher::NewL();
    	CleanupStack::PushL( watcher );
    	
    	TRAPD(err, plugin->HandleCommandL( *item, _L8("NonExistingOpperation"), KNullDesC8, watcher->iStatus));
    	if ( err == KErrNotSupported)
    		{
    		result = KErrNone;	
    		}	
    	CleanupStack::PopAndDestroy(watcher);                
    	result = KErrNone;	
    	}
    
    CleanupStack::PopAndDestroy( plugin );
    CleanupStack::PopAndDestroy( item );
    CleanupStack::PopAndDestroy( &menu );
    return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMCSAPITest::Test_CMenuHandlerDestructor(CStifItemParser& /*aItem*/ )
	{
    TInt result (KErrGeneral);
    RMenu menu; 
    CleanupClosePushL( menu ); 
    SetupMenuL( menu );
    WaitL( 200000 );

    CMenuHandlerPlugin* plugin = CMenuHandlerPlugin::NewL( TUid::Uid(0x200113E1), menu );
    
    if( plugin )
    	{
    	// Destructor call by delete
    	delete plugin;
    	result = KErrNone;	
    	}
    
    CleanupStack::PopAndDestroy( &menu );
    return result;
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 

//  End of File
