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

// INCLUDE FILES

#include "menuengflags.h"
#include "mcsmenuitem.h"

// CLASS DECLARATION

NONSHARABLE_STRUCT( TMenuFlagEntry )
    {
    TUint32 iFlag; // Flag bit value (one bit only).
    const TText* iString; // Flag string name.
    };

// CONSTANTS

/// Flag name table. Concatenated names (with spaces between) should fit into
// KMenuMaxAttrValueLen!
LOCAL_D const TMenuFlagEntry KTable[] =
    {
        { TMenuItem::ELockDelete,   _S("lock_delete") },
        { TMenuItem::ELockName,     _S("lock_name") },
        { TMenuItem::ELockIcon,     _S("lock_icon") },
        { TMenuItem::ELockMoveInto, _S("lock_moveinto") },
        { TMenuItem::ELockItem,     _S("lock_item") },
        { TMenuItem::EHidden,       _S("hidden") },
        { TMenuItem::EMissing,      _S("missing") },
        { 0 }
    };

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// MenuEngFlags::AsInt
// ---------------------------------------------------------
//
TUint32 MenuEngFlags::AsInt( const TDesC& aStringValue )
    {
    TUint32 flags = 0;
    TLex lex( aStringValue );
    TPtrC token( lex.NextToken() );
    while ( token.Length() )
        {
        flags |= FlagValue( token );
        token.Set( lex.NextToken() );
        }
    return flags;
    }

// ---------------------------------------------------------
// MenuEngFlags::AsString
// ---------------------------------------------------------
//
void MenuEngFlags::AsString( TUint32 aIntValue, TDes& aStringValue )
    {
    // Note, we assume that the result fits in aStringValue.
    // It is expected to be of length KMenuMaxAttrValueLen.
    // If new flags are added, we should check that their concatenated
    // names (with spaces between) still fit in one attribute. If not,
    // then we should use more than one "flags" attribute; but this is
    // easy to avoid if flag names are kept short. (32 flags should normally
    // fit in 512 characters!)
    __ASSERT_DEBUG( aStringValue.MaxLength() >= KMenuMaxAttrValueLen, \
        User::Invariant() );
    aStringValue.SetLength( 0 );
    TUint32 mask;
    TPtrC ptr;
    TBool leadingSpace = EFalse;
    for ( mask = 0x1; mask; mask = mask << 1 )
        {
        ptr.Set( StringValue( mask & aIntValue ) );
        if ( ptr.Length() )
            {
            if ( leadingSpace )
                {
                aStringValue.Append( TChar(' ') );
                }
            aStringValue.Append( ptr );
            leadingSpace = ETrue; // Need a leading space from the 2nd onwards.
            }
        }
    }

// ---------------------------------------------------------
// MenuEngFlags::FlagValue
// ---------------------------------------------------------
//
TUint32 MenuEngFlags::FlagValue( const TDesC& aString )
    {
    // Note: This could be optimized
    const TMenuFlagEntry* entry;
    for ( entry = &KTable[0]; entry->iFlag; entry++ )
        {
        if ( aString == TPtrC( entry->iString ) )
            {
            return entry->iFlag;
            }
        }
    return 0;
    }

// ---------------------------------------------------------
// MenuEngFlags::StringValue
// ---------------------------------------------------------
//
TPtrC MenuEngFlags::StringValue( TUint32 aFlag )
    {
    // It is expected that aFlag has exactly one bit set -> first
    // match is accepted.
    const TMenuFlagEntry* entry;
    for ( entry = &KTable[0]; entry->iFlag; entry++ )
        {
        if ( aFlag & entry->iFlag )
            {
            return TPtrC( entry->iString );
            }
        }
    return KNullDesC();
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// MenuEngId::AsInt
// ---------------------------------------------------------
//
TInt MenuEngId::AsInt( const TDesC& aStringValue )
    {
    TInt position( aStringValue.Find(KHexPrefix) );
	TPtrC string( aStringValue );
	TRadix radix( EDecimal );
	if ( position == 0 )
		{
		// is hex
		radix = EHex;
		string.Set( aStringValue.Mid( KHexPrefix().Length() ) );
		}
    
    TUint id = 0;
    if ( KErrNone != TLex( string ).Val( id, radix ) )
    	{
    	return 0;
    	}
    return id;
    }

// ---------------------------------------------------------
// MenuEngId::AsString
// ---------------------------------------------------------
//
void MenuEngId::AsString( TInt aIntValue, TDes& aStringValue )
    {
    // Note, we assume that the result fits in aStringValue.
    // It is expected to be of length KMenuMaxAttrValueLen.
    __ASSERT_DEBUG( aStringValue.MaxLength() >= KMenuMaxAttrValueLen, \
        User::Invariant() );
    aStringValue.SetLength( 0 );
    aStringValue.AppendNum( aIntValue );
    }

//  End of File  
