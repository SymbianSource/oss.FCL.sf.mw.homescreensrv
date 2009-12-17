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
* Description:  Implementation of ChspsLiwDump.
*
*/


#include "hspsbytebuffer.h"
#include "hspsliwdump.h"
#include "hspslogbus.h"
#include <s32mem.h>
#include <liwcommon.h>

#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
// indentation delimiter
_LIT8 (KDelim8, "\t" );
_LIT( KDelim, "\t" );
_LIT( KCPlusPlusArrayStart, "const TUint8 liwDump[] = {" );
_LIT( KCPlusPlusArrayEnd, "};" );
const TInt KCPlusPlusArraySanityLimit = 16384;   // 16kb.
_LIT( KByteFormat, "0x%02x" );
_LIT( KLiwListBinaryDumpStartTag, "#LIW LIST BINARY DUMP START" );
_LIT( KLiwListBinaryDumpEndTag, "#LIW LIST BINARY DUMP END" );
_LIT( KLiwListTextDumpStartTag, "#LIW LIST TEXT DUMP START" );
_LIT( KLiwListTextDumpEndTag, "#LIW LIST TEXT DUMP END" );
#endif 

//------------------------------------------------------------------------------
// ChspsLiwDump::~ChspsLiwDump
//------------------------------------------------------------------------------
EXPORT_C ChspsLiwDump::~ChspsLiwDump()
    {
    }

