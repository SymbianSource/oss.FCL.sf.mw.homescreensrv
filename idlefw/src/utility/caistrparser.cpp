/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


#include "caistrparser.h"
#include "aistrcnv.h"

// CONSTANTS

const TInt KThreeOctets = 3;
const TInt KTwoOctets = 2;

namespace
    {
    
    _LIT8( KHexPrefix8, "0x" );
    _LIT16( KHexPrefix16, "0x" );
    
    const TInt KMaxOneOctetValue = 0x007F;
    const TInt KMaxTwoOctetValue = 0x07FF;
    
    TInt Utf8OctetCount( const TDesC16& aString )
    	{
        TInt numOctets(0);
        TInt length(aString.Length());
        const TUint16* stringData = aString.Ptr();
            
       	for ( TInt i = 0; i < length; ++i)
        	{
            if (stringData[i] > KMaxOneOctetValue)
            	{
                if (stringData[i] > KMaxTwoOctetValue)
                	{
                    numOctets += KThreeOctets;
                    }
               	else
                    {
                    numOctets += KTwoOctets;
                    }
                }
           	else
               	{
                ++numOctets;
                }
            }
                
      	return numOctets;
       	}
    
    TPtr8 EnsureBufMaxLengthL(HBufC8*& aBuffer, TInt aMaxLength )
        {
        if ( !aBuffer )
            {
            aBuffer = HBufC8::NewL( aMaxLength );
            }
        else if ( aBuffer->Des().MaxLength() < aMaxLength )
            {
            aBuffer = aBuffer->ReAllocL( aMaxLength );
            }
            
        return aBuffer->Des();
        }
    
    }

namespace AiUtility
    {
    
template< class DescType, class LexType > inline
TInt ParseIntImpl
        ( TInt32& aValue, const DescType& aStringValue, 
        const DescType& aHexPrefix )
    {
    const TInt pos = aStringValue.FindF( aHexPrefix );
    if (pos != KErrNotFound)
        {
        LexType lex( aStringValue.Mid( pos + aHexPrefix.Length() ) );
        // Hex parsing needs unsigned int
        TUint32 value = 0;
        const TInt parseResult = lex.Val( value, EHex );
        if ( parseResult == KErrNone )
            {
            aValue = value;
            }
        return parseResult;
        }
    else
        {
        LexType lex( aStringValue );
        return lex.Val(aValue);
        }
    }

EXPORT_C TInt ParseInt( TInt32& aResult, const TDesC8& aSourceString )
    {
    return ParseIntImpl< TDesC8, TLex8 >
        ( aResult, aSourceString, KHexPrefix8 );
    }
	
EXPORT_C TInt ParseInt( TInt32& aResult, const TDesC16& aSourceString )
    {
    return ParseIntImpl< TDesC16, TLex16 >
        ( aResult, aSourceString, KHexPrefix16 );
    }
	
EXPORT_C HBufC16* CopyToBufferL(HBufC16* aTargetBuffer, const TDesC16& aSourceText)
    {
    EnsureBufMaxLengthL( aTargetBuffer, aSourceText.Length() ).Copy( aSourceText );
    
    return aTargetBuffer;
    }
    
EXPORT_C HBufC16* CopyToBufferL(HBufC16* aTargetBuffer, const TDesC8& aSourceText)
    {
    if (!aTargetBuffer)
        {
        return CnvUtfConverter::ConvertToUnicodeFromUtf8L( aSourceText );
        }
        
    TPtr16 des( EnsureBufMaxLengthL( aTargetBuffer, aSourceText.Length() ) );
    
    CnvUtfConverter::ConvertToUnicodeFromUtf8( des, aSourceText );
    
    return aTargetBuffer;
    }
    
EXPORT_C HBufC8* CopyToBufferL(HBufC8* aTargetBuffer, const TDesC8& aSourceText)
    {
    ::EnsureBufMaxLengthL( aTargetBuffer, aSourceText.Length() ).Copy( aSourceText );
    return aTargetBuffer;
    }

EXPORT_C HBufC8* CopyToBufferL(HBufC8* aTargetBuffer, const TDesC16& aSourceText)
    {
    if ( !aTargetBuffer )
        {
        return CnvUtfConverter::ConvertFromUnicodeToUtf8L( aSourceText );
        }
    
    TInt length = Utf8OctetCount( aSourceText );
    
    TPtr8 des( ::EnsureBufMaxLengthL( aTargetBuffer, length ) );    
    CnvUtfConverter::ConvertFromUnicodeToUtf8( des, aSourceText );
    return aTargetBuffer;
    }
    
EXPORT_C TPtr16 EnsureBufMaxLengthL(HBufC16*& aTargetBuffer, TInt aMaxLength )
    {
    if ( !aTargetBuffer )
        {
        aTargetBuffer = HBufC16::NewL( aMaxLength );
        }
    else
        {
        if ( aTargetBuffer->Des().MaxLength() < aMaxLength )
            {
            aTargetBuffer = aTargetBuffer->ReAllocL( aMaxLength );
            }
        }
        
    return aTargetBuffer->Des();
    }

    }


