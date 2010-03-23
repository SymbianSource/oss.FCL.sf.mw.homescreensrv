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


// INCLUDE FILES
#include <xcfwnodeinterface.h>
#include <GECODefaultObject.h>

#include "chfactsetutils.h"
#include "chfactorysettingsglobals.h"
#include "cpdebug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// FactSetCHUtils::StrDec2Int
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint FactSetCHUtils::StrDec2Uint( const TDesC& aStr )
    {
    CP_DEBUG(_L8("FactSetCHUtils::StrDec2Uint" ));
    TLex lexer(aStr);
    TInt ret;
    if ( lexer.Val( ret ) )
        {
        _LIT(KDesc, "Int");
        BadArgument( KDesc, aStr, KErrArgument ) ;
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// FactSetCHUtils::Str2Uint
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
TUint FactSetCHUtils::Str2Uint( const TDesC& aStr )
    {
    TUint ret(Str2Int( aStr ) );
    return ret;
    }

// -----------------------------------------------------------------------------
// FactSetCHUtils::Str2Bool
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
TBool FactSetCHUtils::Str2Bool( const TDesC& aStr )
    {
    CP_DEBUG(_L8("FactSetCHUtils::Str2Bool" ));
    TBool ret;
    if ( !aStr.Compare( KTrue ) )
        {
        ret = ETrue;
        }
    else if ( !aStr.Compare( KFalse ) )
        {
        ret = EFalse;
        }
    else
        ret = (Str2Int( aStr ) );
    return ret;
    }

// -----------------------------------------------------------------------------
// FactSetCHUtils::Str2Int
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//   
TInt FactSetCHUtils::Str2Int( const TDesC& aStr )
    {
    CP_DEBUG(_L8("FactSetCHUtils::Str2Int" ));
    // if a string starts from '0x' then convert aStr from hex to int.
    // else send aStr to the StrDec2Uint	
    _LIT(KHexStart,"0x");

    TInt position(aStr.Find( KHexStart ) );
    TInt ret(KErrArgument);
    if ( position == 0 )
        {
        // is hex
        TPtrC string(aStr.Mid( KHexStart().Length( ) ) );

        ret = StrHex2Uint( string );
        }
    else
        {
        ret = StrDec2Uint( aStr );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// FactSetCHUtils::StrHex2UintL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint FactSetCHUtils::StrHex2Uint( const TDesC& aStr )
    {
    CP_DEBUG(_L8("FactSetCHUtils::StrHex2Uint" ));
    TLex lexer(aStr);
    TUint ret;
    if ( lexer.Val( ret, EHex ) )
        {
        _LIT(KDesc, "Hex");
        BadArgument( KDesc, aStr, KErrArgument );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// FactSetCHUtils::Str2Str8L
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC8* FactSetCHUtils::Str2Str8LC( const TDesC& aStr )
    {
    CP_DEBUG(_L8("FactSetCHUtils::Str2Str8LC" ));
    HBufC8* ret = HBufC8::NewLC( aStr.Length( ) );
    ret->Des().Copy( aStr );
    return ret;
    }
// -----------------------------------------------------------------------------
// FactSetCHUtils::Str2UidL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUid FactSetCHUtils::Str2Uid( const TDesC& aStr )
    {
    return TUid( TUid::Uid( Str2Uint( aStr ) ) );
    }
// -----------------------------------------------------------------------------
// FactSetCHUtils::Str2UidL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TReal FactSetCHUtils::Str2Real( const TDesC& aStr )
    {
    CP_DEBUG(_L8("FactSetCHUtils::Str2Real" ));
    TLex lexer(aStr);
    TReal ret;
    if ( lexer.Val( ret ) )
        {
        _LIT(KDesc, "Real");
        BadArgument( KDesc, aStr, KErrArgument );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// FactSetCHUtils::Str2IntL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt32 FactSetCHUtils::Str2Int32( const TDesC& aStr )
    {
    TInt32 ret(Str2Int( aStr ) );
    return ret;
    }

// -----------------------------------------------------------------------------
// FactSetCHUtils::BadArgument
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void FactSetCHUtils::BadArgument( const TDesC& aCategory,
    const TDesC& aArgument, TInt aReason )
    {
    TRAP_IGNORE( BadArgumentL(aCategory, aArgument, aReason) );
    }
// -----------------------------------------------------------------------------
// FactSetCHUtils::BadArgumentL
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void FactSetCHUtils::BadArgumentL( const TDesC& aCategory,
    const TDesC& aArgument, TInt aReason )
    {
    CP_DEBUG(_L8("FactSetCHUtils::BadArgumentL" ));
    _LIT(K2Dot,":");
    HBufC* buf = HBufC::NewLC( aCategory.Length( ) + aArgument.Length( )
            + K2Dot().Length( ) );
    TPtr ptr(buf->Des( ) );
    ptr.Copy( aCategory );
    ptr.Append( K2Dot );
    ptr.Append( aArgument );
    BadArgument( ptr, aReason );
    CleanupStack::PopAndDestroy( buf );
    }
// -----------------------------------------------------------------------------
// FactSetCHUtils::BadArgument
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void FactSetCHUtils::BadArgument( const TDesC& aCategory, TInt aArgument,
    TInt aReason )
    {
    TBuf<KMaxIntString> buf;
    buf.Num( aArgument );
    BadArgument( aCategory, buf, aReason );
    }
// -----------------------------------------------------------------------------
// FactSetCHUtils::BadArgument
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void FactSetCHUtils::BadArgument( const TDesC& aCategory, TInt aReason )
    {
    User::Panic( aCategory, aReason );
    }

//  End of File  
