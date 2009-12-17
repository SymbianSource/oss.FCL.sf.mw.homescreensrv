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
* Description:  Server session for BS engine
 *
*/


#ifndef C_CBSSERVERSESSION_H
#define C_CBSSERVERSESSION_H

class CBSServer;

/** 
 * Server session.
 * An instance of class CBSServerSession is created for each client
 */
class CBSServerSession : public CSession2
    {
public:
    /**
     * Create a CBSServerSession object using two phase construction,
     * and return a pointer to the created object
     *
     * @since S60 3.2
     * @param aServer CBSServer
     * @return pointer to new session
     */
    static CBSServerSession* NewL( CBSServer* aServer );

    /**
     * Create a CBSServerSession object using two phase construction,
     * and return a pointer to the created object
     *
     * @since S60 3.2
     * @param aServer CBSServer
     * @return pointer to new session
     */
    static CBSServerSession* NewLC( CBSServer* aServer );

    /**
     * Destructor.
     */
    virtual ~CBSServerSession();

    // from base class CSession

    /**
     * From CSession.
     * Called after a service request from client; from class CSession
     *
     * @since S60 3.2
     * @param aMessage message from client (containing requested operation and any data)
     */
    void ServiceL( const RMessage2& aMessage );

private:

    CBSServerSession( CBSServer* aServer );

    void ConstructL();

    /**
     * Read text from message from given slot
     * 
     * @since S60 3.2
     * @param aMessage message 
     * @param aSlot slot number
     * @return descriptor
     */
    HBufC8* ReadTextLC( const RMessage2& aMessage, TInt aSlot );

private:
    // data

    /** 
     * Pointer to server
     * Not own. 
     */
    CBSServer* iBSServer;
    /**
     * UID of application assigned to the session
     */
    TUid iAppUid;
    };

#endif // C_CADATSERVERSESSION_H
