/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CHSWIMASSMODEOBSERVER_H_
#define CHSWIMASSMODEOBSERVER_H_


/**
 * Mass storage mode observer.
 */
class MCHSwiMassModeObserver
	{
public:
	/**
	 * Sets Mass Storage Mode
	 * @param aMode Mass Storage mode
	 */
	virtual void SetMassStorageMode( TBool aMode ) = 0;
	
	/**
	 * Sets Mass Storage Mode
	 * @returns Mass Storage mode
	 */
	virtual TBool IsMassStorageMode() = 0;
	
	/**
	 * Handles ending of mass storage mode (e.g. unplugging usb cable when in mass storage mode)
	 */
	virtual void HandleMassStorageModeEndEvent() = 0;
	
	/*
	 * Handles successful drive scan, which has been executed asynchronously after either:
	 * - Explicitly postponed drive scan, or
	 * - Error recovery from synchronous drive scan. 
	 */
	virtual void HandleSuccessfulAsynchDriveScan() = 0;
	};

#endif /* CHSWIMASSMODEOBSERVER_H_ */
