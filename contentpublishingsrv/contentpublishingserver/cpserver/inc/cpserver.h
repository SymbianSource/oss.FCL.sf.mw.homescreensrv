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
* Description:  Content Publisher Server - Main class
*
*/


#ifndef C_CCPSERVER_H
#define C_CCPSERVER_H

// INCLUDES
#include <e32base.h>
#include "cpserverdef.h"
#include "cpserverburlistener.h"

#ifdef CONTENT_PUBLISHER_DEBUG
class CCPDebug;
#endif

class CCPDataManager;
class CCPActionHandlerThread;
class CCPServer;

struct TPointersForSession
    {
    CCPServer* iServer;
    CCPDataManager* iDataManager;
    CCPActionHandlerThread* iActionHandlerThread;
    };

// CLASS DECLARATION
/**
 *  Content Publisher Server implementation
 *
 *  @since Series 60 5.0
 */
class CCPServer : public CPolicyServer, public MBURListenerCallback
    {
public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CCPServer* NewLC();

    /**
     * Destructor.
     */
    virtual ~CCPServer();

public:
    // New functions        

    /**
     * Stops server.
     */
    void Stop();

    /**
     * Panic client. 
     * @param aMessage RMessage2
     * @param aPanic panic code
     */
    static void PanicClient( const RMessage2& aMessage, 
        TCPServerPanic aPanic );

    /**
     * start server
     * @return aErr Error code.
     */
    static TInt ThreadStart();

    /**
     * Decrements session counter
     */
    void RemoveSession();

    /**
     * Increments session counter
     */
    void AddSession();

    /**
     * HandleBUREventL is called when Backup and Restore state has been changed.
     *
     * @since S60 v3.1
     * @param aStatus Current Backup and Restore status.
     */
    void HandleBUREventL( TBURStatus aStatus );

    /**
     * Get Lock
     *
     * @return ETrue if Backup or Restore are running 
     */
    TBool GetLock();

    /**
     * Returns notifications array
     */
    RPointerArray<CLiwDefaultList>& CCPServer::GetNotifications( ); 

private:
    // From CActive

    /**
     * Process any errors.
     * @param aError the leave code reported
     * @return KErrNone if leave is handled
     */
    TInt RunError( TInt aError );

private:
    // New methods

    /**
     * Constructs the server. 
     * @param aPriority CServer2 input parameter
     */
    CCPServer( TInt aPriority );

    /**
     * Perform the second phase construction of a CCPServer object.
     */
    void ConstructL();

    /**
     * Panic the server. 
     * @param param aPanic the panic code
     * @return a updateId of container
     */
    static void PanicServer( TCPServerPanic aPanic );
    
    /**
     * Send notification
     * @param Map containing ids of item
     */
    void AppendNotificationL( CCPLiwMap* aMap ); 

    
private:
    // From CServer2

    /**
     * Create a time server session, and return a pointer to the created object.
     * @param aVersion the client version 
     * @param aMessage RMessage2 
     * @return pointer to new session
     */
    CSession2* NewSessionL( const TVersion& /*aVersion*/,
        const RMessage2& /*aMessage*/) const;

    /**
     * create and run server
     */
    static void CreateAndRunServerL();


    /**
     * Publishers deactivation
     */
    void DeactivatePublishersL();


    /**
     * Resets Activate flag and update item in the DB
     */
    void ResetAndUpdateFlagL( CCPLiwMap* aMap );
        
private:
    //data

    /*
     * Manager to send and receive data beetween server and storage 
     * Own.
     */
    CCPDataManager* iDataManager;

    /*
     * Manager to prepare and execute action
     * Own. 
     */
    CCPActionHandlerThread* iActionHandlerThread;

    /*
     * Count of sessions
     * Own.
     */
    TInt iCountSession;

    /**
     * Notifier for Backup and Restore changes.
     * Own.
     */
    CCPServerBURListener* iBURListener;

#ifdef CONTENT_PUBLISHER_DEBUG
    CCPDebug* iDebug;
#endif

    /*
     * Own.
     * True if database is lock
     */
    TBool iBURLock;
    
    /*
     * Own.
     * List of waiting notifications about publisher 
     * activation during startup 
     */
    RPointerArray<CLiwDefaultList> iNotifications;
    };

#endif // C_CCPSERVER_H
