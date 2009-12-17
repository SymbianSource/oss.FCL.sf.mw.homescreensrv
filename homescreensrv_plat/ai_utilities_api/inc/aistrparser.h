/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Collection of string parsing methods.
*
*/


#ifndef AISTRPARSER_H
#define AISTRPARSER_H

// INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
*  It is the interface for handling string parsing.
*
*  @since S60 3.2
*/
class MAiStrParser
    {
public:
    
	/**
	 * Parses an integer value from string. The string may contain
	 * the integer value either in hexadecimal or decimal format.
	 *
	 * @param aValue the result integer value
	 * @param aStringValue the source string
	 * @return KErrNone if aStringValue is parsed to aValue.
	 */	 
	virtual TInt ParseInt( TInt32& aValue, const TDesC8& aStringValue ) = 0;
	
	/**
	 * Copies aText to aBuffer.
	 *
	 * @param aBuffer a buffer to hold text. If aBuffer is NULL or
	 * insufficient, the function allocates a buffer.
	 * @param aText the text to copy.
	 * @return pointer to result buffer.
	 */ 	
	virtual HBufC16* CopyToBufferL(HBufC16* aBuffer, const TDesC16& aText) = 0;
    
    /**
	 * Copies and converts aText to aBuffer. UTF-8 to UCS-2 conversion
	 * is applied to aText.
	 *
	 * @param aBuffer a buffer to hold text. If aBuffer is NULL or
	 * insufficient, the function allocates a buffer.
	 * @param aText the text to copy.
	 * @return pointer to result buffer.
 	 */ 
    virtual HBufC16* CopyToBufferL(HBufC16* aBuffer, const TDesC8& aText) = 0;
    
    /**
	 * Copies aText to aBuffer.
	 *
	 * @param aBuffer a buffer to hold text. If aBuffer is NULL or
	 * insufficient, the function allocates a buffer.
	 * @param aText the text to copy.
	 * @return pointer to result buffer.
	 */ 
    virtual HBufC8* CopyToBufferL(HBufC8* aBuffer, const TDesC8& aText) = 0;
    
    /**
	 * Copies and converts aText to aBuffer. UCS-2 to UTF-8 conversion
	 * is applied to aText.
	 *
	 * @param aBuffer a buffer to hold text. If aBuffer is NULL or
	 * insufficient, the function allocates a buffer.
	 * @param aText the text to copy.
	 * @return pointer to result buffer.
	 */ 
    virtual HBufC8* CopyToBufferL(HBufC8* aBuffer, const TDesC16& aText) = 0;
    
    /**
     * Release the parser
     */
    virtual void Release() = 0;
	    
protected:
	    
	~MAiStrParser() { }
    
    };

/**
 * Helper for calling MAiStrParser::Release() with NULL checking.
 */
inline void Release(MAiStrParser* obj)
    {
    if (obj)
        {
        obj->Release();
        }
    }


#endif      // AISTRPARSER_H

// End of File