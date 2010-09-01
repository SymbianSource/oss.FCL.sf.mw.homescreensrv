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


#ifndef _MT_HSPSLIWLISTPRINTER_H_
#define _MT_HSPSLIWLISTPRINTER_H_

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <liwservicehandler.h>

// CLASS DECLARATION

/**
 *  CHspsLiwListPrinter
 */
NONSHARABLE_CLASS( CHspsLiwListPrinter ) : public CBase
	{

// 
// Construction
//
	
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CHspsLiwListPrinter();

	/**
	 * Two-phased constructor.
	 */
	static CHspsLiwListPrinter* NewL();

	/**
	 * Two-phased constructor.
	 */
	static CHspsLiwListPrinter* NewLC();

private:
	
	/**
	 * Constructor for performing 1st stage construction
	 */
	CHspsLiwListPrinter();

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL();
	
//
// New methods
//
public:
	
	/**
	 * Prints LiwGenericParamList to a file.
	 * The defaultpath is c:\logs\ + additional folder defined by 
	 * KLogFolder.
	 * 
	 * @param aLiwList A list to be printed.
	 */
	void PrintLiwListL( CLiwGenericParamList& aLiwList );
	
private:	
	/**
	 * Parses the variant type.
	 * 
	 * @param value Variant to be printed.
	 */
	void PrintVariantL( TLiwVariant value );
	
	/**
	 * Prints liw map.
	 * 
	 *  @param value Map variant to be printed.
	 */
	void PrintMapL( TLiwVariant value );
	
	/**
	 * Prints liw string variant.
	 *
	 * @param value String variant to be printed.
	 */
	void PrintStringL( TLiwVariant value );
	
	/**
	 * Prints liw string variant. 8 bit variant.
	 *
	 * @param value String variant to be printed.
	 */
	void PrintString8L( TLiwVariant value );
	
	/**
	 * Prints liw list variant.
	 * 
	 * @param value List variant to be printed.
	 */
	void PrintListL( TLiwVariant value );
	
	/**
	 * Appends indentation (delimiters) to the line of text. 8 bit variant.
	 * 
	 * @param aText text to be printed.
	 * @param aDepth intendation depth.
	 * @param aDelim delimiter character or string.
	 */
	HBufC8* GetFixedText8LC( const TDesC8& aText, 
							const TInt aDepth, 
							const TDesC8& aDelim );
	
	/**
	 * Appends indentation (delimiters) to the line of text.
	 * 
	 * @param aText text to be printed.
	 * @param aDepth intendation depth.
	 * @param aDelim delimiter character or string.
	 */
	HBufC* GetFixedTextLC( const TDesC& aText, 
					       const TInt aDepth, 
						   const TDesC& aDelim );

//
// Attributes
//
	
private:
	/**
	 * Depth level of indentation.
	 */
	TInt iDepth;
	};

#endif // _MT_HSPSLIWLISTPRINTER_H_
