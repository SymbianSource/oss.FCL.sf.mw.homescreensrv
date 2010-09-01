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

#ifndef __MENUSRVBUF_H__
#define __MENUSRVBUF_H__

//  INCLUDES

#include <e32base.h>
#include <s32mem.h>

// CLASS DECLARATION

/**
* Server side dynamic stream buffer. Hosts streamed data on the server side.
*/
NONSHARABLE_CLASS( HMenuSrvBuf ): public TBufBuf
    {

public:     // Constructors and destructor

    /**
    * Two phased constructor. Leaves on failure.
    * @return The constructed buffer.
    */
    static HMenuSrvBuf* NewL();

    /**
    * Two phased constructor. Leaves on failure.
    * @return The constructed buffer.
    */
    static HMenuSrvBuf* NewLC();

private:    // Constructors and destructor

    /**
    * Constructor.
    */
    inline HMenuSrvBuf();

    /**
    * Second phase constructor. Leaves on failure.
    */
    void ConstructL();

    /**
    * Destructor.
    */
    virtual inline ~HMenuSrvBuf();

private:    // From TBufBuf

    /**
    * Release this buffer. Deletes this.
    */
    void DoRelease();

private:    // Data

    CBufBase* iBuf; ///< The buffer. Owned.

    };

#include "menusrvbuf.inl"

#endif // __MENUSRVBUF_H__
            
// End of File
