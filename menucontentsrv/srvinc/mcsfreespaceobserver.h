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

#ifndef CMCSFREESPACEOBSERVER_H
#define CMCSFREESPACEOBSERVER_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <f32file.h>

/**
 * Interface for monitoring free space.
 *
 * @since S60 v5.0
 */
class MMcsFreeSpaceObserver
	{
public:
    /**
     * Handles free space threshold events.
     * It must be implemented in derived clesses.
     */
	virtual void HandleFreeSpaceEventL() = 0;
    static const TInt64 EFreeDiskSpace = 40*1024;
	};


/**
 *  Free space Observer Interface.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CMcsFreeSpaceObserver) : public CActive
	{
public:
	// Cancel and destroy
	~CMcsFreeSpaceObserver();

	// Two-phased constructor.
	static CMcsFreeSpaceObserver* NewL(MMcsFreeSpaceObserver& aCallback,
	    TInt64 aFreeDiskSpace = MMcsFreeSpaceObserver::EFreeDiskSpace,
	    TInt aDrive=EDriveC);

	// Two-phased constructor.
	static CMcsFreeSpaceObserver* NewLC(MMcsFreeSpaceObserver& aCallback,
	    TInt64 aFreeDiskSpace = MMcsFreeSpaceObserver::EFreeDiskSpace,
	    TInt aDrive = EDriveC);

private:
	// C++ constructor
    CMcsFreeSpaceObserver(MMcsFreeSpaceObserver& aCallback,
        TInt64 aFreeDiskSpace,
        TInt aDrive);

	// Second-phase constructor
	void ConstructL();

private:
	// From CActive
	// Handle completion
	void RunL();

	void DoCancel();

	TInt RunError(TInt aError);

private:
    /*
     * Observed drive.
     */
    TInt iDrive;
    /*
     * Free space threshold (bytes).
     */
    TInt64 iFreeDiskSpace;
    /*
     * A handle to a file server session.
     */
	RFs iFs;
	/*
	 * An object of a class implementing callback
	 * function (HandleFreeSpaceEventL)
	 * of MMcsFreeSpaceObserver interface.
	 */
	MMcsFreeSpaceObserver& iCallback;
	
	};

#endif // CMCSFREESPACEOBSERVER_H
