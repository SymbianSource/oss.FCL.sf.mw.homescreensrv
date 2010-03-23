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

#include <driveinfo.h>

#include "cawidgetmmcwatcher.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCaWidgetMmcWatcher::NewL
// Second phase constructor
// ---------------------------------------------------------
//
CCaWidgetMmcWatcher* CCaWidgetMmcWatcher::NewL( RFs& aFs, 
        MWidgetMmcWatcherCallback* aObserver )
	{
	CCaWidgetMmcWatcher* self = NewLC( aFs, aObserver );
	CleanupStack::Pop( self );
	return self;
	}
	
// ---------------------------------------------------------
// CCaWidgetMmcWatcher::NewLC
// Second phase constructor
// ---------------------------------------------------------
//
CCaWidgetMmcWatcher* CCaWidgetMmcWatcher::NewLC( RFs& aFs,
        MWidgetMmcWatcherCallback* aObserver )
	{
	CCaWidgetMmcWatcher* self = new (ELeave) CCaWidgetMmcWatcher( aFs, 
	        aObserver );
	CleanupStack::PushL ( self );
	self->ConstructL();
	return self;	
	}
	
// ---------------------------------------------------------
// CCaWidgetMmcWatcher::~CCaWidgetMmcWatcher
// Destructor
// ---------------------------------------------------------
//
CCaWidgetMmcWatcher::~CCaWidgetMmcWatcher()
	{	
	Cancel();
	}

// ---------------------------------------------------------
// CCaWidgetMmcWatcher::CCaWidgetMmcWatcher
// Default constructor
// ---------------------------------------------------------
//
CCaWidgetMmcWatcher::CCaWidgetMmcWatcher( RFs& aFs,
        MWidgetMmcWatcherCallback* aObserver )
	: CActive( CActive::EPriorityStandard ),
	iFs(aFs),
	iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

// ---------------------------------------------------------
// CCaWidgetMmcWatcher::ConstructL
// default Symbian OS constructor
// ---------------------------------------------------------
//
void CCaWidgetMmcWatcher::ConstructL()
	{
	WaitForChangeL();
	}

// ---------------------------------------------------------
// CCaWidgetMmcWatcher::WaitForChangeL
// Request notification for disk change
// ---------------------------------------------------------
//	
void CCaWidgetMmcWatcher::WaitForChangeL()
	{
	TInt mmcDrive;
	User::LeaveIfError( DriveInfo::GetDefaultDrive(
	    DriveInfo::EDefaultRemovableMassStorage, mmcDrive ) );
	TDriveName mmcDriveName( TDriveUnit( mmcDrive ).Name() );
	iFs.NotifyChange( ENotifyEntry, iStatus, mmcDriveName );
	SetActive();
	}

// ---------------------------------------------------------
// CCaWidgetMmcWatcher::DoCancel
// ---------------------------------------------------------
//
void CCaWidgetMmcWatcher::DoCancel()
	{
	iFs.NotifyChangeCancel();
	}
	
// ---------------------------------------------------------------------------
// CCaWidgetMmcWatcher::RunError
// ---------------------------------------------------------------------------
//
TInt CCaWidgetMmcWatcher::RunError( TInt /*aError*/ )
    {
    // No need to do anything  
    return KErrNone;
    }	

// ---------------------------------------------------------
// CCaWidgetMmcWatcher::RunL
// ---------------------------------------------------------
//	
void CCaWidgetMmcWatcher::RunL()
    {
	TInt status( iStatus.Int() );
	WaitForChangeL();
	if ( status >= KErrNone ) 
		{
		iObserver->MmcChangeL();
		}	
	}
	
