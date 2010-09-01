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
* Description:  This class maintains a list of the running apps, and listen
*  for changes in the running apps. It calls the observer if there is such a
*  change.
*
*
*/


#ifndef __MCSRUNNINGAPPSHANDLER_H__
#define __MCSRUNNINGAPPSHANDLER_H__

#include <e32base.h>
#include <e32property.h>
#include <w32std.h>

#include "menuengoperation.h"
#include "menusrvengutils.h"

//Forward declatations
class CMenuEng;
class CMcsCacheHandler;
class CMcsRunningAppsWgMonitor;
class CMcsRunningAppsFswMonitor;
class CMcsRunningAppsHiddenAttrScanner;
class CMcsRunningAppsAddRemoveHandler;


/*
*
*/
class TRunningAppsAttr
    {
public:
    /*
    *
    */
    TRunningAppsAttr ( TUid aUid, TBool aFswHidden = EFalse,
        TBool aHidden = EFalse, TBool aIsRunning = EFalse )
        {
        iUid = aUid;
        iHidden = aHidden;
        iFswHidden = aFswHidden;
        iIsRunning = aIsRunning;
        }

    /*
    *
    */
    TUid GetUid() const
        {
        return iUid;
        }

    /*
    *
    */
    TBool IsHidden() const
        {
        return iHidden;
        }

    /*
    *
    */
    void SetHidden( TBool aHidden )
        {
        iHidden = aHidden;
        }

    /*
    *
    */
    TBool IsFswHidden() const
        {
        return iFswHidden;
        }

    /*
    *
    */
    void SetFswHidden( TBool aFswHidden )
        {
        iFswHidden = aFswHidden;
        }


    /*
    *
    */
    TBool IsRunning() const
        {
        return iIsRunning;
        }

    /*
    *
    */
    void SetRunning( TBool aIsRunning )
        {
        iIsRunning = aIsRunning;
        }

    /*
    *
    */
    static TBool MatchByUid( const TRunningAppsAttr& aArg1,
            const TRunningAppsAttr& aArg2)
        {
        return aArg1.GetUid() == aArg2.GetUid();
        }

private:    // data

    TUid iUid;
    TBool iHidden;
    TBool iFswHidden;
    TBool iIsRunning;

    };




/**
 * Interface for updating RunningApps list stored in CMcsRunningAppsHandler class
 *
 * @since S60 v3.2
 */
class MMcsRunningAppsHandler
    {
public:
	enum TArrayDataType
    	{
    	ERunningApps,
    	EFswHiddenApps,
    	EHiddenApps
    	};
    /*
	 * Called by HiddenAttr scanner, FSW monitor or WindowsGroup
	 * Monitor.
     * @param aArray an array containing UIDs of running, hidden or
     * 	fsw apps
     * @param aType type of data stored in aArray
	 */
    virtual void HandleListUpdateL(const RArray<TUid>& aArray,
    		TArrayDataType aType ) = 0;

	/*
	 * Called by AddRemoveHandler
	 * @param aFolder id of folder for which the method was invoked
	 */
    virtual void HandleAddRemoveRunningAppL( TInt aFolder ) = 0;

    };

/*
 *  RunningApps Handler.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMcsRunningAppsHandler ): public CBase, MMcsRunningAppsHandler
    {
    public:

	/**
	*   Factory method.
	*/
    static CMcsRunningAppsHandler* NewL(
		CMenuEng& aEng,
    	CMcsCacheHandler& aCacheHandler );

    /**
    *   Destructor.
    */
    ~CMcsRunningAppsHandler();

    /**
     * Called by EngineEvents method
     * @param aFolder folder id
     * @param aEvents events
     */
    void EngineEventsL( TInt aFolder, TInt aEvents );

    /**
     * Called by CMenuSrvEng class object
     * @param aFolder folder id
     * @param aEvents events
     */
    void EngineEvents( TInt aFolder, TInt aEvents );

    /**
     * Fills aArray with UIDs of running apps
     * @param aArray array to be filled with UIDs of running apps
     */
    void GetRunningAppsL( RArray<TUid>& aArray );

    /**
     * Returns running status of an item
     * @param aItemId id of the item
     * @return running status of an item
     */
    TBool GetRunningStatusL( TInt aItemId );


