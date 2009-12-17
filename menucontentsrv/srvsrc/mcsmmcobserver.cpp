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


#include "mcsmmcobserver.h"

_LIT(KMcsDir,"*:\\private\200113dd\\import");

// ---------------------------------------------------------------------------
// CCMCsMmcObserver::CCMCsMmcObserver
// ---------------------------------------------------------------------------
//
CMcsMmcObserver::CMcsMmcObserver(MMcsMmcObserver& aCallback) :
	CActive(EPriorityStandard), // Standard priority
	iCallback(aCallback)
	{
	CActiveScheduler::Add(this); // Add to scheduler
	}

// ---------------------------------------------------------------------------
// CCMCsMmcObserver::NewLC
// ---------------------------------------------------------------------------
//
CMcsMmcObserver* CMcsMmcObserver::NewLC(MMcsMmcObserver& aCallback)
	{
	CMcsMmcObserver* self = new (ELeave) CMcsMmcObserver(aCallback);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CCMCsMmcObserver::NewL
// ---------------------------------------------------------------------------
//
CMcsMmcObserver* CMcsMmcObserver::NewL(MMcsMmcObserver& aCallback)
	{
	CMcsMmcObserver* self = CMcsMmcObserver::NewLC(aCallback);
	CleanupStack::Pop(); // self;
	return self;
	}

// ---------------------------------------------------------------------------
// CCMCsMmcObserver::ConstructL
// ---------------------------------------------------------------------------
//
void CMcsMmcObserver::ConstructL()
	{
	User::LeaveIfError( iFs.Connect( ) );
	User::LeaveIfError(iFs.DriveList(iDriveList));
	iDriveCount = DriveCount(iDriveList);
	iFs.NotifyChange(ENotifyEntry, iStatus, KMcsDir);
	SetActive();
	}

// ---------------------------------------------------------------------------
// CCMCsMmcObserver::~CCMCsMmcObserver
// ---------------------------------------------------------------------------
//
CMcsMmcObserver::~CMcsMmcObserver()
	{
	Cancel(); // Cancel any request, if outstanding
	iFs.Close();
	}

// ---------------------------------------------------------------------------
// CCMCsMmcObserver::DoCancel
// ---------------------------------------------------------------------------
//
void CMcsMmcObserver::DoCancel()
	{
	iFs.NotifyChangeCancel();
	}

// ---------------------------------------------------------------------------
// CCMCsMmcObserver::RunL
// ---------------------------------------------------------------------------
//
void CMcsMmcObserver::RunL()
	{
	if (iStatus == KErrNone)
		{
		// Get driver number and compare to last state
		User::LeaveIfError(iFs.DriveList(iDriveList));
		TInt driveCountNow = DriveCount(iDriveList);
		if (iDriveCount < driveCountNow)
			{
			iCallback.HandleMmcEventL(MMcsMmcObserver::EMmcInsert);
			}
		else if (iDriveCount > driveCountNow)
			{
			iCallback.HandleMmcEventL(MMcsMmcObserver::EMmcRemove);
			}
		iDriveCount = driveCountNow;
		}
	iFs.NotifyChange(ENotifyEntry, iStatus, KMcsDir);
	SetActive(); // Tell scheduler a request is active
	}

// ---------------------------------------------------------------------------
// CCMCsMmcObserver::RunError
// ---------------------------------------------------------------------------
//
TInt CMcsMmcObserver::RunError(TInt /*aError*/)
	{
	iFs.NotifyChange(ENotifyEntry, iStatus, KMcsDir);
	SetActive(); // Tell scheduler a request is active
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// CCMCsMmcObserver::DriveCount
// ---------------------------------------------------------------------------
//
TInt CMcsMmcObserver::DriveCount(TDriveList& aDriveList)
	{
	TInt driveCount(0);
	TDriveInfo driveInfo;
	
	// Count drives
	for (TInt driveNumber = EDriveA; driveNumber <= EDriveZ; driveNumber++)
		{
		if (aDriveList[driveNumber])
			{
			// Be sure it's not unmounted
			iFs.Drive(driveInfo, driveNumber);
			if (driveInfo.iType != EMediaNotPresent)
				{
				++driveCount;
				}
			}
		}
	return driveCount;
	}
