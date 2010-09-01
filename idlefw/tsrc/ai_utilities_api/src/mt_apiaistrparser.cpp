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

//  CLASS HEADER
#include <mt_apiaistrparser.h>

//  external includes
#include <digia/eunit/eunitmacros.h>
#include <utf.h>

//  internal includes
#include <aistrparser.h>
#include <aiutility.h>
#include <aistrcnv.h>

// CONSTANTS
namespace
    {
    _LIT( KUnicode, "Text with unicode characters ä and \x20AC" );
    _LIT8( KUtf8, "Text" );
    }

_LIT16( KHexPrefix16, "0x" );

TInt ParseInt
        ( TInt32& aValue, const TDesC16& aStringValue )
    {
    const TInt pos = aStringValue.FindF( KHexPrefix16 );
    if (pos != KErrNotFound)
        {
        TLex16 lex( aStringValue.Mid( pos + KHexPrefix16().Length() ) );
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
        TLex16 lex( aStringValue );
        return lex.Val(aValue);
        }
    }


// CONSTRUCTION
MT_ApiAiStrParser* MT_ApiAiStrParser::NewL()
    {
    MT_ApiAiStrParser* self = MT_ApiAiStrParser::NewLC();
    CleanupStack::Pop();

    return self;
    }

MT_ApiAiStrParser* MT_ApiAiStrParser::NewLC()
    {
    MT_ApiAiStrParser* self = new( ELeave ) MT_ApiAiStrParser();
    CleanupStack::PushL( self );

    self->ConstructL();

    return self;
    }

// Destructor (virtual by CBase)
MT_ApiAiStrParser::~MT_ApiAiStrParser()
    {
    }

// Default constructor
MT_ApiAiStrParser::MT_ApiAiStrParser()
    {
    }

// Second phase construct
void MT_ApiAiStrParser::ConstructL()
    {
    // The ConstructL from the base class CEUnitTestSuiteClass must be called.
    // It generates the test case table.
    CEUnitTestSuiteClass::ConstructL();
    }

//  METHODS


void MT_ApiAiStrParser::SetupL()
    {
    iStrParser = AiUtility::CreateStrParserL();
    }

void MT_ApiAiStrParser::SetupUtf8BufferL()
    {
    iStrParser = AiUtility::CreateStrParserL();
    iUtf8Buffer = HBufC8::NewL(0);    
    }
         
void MT_ApiAiStrParser::SetupUnicodeBufferL()
    {
    iStrParser = AiUtility::CreateStrParserL();
    iUtf8Buffer = HBufC8::NewL(0);
    }
         
void MT_ApiAiStrParser::SetupHexIntL()
    {
    iStrParser = AiUtility::CreateStrParserL();
    _LIT8( KTestValue, "0xbad" );
    iUtf8Buffer = KTestValue().AllocL();
    iTestValue = 0xbad;
    iTestResult = KErrNone;
    }
         
void MT_ApiAiStrParser::SetupHexIntUnicodeL()
    {
    iStrParser = AiUtility::CreateStrParserL();
    _LIT( KTestValue, "0xbad" );
    iUnicodeBuffer = KTestValue().AllocL();
    iTestValue = 0xbad;
    iTestResult = KErrNone;
    }
         
void MT_ApiAiStrParser::SetupIntL()
    {
    iStrParser = AiUtility::CreateStrParserL();
    _LIT8( KTestValue, "42" );
    iUtf8Buffer = KTestValue().AllocL();
    iTestValue = 42;
    iTestResult = KErrNone;
    }

void MT_ApiAiStrParser::SetupIntUnicodeL()
    {
    iStrParser = AiUtility::CreateStrParserL();
    _LIT( KTestValue, "42" );
    iUnicodeBuffer = KTestValue().AllocL();
    iTestValue = 42;
    iTestResult = KErrNone;
    }

