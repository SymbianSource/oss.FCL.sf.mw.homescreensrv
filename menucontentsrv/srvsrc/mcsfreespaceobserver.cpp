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

#include <sysutil.h>

#include "mcsfreespaceobserver.h"

// ---------------------------------------------------------------------------
// CMcsFreeSpaceObserver::CMcsFreeSpaceObserver
// ---------------------------------------------------------------------------
//
CMcsFreeSpaceObserver::CMcsFreeSpaceObserver(MMcsFreeSpaceObserver& aCallback,
    TInt64 aFreeDiskSpace, TInt aDrive) :
    CActive(EPriorityStandard), // Standard priority
    iDrive(aDrive),
    iFreeDiskSpace(aFreeDiskSpace),
	iCallback(aCallback)
	{
	CActiveScheduler::Add(this); // Add to scheduler
	}

// ---------------------------------------------------------------------------
// CMcsFreeSpaceObserver::NewLC
// ---------------------------------------------------------------------------
//
CMcsFreeSpaceObserver* CMcsFreeSpaceObserver::NewLC(MMcsFreeSpaceObserver& aCallback,
    TInt64 aFreeDiskSpace,
    TInt aDrive)
	{
	CMcsFreeSpaceObserver* self = new (ELeave) CMcsFreeSpaceObserver(aCallback,
	    aFreeDiskSpace, aDrive);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

// ---------------------------------------------------------------------------
// CMcsFreeSpaceObserver::NewL
// ---------------------------------------------------------------------------
//
CMcsFreeSpaceObserver* CMcsFreeSpaceObserver::NewL(MMcsFreeSpaceObserver& aCallback,
    TInt64 aFreeDiskSpace,
    TInt aDrive)
	{
	CMcsFreeSpaceObserver* self = CMcsFreeSpaceObserver::NewLC(aCallback,
	    aFreeDiskSpace, aDrive);
	CleanupStack::Pop(); // self;
	return self;
	}

// ---------------------------------------------------------------------------
// CMcsFreeSpaceObserver::ConstructL
// ---------------------------------------------------------------------------
//
void CMcsFreeSpaceObserver::ConstructL()
	{
	User::LeaveIfError( iFs.Connect( ) );
    iFs.NotifyDiskSpace( iFreeDiskSpace, iDrive, iStatus );
	SetActive();
	}

// ---------------------------------------------------------------------------
// CMcsFreeSpaceObserver::~CMcsFreeSpaceObserver
// ---------------------------------------------------------------------------
//
CMcsFreeSpaceObserver::~CMcsFreeSpaceObserver()
	{
	Cancel(); // Cancel any request, if outstanding
	iFs.Close();
	}

// ---------------------------------------------------------------------------
// CMcsFreeSpaceObserver::DoCancel
// ---------------------------------------------------------------------------
//
void CMcsFreeSpaceObserver::DoCancel()
	{
	iFs.NotifyDiskSpaceCancel();
	}

// ---------------------------------------------------------------------------
// CMcsFreeSpaceObserver::RunL
// ---------------------------------------------------------------------------
//
void CMcsFreeSpaceObserver::RunL()
	{
	TVolumeInfo vol;
	if ( ( iStatus == KErrNone ) &&
	    ( iFs.Volume(vol, iDrive )==KErrNone ) &&
		( vol.iFree>iFreeDiskSpace ) )
        {
        if( SysUtil::DiskSpaceBelowCriticalLevelL( &iFs,
            MMcsFreeSpaceObserver::EFreeDiskSpace,
            EDriveC ) )
            {
            iFreeDiskSpace = vol.iFree + MMcsFreeSpaceObserver::EFreeDiskSpace;
            }
        else
            {
            iCallback.HandleFreeSpaceEventL();
            }
        }
    iFs.NotifyDiskSpace( iFreeDiskSpace, iDrive, iStatus );
    SetActive();
	}

// ---------------------------------------------------------------------------
// CMcsFreeSpaceObserver::RunError
// ---------------------------------------------------------------------------
//
TInt CMcsFreeSpaceObserver::RunError(TInt /*aError*/)
	{
    iFs.NotifyDiskSpace( iFreeDiskSpace, iDrive, iStatus );
	SetActive();
	return KErrNone;
	}
