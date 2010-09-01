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
* Description:  
*
*/


#include <e32cmn.h> 

#include "mcsrunningappshiddenattrscanner.h"
#include "mcsrunningappshandler.h"
#include "menueng.h"
#include "mcsmenuitem.h"

const TInt KMaxUidLength = 10; 
// ---------------------------------------------------------
// CMcsRunningAppsHiddenAttrScanner::NewL
// ---------------------------------------------------------
//
CMcsRunningAppsHiddenAttrScanner* CMcsRunningAppsHiddenAttrScanner::NewL(
    CMenuEng& aEng, MMcsRunningAppsHandler* aRunningAppsHandler )
	{
	CMcsRunningAppsHiddenAttrScanner* self = 
		new ( ELeave ) CMcsRunningAppsHiddenAttrScanner( aEng, 
		aRunningAppsHandler );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------
// CMcsRunningAppsHiddenAttrScanner::~CMcsRunningAppsHiddenAttrScanner
// ---------------------------------------------------------
//
CMcsRunningAppsHiddenAttrScanner::~CMcsRunningAppsHiddenAttrScanner()
    {
    iEng.DequeueOperation( *this );
    iHiddenApps.Close();
    }

// ---------------------------------------------------------
// CMcsRunningAppsHiddenAttrScanner::CMcsRunningAppsHiddenAttrScanner
// ---------------------------------------------------------
//
CMcsRunningAppsHiddenAttrScanner::CMcsRunningAppsHiddenAttrScanner(
    CMenuEng& aEng, MMcsRunningAppsHandler* aRunningAppsHandler):
    iEng(aEng),
    iRunningAppsHandler(aRunningAppsHandler)
    {
    iOperationInProgress = EFalse;
    }

// ---------------------------------------------------------
// CMcsRunningAppsHiddenAttrScanner::ScanL
// ---------------------------------------------------------
//
void CMcsRunningAppsHiddenAttrScanner::ScanL()
    {
    if( !iOperationInProgress )
    	{
    	iEng.QueueOperationL( *this );
    	iOperationInProgress = ETrue;
    	}
    }    

// ---------------------------------------------------------
// CMcsRunningAppsHiddenAttrScanner::IsFswHidden
// ---------------------------------------------------------
//
TBool CMcsRunningAppsHiddenAttrScanner::IsHidden( TUid aUid )   
	{
	TBool result(EFalse);
	if( KErrNotFound != iHiddenApps.Find( aUid ) )
		{
		result = ETrue;
		}
	return result;
    } 

// ---------------------------------------------------------
// CMcsRunningAppsHiddenAttrScanner::ConstructL
// ---------------------------------------------------------
//
void CMcsRunningAppsHiddenAttrScanner::ConstructL()
	{
	UpdateHiddenApplicationsL();
	}

// ---------------------------------------------------------
// CMcsRunningAppsHiddenAttrScanner::RunMenuEngOperationL
// ---------------------------------------------------------
//
void CMcsRunningAppsHiddenAttrScanner::RunMenuEngOperationL()
    {
    UpdateHiddenApplicationsL();
    iOperationInProgress = EFalse;
    } 
       
// ---------------------------------------------------------
// CMcsRunningAppsHiddenAttrScanner::CompletedMenuEngOperation
// ---------------------------------------------------------
//
void CMcsRunningAppsHiddenAttrScanner::CompletedMenuEngOperation( TInt /*aErr*/ )
    {
    // If there was error, ignore it (what else could we do?).
    // When next AppArc update occurs, we will run again.
    }

// ---------------------------------------------------------
// CMcsRunningAppsHiddenAttrScanner::UpdateHiddenApplicationsL
// ---------------------------------------------------------
//
void CMcsRunningAppsHiddenAttrScanner::UpdateHiddenApplicationsL()
    {
    iHiddenApps.Reset();

    TInt root;
    iEng.RootFolderL( root );
    TMenuSrvHiddenAppFilter filter;
    
    RArray<TMenuItem> items;
    CleanupClosePushL( items );
    iEng.GetItemsL( items, root, &filter, ETrue );
       
    for (TInt i=0; i < items.Count(); i++)
        {        
        const CMenuEngObject& object = iEng.ObjectL( items[i].Id() );
        TBool tmp;
        TPtrC uidDesC(KNullDesC);
        object.FindAttribute( KMenuAttrUid(), uidDesC, tmp);        

        TBuf< KMaxUidLength > appUidDesC;
        TRadix radix;
        if ( KErrNotFound == uidDesC.Find( KHex ) )
        	{
        	radix = EDecimal;
        	appUidDesC = uidDesC;
        	}
        else
        	{
        	radix= EHex;
        	appUidDesC = uidDesC.Right( uidDesC.Length() - KHex().Length());
        	}
        
        TLex uidLex( appUidDesC );
        TUint uidInt ( NULL );
        uidLex.Val( uidInt, radix );
        TUid appUid (TUid::Uid( uidInt ));
        iHiddenApps.AppendL( appUid );
        }        
	iRunningAppsHandler->HandleListUpdateL( iHiddenApps, 
			MMcsRunningAppsHandler::EHiddenApps );
    CleanupStack::PopAndDestroy(&items);
    }


//  End of File  