// from MMcsRunningAppsHandler

    /*
	 * Called by HiddenAttr scanner, FSW monitor or WindowsGroup
	 * Monitor.
     * @param aArray an array containing UIDs of running, hidden or
     * 	fsw apps
     * @param aType type of data stored in aArray
	 */
    void HandleListUpdateL(const RArray<TUid>& aArray, TArrayDataType aType );

	/*
	 * Called by AddRemoveHandler
	 * @param id of folder for which the method was invoked
	 */
	void HandleAddRemoveRunningAppL( TInt aFolder );

private:
	/**
	*   Constructor.
	*/
	CMcsRunningAppsHandler(
		CMenuEng& aEng,
	    CMcsCacheHandler& aCacheHandler );

	/**
	 * 2nd phase constructor
	 */
	void ConstructL();

    /**
    * Called by HandleListUpdateL
    * @param aArray array with UIDs of running apps
    */
	void HandleRunningAppsChangedL( const RArray<TUid>& aArray );

    /**
    * Called by HandleListUpdateL
    * @param aArray array with UIDs of FSW hidden apps
    */
	void HandleFswHiddenAppsChangedL( const RArray<TUid>& aArray );

    /**
    * Called by HandleListUpdateL
    * @param aArray array with UIDs of hidden apps
    */
	void HandleHiddenAppsChangedL(const RArray<TUid>& aArray );

	/**
	 * Sends AttributeChange notification for item and parents of the item
	 * it it is necessary, it is also responsible for removing data from cache
	 * if attribute value cahnged
	 * @param aId id of item with changed running status attribute
	 */
	void HandleNotificationL( TInt aId );

	/**
	 * Sends AttributeChange notification for items and parents of items with
	 * given uid, it is also responsible for removing data from cache
	 * if attribute value has changed
	 * @param aAppUid uid of items with changed running status attribute
	 */
	void HandleNotificationL( TUid aAppUid );

	/**
	 * Gets list of items with secified uid
	 * @param aAppUid items uid
	 * @return KErrNotFound if no item was found KErrNone if succesfull
	 */
	TInt GetItemsListL( TUid aAppUid, RArray<TMenuItem>& aItemArray );

	/*
	 * Updates array containing folders with running status
	 * @param aFolderId id of a folder
	 * @param aNewRunningStatus current running status
	 */
	void UpdateFolderRunningStatusL( TInt aFolderId, TBool aNewRunningStatus );


	/**
	 * Updates array containing folders with running status.
	 * It is called at startup and it scans all folders
	 */
	void UpdateFoldersRunningStatusL( );

	/**
	 * Gets running status for CMenuEngObject
	 * @param aEngObj object to be checked
	 * @return running status of an object
	 */
	TBool GetRunningStatusL( const CMenuEngObject& aEngObj );

	/**
	 * Gets previous Folder running status
	 * @param aId id of the folder
	 * @return previous running status of a folder (stored in
	 * 	iRunningFolderStatus array )
	 */
	TBool GetFolderPreviousRunningStatusL( TInt aId );


	/**
	 * Gets current folder running status
	 * @param aId id of the folder
	 * @return running status of a folder
	 */
	TBool GetFolderRunningStatusL( TInt aId );

	/**
	 * Gets current application running status
	 * @param aUid UID of an application
	 * @return running status of an application
	 */
	TBool GetAppRunningStatusL( TUid aUid );

private:

    CMenuEng& iEng ; ///< Engine. not own

    /** not own
    */
    CMcsCacheHandler& iCacheHandler;

    /*
    * own
    */
    CMcsRunningAppsWgMonitor* iWindowGroupMonitor;

    /*
    * own
    */
    CMcsRunningAppsFswMonitor* iFswMonitor;

    /*
     * own
     */
    CMcsRunningAppsHiddenAttrScanner* iHiddenAttrScanner;

    /*
     * own
     */
    CMcsRunningAppsAddRemoveHandler* iAddRemoveHandler;

    /*
    * own
    */
    RArray<TRunningAppsAttr> iRunningApps;

    /*
    * own
    */
    RArray<TInt> iRunningFolderStatus;
    };

#endif // __MCSRUNNINGAPPSHANDLER_H__
