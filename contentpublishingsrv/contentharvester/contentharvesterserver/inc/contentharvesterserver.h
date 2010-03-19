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
* Description:  Content Harvester Server - implemenration of CServer2 
 *
*/


#ifndef C_CONTENTHARVESTERSERVER_H
#define C_CONTENTHARVESTERSERVER_H

#include <eikenv.h>
#include <eikappui.h>
#include "contentharvesterpanic.h"

class CContentHarvesterEngine;

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 *  CP Active Data Server container.
 *  Provides interface for CP Active Data Server maintanace purposes.
 *
 *  @lib ???.lib
 *  @since Series 60 3.1
 */
class CContentHarvesterServer : public CServer2
    {
    /**
     *  EIkonEnv for the server process.
     *
     *  @lib ???.lib
     *  @since Series 60 3.1
     */
    class CContentHarvesterEikonEnv : public CEikonEnv
        {
public:
        void DestroyEnvironment();
        void ConstructL();
        };

    /**
     *  EikAppUi for the server process.
     *
     *  @lib ???.lib
     *  @since Series 60 3.1
     */
    class CContentHarvesterAppUi : public CEikAppUi
        {
public:
        ~CContentHarvesterAppUi();
        void ConstructL();
        };

public:
    // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CContentHarvesterServer* NewLC();

    /**
     * Destructor.
     */
    virtual ~CContentHarvesterServer();

public:
    // New functions        

    /**
     * Return the reference to the engine.
     * @return engine reference
     */
    CContentHarvesterEngine& Engine();

    /**
     * Stops server.
     */
    void Stop();

    /**
     * First stage startup for the server thread. 
     * @return return KErrNone or panics thread
     */
    static TInt ThreadFunction();

    /**
     * Panic client. 
     * @param aMessage RMessage2
     * @param aPanic panic code
     */
    static void PanicClient( const RMessage2& aMessage,
        TContentHarvesterPanic aPanic );

protected:
    // From CActive
    /**
     * Process any errors.
     * @param aError the leave code reported
     * @return return KErrNone if leave is handled
     */
    TInt RunError( TInt aError );

private:
    // New methods

    /**
     * Constructs the server. 
     * @param aPriority CServer2 input parameter
     */
    CContentHarvesterServer( TInt aPriority );

    /**
     * Perform the second phase construction of a CContentHarvesterServer object.
     */
    void ConstructL();

    /**
     * Panic the server. 
     * @param param aPanic the panic code
     * @return a updateId of container
     */
    static void PanicServer( TContentHarvesterPanic aPanic );

    /**
     * Second stage startup for the server thread.
     */
    static void ThreadFunctionL();

private:
    // From CServer2

    /**
     * Create a time server session, and return a pointer to the created object.
     * @param aVersion the client version 
     * @param aMessage RMessage2 
     * @return pointer to new session
     */
    CSession2* NewSessionL( const TVersion& aVersion,
        const RMessage2& aMessage ) const;

private:

    /**
     * instance of engine, where the whole logic is implemented	
     * Own.
     */
    CContentHarvesterEngine* iEngine;

    };

#endif // C_CONTENTHARVESTERSERVER_H
