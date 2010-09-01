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


#ifndef MCSBITMAPBUFFER_H_
#define MCSBITMAPBUFFER_H_

#include <liwvariant.h>
#include <liwvariant.h>
#include <liwbufferextension.h>


/**
 * 
 */
class CMcsBitmapBuffer : public CLiwBitmapBuffer
    {
    public:
        
        /**
         * Two-phased constructor.
         * 
         * @return CMcsBitmapBuffer* Pointer.
         */         
        static CMcsBitmapBuffer* NewL( CFbsBitmap* aBitmapPtr );
        
        /**
         * Destructor
         */
        virtual ~CMcsBitmapBuffer();
        
    private:
        
        /**
         * Default Constructor.
         */
        CMcsBitmapBuffer( CFbsBitmap* aBitmapPtr);
    
    public:
        
        /**
         * Returns the pointer to the underlying buffer.
         *
         * @return the pointer to the underlying buffer
         */
        virtual TUint8* Buf() const;
        
        /**
         * Returns the length of the buffer data.
         *
         * @return the length of the buffer data
         */
        virtual TInt Len();
        
        /**
         * Releases the buffer. The service provider implementing concrete
         * buffer type should provide appropriate implementation.
         */
        virtual void Release();
        
        /**
         * Returns the appropriate type of buffer.
         */
        virtual TInt TypeID();
        
        /**
         * This returns the pointer of the type CFbsBitmap. 
         * @return platform specific bitmap representation. By default,
         *         this method returns NULL
         */
        virtual CFbsBitmap* AsBitmap();
        
    public:
        
        CFbsBitmap* iBitmapPtr;
        
    private:
        /**
         * Dummy implementation for ==  operator not applicable in this case.
         */
        TBool operator==( CLiwBuffer& /*aBuffer*/ )
        {
        return ETrue;
        }
    };

#endif /*MCSBITMAPBUFFER_H_*/

// End of file
