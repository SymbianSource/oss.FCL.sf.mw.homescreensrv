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
* Description:  Implementation of ChspsByteBuffer.
*
*/


#include "hspsbytebuffer.h"

//----------------------------------------------------------------------------
// ChspsByteBuffer::~ChspsByteBuffer
// ----------------------------------------------------------------------------
//
ChspsByteBuffer::~ChspsByteBuffer()
    {
    delete[] iBuffer;
    iBuffer = NULL;
    }

//----------------------------------------------------------------------------
// ChspsByteBuffer::NewL
// ----------------------------------------------------------------------------
//
ChspsByteBuffer* ChspsByteBuffer::NewL( const TInt aSize )
    {
    ChspsByteBuffer* self = new (ELeave) ChspsByteBuffer();
    CleanupStack::PushL( self );
    self->ConstructL( aSize );
    CleanupStack::Pop( self );
    return self;
    }

//----------------------------------------------------------------------------
// ChspsByteBuffer::Buffer
// ----------------------------------------------------------------------------
//
TUint8* ChspsByteBuffer::Buffer()
    {
    return iBuffer;
    }

//----------------------------------------------------------------------------
// ChspsByteBuffer::Size
// ----------------------------------------------------------------------------
//
TInt ChspsByteBuffer::Size()
    {
    return iSize;
    }

//----------------------------------------------------------------------------
// ChspsByteBuffer::ChspsByteBuffer
// ----------------------------------------------------------------------------
//
ChspsByteBuffer::ChspsByteBuffer()
    {
    // No implementation required
    }

//----------------------------------------------------------------------------
// ChspsByteBuffer::ConstructL
// ----------------------------------------------------------------------------
//
void ChspsByteBuffer::ConstructL( const TInt aSize )
    {
    if( aSize <= 0 )
        {
        User::Leave( KErrArgument );
        }
    
    iBuffer = new (ELeave) TUint8[ aSize ];
    iSize = aSize;
    }