void MT_ApiAiStrParser::SetupNoIntL()
    {
    SetupUtf8BufferL();
    iTestResult = KErrGeneral;
    }
    
void MT_ApiAiStrParser::SetupNoIntUnicodeL()
    {
    iStrParser = AiUtility::CreateStrParserL();
    iUnicodeBuffer = HBufC::NewL(0);
    iTestResult = KErrGeneral;
    }
    
void MT_ApiAiStrParser::Teardown(  )
    {
    delete iUnicodeBuffer;
    iUnicodeBuffer = NULL;
    
    delete iUtf8Buffer;
    iUtf8Buffer = NULL;
    
    Release( iStrParser );
    }

void MT_ApiAiStrParser::TestCopyUnicodeToUnicodeL()
    {
    iUnicodeBuffer = iStrParser->CopyToBufferL(iUnicodeBuffer, KUnicode );
    
    EUNIT_ASSERT( *iUnicodeBuffer == KUnicode );
    }
         
void MT_ApiAiStrParser::TestCopyUnicodeToUtf8L()
    {
    iUtf8Buffer = iStrParser->CopyToBufferL( iUtf8Buffer, KUnicode );
    
    HBufC8* reference = CnvUtfConverter::ConvertFromUnicodeToUtf8L( KUnicode );
    
    EUNIT_ASSERT_EQUALS( *iUtf8Buffer, *reference );
    
    delete reference;
    }
            
void MT_ApiAiStrParser::TestCopyUtf8ToUnicodeL()
    {
    iUnicodeBuffer = iStrParser->CopyToBufferL( iUnicodeBuffer, KUtf8 );
    
    HBufC* reference = CnvUtfConverter::ConvertToUnicodeFromUtf8L( KUtf8 );
    
    EUNIT_ASSERT_EQUALS( *iUnicodeBuffer, *reference );
    
    delete reference;
    }
                 
void MT_ApiAiStrParser::TestCopyUtf8ToUtf8L()
    {
    iUtf8Buffer = iStrParser->CopyToBufferL( iUtf8Buffer, KUtf8 );
    
    EUNIT_ASSERT_EQUALS( *iUtf8Buffer, KUtf8 );
    }
         
void MT_ApiAiStrParser::TestParseInt()
    {
    TInt32 value(0);
    
    TInt result = iStrParser->ParseInt( value, *iUtf8Buffer );
    
    EUNIT_ASSERT_EQUALS( result, iTestResult );
    
    if ( result == KErrNone )
        {
        EUNIT_ASSERT_EQUALS( value, iTestValue );
        }
    }
         
void MT_ApiAiStrParser::TestParseIntUnicode()
    {
    TInt32 value(0);
    
    TInt result = ParseInt( value, *iUnicodeBuffer );
    
    EUNIT_ASSERT_EQUALS( result, iTestResult );
    
    if ( result == KErrNone )
        {
        EUNIT_ASSERT_EQUALS( value, iTestValue );
        }
    }

void MT_ApiAiStrParser::TestEnsureBufMaxLengthL()
    {
    _LIT( KTestValue, "text1 text2 text3 " );
    _LIT( KTestValue2, "text4" );
    _LIT( KTestValue3, "text1 text2 text3 text4" );
    
    HBufC* buffer = KTestValue().AllocL();
    CleanupStack::PushL(buffer);
    TPtr ptr = AiUtility::EnsureBufMaxLengthL(buffer , 25 );
    ptr.Append(KTestValue2);
    
    
    
    EUNIT_ASSERT(!ptr.Compare(KTestValue3));
    
    CleanupStack::PopAndDestroy(buffer);
    
    }
         
//  TEST TABLE

EUNIT_BEGIN_TEST_TABLE(
    MT_ApiAiStrParser,
    "Unit test suite for class XmlUiController utility functions",
    "UNIT" )

EUNIT_TEST(
    "Copy unicode to unicode, no buffer",
    "AiXmlUiController",
    "CopyToBufferL( HBufC16*, const TDesC16& )",
    "FUNCTIONALITY",
    SetupL, TestCopyUnicodeToUnicodeL, Teardown)
    
