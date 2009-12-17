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
* Description:  
*
*/


#include "mcsmenuutils.h"

_LIT(KHexPrefix, "0x");

// ---------------------------------------------------------
// MenuUtils::GetTUint
// ---------------------------------------------------------
//
EXPORT_C TInt MenuUtils::GetTUint( const TDesC& aStr, TUint& aInt )
    {
    TInt position( aStr.Find(KHexPrefix) );
	TPtrC string( aStr );
	TRadix radix( EDecimal );
	if ( position == 0 )
		{
		// is hex
		radix = EHex;
		string.Set( aStr.Mid( KHexPrefix().Length() ) );
		}
    
    return TLex( string ).Val( aInt, radix );
    }
    
// ---------------------------------------------------------
// MenuUtils::SetTUint
// ---------------------------------------------------------
//
EXPORT_C void MenuUtils::SetTUint( TDes& aStr, TUint aInt )
    {
    // save in new version
    aStr.Zero();
    aStr.Append( KHexPrefix );
    aStr.AppendNum( aInt, EHex );		    
    }    

