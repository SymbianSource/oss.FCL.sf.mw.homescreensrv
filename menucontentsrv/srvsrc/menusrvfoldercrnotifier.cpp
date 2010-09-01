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
#include "menusrvfoldercrnotifier.h"
//#include "menusrvappscanner.h"
#include "menusrvengutils.h"

#include "menusrvmmchistory.h"
#include "menueng.h"
#include "menuengobject.h"
#include "mcsmenuitem.h"
#include "pathinfo.h"
#include "menulogger.h"
#include "centralrepository.h"



// ==================== MEMBER FUNCTIONS ====================

// ---------------------------------------------------------
// CMenuSrvFolderCrNotifier::NewL
// ---------------------------------------------------------
//
CMenuSrvFolderCrNotifier* CMenuSrvFolderCrNotifier::NewL( CMenuEng& aEng )
    {
    CMenuSrvFolderCrNotifier* scanner = new (ELeave) CMenuSrvFolderCrNotifier( aEng );
    CleanupStack::PushL( scanner );
    scanner->ConstructL();
    CleanupStack::Pop( scanner );
    return scanner;
    }

// ---------------------------------------------------------
// CMenuSrvFolderCrNotifier::CMenuSrvFolderCrNotifier
// ---------------------------------------------------------
//
CMenuSrvFolderCrNotifier::~CMenuSrvFolderCrNotifier()
    {
    iHiddenCRFolders.Close();
    iNewHiddenCRFolders.Close();
    
    Cancel();
    iEng.DequeueOperation( *this );
    iFs.Close();

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
CMenuSrvFolderCrNotifier::CMenuSrvFolderCrNotifier( CMenuEng& aEng )
: CActive( CActive::EPriorityStandard ), iEng( aEng )
    {
    CActiveScheduler::Add( this );
    iOpStatus = EFalse;
    }

// ---------------------------------------------------------
// CMenuSrvFolderCrNotifier::ConstructL
// ---------------------------------------------------------
//
void CMenuSrvFolderCrNotifier::ConstructL()
    {
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
    
    iFreeSpaceObserver = CMcsFreeSpaceObserver::NewL( *this );
   	iCenRepSession = CRepository::NewL( KCRUidMenu );

	iCenRepNotifyHandlerCPHiddenFolders = CCenRepNotifyHandler::NewL(
	    *this, *iCenRepSession, 
		CCenRepNotifyHandler::EStringKey, KMenuHideCPFolder );

	iCenRepNotifyHandlerCPHiddenFolders->StartListeningL();
	iHiddenCRFolders.CreateL( KCenRepBufferSize );
	iNewHiddenCRFolders.CreateL( KCenRepBufferSize );
	HandleNotifyString( KMenuHideCPFolder, KNullDesC() );
    }

// ---------------------------------------------------------
// CMenuSrvFolderCrNotifier::RunL
// ---------------------------------------------------------
//
void CMenuSrvFolderCrNotifier::RunL()
    {
    User::LeaveIfError( iStatus.Int() ); // Handle errors in RunL.
    // Now queue this operation to be executed by the Engine.
    iEng.QueueOperationL( *this );
    }

// ---------------------------------------------------------
// CMenuSrvFolderCrNotifier::DoCancel
// ---------------------------------------------------------
//
void CMenuSrvFolderCrNotifier::DoCancel()
    {
    }

// ---------------------------------------------------------
// CMenuSrvFolderCrNotifier::RunError
// ---------------------------------------------------------
//
TInt CMenuSrvFolderCrNotifier::RunError( TInt /*aError*/ )
    {
    // Ignore the error 
    return KErrNone;
    }

// ---------------------------------------------------------
// CMenuSrvFolderScanner::HandleFreeSpaceEventL
// ---------------------------------------------------------
//
void CMenuSrvFolderCrNotifier::HandleFreeSpaceEventL()
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
// CMenuSrvFolderCrNotifier::RunMenuEngOperationL
// ---------------------------------------------------------
//
void CMenuSrvFolderCrNotifier::RunMenuEngOperationL()
    {
    HandleNewCrHiddenFoldersL( iNewHiddenCRFolders );
    }

// ---------------------------------------------------------
// CMenuSrvFolderCrNotifier::CompletedMenuEngOperation
// ---------------------------------------------------------
//
void CMenuSrvFolderCrNotifier::CompletedMenuEngOperation( TInt /*aErr*/ )
    {
    // If there was error, ignore it (what else could we do?).
    // When next AppArc update occurs, we will run again.
    }

// -----------------------------------------------------------------------------
// CMenuSrvFolderCrNotifier::HandleNotifyString
// -----------------------------------------------------------------------------
//
void CMenuSrvFolderCrNotifier::HandleNotifyString( TUint32 aKey, 
        const TDesC16& aNewValue )
    {
    if (( aKey == KMenuHideCPFolder ))
        {
        iOpStatus = ETrue;
        TRequestStatus* ownStatus = &iStatus;
        *ownStatus = KRequestPending;
        SetActive();
        User::RequestComplete( ownStatus, KErrNone );

        iHiddenCRFolders = iNewHiddenCRFolders;
        iNewHiddenCRFolders = aNewValue;
        }
    }  


// ---------------------------------------------------------
// CMenuSrvFolderCrNotifier::ParseHiddenFoldersL
// ---------------------------------------------------------
//
void CMenuSrvFolderCrNotifier::ParseHiddenFoldersL( 
            const TDesC& aHiddenFolders, RArray<TPtrC>& aHiddenFoldersArray )
    {
    TLex input( aHiddenFolders );
    TLexMark startMark;
    input.Mark( startMark ); 
    TBool notEmpty = false;
    while ( !input.Eos() )
        {
        if( input.Peek() == ',')
            {
            User::LeaveIfError( aHiddenFoldersArray.
                            Append( input.MarkedToken( startMark )  ) );            
            input.Inc();
            input.Mark( startMark );
            }
        input.Inc();     
        notEmpty = true;       
        }
	if ( notEmpty )
		{
		User::LeaveIfError( aHiddenFoldersArray.
						Append( input.MarkedToken( startMark )  ) );            
		}
   }
// ---------------------------------------------------------
// CMenuSrvFolderCrNotifier::NotifyEngineL
// ---------------------------------------------------------
//
void CMenuSrvFolderCrNotifier::NotifyEngineL( const TDesC& aFolder )
    {
	//find id by group name
	TInt root;
	iEng.RootFolderL( root );
	TMenuSrvTypeAttrFilter appFilter;
	appFilter.SetType( KMenuTypeFolder() );
	appFilter.SetAttr( KMenuAttrAppGroupName(), aFolder );
	
	RArray<TMenuItem> items; // All folders items.
	CleanupClosePushL( items );
	iEng.GetItemsL( items, root, &appFilter, ETrue );
	for ( TInt j = 0; j < items.Count(); j++ )
		{
		//we append notification for parrent
		iEng.ModifiableObjectL( items[j].Id(), RMenuNotifier::EItemsAddedRemoved );
		}
	
	CleanupStack::PopAndDestroy( &items );
    }

// ---------------------------------------------------------
// CMenuSrvFolderCrNotifier::NotifyEngineL
// ---------------------------------------------------------
//
void CMenuSrvFolderCrNotifier::HandleNewCrHiddenFoldersL( const TDesC& aNewHiddenFolders )
    {
	RArray<TPtrC> hiddenFoldersArray;
    CleanupClosePushL( hiddenFoldersArray );
	ParseHiddenFoldersL( iHiddenCRFolders, hiddenFoldersArray );
	
	RArray<TPtrC> newHiddenFoldersArray;
    CleanupClosePushL( newHiddenFoldersArray );
    ParseHiddenFoldersL( aNewHiddenFolders, newHiddenFoldersArray );

    //send notify for new values,
    NotifyEngineL( hiddenFoldersArray, newHiddenFoldersArray );
    //send notify for removed values,
    NotifyEngineL( newHiddenFoldersArray, hiddenFoldersArray );
    
    CleanupStack::PopAndDestroy( &newHiddenFoldersArray );
    CleanupStack::PopAndDestroy( &hiddenFoldersArray );
    }

// ---------------------------------------------------------
// CMenuSrvFolderCrNotifier::NotifyEngineL
// ---------------------------------------------------------
//
void CMenuSrvFolderCrNotifier::NotifyEngineL( 
		const RArray<TPtrC>& aLeftArray, 
		const RArray<TPtrC>& aRightArray )
    {
  	for ( TInt i = 0; i < aRightArray.Count(); i++ )
 		{
        TInt id = aLeftArray.Find( aRightArray[i] );
        if ( id == KErrNotFound )
            {
            NotifyEngineL( aRightArray[i] );
            }
 		}
    }

//  End of File  

