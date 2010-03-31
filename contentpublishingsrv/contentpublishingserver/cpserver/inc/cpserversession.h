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
 * Description:  Server Session
 *
 */

#ifndef C_CCPSERVERSESSION_H
#define C_CCPSERVERSESSION_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
#ifdef CONTENT_PUBLISHER_DEBUG
class CCPDebug;
#endif
class CLiwGenericParamList;
class CLiwDefaultMap;
class CLiwDefaultList;
class CCPServer;
class CCPDataManager;
class CCPLiwMap;
class CCPActionHandlerThread;
class CCPNotificationHandler;
struct TPointersForSession;

// CLASS DECLARATION
/** 
 *  Server side session representation
 *
 *  @since Series 60 5.0
 */
class CCPServerSession : public CSession2
    {

public:
    // New methods

    /**
     * Two-phased constructor.
     */
    static CCPServerSession* NewL(TPointersForSession& aPasser);

    /**
     * Two-phased constructor.
     */
    static CCPServerSession* NewLC(TPointersForSession& aPasser);

    /**
     * Destroy the object and release all memory objects
     */
    virtual ~CCPServerSession();

public:
    // From CSession

    /**
     * Called after a service request from client
     * @param aMessage message from client (containing requested operation 
     * and any data)
     */
    void ServiceL(const RMessage2& aMessage);

    /**
     * Selects correct function from message
     * @param aMessage message from client (containing requested operation 
     * and any data)
     */
    void DispatchMessageL(const RMessage2& aMessage, TBool& aPanicedClient);

private:
    // New methods

    /**
     * Standard C++ constructor.
     */
    CCPServerSession();

    /**
     * Perform the second phase construction of a CCPServerSession object
     */
    void ConstructL(TPointersForSession& aPasser);

    /**
     * Add Data request
     * @param Message from client
     */
    void AddDataL(const RMessage2& aMessage);

    /**
     * Specific add data request - data is not actually added to database
     * but notification is send.
     * @param Message from client
     */
    void AddDataNonPersistentL(const RMessage2& aMessage);

    /**
     * Get data request - first phase
     * @param Message from client
     */
    void GetListSizeL(const RMessage2& aMessage);

    /**
     * Get data request - second phase
     * @param Message from client
     */
    void GetListDataL(const RMessage2& aMessage);

    /**
     * Remove data request
     * @param Message from client
     */
    void RemoveDataL(const RMessage2& aMessage);

    /**
     * Executes action request
     * @param Message from client
     */
    void ExecuteActionL(const RMessage2& aMessage);

    /**
     * Executes actions and sends notifications
     * @param aMap input map from client
     * @param aEnableCache indicates if action data should be cached
     * @param aOptions command options
     */
    void ExecuteActionL(const CCPLiwMap* aMap,
            TBool aEnableCache, TUint aOptions);
    
    /**
     * Executes multiple actions request
     * @param Message from client
     */
    void ExecuteMultipleActionsL(const RMessage2& aMessage);

    /**
     * Executes action request
     * @param aActionParams list with actions
     */
    void ExecuteL(const CLiwGenericParamList& aActionParams);

    /**
     * Register for notification request
     * @param Message from client
     */
    void RegisterObserverL(const RMessage2& aMessage);

    /**
     * Adds new observer
     * @param Message from client
     */
    void AddObserverL(const RMessage2& aMessage);

    /**
     * Removes observer
     * @param Message from client
     */
    void RemoveObserverL(const RMessage2& aMessage);

    /**
     * Unregister from notification request
     * @param Message from client
     */
    void UnregisterObserverL();

    /**
     * Send information about change in database to client
     * @param Message from client
     */
    void GetChangeInfoDataL(const RMessage2& aMessage);

    /**
     * Converts CLiwGenericParamList to descriptor and
     * sends to client
     * @param Message to complete 
     * @param Parameters for message
     */
    void ExternalizeAndWriteToClientL(const RMessage2& aMessage,
            const CLiwGenericParamList* outParamList);

    /**
     * Unpacks message from client to map
     * @param Message to complete 
     * @return CCPLiwMap with data from client
     */
    CCPLiwMap* UnpackFromClientLC(const RMessage2& aMessage);

    /**
     * Send notification
     * @param aNotificationList
     */
    void SendNotificationL(CCPLiwMap* aMap,
            CLiwDefaultList* aNotificationList);

    /**
     * Get and Execute Activate or Deactivate action from the DB 
     * @param Map containing ids of item
     */
    void GetAndExecuteActionL( CCPLiwMap* aMap, CLiwDefaultList* aNotificationList,
    		TBool aInsertTrigger = EFalse );

    /**
     * Get Server Lock
     * @return ETrue if aMessege cannot be processed
     * because Backup or Restore is running 
     **/
    TBool GetServerLock(const RMessage2& aMessage);

    /**
     * Unpacks data for ExecuteMultipleActions from client
     * @param Message from client
     * @return list containing input data from the client
     **/
    CLiwGenericParamList* UnpackForMultiExecuteLC(const RMessage2& aMessage);

    /**
     * Checks map validity of input data and returns list 
     * that can be used to call execute
     * @param aMaps input data
     * @return list containing input maps for Execute
     **/
    CLiwDefaultList* CheckValidityLC(const CLiwList* aMaps);
    
private:

    /*
     * Own.
     * Handler for asynchronous request for notifications
     */
    CCPNotificationHandler* iNotificationHandler;

    /*
     * Not own.
     * True if message is register
     */
    TBool isRegister;

    /*
     * Not own.
     * Pointer to Content Publisher Server
     */
    CCPServer* iServer;

    /*
     * Not own.
     * Manager to send and receive data beetween server and storage 
     */
    CCPDataManager* iDataManager;

    /*
     * Own.
     * List write to message with getList function
     */
    CLiwGenericParamList* iParamList;

    /*
     * Not Own.
     * Manager to prepare and execute action
     */
    CCPActionHandlerThread* iActionHandlerThread;

#ifdef CONTENT_PUBLISHER_DEBUG
    CCPDebug* iDebug;
#endif
    };

#endif // C_CCPSERVERSESSION_H
