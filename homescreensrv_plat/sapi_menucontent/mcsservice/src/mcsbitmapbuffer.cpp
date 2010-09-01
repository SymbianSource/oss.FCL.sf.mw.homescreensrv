/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies).
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


#include "mcsbitmapbuffer.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//  
CMcsBitmapBuffer::CMcsBitmapBuffer( CFbsBitmap* aBitmapPtr )
                   : iBitmapPtr( aBitmapPtr )
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
CMcsBitmapBuffer* CMcsBitmapBuffer::NewL( CFbsBitmap* aBitmapPtr )
    {
    return new( ELeave ) CMcsBitmapBuffer( aBitmapPtr );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
CMcsBitmapBuffer::~CMcsBitmapBuffer()
    {
    Release();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TUint8* CMcsBitmapBuffer::Buf() const
    {
    return NULL;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMcsBitmapBuffer::Len()
    {
    return 0;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void CMcsBitmapBuffer::Release()
    {
    delete iBitmapPtr;
    iBitmapPtr = NULL;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt CMcsBitmapBuffer::TypeID()
    {
    return KLiwBufferBitmap;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
CFbsBitmap* CMcsBitmapBuffer::AsBitmap()
    {
    return iBitmapPtr;
    }

// End of file
