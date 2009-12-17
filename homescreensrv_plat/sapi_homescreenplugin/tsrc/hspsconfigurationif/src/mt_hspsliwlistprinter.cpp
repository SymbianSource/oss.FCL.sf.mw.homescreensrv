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
* Description:  Prints liw list to a file.
*
*/


#include <liwcommon.h>
#include <flogger.h>

#include "mt_hspsliwlistprinter.h"


// File logging macros
// filename
_LIT(KLog,"printliw.log");
// additional path
_LIT(KLogFolder,"hspsprintdom");
#define _LOGT(aDescription) RFileLogger::Write( KLogFolder,	KLog, EFileLoggingModeOverwrite, aDescription );    
#define _LOGTFRM1(a,b) RFileLogger::WriteFormat( KLogFolder, KLog, EFileLoggingModeOverwrite, (a), (b) ) 
#define _LOGTFRM2(a,b,c) RFileLogger::WriteFormat( KLogFolder, KLog, EFileLoggingModeOverwrite, (a),(b),(c) ); 
#define _LOGTFRM3(a,b,c,d) RFileLogger::WriteFormat( KLogFolder, KLog, EFileLoggingModeOverwrite, (a),(b),(c),(d) ); 
// indentation delimiter
_LIT8(KDelim8, "\t" );
_LIT(KDelim, "\t" );

//------------------------------------------------------------------------------
// Constuctor
//------------------------------------------------------------------------------
CHspsLiwListPrinter::CHspsLiwListPrinter()
	{
	iDepth = 0;
	}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
CHspsLiwListPrinter::~CHspsLiwListPrinter()
	{
	}

//------------------------------------------------------------------------------
// Symbian constructor
//------------------------------------------------------------------------------
CHspsLiwListPrinter* CHspsLiwListPrinter::NewLC()
	{
	CHspsLiwListPrinter* self = new (ELeave)CHspsLiwListPrinter();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

//------------------------------------------------------------------------------
// Symbian constructor
//------------------------------------------------------------------------------
CHspsLiwListPrinter* CHspsLiwListPrinter::NewL()
	{
	CHspsLiwListPrinter* self=CHspsLiwListPrinter::NewLC();
	CleanupStack::Pop( self ); // self;
	return self;
	}

//------------------------------------------------------------------------------
// 2nd tier constructor
//------------------------------------------------------------------------------
void CHspsLiwListPrinter::ConstructL()
	{
	}

//------------------------------------------------------------------------------
// Prints liw list to a file.
//------------------------------------------------------------------------------
void CHspsLiwListPrinter::PrintLiwListL( CLiwGenericParamList& aLiwList )
	{
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
		_LOGTFRM1(_L8(" SemanticId: %d"), identifier );
		if( identifier == LIW::EGenericParamError )
			{
			_LOGT(_L8(" ERROR: EGenericParamError") );
			}
		//get generic param name.
		//print name.
		_LOGT( param.Name() );
		//get Variant
		TLiwVariant value = param.Value();
		//get variant type.
		PrintVariantL( value );
		}
	}

//------------------------------------------------------------------------------
// Parses variant type for printing.
//------------------------------------------------------------------------------
void CHspsLiwListPrinter::PrintVariantL( TLiwVariant value )
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
		_LOGT( *text );
		CleanupStack::PopAndDestroy( text );
		PrintMapL( value );
		}
	else if ( variantId == LIW::EVariantTypeList )
		{
		HBufC* text = GetFixedTextLC( _L("<List>"), iDepth, KDelim );
		_LOGT( *text );
		CleanupStack::PopAndDestroy( text );
		PrintListL( value );
		}
	else 
		{
		_LOGT(_L8("--- TODO: Unknown --- "));
		}
	}

//------------------------------------------------------------------------------
// Prints map type variant.
//------------------------------------------------------------------------------
void CHspsLiwListPrinter::PrintMapL( TLiwVariant value )
	{
	//get map from the variant.
	//get map values.
	const CLiwMap* map = value.AsMap();
	TInt count = map->Count();
	for( TInt i = 0; i < count; i++ )
		{
		HBufC* index = GetFixedTextLC( _L("---"), iDepth, KDelim );
		_LOGT( *index );
		CleanupStack::PopAndDestroy( index );
		TBuf8<100> buf; //TODO: bad solution...
		map->AtL( i, buf);
		HBufC8* text = GetFixedText8LC( buf, iDepth, KDelim8 );
		_LOGT( *text );
		CleanupStack::PopAndDestroy( text );
		TLiwVariant value;
		map->FindL( buf, value );
		PrintVariantL( value );
		value.Reset();
		}
	}

//------------------------------------------------------------------------------
// Prints string type variant.
//------------------------------------------------------------------------------
void CHspsLiwListPrinter::PrintStringL( TLiwVariant value )
	{
	TPtrC textValue = value.AsDes();
	HBufC* text = GetFixedTextLC( textValue, iDepth, KDelim );
	_LOGT( *text );
	CleanupStack::PopAndDestroy( text );
	}

//------------------------------------------------------------------------------
// Prints string type variant.
//------------------------------------------------------------------------------
void CHspsLiwListPrinter::PrintString8L( TLiwVariant value )
	{
	TPtrC8 textValue = value.AsData();
	HBufC8* text = GetFixedText8LC( textValue, iDepth, KDelim8 );
	_LOGT( *text );
	CleanupStack::PopAndDestroy( text );
	}

//------------------------------------------------------------------------------
// Prints list type variant.
//------------------------------------------------------------------------------
void CHspsLiwListPrinter::PrintListL( TLiwVariant value )
	{
	iDepth++;
	const CLiwList* list = value.AsList();
	TInt count = list->Count();
	for( TInt i = 0; i < count; i++ )
		{
		HBufC* index = GetFixedTextLC( _L("---"), iDepth, KDelim );
		_LOGT( *index );
		CleanupStack::PopAndDestroy( index );
		TLiwVariant value;
		list->AtL( i, value );
		PrintVariantL( value );
		value.Reset();
		}
	iDepth--;
	}

//------------------------------------------------------------------------------
// Utility method for generating a custom text descriptor.
// Adds indentation to a text string. 8 bit variant.
//------------------------------------------------------------------------------
HBufC8* CHspsLiwListPrinter::GetFixedText8LC( const TDesC8& aText, 
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

//------------------------------------------------------------------------------
// Debug utility for generating a custom text descriptor.
// Adds indentation to a text string. 8 bit variant.
//------------------------------------------------------------------------------
HBufC* CHspsLiwListPrinter::GetFixedTextLC( const TDesC& aText, 
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
