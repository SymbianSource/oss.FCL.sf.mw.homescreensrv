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
* Description:  BS Server - implemenration of CServer2 
 *
*/


#ifndef C_CBSSERVER_H
#define C_CBSSERVER_H

// INCLUDES
#include <eikenv.h>
#include <eikappui.h>

#include "bspanic.h"

class CBSEngine;

/**
 * Back Stepping server implementation
 *
 * @since Series 60 3.2
 */

class CBSServer : public CServer2
    {
    /**
     * EIkonEnv for the server process.
     *
     * @since Series 60 3.2
     */
    class CBSEikonEnv : public CEikonEnv
        {
    public:
        void DestroyEnvironment();
        void ConstructL();
        };

    /**
     * EikAppUi for the server process.
     * Inform engine if focused application was changed
     *
     * @since Series 60 3.2
     */
    class CBSAppUi : public CEikAppUi
        {
    public:
        ~CBSAppUi();
        void ConstructL();
        void HandleWsEventL( const TWsEvent& aEvent,
            CCoeControl* aDestination );
        CBSEngine* iEngine;
        };

public:

    static CBSServer* NewLC();

    /**
     * Destructor.
     */
    virtual ~CBSServer();

    /**
     * Return the reference to the engine.
     *
     * @since Series 60 3.2
     * @return engine reference
     */
    CBSEngine& Engine();

    /**
     * Increment session count
     *
     * @since Series 60 3.2     
     */
    void IncrementSessions();

    /**
     * Decrement session count
     * Stops server if last session was closed
     * @since Series 60 3.2
     */
    void DecrementSessions();

    /**
     * First stage startup for the server thread. 
     *
     * @since Series 60 3.2
     * @return return KErrNone or panics thread
     */
    static TInt ThreadFunction();

    /**
     * Panic client. 
     *
     * @since Series 60 3.2
     * @param aMessage RMessage2
     * @param aPanic panic code
     */
    static void PanicClient( const RMessage2& aMessage, 
        TBSEnginePanic aPanic );

protected:

    // from base class CActive
    /**
     * From CActive.
     * Process any errors.
     *
     * @since Series 60 3.2
     * @param aError the leave code reported
     * @return return KErrNone if leave is handled
     */
    TInt RunError( TInt aError );

private:

    /**
     * Constructs the server. 
     *
     * @since Series 60 3.2
     * @param aPriority CServer2 input parameter
     */
    CBSServer( TInt aPriority );

    void ConstructL();

    /**
     * Panic the server. 
     *
     * @since Series 60 3.2
     * @param aPanic the panic code
     */
    static void PanicServer( TBSEnginePanic aPanic );

    /**
     * Second stage startup for the server thread.
     *
     * @since Series 60 3.2
     */
    static void ThreadFunctionL();

    /**
     * Create a  server session, and return a pointer to the created object.
     * @param aVersion the client version 
     * @param aMessage RMessage2 
     * @return pointer to new session
     */
    CSession2* NewSessionL( const TVersion& aVersion,
        const RMessage2& aMessage ) const;

private:
    // data
    /**
     * Instance of engine
     * Own.
     */
    CBSEngine* iEngine;

    /*
     * Session count
     */
    TInt iSessionCount;
    };

#endif // C_CBSSERVER_H
