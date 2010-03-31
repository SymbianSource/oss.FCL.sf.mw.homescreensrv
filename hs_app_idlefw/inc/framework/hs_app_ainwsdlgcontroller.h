/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Network selection dialog controller
*
*/


#ifndef C_AINWSDLGCONTROLLER_H
#define C_AINWSDLGCONTROLLER_H

#include <e32base.h>
#include <AknSoftNotifier.h>
#include <NWHandlingEngine.h>

class CNWSession;

/**
 * Network Selection Dialog controller.
 */
NONSHARABLE_CLASS( CAiNwSDlgController ) : 
        public CBase,
        public MNWMessageObserver
	{
public:

// Constructors and destructors

    /**
     * Creates a new instance of the class.
     *
     * @return    A new instance of the class.
	 * @exception Any of the system-wide error codes if instance creation
	 *            fails due to an unrecoverable error.
     */
    static CAiNwSDlgController* NewL();

    ~CAiNwSDlgController();
	
// from MNWMessageObserver

	void HandleNetworkMessage( const TNWMessages aMessage );
    void HandleNetworkError( const TNWOperation aOperation, TInt aErrorCode );
		
private:

// Constructors
	
    CAiNwSDlgController();
	
	void ConstructL();
	
private:
    
// New functions
    
    /**
     * Take action according to current state change.
     */
    void HandleStateChange();

    /**
     * 
     */
    void HandleNetworkFound();

    /**
     * 
     */
    void HandleNetworkLost();

    /**
     * Opens the network selection dialog.
     */
    void LaunchDialog();

    /**
     * Cancels the network selection dialog.
     */
    void CancelDialog();

    /**
     * Tests if offline mode is engaged.
     *
     * @return ETrue if offline mode is currently active
     */
    TBool IsOffLineMode() const;

    /**
     * Tests if Bluetooth SAP is in connected mode.
     *
     * @return ETrue if Bluetooth SAP is active.
     */
    TBool IsBluetoothSAPConnected() const;

    static TInt DelayCallBack(TAny* aParam);

private:
    
// Data

    /**
     * Notifier service
     * Own
     */
    CAknSoftNotifier* iSoftNotifier;
    
    /**
     * Session to network handling engine.
     * Own.
     */
    CNWSession* iSession;

    /**
     * Cached network info structure.
     */
    TNWInfo iInfo;

    /**
     * A true value if registered to network.
     */
    TBool iRegistered;

    /**
     * Profile API.
     */
    CRepository* iProfileApi;

    /**
     * Timer object for 1-minute delay
     */
    CPeriodic* iPeriodic;
    };

#endif // C_AINWSDLGCONTROLLER_H

// End of file.
