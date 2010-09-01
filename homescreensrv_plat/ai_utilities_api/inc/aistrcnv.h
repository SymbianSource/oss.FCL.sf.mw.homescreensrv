/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  String conversion utility functions
*
*/


#ifndef AISTRCNV_H
#define AISTRCNV_H

#include <e32std.h>

namespace AiUtility
    {
    /**
	 * Parses an integer value from string. The string may contain
	 * the integer value either in hexadecimal or decimal format.
	 *
	 * @param aResult the result integer value
	 * @param aSourceString the source string
	 * @return KErrNone if aStringValue is parsed to aValue.
	 */	 
	IMPORT_C TInt ParseInt( TInt32& aResult, const TDesC8& aSourceString );

    /**
	 * Parses an integer value from string. The string may contain
	 * the integer value either in hexadecimal or decimal format.
	 *
	 * @param aResult the result integer value
	 * @param aSourceString the source string
	 * @return KErrNone if aStringValue is parsed to aValue.
	 */	 
	IMPORT_C TInt ParseInt( TInt32& aResult, const TDesC16& aSourceString );
	
	/**
	 * Copies aText to aBuffer.
	 *
	 * @param aTargetBuffer a buffer to hold text. If aBuffer is NULL or
	 * insufficient, the function allocates a buffer.
	 * @param aSourceText the text to copy.
	 * @return pointer to result buffer.
	 */ 	
	IMPORT_C HBufC16* CopyToBufferL(HBufC16* aTargetBuffer, const TDesC16& aSourceText);
    
    /**
	 * Copies and converts aText to aBuffer. UTF-8 to UCS-2 conversion
	 * is applied to aText.
	 *
	 * @param aTargetBuffer a buffer to hold text. If aBuffer is NULL or
	 * insufficient, the function allocates a buffer.
	 * @param aSourceText the text to copy.
	 * @return pointer to result buffer.
 	 */ 
    IMPORT_C HBufC16* CopyToBufferL(HBufC16* aTargetBuffer, const TDesC8& aSourceText);
    
    /**
	 * Copies aText to aBuffer.
	 *
	 * @param aTargetBuffer a buffer to hold text. If aBuffer is NULL or
	 * insufficient, the function allocates a buffer.
	 * @param aSourceText the text to copy.
	 * @return pointer to result buffer.
	 */ 
    IMPORT_C HBufC8* CopyToBufferL(HBufC8* aTargetBuffer, const TDesC8& aSourceText);
    
    /**
	 * Copies and converts aText to aBuffer. UCS-2 to UTF-8 conversion
	 * is applied to aText.
	 *
	 * @param aTargetBuffer a buffer to hold text. If aBuffer is NULL or
	 * insufficient, the function allocates a buffer.
	 * @param aSourceText the text to copy.
	 * @return pointer to result buffer.
	 */ 
    IMPORT_C HBufC8* CopyToBufferL(HBufC8* aTargetBuffer, const TDesC16& aSourceText);
    
    /**
     * Ensures the buffer maximum length to be at least given number of characters.
     * This function checks the maximum length of given buffer and reallocates a
     * new buffer if the maximum length is not at least given number of characters.
     *
     * @param aBuffer refence to pointer to buffer to be checked.
     * @param aMaxLength new maximum length
     * @return modifiable pointer descriptor to the buffer
     *
     */
    IMPORT_C TPtr16 EnsureBufMaxLengthL(HBufC16*& aBuffer, TInt aMaxLength );

    }

#endif // AISTRCNV_H
