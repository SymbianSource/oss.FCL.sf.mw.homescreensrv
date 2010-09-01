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
* Description:  Engine for content harvester server
 *
*/


#ifndef C_CONTENTHARVESTERSESSION_H
#define C_CONTENTHARVESTERSESSION_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CContentHarvesterServer;

// CLASS DECLARATION

/** 
 An instance of class CContentHarvesterSession is created for each client
 */
class CContentHarvesterSession : public CSession2
    {

public:
    // New methods

    /**
     * Create a CContentHarvesterSession object using two phase construction,
     * and return a pointer to the created object
     * @param aServer CContentHarvesterServer
     * @return pointer to new session
     */
    static CContentHarvesterSession* NewL( CContentHarvesterServer* aServer );

    /**
     * Create a CContentHarvesterSession object using two phase construction,
     * and return a pointer to the created object
     * @param aServer CContentHarvesterServer
     * @return pointer to new session
     */
    static CContentHarvesterSession* NewLC( CContentHarvesterServer* aServer );

    /**
     * Destroy the object and release all memory objects
     */
    virtual ~CContentHarvesterSession();

public:
    // From CSession
    /**
     * Called after a service request from client; from class CSession
     * @param aMessage message from client (containing requested operation and any data)
     */
    void ServiceL( const RMessage2& aMessage );

private:
    // New methods

    /**
     * Perform the first phase of two phase construction 
     * @param aServer CContentHarvesterServer
     */
    CContentHarvesterSession( CContentHarvesterServer* aServer );

    /**
     * Perform the second phase construction of a CContentHarvesterSession object
     */
    void ConstructL();

private:

    /** 
     * CP Active Data Server container
     * Own.
     */
    CContentHarvesterServer* iContentHarvesterServer;
    };

#endif // C_CONTENTHARVESTERSESSION_H
