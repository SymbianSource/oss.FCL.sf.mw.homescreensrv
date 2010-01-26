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

#ifndef C_CCHSWIPLUGIN_H
#define C_CCHSWIPLUGIN_H

#include <contentharvesterplugin.h>
#include <apgnotif.h>

#include "chswimassmodeobserver.h"

class MLiwInterface;
class CLiwGenericParamList;

class CCHSwiUsbHandler;
class CCHSwiUsbObserver;

/**
 *  Active Data plugin for SIS installation events.
 *
 *  @since S60 S60 v3.1
 */
class CCHSwiPlugin: public CContentHarvesterPlugin,
        MApaAppListServObserver, MCHSwiMassModeObserver
	{
	
public:
	/**
	 * Two-phased constructor.
     * @param aInterface liw interface
	 */
	static CCHSwiPlugin* NewL( MLiwInterface* aInterface );
	
	/**
	 * Destructor.
	 */
	virtual ~CCHSwiPlugin();			 		

	/**
	 * From CContentHarvesterPlugin.
	 * This function updates all data entries which could change when an applicaion is installed
	 * or when the MMC is removed/inserted.
	 */
	void UpdateL();
	
private:
	
	// from MCHSwiMassModeObserver
	void SetMassStorageMode( TBool aMode );
	TBool IsMassStorageMode();
	void HandleMassStorageModeEndEvent();
	void HandleSuccessfulAsynchDriveScan();
	
    // from MApaAppListServObserver

    void HandleAppListEvent( TInt aEvent );    
    /**
	 * Performs the second phase construction of a CADatSwi object.
	 */		
	void ConstructL();
	
	/**
	 * Default Constructor.
	 * @param aActiveSpace An instance of the Active Space Client.
	 */
	CCHSwiPlugin( MLiwInterface* aInterface );
	
    /**
     * Removes publishers from database when an applicaion 
     * is uninstalled or when the MMC is removed.
     */
	void UpdateWidgetsL();
	
	/**
	 * Removes widgets.
	 * @param aWidgets Widgets list.
	 */
	void RemoveWidgetsL( CLiwGenericParamList* aWidgets );
	
	/** 
	 * Removes single widget.
	 * @param aType Widget type.
	 * @param aContentId Widget UID.
	 */
	void RemoveWidgetL( const TDesC& aType, 
        const TDesC& aContentId );
	

private:
    
    /**
     * An interface to Content Publisher Service
     */
    MLiwInterface* iCPSInterface;
   
    /**
     * AppArc session.
     * Own.
     */     
    RApaLsSession iApaLsSession;
    
    /**
     * File session.
     */
    RFs iFs;
    
    /**
     * Change notifier.
     * Own.
     */
    CApaAppListNotifier* iNotifier; 
    
    /**
     * Mass storage mode handler. 
     * Own.
     */
    CCHSwiUsbHandler* iUsbHandler;
    
    /**
     * Disk observer for monitoring status of mass drive. 
     * Own.
     */
    CCHSwiUsbObserver* iUsbObserver;
    
    /**
     * Mass storage mode flag.
     */
    TBool iMassStorageMode;
	};



#endif // C_CCHSWIPLUGIN_H
