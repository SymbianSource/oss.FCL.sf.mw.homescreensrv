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
* Description:  Interface to MCS service
 *
*/


#include "mcsdef.h"

#include "mcsservice.h"
#include "mcsgetlistimpl.h"
#include "mcsexecuteaction.h"
#include "mcsrunningapps.h"
#include "mcsrequestnotification.h"
#include "mcscallback.h"
#include "mcsconstants.h"
#include "debug.h"
#include "mcsupdateitem.h"
#include "mcsdeleteitem.h"
#include "mcsorganizeimpl.h"
//_LIT( KRunningStatus, "RunningStatus" );

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
EXPORT_C CMCSService* CMCSService::NewL(const TDesC& aContent)
	{
	CMCSService* self = new (ELeave) CMCSService;
	CleanupStack::PushL( self );
	self->ConstructL(aContent);
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CMCSService::~CMCSService()
	{
	if( iReqNotifier )
		{
		TRAP_IGNORE(iReqNotifier->StopNotificationL());
		delete iReqNotifier;
		iReqNotifier = NULL;
		}
	iOperationManager.Close();	
	iMCS.Close();
	delete iIgnoredAttributes;
	delete iRequiredAttributes;
	}

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CMCSService::CMCSService()
	{
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
void CMCSService::ConstructL(const TDesC& aContent)
	{
	iMCS.OpenL(aContent);
	iIsRegistered = EFalse;
	
	iIgnoredAttributes = new (ELeave) CDesC16ArrayFlat(1);
	iRequiredAttributes = new (ELeave) CDesC16ArrayFlat(8);
	}

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CMCSService::SetFlagInMenuitemL( CLiwMap& aInputMap, 
		CMenuItem& aMenuitem, TMenuItem::TFlags aFlag)
	{
	TLiwVariant param;
	TPtrC8 key ( KNullDesC8 );
	switch ( aFlag )
		{
		case TMenuItem::ELockDelete:
			key.Set( KDeleteLocked );
			break;
		case TMenuItem::ELockName:
			key.Set( KNameLocked );
			break;
		case TMenuItem::ELockIcon:
			key.Set( KIconLocked );
			break;
		case TMenuItem::ELockMoveInto:
			key.Set( KMoveLocked  );
			break;
		case TMenuItem::ELockItem:
			key.Set( KAddingLocked );
			break;
		case TMenuItem::EHidden:
			key.Set( KHidden );
			break;	    	    	    	    	    
		case TMenuItem::EMissing:
			key.Set( KMissing );
			break;	    
		}
	if( aInputMap.FindL( key, param))
		{
		if (param.AsTBool())
			{
			aMenuitem.SetFlags(aFlag, ETrue);
			}
		else
			{
			aMenuitem.SetFlags(aFlag, EFalse);
			}
		aInputMap.Remove( key );
		param.Reset();
		}

	}

// ---------------------------------------------------------------------------
// Update item and its attributes.
// ---------------------------------------------------------------------------
//
EXPORT_C void   CMCSService::UpdateItemL(MMCSCallback* aCallBack, TDesC& aType, 
		TInt aId, TInt aParentId, TInt aBeforeId, CLiwMap& aInputMap)
	{
	TBuf8<KMenuMaxAttrNameLen> attrname;
	TBuf16<KMenuMaxAttrNameLen> attrname16;
	TBuf16<KMenuMaxAttrValueLen> paramValStr;
	TPtrC paramvalue;
	TLiwVariant param;

	if( !aCallBack )
		{
		User::Leave( KErrArgument );
		}
	CMenuItem* menuItem;

	// If if is zero then create new menu item
	if (!aId)
		{
		menuItem = CMenuItem::CreateL( iMCS, aType, aParentId, aBeforeId );
		}
	// Otherwise open existing menu item
	else
		{
		menuItem = CMenuItem::OpenL(iMCS, aId);
		}
	CleanupStack::PushL( menuItem ); 

	attrname.Copy(KMenuAttrRefId());  
	if( aInputMap.FindL( attrname, param))
		{
		paramValStr.Num( param.AsTInt32());
		aInputMap.Remove(attrname);
		menuItem->SetAttributeL(KMenuAttrRefId(), paramValStr);
		}
	else
		{
		if( aType == KMenuTypeLink)
			{
			User::Leave( KErrArgument );
			}
		}
	param.Reset();

	// Flags parsing
	SetFlagInMenuitemL( aInputMap, *menuItem, TMenuItem::ELockName);
	SetFlagInMenuitemL( aInputMap, *menuItem, TMenuItem::ELockIcon);
	SetFlagInMenuitemL( aInputMap, *menuItem, TMenuItem::ELockDelete);
	SetFlagInMenuitemL( aInputMap, *menuItem, TMenuItem::ELockMoveInto);
	SetFlagInMenuitemL( aInputMap, *menuItem, TMenuItem::ELockItem);
	SetFlagInMenuitemL( aInputMap, *menuItem, TMenuItem::EHidden);
	SetFlagInMenuitemL( aInputMap, *menuItem, TMenuItem::EMissing);

	for(TInt index = 0; index < aInputMap.Count(); index++)
		{
		aInputMap.AtL(index, attrname);
		aInputMap.FindL( attrname, param);
		paramvalue.Set(param.AsDes());
		attrname16.Copy(attrname);
		menuItem->SetAttributeL( attrname16,  paramvalue );
		param.Reset();
		}

	CMCSUpdateItem* updateAO = CMCSUpdateItem::NewL( *menuItem, aCallBack,
			iOperationManager );
	CleanupStack::PushL( updateAO );
	iOperationManager.AddL( updateAO );
	CleanupStack::Pop(updateAO);
	updateAO->UpdateItemL();

	CleanupStack::Pop(menuItem);
	}

// ---------------------------------------------------------------------------
// GetListL 
// ---------------------------------------------------------------------------
//
EXPORT_C void CMCSService::GetListL( const CLiwGenericParamList& aInParamList, MMCSCallback* aCallBack )
	{
	if( !aCallBack )
		{
		User::Leave( KErrArgument );
		}
	CMCSGetListImpl* getListAO = CMCSGetListImpl::NewL( iMCS, aCallBack, 
			iOperationManager );
	CleanupStack::PushL(getListAO);
	iOperationManager.AddL( getListAO );
	CleanupStack::Pop(getListAO);
	getListAO->GetListL( aInParamList );
	}

// ---------------------------------------------------------------------------
// GetRunningAppsL 
// ---------------------------------------------------------------------------
//
EXPORT_C  void CMCSService::GetRunningAppsL( MMCSCallback* aCallback ) 
	{
	if( !aCallback )
		{
		User::Leave( KErrArgument );
		}

	CMCSRunningApps* getRunningAppsAO = CMCSRunningApps::NewL( iMCS, aCallback, 
			iOperationManager );
	CleanupStack::PushL(getRunningAppsAO);
	iOperationManager.AddL( getRunningAppsAO );
	CleanupStack::Pop(getRunningAppsAO);
	getRunningAppsAO->GetRunningApplicationL();
	}

// ---------------------------------------------------------------------------
// ExecuteActionL 
// ---------------------------------------------------------------------------
//
EXPORT_C  void CMCSService::ExecuteActionL( MMCSCallback* aCallback, 
		TInt aItemId, TDesC8& aAction )
	{
	if( !aCallback )
		{
		User::Leave( KErrArgument );
		}
	CMCSExecuteAction* executeAO = CMCSExecuteAction::NewL( iMCS, aCallback, 
			iOperationManager );
	CleanupStack::PushL( executeAO );
	iOperationManager.AddL( executeAO );
	CleanupStack::Pop(executeAO);
	executeAO->ExecuteActionL( aItemId, aAction);
	}

// ---------------------------------------------------------------------------
// DeleteL 
// ---------------------------------------------------------------------------
//
EXPORT_C void CMCSService::DeleteL( MMCSCallback* aCallBack, TInt aItemId )
	{
	if( !aCallBack )
		{
		User::Leave( KErrArgument );
		}
	CMCSDeleteItem* deleteAO = CMCSDeleteItem::NewL( iMCS, aCallBack,
			iOperationManager );
	CleanupStack::PushL(deleteAO);
	iOperationManager.AddL( deleteAO );
	CleanupStack::Pop(deleteAO);
	deleteAO->DeleteItemL( aItemId );
	}


// ---------------------------------------------------------------------------
// Set GetRunningAppsL output to output parameter
// ---------------------------------------------------------------------------
//                         
EXPORT_C void CMCSService::SetRunningAppsOutputL( RArray<TUid>& aOutputArray, 
		CLiwGenericParamList& aOutParamList )
	{
	//List of Maps
	TBuf8<KMenuMaxAttrNameLen> attrname;
	CLiwDefaultList *list = CLiwDefaultList::NewL();
	CleanupStack::PushL( list );

	TInt arrCount = aOutputArray.Count();

	for( TInt index = 0; index < arrCount; ++index )
		{  
		CLiwDefaultMap* map = CLiwDefaultMap::NewLC(); 

		TUid auid = aOutputArray[index];
		attrname.Copy(KMenuAttrUid());
	
		map->InsertL( attrname, TLiwVariant( (TInt32) aOutputArray[index].iUid ));

		list->AppendL( TLiwVariant( map ) );

		CleanupStack::PopAndDestroy( map );        
		}

	aOutParamList.AppendL( TLiwGenericParam( KReturnValue, TLiwVariant( list ) ) );

	CleanupStack::Pop( list );
	list->DecRef();
	}

// ---------------------------------------------------------------------------
// Organize
// ---------------------------------------------------------------------------
//
EXPORT_C void CMCSService::OrganizeL( MMCSCallback* aCallBack, 
		TInt aMoveItemId, TInt aBeforeItemId )
	{
	if( !aCallBack )
		{
		User::Leave( KErrArgument );
		}

	CMCSOrganizeImpl* organizeAO = CMCSOrganizeImpl::NewL( iMCS, aCallBack,
			iOperationManager );
	CleanupStack::PushL( organizeAO );
	iOperationManager.AddL( organizeAO );
	CleanupStack::Pop( organizeAO );// we pop item from cleanupstack because
	//ownership was transfered to operation manager                                                         
	organizeAO->OrganizeL( aMoveItemId, aBeforeItemId );
	}

// ---------------------------------------------------------------------------
// Organize
// ---------------------------------------------------------------------------
//
EXPORT_C void CMCSService::OrganizeL( MMCSCallback* aCallBack, 
		const RArray<TInt>& aList, TInt aFolderId, TInt aBeforeItemId )
	{
	if( !aCallBack )
		{
		User::Leave( KErrArgument );
		}


	CMCSOrganizeImpl* organizeAO = CMCSOrganizeImpl::NewL( iMCS, aCallBack,
			iOperationManager );
	CleanupStack::PushL( organizeAO );
	iOperationManager.AddL( organizeAO );
	CleanupStack::Pop( organizeAO );// we pop item from cleanupstack because
	//ownership was transfered to operation manager
	organizeAO->OrganizeL( aList, aFolderId, aBeforeItemId );
	}

// ---------------------------------------------------------------------------
// Set RequestNotification output to output parameter
// ---------------------------------------------------------------------------
//   
EXPORT_C void CMCSService::SetReqNotificationOutputL( TInt aFolderId,
		TInt aEvent, CLiwGenericParamList& aOutParamList )
	{

	TBuf8<KMenuMaxAttrNameLen> attrname;
	CLiwDefaultMap* map = CLiwDefaultMap::NewLC();

	attrname.Copy(KMenuAttrId());
	map->InsertL( attrname, TLiwVariant( (TInt32)aFolderId));

	if(aEvent & RMenuNotifier::EItemsAddedRemoved)
		{
		map->InsertL( KAddRemove, TLiwVariant( (TBool) ETrue) );

		}
	if( aEvent & RMenuNotifier::EItemsReordered)
		{
		map->InsertL( KReorder, TLiwVariant( (TBool) ETrue) );
		}
	if( aEvent & RMenuNotifier::EItemAttributeChanged)
		{
		map->InsertL( KAttributeChange, TLiwVariant( (TBool) ETrue) );
		}

	aOutParamList.AppendL( TLiwGenericParam( KReturnValue, TLiwVariant( map ) ) );
	CleanupStack::PopAndDestroy( map );
	}


// ---------------------------------------------------------------------------
// Stop Notification for changes in mcs
// ---------------------------------------------------------------------------
//
EXPORT_C void CMCSService::UnRegisterObserverL()
	{
	if ( iReqNotifier == NULL )
		{
		User::Leave( KErrNotFound );  
		}
	iReqNotifier->StopNotificationL();

	delete iReqNotifier;
	iReqNotifier = NULL;
	iIsRegistered = EFalse;
	}

// ---------------------------------------------------------------------------
// Start Notification for changes in mcs
// ---------------------------------------------------------------------------
//
EXPORT_C void CMCSService::RegisterObserverL(TInt aFolderId,TInt aEvents,MMCSCallback* aCallBack )
	{
	if( !aCallBack )
		User::Leave( KErrArgument );

	if(!iIsRegistered)
		{
		iReqNotifier = CMCSReqNotification::NewL( iMCS );
		iIsRegistered = ETrue;
		}
	iReqNotifier->StartNotificationL(aFolderId, aEvents, aCallBack);
	}