// ---------------------------------------------------------------------------
// CStrParser()
// ---------------------------------------------------------------------------
//
CStrParser::CStrParser()
    {
    }

// ---------------------------------------------------------------------------
// CStrParser* NewL()
// ---------------------------------------------------------------------------
//
CStrParser* CStrParser::NewL()
    { 
    CStrParser* self = new ( ELeave ) CStrParser();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// void ConstructL()
// ---------------------------------------------------------------------------
//
void CStrParser::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// void Release()
// ---------------------------------------------------------------------------
//
void CStrParser::Release()
    {
    delete this;
    }
    
// ---------------------------------------------------------------------------
// TInt ParseInt( TInt32& aValue, const TDesC8& aStringValue )
// ---------------------------------------------------------------------------
//
TInt CStrParser::ParseInt( TInt32& aResult, const TDesC8& aSourceString )
    {
    return AiUtility::ParseInt( aResult, aSourceString );
    }

// ---------------------------------------------------------------------------
// HBufC16* CopyToBufferL( HBufC16* aTargetBuffer, const TDesC16& aSourceText )
// ---------------------------------------------------------------------------
//    
HBufC16* CStrParser::CopyToBufferL( HBufC16* aTargetBuffer, const TDesC16& aSourceText )
    {
    return AiUtility::CopyToBufferL( aTargetBuffer, aSourceText );
    }

// ---------------------------------------------------------------------------
// HBufC16* CopyToBufferL( HBufC16* aTargetBuffer, const TDesC8& aSourceText )
// ---------------------------------------------------------------------------
//
HBufC16* CStrParser::CopyToBufferL( HBufC16* aTargetBuffer, const TDesC8& aSourceText )
    {
    return AiUtility::CopyToBufferL( aTargetBuffer, aSourceText );
    }

// ---------------------------------------------------------------------------
// HBufC8* CopyToBufferL( HBufC8* aTargetBuffer, const TDesC8& aSourceText )
// ---------------------------------------------------------------------------
//    
HBufC8* CStrParser::CopyToBufferL( HBufC8* aTargetBuffer, const TDesC8& aSourceText )
    {
    return AiUtility::CopyToBufferL( aTargetBuffer, aSourceText );
    }

// ---------------------------------------------------------------------------
// HBufC8* CopyToBufferL( HBufC8* aTargetBuffer, const TDesC16& aSourceText )
// ---------------------------------------------------------------------------
//
HBufC8* CStrParser::CopyToBufferL( HBufC8* aTargetBuffer, const TDesC16& aSourceText )
    {
    return AiUtility::CopyToBufferL( aTargetBuffer, aSourceText );
    }
    
// End of File.


