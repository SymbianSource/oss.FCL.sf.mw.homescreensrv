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
* Description:  Utils methods.
 *
*/


#ifndef FACTSETCHUTILS_H
#define FACTSETCHUTILS_H

//  INCLUDES
#include <E32BASE.H>

// CLASS DECLARATION
class CGECODefaultObject;
class MXCFWNode;

/**
 *  Common utils methods.
 *
 *  @lib aspafactorysettings.lib
 *  @since Series 60 3.1
 */
class FactSetCHUtils
    {
public:
    // New functions

    /**
     * Extracts a TUint from the given string.
     * @since Series 60 3.1
     * @param aStr A string defining the uint.
     * @return TUint Object.
     */
    static TUint Str2Uint( const TDesC& aStr );
    
    /**
     * Extracts a TInt from the given string.
     * @since Series 60 3.1
     * @param aStr A string defining the int.
     * @return TInt Object.
     */
    static TInt Str2Int( const TDesC& aStr );
    
    /**
     * Extracts a TBool from the given string.
     * @since Series 60 3.1
     * @param aStr A string defining the bool.
     * @return TBool Object.
     */
    static TBool Str2Bool( const TDesC& aStr );

    /**
     * Extracts a TUid from the given string.
     * @since Series 60 3.1
     * @param aStr A string defining the uid.
     * @return Uid Object.
     */
    static TUid Str2Uid( const TDesC& aStr );

    /**
     * Creates HBufC8* from the given string.
     * @since Series 60 3.1
     * @param aStr A string to convert to HBufC8*.
     * @return HBufC8* Pointer.
     */
    static HBufC8* Str2Str8LC( const TDesC& aStr );

    /**
     * Extracts a TReal from the given string.
     * @since Series 60 3.1
     * @param aStr A string defining the real value.
     * @return Real Object.
     */
    static TReal Str2Real( const TDesC& aStr );

    /**
     * Extracts a TInt from the given string.
     * @since Series 60 3.1
     * @param aStr A string defining the real value.
     * @return Real Object.
     */
    static TInt32 Str2Int32( const TDesC& aStr );

private:

    /**
     * Extracts an integer from the given hex string.
     * @since Series 60 3.1
     * @param aStr A hex string containg an integer number.
     * @return Integer value of the number.
     */
    static TUint StrHex2Uint( const TDesC& aStr );

    /**
     * Extracts an integer from the given string.
     * @since Series 60 3.1
     * @param aStr A string containg an integer number.
     * @return Integer value of the number.
     */
    static TUint StrDec2Uint( const TDesC& aStr );

    /**
     * Handles bad argument passed into utility function
     * @since Series 60 3.1
     * @param aCategory describes bad argument
     * @param aArgument bad argument
     * @param aReason bad argument code
     */
    static void BadArgument( const TDesC& aCategory, const TDesC& aArgument,
        TInt aReason );
    static void BadArgumentL( const TDesC& aCategory, const TDesC& aArgument,
        TInt aReason );

    /**
     * Handles bad argument passed into utility function
     * @since Series 60 3.1
     * @param aCategory describes bad argument
     * @param aArgument bad argument
     * @param aReason bad argument code
     */
    static void BadArgument( const TDesC& aCategory, TInt aArgument,
        TInt aReason );

    /**
     * Handles bad argument passed into utility function
     * @since Series 60 3.1
     * @param aCategory describes bad argument
     * @param aReason bad argument code
     */
    static void BadArgument( const TDesC& aCategory, TInt aReason );

    };

#endif      // FACTSETCHUTILS_H   
// End of File
