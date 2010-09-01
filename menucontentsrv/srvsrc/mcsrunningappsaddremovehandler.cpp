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

#include "mcsrunningappsaddremovehandler.h"
#include "menueng.h"
#include "mcsrunningappshandler.h"

// ---------------------------------------------------------
// CMcsRunningAppsAddRemoveHandler::NewL
// ---------------------------------------------------------
//
CMcsRunningAppsAddRemoveHandler* CMcsRunningAppsAddRemoveHandler::NewL(
    CMenuEng& aEng, MMcsRunningAppsHandler* aRunningAppsHandler )
	{
	CMcsRunningAppsAddRemoveHandler* self = 
		new ( ELeave ) CMcsRunningAppsAddRemoveHandler( aEng, 
		aRunningAppsHandler );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------
// CMcsRunningAppsAddRemoveHandler::~CMcsRunningAppsAddRemoveHandler
// ---------------------------------------------------------
//
CMcsRunningAppsAddRemoveHandler::~CMcsRunningAppsAddRemoveHandler()
    {
    iEng.DequeueOperation( *this );
    iFolders.Close();
    }

// ---------------------------------------------------------
// CMcsRunningAppsAddRemoveHandler::CMcsRunningAppsAddRemoveHandler
// ---------------------------------------------------------
//
CMcsRunningAppsAddRemoveHandler::CMcsRunningAppsAddRemoveHandler(
    CMenuEng& aEng, MMcsRunningAppsHandler* aRunningAppsHandler):
    iEng(aEng),
    iRunningAppsHandler(aRunningAppsHandler)
    {
    iOperationInProgress = EFalse;
    }

// ---------------------------------------------------------
// CMcsRunningAppsAddRemoveHandler::HandleAddRemoveL
// ---------------------------------------------------------
//
void CMcsRunningAppsAddRemoveHandler::HandleAddRemoveL( TInt aFolder )
    {
    iFolders.AppendL( aFolder );
    if( !iOperationInProgress )
    	{
    	iEng.QueueOperationL( *this );
    	iOperationInProgress = ETrue;
    	}
    }    

// ---------------------------------------------------------
// CMcsRunningAppsAddRemoveHandler::ConstructL
// ---------------------------------------------------------
//
void CMcsRunningAppsAddRemoveHandler::ConstructL()
	{
	}

// ---------------------------------------------------------
// CMcsRunningAppsAddRemoveHandler::RunMenuEngOperationL
// ---------------------------------------------------------
//
void CMcsRunningAppsAddRemoveHandler::RunMenuEngOperationL()
    {
    iRunningAppsHandler->HandleAddRemoveRunningAppL( iFolders[0] );
    iFolders.Remove( 0 );
    iOperationInProgress = EFalse;
    if ( iFolders.Count() > 0 )
    	{
    	iEng.QueueOperationL( *this );
    	iOperationInProgress = ETrue;
    	}
    } 
       
// ---------------------------------------------------------
// CMcsRunningAppsAddRemoveHandler::CompletedMenuEngOperation
// ---------------------------------------------------------
//
void CMcsRunningAppsAddRemoveHandler::CompletedMenuEngOperation( TInt /*aErr*/ )
    {
    // If there was error, ignore it (what else could we do?).
    // When next AppArc update occurs, we will run again.
    }
//  End of File  
