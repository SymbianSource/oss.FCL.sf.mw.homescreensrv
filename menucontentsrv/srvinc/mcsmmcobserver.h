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
* Description:  This class implements MMcsGetListCreatorInterface. It builds tree 
*   output list for getlist operation 
*
*
*/

#ifndef CMCSMMCOBSERVER_H
#define CMCSMMCOBSERVER_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <f32file.h>

/**
 * Interface for handling Mmc inserrtion / removal.
 *
 * @since S60 v5.0
 */
class MMcsMmcObserver 
	{
public:
	virtual void HandleMmcEventL(TInt aEvent) = 0;
    enum TInstOp
    	{
    	EMmcInsert = 0x00000001,
    	EMmcRemove = 0x00000002,
		};
	};


/**
 *  Mmc Observer Interface.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CMcsMmcObserver) : public CActive
	{
public:
	// Cancel and destroy
	~CMcsMmcObserver();

	// Two-phased constructor.
	static CMcsMmcObserver* NewL(MMcsMmcObserver& aCallback);

	// Two-phased constructor.
	static CMcsMmcObserver* NewLC(MMcsMmcObserver& aCallback);

private:
	// C++ constructor
	CMcsMmcObserver(MMcsMmcObserver& aCallback);

	// Second-phase constructor
	void ConstructL();

private:
	// From CActive
	// Handle completion
	void RunL();

	// How to cancel me
	void DoCancel();


	TInt RunError(TInt aError);
	
	// Count acvtive drives
	TInt DriveCount(TDriveList& aDriveList);

private:


	TDriveList iDriveList;
	
	TInt iDriveCount;
	RFs iFs;
	MMcsMmcObserver& iCallback;
	
	};

#endif // CMCSMMCOBSERVER_H
