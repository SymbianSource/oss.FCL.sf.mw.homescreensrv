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

#ifndef C_CCHSWIUSBHANDLER_H
#define C_CCHSWIUSBHANDLER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>




// FORWARD DECLARATION
class MCHSwiMassModeObserver;


/*
 * Inherited CActive, performs a asynchronous checks for mass storage mode
 *
 * @since 3.1
 */
class CCHSwiUsbHandler : public CActive
    {
public:
	/**
	 * Two-phased constructor.
     * @param aPlugin Owning plugin.
     * @param aFs File session. 
	 */
    static CCHSwiUsbHandler* NewL( MCHSwiMassModeObserver* aMassModeObserver,
    		RFs& aFs );

	/**
	 * Destructor.
	 */
    virtual ~CCHSwiUsbHandler();

    /**
     * Performs synchronous drive scan. Checks whether Mass Drive is available.
     * If it available: observer's Mass Mode flag is set to EFalse.
     * If it is not available: Mass Mode flag is set to ETrue.
     * In case of error, the check is postponed to be done again in RunL and Mass 
     * Mode flag is set to ETrue.
     */
    void SynchronousDriveScan();

    /**
     * Performs postponed drive scan. Checks whether Mass Drive is available.
     * If it available: observer's Mass Mode flag is set to EFalse.
     * If it is not available: Mass Mode flag is set to ETrue.
     * In case of error, the check is postponed to be done again in RunL and Mass 
     * Mode flag is set to ETrue.
     */
    void PostponedDriveScan();
    
protected:

	/**
	 * Handles timer events to start drive scan.
	 */
    void RunL();

    /**
     * RunL error handling.
     * @param aError Error.
     */
    TInt RunError( TInt aError );

    /**
     * Cancels notifications.
     */
    void DoCancel();

private:

    /**
     * no copy constructor
     */
    CCHSwiUsbHandler( const CCHSwiUsbHandler& );
    
    /**
     * no assignment
     */    
    CCHSwiUsbHandler& operator=( const CCHSwiUsbHandler& );

    /**
     * C++ default constructor
     */
    CCHSwiUsbHandler( MCHSwiMassModeObserver* aMassModeObserver, RFs& aFs );

    /**
     * Symbian default constructor.
     */
    void ConstructL();

    /**
     * Scans drives to check mass storage mode.
     */
    TInt ScanDrives( TInt& aDriveFlags );

    /**
     * Handles drive access errors by launching new postponed scan.
     */
    void HandleDriveError();
    
    /**
     * Scans drives and updates Mass Mode flag in the observer. 
     * @param aFlagsValid indicates whether current iDriveFlags are valid 
     * (they become invalid in case of an error).
     */
    TInt ScanDrivesAndUpdateFlag( TBool aFlagsValid );
    
    /**
     * Updates mass mode flag in observer.
     */
    TInt UpdateMassModeFlag();

private:
	/**
	 * Error from last drive scan.
	 */
	TInt iLastDriveScanError;
	
	/**
	 * Observer.
	 */
	MCHSwiMassModeObserver* iMassModeObserver;
	
	/**
	 * File session. Not owned.
	 */
    RFs iFs;
    
    /**
     * Drive flags obtained in last drive scan. Every bit indicates presence 
     * of given drive. This member is used for drive access optimalization -
     * if flags are valid (i.e. no error occured)
     */
    TInt iDriveFlags;
    
    /**
     * Timer for postponed drive scan.
     */
    RTimer iTimer;
    };

#endif // #ifndef C_CCHSWIUSBHANDLER_H