//------------------------------------------------------------------------------
// ChspsLiwDump::NewLC
//------------------------------------------------------------------------------
EXPORT_C ChspsLiwDump* ChspsLiwDump::NewLC( ChspsLogBus& aLogBus )
    {
    ChspsLiwDump* self = new (ELeave)ChspsLiwDump( aLogBus );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

//------------------------------------------------------------------------------
// ChspsLiwDump::NewL
//------------------------------------------------------------------------------
EXPORT_C ChspsLiwDump* ChspsLiwDump::NewL( ChspsLogBus& aLogBus )
    {
    ChspsLiwDump* self = ChspsLiwDump::NewLC( aLogBus );
    CleanupStack::Pop( self ); // self;
    return self;
    }

//------------------------------------------------------------------------------
// ChspsLiwDump::LogLiwListAsText
//------------------------------------------------------------------------------
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
EXPORT_C void ChspsLiwDump::LogLiwListAsText( CLiwGenericParamList& aLiwList )
    {
    TRAP_IGNORE( LogLiwListAsTextL( aLiwList ); );
    }
#else
EXPORT_C void ChspsLiwDump::LogLiwListAsText( CLiwGenericParamList& /*aLiwList*/ )
    {    
    }
#endif

#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
//------------------------------------------------------------------------------
// ChspsLiwDump::LogLiwListAsBinary
//------------------------------------------------------------------------------
EXPORT_C void ChspsLiwDump::LogLiwListAsBinary( CLiwGenericParamList& aLiwList )
    {
    TRAP_IGNORE( LogLiwListAsBinaryL( aLiwList ); );
    }
#else
EXPORT_C void ChspsLiwDump::LogLiwListAsBinary( CLiwGenericParamList& /*aLiwList*/ )
    {    
    }
#endif

//------------------------------------------------------------------------------
// ChspsLiwDump::ChspsLiwDump
//------------------------------------------------------------------------------
ChspsLiwDump::ChspsLiwDump( ChspsLogBus& aLogBus ) : iLogBus( aLogBus )
    {
    iDepth = 0;
    }

//------------------------------------------------------------------------------
// ChspsLiwDump::ConstructL
//------------------------------------------------------------------------------
void ChspsLiwDump::ConstructL()
    {
    }

//------------------------------------------------------------------------------
// ChspsLiwDump::LogLiwListAsTextL
//------------------------------------------------------------------------------
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
void ChspsLiwDump::LogLiwListAsTextL( CLiwGenericParamList& aLiwList )
    {
    iLogBus.LogText( KLiwListTextDumpStartTag );
    
    // implementation for GetActiveAppConf and GetPluginConf that has map
    // as base root.      
    iDepth = 0;
    TInt count = aLiwList.Count();
    for( TInt i = 0; i < count; i++ )
        {
        //get Generic param and check for param id and name.
        TLiwGenericParam param = aLiwList[i];
        //Check for the param identifier:
        LIW::TGenericParamId identifier = param.SemanticId();
        //print semanticId:
        iLogBus.LogText( _L8(" SemanticId: %d"), identifier );
        if( identifier == LIW::EGenericParamError )
            {
            iLogBus.LogText( _L8(" ERROR: EGenericParamError") );
            }
        //get generic param name.
        //print name.
        iLogBus.LogText( param.Name() );
        //get Variant
        TLiwVariant value = param.Value();
        //get variant type.
        PrintVariantL( value );
        }
    
    iLogBus.LogText( KLiwListTextDumpEndTag );
    }
#else
void ChspsLiwDump::LogLiwListAsTextL( CLiwGenericParamList& /*aLiwList*/ )
    {    
    }
#endif

#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
//------------------------------------------------------------------------------
// ChspsLiwDump::LogLiwListAsBinaryL
//------------------------------------------------------------------------------
void ChspsLiwDump::LogLiwListAsBinaryL( CLiwGenericParamList& aLiwList )
    {    
    iLogBus.LogText( KLiwListBinaryDumpStartTag );
    
    const TInt byteSize = aLiwList.Size();
    if( byteSize <= 0 )
        {
        User::Leave( KErrArgument );
        }

    // Create memory buffer.
    ChspsByteBuffer* buffer = ChspsByteBuffer::NewL( byteSize );
    CleanupStack::PushL( buffer );    

    // Wrap buffer using memory stream.
    RMemWriteStream stream;    
    stream.Open( buffer->Buffer(), byteSize );
    stream.PushL();
    
    // Use stream that wraps buffer to enable direct-to-RAM
    // streaming from CLiwGenericParamList.
    aLiwList.ExternalizeL( stream );
    stream.CommitL();
    
    // Clean stream wrapper.
    stream.Release();
    stream.Close();
    stream.Pop();
    
    // Create C++ array and print it.
    HBufC* cplusplusDump = CreateCPlusPlusDumpL( *buffer );
    iLogBus.LogText( *cplusplusDump );
    delete cplusplusDump;
    cplusplusDump = NULL;
    
    // Clean memory buffer
    CleanupStack::PopAndDestroy( buffer );        
    
    iLogBus.LogText( KLiwListBinaryDumpEndTag );
    }
#else
void ChspsLiwDump::LogLiwListAsBinaryL( CLiwGenericParamList& /*aLiwList*/ )
    {    
    }
#endif

//------------------------------------------------------------------------------
// ChspsLiwDump::PrintVariantL
//------------------------------------------------------------------------------
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
void ChspsLiwDump::PrintVariantL( TLiwVariant value )
    {
    LIW::TVariantTypeId variantId = value.TypeId();
    if( variantId == LIW::EVariantTypeDesC8 )
        {
        PrintString8L( value );
        }
    else if( variantId == LIW::EVariantTypeDesC )
        {
        PrintStringL( value );
        }
    else if ( variantId == LIW::EVariantTypeMap )
        {
        HBufC* text = GetFixedTextLC( _L("<Map>"), iDepth, KDelim );
        iLogBus.LogText( *text );
        CleanupStack::PopAndDestroy( text );
        PrintMapL( value );
        }
    else if ( variantId == LIW::EVariantTypeList )
        {
        HBufC* text = GetFixedTextLC( _L("<List>"), iDepth, KDelim );
        iLogBus.LogText( *text );
        CleanupStack::PopAndDestroy( text );
        PrintListL( value );
        }
    else 
        {
        iLogBus.LogText( _L8("--- TODO: Unknown --- "));
        }
    }
#else
void ChspsLiwDump::PrintVariantL( TLiwVariant /*value*/ )
    {    
    }
#endif

//------------------------------------------------------------------------------
// ChspsLiwDump::PrintMapL
//------------------------------------------------------------------------------
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
void ChspsLiwDump::PrintMapL( TLiwVariant value )
    {
    //get map from the variant.
    //get map values.
    const CLiwMap* map = value.AsMap();
    TInt count = map->Count();
    for( TInt i = 0; i < count; i++ )
        {
        HBufC* index = GetFixedTextLC( _L("---"), iDepth, KDelim );
        iLogBus.LogText( *index );
        CleanupStack::PopAndDestroy( index );
        TBuf8<100> buf;
        map->AtL( i, buf);
        HBufC8* text = GetFixedText8LC( buf, iDepth, KDelim8 );
        iLogBus.LogText( *text );
        CleanupStack::PopAndDestroy( text );
        TLiwVariant value;
        map->FindL( buf, value );
        PrintVariantL( value );
        value.Reset();
        }
    }
#else
void ChspsLiwDump::PrintMapL( TLiwVariant /*value*/ )
    {
    }
#endif

//------------------------------------------------------------------------------
// ChspsLiwDump::PrintStringL
//------------------------------------------------------------------------------
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
void ChspsLiwDump::PrintStringL( TLiwVariant value )
    {
    TPtrC textValue = value.AsDes();
    HBufC* text = GetFixedTextLC( textValue, iDepth, KDelim );
    iLogBus.LogText( *text );
    CleanupStack::PopAndDestroy( text );
    }
#else
void ChspsLiwDump::PrintStringL( TLiwVariant /*value*/ )
    {    
    }
#endif

//------------------------------------------------------------------------------
// ChspsLiwDump::PrintString8L
//------------------------------------------------------------------------------
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
void ChspsLiwDump::PrintString8L( TLiwVariant value )
    {
    TPtrC8 textValue = value.AsData();
    HBufC8* text = GetFixedText8LC( textValue, iDepth, KDelim8 );
    iLogBus.LogText( *text );
    CleanupStack::PopAndDestroy( text );
    }
#else
void ChspsLiwDump::PrintString8L( TLiwVariant /*value*/ )
    {    
    }
#endif

//------------------------------------------------------------------------------
// ChspsLiwDump::PrintListL
//------------------------------------------------------------------------------
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
void ChspsLiwDump::PrintListL( TLiwVariant value )
    {
    iDepth++;
    const CLiwList* list = value.AsList();
    TInt count = list->Count();
    for( TInt i = 0; i < count; i++ )
        {
        HBufC* index = GetFixedTextLC( _L("---"), iDepth, KDelim );
        iLogBus.LogText( *index );
        CleanupStack::PopAndDestroy( index );
        TLiwVariant value;
        list->AtL( i, value );
        PrintVariantL( value );
        value.Reset();
        }
    iDepth--;
    }
#else
void ChspsLiwDump::PrintListL( TLiwVariant /*value*/ )
    {    
    }
#endif

//------------------------------------------------------------------------------
// ChspsLiwDump::GetFixedText8LC
//------------------------------------------------------------------------------
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
HBufC8* ChspsLiwDump::GetFixedText8LC( const TDesC8& aText, 
                                       const TInt aDepth, 
                                       const TDesC8& aDelim )
    {
    HBufC8 *buf = HBufC8::NewLC( aDepth * aDelim.Length() + aText.Length() + 1 );   
    TInt i = 0;
    for( ; i < aDepth; i++ )
        {
        buf->Des().Append( aDelim );
        }
    buf->Des().Append( aText );
    return buf;
    }
#else
HBufC8* ChspsLiwDump::GetFixedText8LC( const TDesC8& /*aText*/, 
                                       const TInt /*aDepth*/, 
                                       const TDesC8& /*aDelim*/ )
    {
    return NULL;
    }
#endif

//------------------------------------------------------------------------------
// ChspsLiwDump::GetFixedTextLC
//------------------------------------------------------------------------------
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
HBufC* ChspsLiwDump::GetFixedTextLC( const TDesC& aText, 
                                     const TInt aDepth, 
                                     const TDesC& aDelim )
    {
    HBufC *buf = HBufC::NewLC( aDepth * aDelim.Length() + aText.Length() + 1 ); 
    TInt i = 0;
    for( ; i < aDepth; i++ )
        {
        buf->Des().Append( aDelim );
        }
    buf->Des().Append( aText );
    return buf;
    }
#else
HBufC* ChspsLiwDump::GetFixedTextLC( const TDesC& /*aText*/, 
                                     const TInt /*aDepth*/, 
                                     const TDesC& /*aDelim*/ )
    {
    return NULL;
    }
#endif

//------------------------------------------------------------------------------
// ChspsLiwDump::CreateCPlusPlusDumpL
//------------------------------------------------------------------------------
#ifdef HSPS_BUILD_LOG_IMPLEMENTATION
HBufC* ChspsLiwDump::CreateCPlusPlusDumpL( ChspsByteBuffer& aBuffer ) const
    {
    if( aBuffer.Size() <= 0 )
        {
        User::Leave( KErrArgument );
        }

    if( aBuffer.Size() > KCPlusPlusArraySanityLimit )
        {
        User::Leave( KErrArgument );
        }
    
    // Calcute required space.
    const TInt KOneByteFootprint = 5; // hex is like 0x00 = 4 bytes, then trailing ',' for each one.   
    const TInt KRequiredSpace = KCPlusPlusArrayStart().Length() +                                
                                ( aBuffer.Size() * KOneByteFootprint ) +
                                KCPlusPlusArrayEnd().Length();                                       
    
    // Create descriptor.
    HBufC* descriptor = HBufC::NewLC( KRequiredSpace );
    TPtr16 pointer = descriptor->Des();
    pointer.Append( KCPlusPlusArrayStart );
    for( TInt i = 0; i < aBuffer.Size(); i++ )
        {
        TUint byte = ( TUint ) ( ( aBuffer.Buffer() )[i] );
        pointer.AppendFormat( KByteFormat, byte );
        
        // Add ',' for all but the last element.
        if( i != ( aBuffer.Size() - 1 ) )
            {
            pointer.Append( TChar( ',' ) );
            }
        }
    pointer.Append( KCPlusPlusArrayEnd );
    
    CleanupStack::Pop( descriptor );
    return descriptor;
    }
#else
HBufC* ChspsLiwDump::CreateCPlusPlusDumpL( ChspsByteBuffer& /*aBuffer*/ ) const
    {
    return NULL;
    }
#endif
