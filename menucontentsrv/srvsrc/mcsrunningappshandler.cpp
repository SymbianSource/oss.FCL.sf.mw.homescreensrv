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
#include <mcsdef.h>
#include "mcsmenuitem.h"
#include "mcsrunningappshandler.h"
#include "mcsrunningappswgmonitor.h"
#include "mcsrunningappsfswmonitor.h"
#include "mcsrunningappshiddenattrscanner.h"
#include "mcsrunningappsaddremovehandler.h"
#include "mcsmenunotifier.h"
#include "mcscachehandler.h"


// ---------------------------------------------------------
// CMcsRunningAppsHandler::NewL
// ---------------------------------------------------------
//
CMcsRunningAppsHandler* CMcsRunningAppsHandler::NewL(
    CMenuEng& aEng, CMcsCacheHandler& aCacheHandler )
	{
	CMcsRunningAppsHandler* self;
	self = new ( ELeave ) CMcsRunningAppsHandler( aEng, aCacheHandler );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------
// CMcsRunningAppsHandler::~CMcsRunningAppsHandler
// ---------------------------------------------------------
//
CMcsRunningAppsHandler::~CMcsRunningAppsHandler()
    {
    delete iWindowGroupMonitor;
    delete iFswMonitor;
    delete iHiddenAttrScanner;
    delete iAddRemoveHandler;
    iRunningApps.Close();
    iRunningFolderStatus.Close();
    }

// ---------------------------------------------------------
// CMcsRunningAppsHandler::CMcsRunningAppsHandler
// ---------------------------------------------------------
//
CMcsRunningAppsHandler::CMcsRunningAppsHandler(
	CMenuEng& aEng, CMcsCacheHandler& aCacheHandler ):
	iEng(aEng), 
	iCacheHandler(aCacheHandler)
    {
    }

// ---------------------------------------------------------
// CMcsRunningAppsHandler::ConstructL
// ---------------------------------------------------------
//
void CMcsRunningAppsHandler::ConstructL()
	{
	//iFswMonitor and iHiddenAttrScanner must be initialized before
	//iWindowGroupMonitor, because they update lists which are used by
	//iWindowGroupMonitor
	iFswMonitor = CMcsRunningAppsFswMonitor::NewL( iEng, this );
    iHiddenAttrScanner = CMcsRunningAppsHiddenAttrScanner::NewL( iEng, this );
    iWindowGroupMonitor = CMcsRunningAppsWgMonitor::NewL( iEng, this );
    iAddRemoveHandler = CMcsRunningAppsAddRemoveHandler::NewL( iEng, this );
    UpdateFoldersRunningStatusL();
    
	}

// ---------------------------------------------------------
// CMcsRunningAppsHandler::EngineEventsL
// ---------------------------------------------------------
//
void CMcsRunningAppsHandler::EngineEventsL( TInt aFolder, TInt aEvents )
	{
    if ( aEvents & RMenuNotifier::EItemsAddedRemoved )
        {
        iAddRemoveHandler->HandleAddRemoveL( aFolder );
        iHiddenAttrScanner->ScanL();
        }    
	}

// ---------------------------------------------------------
// CMcsRunningAppsHandler::EngineEvents
// ---------------------------------------------------------
//
void CMcsRunningAppsHandler::EngineEvents( TInt aFolder, TInt aEvents )
	{
	TRAP_IGNORE( EngineEventsL( aFolder, aEvents ) );      
	}


// ---------------------------------------------------------
// CMcsRunningAppsHandler::EngineEvents
// ---------------------------------------------------------
//
void CMcsRunningAppsHandler::GetRunningAppsL( RArray<TUid>& aArray )
	{
	for ( TInt i = 0; i< iRunningApps.Count(); i++ )
		{
		if ( iRunningApps[i].IsRunning() &&
				!iRunningApps[i].IsHidden() &&
				!iRunningApps[i].IsFswHidden() )
			{
			aArray.AppendL( iRunningApps[i].GetUid() );
			}
		}
	}

// ---------------------------------------------------------
// CMcsRunningAppsHandler::HandleListUpdateL
// ---------------------------------------------------------
//
void CMcsRunningAppsHandler::HandleListUpdateL(const RArray<TUid>& aArray, 
		TArrayDataType aType )
	{
	switch ( aType )
		{
		case ERunningApps:
			HandleRunningAppsChangedL( aArray );
			break;
		case EFswHiddenApps:
			HandleFswHiddenAppsChangedL( aArray );
			break;
		case EHiddenApps:
			HandleHiddenAppsChangedL( aArray );
			break;
		}
	}

// ---------------------------------------------------------
// CMcsRunningAppsHandler::HandleRunningAppsChangedL
// ---------------------------------------------------------
//
void CMcsRunningAppsHandler::HandleRunningAppsChangedL(
		const RArray<TUid>& aArray )
	{
	TInt count = iRunningApps.Count();
	//add new items to iRunningApps
	for ( TInt i = 0; i < aArray.Count(); i++ )
		{
		TRunningAppsAttr item( aArray[i] );
		TInt id = iRunningApps.Find( item, TRunningAppsAttr::MatchByUid );
		if (KErrNotFound == id)
			{
			item.SetRunning( ETrue );
			item.SetFswHidden( iFswMonitor->IsFswHidden( aArray[i] ) );
			item.SetHidden( iHiddenAttrScanner->IsHidden( aArray[i] ) );
			iRunningApps.AppendL( item );
			if ( !item.IsFswHidden() && !item.IsHidden() )
				{
				HandleNotificationL( aArray[i] );
				}
			}
		}
	//set proper running attribute value in iRunningApps 
	//(only for applications that were previously in Array)
	for ( TInt i = 0; i< count; i++ )
		{
		TUid appUid ( iRunningApps[i].GetUid() );
		
		TInt id = aArray.Find( appUid );
		if ( ( KErrNotFound == id ) && ( iRunningApps[i].IsRunning() ) )
			{
			iRunningApps[i].SetRunning( EFalse );
			if ( !iRunningApps[i].IsFswHidden() && 
					!iRunningApps[i].IsHidden() )
				{
				HandleNotificationL( appUid );
				}
			}
		else if ( ( KErrNotFound != id ) && ( !iRunningApps[i].IsRunning() ) )
			{
			iRunningApps[i].SetRunning( ETrue );
			if ( !iRunningApps[i].IsFswHidden() && 
					!iRunningApps[i].IsHidden() )
				{
				HandleNotificationL( appUid );
				}
			}
		}
	}

// ---------------------------------------------------------
// CMcsRunningAppsHandler::HandleFswHiddenAppsChangedL
// ---------------------------------------------------------
//
void CMcsRunningAppsHandler::HandleFswHiddenAppsChangedL(
		const RArray<TUid>& aArray )
	{
	//set proper FSW hidden value in iRunningApps 
	for ( TInt i = 0; i< iRunningApps.Count(); i++ )
		{
		TUid appUid ( iRunningApps[i].GetUid() );
		TInt id = aArray.Find( appUid );
		if ( ( KErrNotFound == id ) && ( iRunningApps[i].IsFswHidden() ) )
			{
			iRunningApps[i].SetFswHidden( EFalse );
			if ( iRunningApps[i].IsRunning() && 
					!iRunningApps[i].IsHidden() )
				{
				HandleNotificationL( appUid );
				}
			}
		else if ( ( KErrNotFound != id ) && ( !iRunningApps[i].IsFswHidden() ) )
			{
			iRunningApps[i].SetFswHidden( ETrue );
			if ( iRunningApps[i].IsRunning() && 
					!iRunningApps[i].IsHidden() )
				{
				HandleNotificationL( appUid );
				}
			}
		}	
	}

// ---------------------------------------------------------
// CMcsRunningAppsHandler::HandleHiddenAppsChangedL
// ---------------------------------------------------------
//
void CMcsRunningAppsHandler::HandleHiddenAppsChangedL(
		const RArray<TUid>& aArray )
	{
	//set proper hidden value in iRunningApps 
	for ( TInt i = 0; i< iRunningApps.Count(); i++ )
		{
		TUid appUid ( iRunningApps[i].GetUid() );
		TInt id = aArray.Find( appUid );
		if ( ( KErrNotFound == id ) && ( iRunningApps[i].IsHidden() ) )
			{
			iRunningApps[i].SetHidden( EFalse );
			if ( iRunningApps[i].IsRunning() && 
					!iRunningApps[i].IsFswHidden() )
				{
				HandleNotificationL( appUid );
				}
			}
		else if ( ( KErrNotFound != id ) && ( !iRunningApps[i].IsHidden() ) )
			{
			iRunningApps[i].SetHidden( ETrue );
			if ( iRunningApps[i].IsRunning() && 
					!iRunningApps[i].IsFswHidden() )
				{
				HandleNotificationL( appUid );
				}			
			}
		}		
	}



// ---------------------------------------------------------
// CMcsRunningAppsHandler::HandleNotificationL
// ---------------------------------------------------------
//
void CMcsRunningAppsHandler::HandleNotificationL( TInt aId )
	{
    TInt root;
    iEng.RootFolderL( root ); 	
	TInt parentId(0);                
	if ( aId != KErrNotFound )
		{
		//remove running status from cache (for application)
		iCacheHandler.HandleRemoveAttribute( aId, KRunningStatus );
		TBool oldStatus;
		TBool newStatus;
	    while( aId != root )
	    	{
	        iEng.ParentFolderL( aId, parentId );
	        iEng.AppendNotifyL(parentId, 
	        		RMenuNotifier::EItemAttributeChanged);
	        
	        oldStatus = GetFolderPreviousRunningStatusL( parentId );
	        newStatus = GetFolderRunningStatusL( parentId );
	        if( oldStatus != newStatus )
				{
				iCacheHandler.HandleRemoveAttribute( 
						parentId, KRunningStatus );
				UpdateFolderRunningStatusL( parentId, newStatus );
				aId = parentId;
				}
	        else
	        	{
	        	break;
	        	}
	    	}//while
		}//if
	}

// ---------------------------------------------------------
// CMcsRunningAppsHandler::HandleNotificationL
// ---------------------------------------------------------
//
void CMcsRunningAppsHandler::HandleNotificationL( TUid aAppUid )
	{
	RArray<TMenuItem> itemArray;
	CleanupClosePushL( itemArray );
	//list of all items representing specified uid 
	TInt error = GetItemsListL( aAppUid, itemArray );
	if( error == KErrNone )
		{
		TInt count = itemArray.Count();
		for( TInt i = 0; i < count; i++ )
			{
			HandleNotificationL( itemArray[i].Id() );
			}
		}
    CleanupStack::PopAndDestroy( &itemArray );
	}


// ---------------------------------------------------------
// CMcsRunningAppsHandler::GetItemsListL
// ---------------------------------------------------------
//

TInt CMcsRunningAppsHandler::GetItemsListL( TUid aAppUid, RArray<TMenuItem>& aItemArray )
    {
    TInt result(KErrNotFound);
    TInt root;
    iEng.RootFolderL( root );
    TMenuSrvTypeAttrFilter appFilter;
    appFilter.SetType( KMenuTypeApp() );
    TBuf<KUidChars> uidString;
    TInt appuid( aAppUid.iUid );
    MenuSrvUtil::UidToStringL( appuid, uidString, EFalse, EHex );
    appFilter.SetAttr( KMenuAttrUid(), uidString );
    iEng.GetItemsL( aItemArray, root, &appFilter, ETrue );
    //if not found in mcs try with UID in decimal format
    if (!aItemArray.Count())
        {
        MenuSrvUtil::UidToStringL( appuid, uidString, EFalse, EDecimal );
        appFilter.SetAttr( KMenuAttrUid(), uidString );
        iEng.GetItemsL( aItemArray, root, &appFilter, ETrue );
        }
   if ( aItemArray.Count() )
        {
        result = KErrNone;
        }
    return result;
    }    


// ---------------------------------------------------------
// CMcsRunningAppsHandler::UpdateFolderRunningStatusL
// ---------------------------------------------------------
//
void CMcsRunningAppsHandler::UpdateFolderRunningStatusL( TInt aFolderId, 
		TBool aNewRunningStatus )
    {
    TInt id = iRunningFolderStatus.Find( aFolderId );
    if ( aNewRunningStatus && ( id == KErrNotFound ))
    	{
    	iRunningFolderStatus.AppendL( aFolderId );
    	}
    if ( !aNewRunningStatus && ( id != KErrNotFound ))
    	{
    	iRunningFolderStatus.Remove( id );
    	}
    }


// ---------------------------------------------------------
// CMcsRunningAppsHandler::UpdateFoldersRunningStatusL
// ---------------------------------------------------------
//
void CMcsRunningAppsHandler::UpdateFoldersRunningStatusL(  )
    {
    TMenuSrvTypeFilter filter;
    filter.SetType( KMenuTypeFolder() );
      
    TInt root;
    iEng.RootFolderL( root );
    
    RArray<TMenuItem> aItemArray;
    CleanupClosePushL(aItemArray);
    iEng.GetItemsL( aItemArray , root, &filter, ETrue );
    for ( TInt i = 0; i < aItemArray.Count(); i++ )
    	{
    	if ( GetFolderRunningStatusL( aItemArray[i].Id() ) )
    		{
    		iRunningFolderStatus.AppendL( aItemArray[i].Id() );
    		}
    	}
    CleanupStack::PopAndDestroy( &aItemArray );
    }

// ---------------------------------------------------------
// CMcsRunningAppsHandler::GetRunningStatusL
// ---------------------------------------------------------
//
TBool CMcsRunningAppsHandler::GetRunningStatusL( TInt aItemId )
    {
    const CMenuEngObject& obj = iEng.ObjectL( aItemId );
    return GetRunningStatusL( obj );
    }

// ---------------------------------------------------------
// CMcsRunningAppsHandler::GetRunningStatusL
// ---------------------------------------------------------
//
TBool CMcsRunningAppsHandler::GetRunningStatusL( 
		const CMenuEngObject& aEngObj )
    {
    TBool runningApp( EFalse );
    if ( aEngObj.Type().CompareF( KMenuTypeApp ) == KErrNone )
        {
        TUid appUid;
        TInt errorUid = CMenuSrvEngUtils::GetAppUid( aEngObj, appUid );
        
        if( !errorUid )
            {
            runningApp = GetAppRunningStatusL( appUid );
            }
        }
    else if( aEngObj.Type().CompareF( KMenuTypeFolder ) == KErrNone )
        {
        runningApp = GetFolderRunningStatusL( aEngObj.Id() );
        }
    return runningApp;
    }

// ---------------------------------------------------------
// CMcsRunningAppsHandler::GetFolderPreviousRunningStatusL
// ---------------------------------------------------------
//
TBool CMcsRunningAppsHandler::GetFolderPreviousRunningStatusL( TInt aId )
	{
	if( KErrNotFound == iRunningFolderStatus.Find( aId ) )
		{
		return EFalse;
		}
	else
		{
		return ETrue;
		}
	
	}

// ---------------------------------------------------------
// CMcsRunningAppsHandler::GetFolderRunningStatusL
// ---------------------------------------------------------
//
TBool CMcsRunningAppsHandler::GetFolderRunningStatusL( TInt aId )
    {
    TBool running ( EFalse );
    TMenuSrvTypeFilter filter;
    filter.SetType( KMenuTypeApp() );
    
    RArray<TMenuItem> aItemArray;
    CleanupClosePushL(aItemArray);
    iEng.GetItemsL( aItemArray , aId , &filter, ETrue );
    for( TInt i = 0; i < aItemArray.Count(); i++)
        {
        const CMenuEngObject& obj = iEng.ObjectL( aItemArray[i].Id() );
        TUid appUid;
        TInt errorUid = CMenuSrvEngUtils::GetAppUid( obj, appUid );
        
        if( !errorUid &&  
                !(obj.Flags() & TMenuItem::EHidden) )
            {
            running = GetAppRunningStatusL( appUid  );
            }
        if( running )
            {
            break;
            }
        }
    CleanupStack::PopAndDestroy( &aItemArray );
    return running;
    }

// ---------------------------------------------------------
// CMcsRunningAppsHandler::GetAppRunningStatusL
// ---------------------------------------------------------
//
TBool CMcsRunningAppsHandler::GetAppRunningStatusL( TUid aUid )
    {
    TBool running ( EFalse );
    
	TRunningAppsAttr item( aUid );
	TInt id = iRunningApps.Find( item, TRunningAppsAttr::MatchByUid );
	
	if( ( id != KErrNotFound ) &&
			( iRunningApps[id].IsRunning() ) && 
			( !iRunningApps[id].IsHidden() ) && 
			( !iRunningApps[id].IsFswHidden() ) )
		{
		running = ETrue;
		}
    return running;
    }

// ---------------------------------------------------------
// CMcsRunningAppsHandler::HandleAddRemoveRunningAppL
// ---------------------------------------------------------
//
void CMcsRunningAppsHandler::HandleAddRemoveRunningAppL( TInt aFolder )
	{
	TBool oldStatus = GetFolderPreviousRunningStatusL( aFolder );
	TBool newStatus = GetFolderRunningStatusL( aFolder );
	if ( oldStatus != newStatus )
		{
		UpdateFolderRunningStatusL( aFolder, newStatus );
		HandleNotificationL( aFolder );
		}
	}

//  End of File  
