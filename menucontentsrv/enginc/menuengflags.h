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

#ifndef __MENUENGFLAGS_H__
#define __MENUENGFLAGS_H__

#include <e32base.h>

/**
* Conversion between string and integer representations of "flags" attribute.
*/
NONSHARABLE_CLASS( MenuEngFlags )
    {

public:     // new methods

    /**
    * Convert string value to integer.
    * @param aStringValue String value.
    * @return Integer value.
    */
    static TUint32 AsInt( const TDesC& aStringValue );

    /**
    * Convert integer value to string.
    * @param Integer value.
    * @param aStringValue Buffer receiving string value.
    */
    static void AsString( TUint32 aIntValue, TDes& aStringValue );

private:    // new methods

    /**
    * Lookup flag name and get flag value.
    * @param aString String token.
    * @return Flag bit.
    */
    static TUint32 FlagValue( const TDesC& aString );

    /**
    * Lookup flag bit and get token value.
    * @param aFlag Flag bit.
    * @return String token.
    */
    static TPtrC StringValue( TUint32 aFlag );

    };

/**
* Conversion between string and integer representations of "id" attribute.
*/
NONSHARABLE_CLASS( MenuEngId )
    {

public:     // new methods

    /**
    * Convert string value to integer.
    * @param aStringValue String value.
    * @return Integer value.
    */
    static TInt AsInt( const TDesC& aStringValue ); 

    /**
    * Convert integer value to string.
    * @param Integer value.
    * @param aStringValue Buffer receiving string value.
    */
    static void AsString( TInt aIntValue, TDes& aStringValue );

    };

#endif // __MENUENGFLAGS_H__
