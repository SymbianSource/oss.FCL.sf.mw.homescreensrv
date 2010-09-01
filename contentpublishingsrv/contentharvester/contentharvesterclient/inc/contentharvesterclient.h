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


#ifndef C_CONTENTHARVESTERCLIENT_H
#define C_CONTENTHARVESTERCLIENT_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS


// CLASS DECLARATION

/**
 * @brief Client-side interface to CP Active Data Server
 *
 * This class provides the client-side interface to the active data server session,
 * it just passes requests to the server.
 *
 * @lib contentharvesterclient.lib
 * @since S60 v3.1
 */
class RContentHarvesterClient : public RSessionBase
    {
public:

    /**
     * Construct the object.
     */
    IMPORT_C RContentHarvesterClient();

    /**
     * Connects to the server and create a session.
     * When the server is not running, the function starts the server.
     * @return error code
     */
    IMPORT_C TInt Connect();

    /**
     * Triggers updating process of Active Data Server, 
     * the function returns right after the server receives the request 
     * (it isn't information about the whole update process.) 
     * @return error code
     */
    IMPORT_C TInt Update();

    /**
     * Triggers stop process on server	
     * @return error code
     */
    IMPORT_C TInt Stop();

    /**
     * Returns the version number
     * @return the version
     */
    TVersion Version() const;
    };

#endif // C_CONTENTHARVESTERCLIENT_H
// End of File
