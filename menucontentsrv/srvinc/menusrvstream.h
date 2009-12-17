/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MENUSRVSTREAM_H__
#define __MENUSRVSTREAM_H__

//  INCLUDES

#include <e32base.h>
#include <s32buf.h>

// FORWARD DECLARATION

class RMessage2;

// CLASS DECLARATION

/**
* Server side stream object.
*/
NONSHARABLE_CLASS( CMenuSrvStream ): public CObject
    {

public:     // Constructors and destructor

    /**
    * Constructor.
    * @param aHost Stream buffer hosting the stream. Ownership taken.
    */
    inline CMenuSrvStream( MStreamBuf& aHost );

    /**
    * Destructor. Releases the stream buffer.
    */
    inline virtual ~CMenuSrvStream();

    /**
    * Read data from host to client.
    * @param aMessage Message.
    * @param aLength Length.
    */
    void ReadL( const RMessage2& aMessage, TInt& aLength );

    /**
    * Write data from client to host.
    * @param aMessage Message.
    */
    void WriteL( const RMessage2& aMessage );

    /**
    * Get the stream buffer.
    */
    inline MStreamBuf& Host();

private:    // Data

    MStreamBuf& iHost;  ///< Stream buffer hosting the stream.

    };

#include "menusrvstream.inl"

#endif // __MENUSRVSTREAM_H__
            
// End of File
