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
* Description:  Client session for BS engine
 *
*/


#ifndef C_CBSCLIENT_H
#define C_CBSCLIENT_H

#include <e32base.h>

/**
 * Client-side interface to BS Engine
 *
 * This class provides the client-side interface to the BS server session,
 * it just passes requests to the server.
 *
 * @since S60 v3.2
 */
class RBSClient : public RSessionBase
    {
public:

    /**
     * Constructor.
     * @since S60 v3.2
     */
    RBSClient();

    /**
     * Connects to the server and create a session.
     * When the server is not running, the function starts the server.
     *
     * @since S60 v3.2
     * @return error code
     */
    TInt Connect();

    /**
     * Initialize session with given application UID.
     *
     * @since S60 v3.2
     * @return error code
     */
    TInt Initialize( TUid aApp );

    /**
     * Forwards activation event.
     *     
     * @since S60 v3.2
     * @param aState state of application like view ids
     * @param aIsItem indicate if it is item/exit activation
     * @return error code
     */
    TInt ForwardActivationEvent( const TDesC8& aState, TBool aIsItem );

    /**
     * Handle back command.
     *
     * @since S60 v3.2
     * @param aState state of application
     * @param aCheckOnly indicates if only checking should be done (no actual action)
     * @return error code
     */
    TInt HandleBackCommand( const TDesC8& aState, TBool aCheckOnly );

    /**
     * Returns the version number.
     *
     * @return the version
     */
    TVersion Version() const;
    };

#endif // C_CBSCLIENT_H
// End of File
