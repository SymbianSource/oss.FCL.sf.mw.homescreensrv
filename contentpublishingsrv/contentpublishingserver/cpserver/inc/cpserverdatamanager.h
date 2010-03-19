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
* Description:  Used by sessions for handling data related requests
 *
*/


#ifndef C_CPDATA_MANAGER_H
#define C_CPDATA_MANAGER_H

#include <e32base.h>
#include "cpstorage.h"

// FORWARD DECLARATIONS
#ifdef CONTENT_PUBLISHER_DEBUG
class CCPDebug;
#endif
class CLiwMap;
class CCPLiwMap;
class CCpStorage;
class CCPServerSession;
class CCPNotificationHandler;
class TLiwVariant;

/**
 *  Content publisher data manager.
 *
 *  Used to add, modify and remove data and actions.
 *  @since S60 v5.0
 */
class CCPDataManager : public CBase, MCPChangeNotifier
    {
public:
    // exported

    /**
     * C++ destructor
     */
    ~CCPDataManager();

    /**
     * Two-phased constructor.
     */
    static CCPDataManager* NewL( TBool aTBool = EFalse );

    /**
     * Two-phased constructor.
     */
    static CCPDataManager* NewLC( TBool aTBool = EFalse );

    /**
     * Add a new data to database
     *
     * @param aData Data to be added.
     * @return Id of a newly created data item
     */
    TUint AddDataL( CCPLiwMap& aMap );

    /**
     * Fetches data from database
     * @param aInParamList filter and sorting criteria
     * @param aOutParamList results
     */
    void GetListL( const CCPLiwMap& aMa, CLiwGenericParamList& aOutParamList );
    /**
     * Fetches action from database
     * @param aInParamList filter and sorting criteria
     * @param aOutParamList results
     * @param aNotificationList change info list (for notifications)
     * @return KErrNotFound if data was not found
     */
    TInt GetActionL( const CCPLiwMap& aMa,
        CLiwGenericParamList& aOutParamList, 
        CLiwDefaultList* aNotificationList = NULL );

    /**
     * Fetches all activated publishers 
     * @param aInParamList filter and sorting criteria
     * @param aOutParamList results
     */
    void GetActivePublishersL( CLiwGenericParamList* aResultList );

    /**
     * Removes data from database
     * @param aInParamList filter criteria
     */
    void RemoveDataL( const CCPLiwMap& aMa );

    /**
     * Adds observer to database
     * @param new observer
     */
    void AddObserverL( CCPNotificationHandler* aNotificationHandler );

    /**
     * Removes observer from database
     * @param aNotificationHandler observer to remove
     */
    void RemoveObserver( CCPNotificationHandler* aNotificationHandler );

    // from MCPChangeNotifier
    /**
     * @param Map containing change information
     * @return 
     */
    void HandleChangeL( CLiwDefaultList* aListOfMaps );

    /**
     * Close database 
     * @return 
     */
    void CloseDatabase();
    /**
     * Open database 
     * @return 
     */
    void OpenDatabaseL();

private:
    // methods

    /**
     * Standard C++ constructor.
     */
    CCPDataManager();

    /**
     * Standard 2nd phase constructor.
     */
    void ConstructL( TBool aTBool );

    /**
     * Fetches action bound to provided trigger.
     *
     * @param aOutParamList of all actions
     * @param aMap for found action
     * @param aTrigger
     */
    void ExtractTriggerL( CLiwGenericParamList& aOutParamList,
        const CLiwDefaultMap* aMap, const TDesC8& aTrigger );
    /**
     * Sends notifications to observers stored in iNotificationsArray
     *
     * @param List of change information
     * @param index od data item
     */
    void SendNotificationL( CLiwDefaultList* aListOfMaps, TInt aIndex );

    /**
     * Exctracts action from input parameters
     *
     * @param aParam for data
     * @param aAction for action
     */
    void ExtractActionL( const TLiwGenericParam* aParam, RBuf8& aAction );
    
    /**
     * Fills aOutParamList with parameters for action.
     *
     * @param aOutParamList parsed list of items
     * @param aParamList list of items from data base
     * @param aActionTrigger action trigger
     */
    void FillActionParamListL(
    		CLiwGenericParamList & aOutParamList, 
    		const TLiwGenericParam* aParam,
			RBuf8 & aActionTrigger);

     /**
     * Creates map for GetList request - publisher, content_type
     * and content_id are copied from aMap to returned map
     * @param aMap - input map
     * @return input map for GetList request
     */
    CCPLiwMap* CreateMapForGetlistLC( const CCPLiwMap& aMap );


     /**
     * Gets Flag for specified item from DB and returns activate
     * info
     * @param aMap - input map
     * @return activate flag
     */
    TBool GetActivateInfoL( const CCPLiwMap* aMap );
    
    /**
    * Builds change info list
    * @param aMap - map containing parameters needed to build change info list
    * @param aParam - param from getlist result 
    * @param aChangeInfoList - output list containing change info list sent
    * as notification
    */
    void BuildChangeInfoL( 
    		const CCPLiwMap* aMap, 
    		const TLiwGenericParam* aParam,	
    		CLiwDefaultList* aChangeInfoList );
    /**
    * Builds change info list when query to database returned nothing
    * @param aMap - map containing parameters needed to build change info list
    * @param aChangeInfoList - output list containing change info list sent
    * as notification
    */
    void BuildDefaultChangeInfoL( 
    		const CCPLiwMap* aMap, 
    		CLiwDefaultList* aChangeInfoList );
    

    /**
    * Copies variant from in to out map if entry with provided key exists
    * @param aKey a key
    * @param aInMap input map
    * @param aOutMap output map
    */    
    void CopyVariantL(const TDesC8& aKey, const CLiwMap* aInMap, 
    		CLiwDefaultMap* aOutMap );

    /**
    * Copies variant from in map for key KActionTrigger to out map with key KActionTrigger16 
    * and formated type(from TDesC8 to TDesC) 
    * @param aInMap input map
    * @param aOutMap output map
    */    
    void CopyActionTrigger16L( const CLiwMap* aInMap, 
    		CLiwDefaultMap* aOutMap );

private:
    // data

    /*
     * Storage database
     * Own.
     */
    CCpStorage* iStorage;

    /*
     * Array of registered notification handlers.
     * Own.
     */
    RPointerArray<CCPNotificationHandler> iNotificationsArray;

#ifdef CONTENT_PUBLISHER_DEBUG
    CCPDebug* iDebug;
#endif
    };

#endif // C_CPDATA_MANAGER_H
// end of file
