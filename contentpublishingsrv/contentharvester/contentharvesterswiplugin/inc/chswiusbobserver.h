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

#ifndef C_CCHSWIUSBOBSERVER_H
#define C_CCHSWIUSBOBSERVER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>

// FORWARD DECLARATION
class CCHSwiUsbHandler;

/*
 * Observes RFs for RawDiskWrite events (these are also triggered when 
 * (un)mounting drive; RFs does not provide better, more specific 
 * notifications for mount and dismount).
 * 
 * After receiving notification, calls 
 * CCHSwiUsbHandler::PostponedDriveScan() and does not subscribe to
 * further notifcations. It is up to the plugin to call
 * CCHSwiUsbObserver::Start() after a successful drive scan.
 *
 * @since 3.1
 */
class CCHSwiUsbObserver : public CActive
    {
public:
	/**
	 * Two-phased constructor.
     * @param aFs File session. 
	 */
    static CCHSwiUsbObserver* NewL( CCHSwiUsbHandler* aHandler, RFs& aFs );

	/**
	 * Destructor.
	 */
    virtual ~CCHSwiUsbObserver();

    /**
     * Active object start.
     */
    void Start();

protected:

	/**
	 * Handle RFs notifications.
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
    CCHSwiUsbObserver( const CCHSwiUsbObserver& );
    
    /**
     * no assignment
     */    
    CCHSwiUsbObserver& operator=( const CCHSwiUsbObserver& );

    /**
     * C++ default constructor
     */
    CCHSwiUsbObserver( CCHSwiUsbHandler* aHandler, RFs& aFs );

    /**
     * Symbian default constructor.
     */
    void ConstructL();
    
private:
	
	/**
	 * File Session.
	 */
    RFs iFs;
    
    /**
     * Usb handler to perform postponed checks for mass storage mode.
     */
    CCHSwiUsbHandler* iUsbHandler;
    };

#endif // #ifndef C_CCHSWIUSBOBSERVER_H