EUNIT_TEST(
    "Copy unicode to unicode, reallocate buffer",
    "AiXmlUiController",
    "CopyToBufferL( HBufC16*, const TDesC16& )",
    "FUNCTIONALITY",
    SetupUnicodeBufferL, TestCopyUnicodeToUnicodeL, Teardown)

EUNIT_TEST(
    "Copy UTF8 to unicode, no buffer",
    "AiXmlUiController",
    "CopyToBufferL( HBufC16*, const TDesC8& )",
    "FUNCTIONALITY",
    SetupL, TestCopyUtf8ToUnicodeL, Teardown)
 

EUNIT_TEST(
    "Copy UTF8 to unicode, reallocate buffer",
    "AiXmlUiController",
    "CopyToBufferL( HBufC16*, const TDesC8& )",
    "FUNCTIONALITY",
    SetupUnicodeBufferL, TestCopyUtf8ToUnicodeL, Teardown)
       
EUNIT_TEST(
    "Copy unicode to UTF8, no buffer",
    "AiXmlUiController",
    "CopyToBufferL( HBufC8*, const TDesC16& )",
    "FUNCTIONALITY",
    SetupL, TestCopyUnicodeToUtf8L, Teardown)
    
EUNIT_TEST(
    "Copy unicode to UTF8, reallocate buffer",
    "AiXmlUiController",
    "CopyToBufferL( HBufC8*, const TDesC16& )",
    "FUNCTIONALITY",
    SetupUtf8BufferL, TestCopyUnicodeToUtf8L, Teardown)

EUNIT_TEST(
    "Copy UTF8 to UTF8, no buffer",
    "AiXmlUiController",
    "CopyToBufferL( HBufC8*, const TDesC8& )",
    "FUNCTIONALITY",
    SetupL, TestCopyUtf8ToUtf8L, Teardown)
    
EUNIT_TEST(
    "Copy UTF8 to UTF8, reallocate buffer",
    "AiXmlUiController",
    "CopyToBufferL( HBufC8*, const TDesC8& )",
    "FUNCTIONALITY",
    SetupUtf8BufferL, TestCopyUtf8ToUtf8L, Teardown)

EUNIT_TEST(
    "Parse hex integer",
    "AiXmlUiController",
    "ParseInt",
    "FUNCTIONALITY",
    SetupHexIntL, TestParseInt, Teardown)

EUNIT_TEST(
    "Parse decimal integer",
    "AiXmlUiController",
    "ParseInt",
    "FUNCTIONALITY",
    SetupIntL, TestParseInt, Teardown)

EUNIT_TEST(
    "Parse invalid buffer",
    "AiXmlUiController",
    "ParseInt",
    "ERROR HANDLING",
    SetupNoIntL, TestParseInt, Teardown)

EUNIT_TEST(
    "Parse UNICODE hex integer",
    "AiXmlUiController",
    "ParseInt",
    "FUNCTIONALITY",
    SetupHexIntUnicodeL, TestParseIntUnicode, Teardown)

EUNIT_TEST(
    "Parse UNICODE decimal integer",
    "AiXmlUiController",
    "ParseInt",
    "FUNCTIONALITY",
    SetupIntUnicodeL, TestParseIntUnicode, Teardown)

EUNIT_TEST(
    "Parse UNICODE invalid buffer",
    "AiXmlUiController",
    "ParseInt",
    "ERROR HANDLING",
    SetupNoIntUnicodeL, TestParseIntUnicode, Teardown)

EUNIT_TEST(
    "Copy string to buffer",
    "AiXmlUiController",
    "EnsureBufMaxLengthL",
    "ERROR HANDLING",
    SetupL, TestEnsureBufMaxLengthL, Teardown)

    EUNIT_END_TEST_TABLE

//  END OF FILE
