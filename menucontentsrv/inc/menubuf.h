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
* Description:  Definition of client side stream buffer
*
*/


#ifndef __MENUBUF_H__
#define __MENUBUF_H__

#include "menusrvdef.h"
#include <e32base.h>
#include <s32buf.h>

class RMenu;

/**
 *  Client side stream buffer. Not seekable.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( RMenuBuf ): public RSubSessionBase, public TStreamBuf
    {

public:

    /**
    * Close the object.
    */
    void Close();

    /**
    * Open the object (initially empty).
    * @param aMenu Session to be used.
    * @return Error code.
    */
    TInt Open( RMenu& aMenu );

    /**
    * Open this object using other object.
    * @param aHandle Handle used to open this object.
    * @param aFunction Function.
    * @param aArgs Arguments. Use arg 0 and 1 only;
    * arg 2 and 3 are used internally.
    * @return Error code.
    */
    TInt Open( RMenu& aMenu, TInt aFunction, TIpcArgs& aArgs );

private:

    /**
    * Read data from server.
    * @param aLength The number of bytes read.
    */
    void IpcReadL( TInt& aLength );

    /**
    * Write data to server.
    */
    void IpcWriteL();

    /**
    * Get read lag (number of bytes transferred from server side but not
    * yet read).
    * @return Read lag.
    */
    inline TInt Lag( TRead aArea ) const;

    /**
    * Get write lag (number of bytes written but not yet transferred to
    * server side).
    * @return Write lag.
    */
    inline TInt Lag( TWrite aArea ) const;

// from base class TStreamBuf

    /**
    * Re-fill the intermediate buffer and reset the start and end points
    * of the read area.
    * @param aMaxLength The maximum amount of data required.
    * @return The amount of data available.
    */
    TInt UnderflowL( TInt aMaxLength );

    /**
    * Empty the intermediate buffer and resets the start and end points
    * of the write area.
    */
    void OverflowL();

    /**
    * Synchronise the stream buffer with the stream. In effect, this
    * ensures that buffered data is delivered to the stream.
    * Write lag flushed.
    * Read lag leaves with KErrNotSupported.
    */
    void DoSynchL();

private:    // Data

    TMenuBuf iBuf; ///< Data buffer.

    };

#include "menubuf.inl"

#endif // __MENUBUF_H__

