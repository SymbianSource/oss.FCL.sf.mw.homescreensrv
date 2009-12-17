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
* Description:  The API supports attributes not present in MCS from SAT Api
*
*/


#ifndef __MCSDRMOBSERVER_H__
#define __MCSDRMOBSERVER_H__

#include <e32base.h>
#include <DRMNotifier.h>

/**
 * Interface for updating DRM rights after change.
 *
 * @since S60 v5.0
 */
class MMcsDrmObserver
    {
    
public:
    
    /**
     */
    virtual void HandleDrmEvent( TUid aUid ) = 0;  
    };


/**
 *  DRM Observer Interface.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMcsDrmObserver ):
	public CBase,
	public MDRMEventObserver
    {
    
public:
	
	/**
	 * Creates an instance of CMcsDrmObserver implementation.
	 *
	 * @since S60 v5.0
	 * @param aCallBack Reference to notifier interface.
	 */
	static CMcsDrmObserver* NewL(
			MMcsDrmObserver& aObserver,
			const TDesC& aDRMContentId,
			TUid aUid,
			TDRMEventType aDRMEventType );
	

    /**
	 * Destructor.
	 */
	virtual ~CMcsDrmObserver();
    
    /**
     */
    TUid Uid();
	
	/**
	 * HandleNotificationL
	 *
	 * Callback function for the event notifications
	 * The MDRMEvent object ownership stays with the notifier,
	 * do not delete it
	 *
	 * @param aEvent : an object of the type MDRMEvent
	 * @since S60Rel2.6
	 */
	void HandleEventL( MDRMEvent* aEvent );
	
private:
    /**
     * Constructor.
     *
     * @since S60 v5.0
     * @param aObserver Reference to MMcsDrmObserver interface.
     * @param aUid UID of application
     * @param aDRMEventType type of an event to register
     */
	CMcsDrmObserver( 
    	MMcsDrmObserver& aObserver,
    	TUid aUid,
    	TDRMEventType aDRMEventType );
    
    /**
     * Symbian 2nd phase constructor.
     */
	void ConstructL( const TDesC& aDRMContentId );
    
	
	
	/**
	 */
	 void RegisterEventObserverL();

	/**
	 */
	 void UnRegisterEventObserver();
	
	
private:

	
    /**
     * Interface for notifying changes.
     * Not own.
     */
    MMcsDrmObserver& iObserver;     

    TUid iAppUid;
    
	//Own
	RBuf8 iContentId;
	//Own
	CDRMNotifier* iDRMNotifier;

	TDRMEventType iDRMEventType;
    };

#endif // __MCSDRMOBSERVER_H__

// End of File
