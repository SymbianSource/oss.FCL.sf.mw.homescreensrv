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

#ifndef MCSINSTALLNOTIFIER_H_
#define MCSINSTALLNOTIFIER_H_

#include <e32base.h>
#include <e32property.h>


/**
 * Interface for updating after installer events.
 *
 * @since S60 v5.0
 */
class MMcsInstallListener
    {
protected:
    /**
     * Enum defining the purpouse of the installation event.
     */
    enum TInstOp
    	{
    	EInstOpNone = 0x00000000,
    	EInstOpInstall = 0x00000001,
    	EInstOpUninstall = 0x00000002,
    	EInstOpRestore = 0x00000004
		};
public:
    virtual void HandleInstallNotifyL(TInt aEvent) = 0;
    };


/**
 *  MCS Install notifier.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMcsInstallNotifier ) :
	public CActive
	{
    /**
     * Enum defining the purpouse of the installation event.
     */
    enum TInstOp
    	{
    	EInstOpNone = 0x00000000,
    	EInstOpInstall = 0x00000001,
    	EInstOpUninstall = 0x00000002,
    	EInstOpRestore = 0x00000004
		};
    
    enum TInstOpStatus
        {
        EInstOpStatusNone = 0x00000000,
        EInstOpStatusSuccess = 0x00000100,
        EInstOpStatusAborted = 0x00000200
        };
public:	
		
    /**
     * Creates an instance of CMCSInstallNotifier implementation.
     * @param aNotifier Reference to notifier interface.
     * @param aCategory Package uid.
     * @param aKey Key for central repository.
     */
	static CMcsInstallNotifier* NewL(MMcsInstallListener& aListener, TInt aKey );
	
	/**
	 * Destructor.
	 */
	virtual ~CMcsInstallNotifier();
	
private:
	
    /**
     * Constructor.
     * @param aNotifier Reference to notifier interface.
     * @param aCategory Package uid.
     * @param aKey Key for central repository.
     */
	CMcsInstallNotifier( MMcsInstallListener& aListener, TInt aKey );
	
	/**
	     * Symbian 2nd phase constructor.
	     */
	    void ConstructL();

	    /**
	     * From CActive.
	     */
	    void DoCancel();

	    /**
	     * From CActive.
	     */
	    void RunL();

	    /**
	     * From CActive.
	     */
	    TInt RunError( TInt aError );
	    
private:
    /**.
     * Own.
     */
    RProperty iProperty;

    /**
     * Interface for notifying changes in folder.
     * Not Own.
     */
    MMcsInstallListener& iListener;

    /*
     * 
     */
    TInt iKey;
	};


#endif /* MCSINSTALLNOTIFIER_H_ */
