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
* Description:  Central Repository about DB state change information observer
 *
*/

#include <centralrepository.h>
#include <f32file.h>
#include "cpserverdef.h"
#include "chdbstatehandler.h"
#include "chfactorysettingsplugin.h"


_LIT( KParsedDir, "parsed\\fs\\" );
_LIT( KDriveC, "C:" );


// ============================ MEMBER FUNCTIONS =============================

// -----------------------------------------------------------------------------
// CCHDbStateHandler::CCHDbStateHandler()
// 
// -----------------------------------------------------------------------------
//
CCHDbStateHandler::CCHDbStateHandler( CCHFactorySettings* aCallback ) : 
	CActive( EPriorityStandard ), // Standard priority
	iCallback( aCallback )
	{
	}

// -----------------------------------------------------------------------------
// CCHDbStateHandler::NewL()
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CCHDbStateHandler* CCHDbStateHandler::NewL( 
	CCHFactorySettings* aCallback )
	{
	CCHDbStateHandler* self = new (ELeave) CCHDbStateHandler( aCallback );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

// -----------------------------------------------------------------------------
// CCHDbStateHandler::NewL()
// Constructor
// -----------------------------------------------------------------------------
//
void CCHDbStateHandler::ConstructL()
	{
	User::LeaveIfError( iFs.Connect() );
	ConstructFactorySettingsPathL();
	iRepository = CRepository::NewL( KServerUid );
	HandleDbStateL();
	CActiveScheduler::Add(this); // Add to scheduler
	iRepository->NotifyRequest( KCPStorageUid, iStatus );
	iStatus = KRequestPending;
	SetActive();
	}

// -----------------------------------------------------------------------------
// CCHDbStateHandler::NewL()
// 
// -----------------------------------------------------------------------------
//
CCHDbStateHandler::~CCHDbStateHandler()
	{
	Cancel(); // Cancel any request, if outstanding
	delete iRepository;
	iFs.Close();
	}

// -----------------------------------------------------------------------------
// CCHDbStateHandler::NewL()
// 
// -----------------------------------------------------------------------------
//
void CCHDbStateHandler::DoCancel()
	{
	iRepository->NotifyCancel( KCPStorageUid );
	}

// -----------------------------------------------------------------------------
// CCHDbStateHandler::RunL()
// 
// -----------------------------------------------------------------------------
//
void CCHDbStateHandler::RunL()
	{
	// status contais information about event type
	if (iStatus.Int() >= KErrNone) 
		{
		HandleDbStateL();
		iRepository->NotifyRequest( KCPStorageUid, iStatus );
		iStatus = KRequestPending;
		SetActive();
     	}
	}

// -----------------------------------------------------------------------------
// CCHDbStateHandler::RunError()
// 
// -----------------------------------------------------------------------------
//
TInt CCHDbStateHandler::RunError(TInt aError)
	{
	return aError;
	}

// -----------------------------------------------------------------------------
// CCHDbStateHandler::ConstructFactorySettingsPathL()
// 
// -----------------------------------------------------------------------------
//
void CCHDbStateHandler::ConstructFactorySettingsPathL()
	{
	TFileName privatePath;
	User::LeaveIfError( iFs.PrivatePath( privatePath ) );
	iFSPath.Append( KDriveC );
	iFSPath.Append( privatePath );
	iFSPath.Append( KParsedDir );
	}

// -----------------------------------------------------------------------------
// CCHDbStateHandler::RemoveFactorySettingsParsedFilesL()
// 
// -----------------------------------------------------------------------------
//
void CCHDbStateHandler::RemoveFactorySettingsParsedFilesL()
	{
	CFileMan* fileManager = CFileMan::NewL( iFs );
	CleanupStack::PushL( fileManager );
	fileManager->Delete( iFSPath,  CFileMan::ERecurse );
	CleanupStack::PopAndDestroy( fileManager );
	}

// -----------------------------------------------------------------------------
// CCHDbStateHandler::HandleDbStateL()
// 
// -----------------------------------------------------------------------------
//
void CCHDbStateHandler::HandleDbStateL()
	{
	TInt value( 0 );
	if( iRepository->Get( KCPStorageUid, value ) == KErrNone )
		{
		if( value & KSQLDBStateRestored )
			{
			HandleRestoredDbStateL();
			value ^= KSQLDBStateRestored;
			value |= KSQLDBStateNormal;
			iRepository->Set( KCPStorageUid, value );
			}
		}
	}

// -----------------------------------------------------------------------------
// CCHDbStateHandler::HandleRestoredStateL()
// 
// -----------------------------------------------------------------------------
//
void CCHDbStateHandler::HandleRestoredDbStateL()
	{
	RemoveFactorySettingsParsedFilesL();
	iCallback->UpdateL();
	}


//End file
