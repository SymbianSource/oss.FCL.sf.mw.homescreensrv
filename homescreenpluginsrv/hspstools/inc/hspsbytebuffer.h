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
* Description:  Header for ChspsByteBuffer.
*
*/

#ifndef HSPS_BYTEBUFFER_H
#define HSPS_BYTEBUFFER_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>

// CLASS DECLARATION

/**
 *  ChspsByteBuffer
 * 
 */
class ChspsByteBuffer : public CBase
    {
public:
    /**
     * Destructor.
     */
    ~ChspsByteBuffer();

    /**
     * Two-phased constructor.
     * 
     * @param aSize     Size of buffer.
     */
    static ChspsByteBuffer* NewL( const TInt aSize );

    /**
     * Access data.
     * 
     * @return  TUint*  Pointer to actual buffer.
     */
    TUint8* Buffer();    
    
    /**
     * Access size
     * 
     * @return  TInt    Buffer size.
     */
    TInt Size();    
    
private:
    /**
     * Constructor for performing 1st stage construction
     */
    ChspsByteBuffer();

    /**
     * EPOC default constructor for performing 2nd stage construction
     * 
     * @param aSize     Size of buffer.
     */
    void ConstructL( const TInt aSize );

private:
    /**
     * Size of buffer.
     */
    TInt iSize;
    
    /**
     * Pointer to actual data.
     */
    TUint8* iBuffer;    
    };

#endif // HSPS_BYTEBUFFER_H
