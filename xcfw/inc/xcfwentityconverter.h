/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class definitions for XCFW Entity Converter
*
*/


#ifndef __XCFWENTITYCONVERTER_H__
#define __XCFWENTITYCONVERTER_H__

#include <e32std.h>
#include <eikenv.h>
#include <gmxmlentityconverter.h>

// CLASS DECLARATION
/**
* Entity converter dummy implementation for GMXML parser. XCFW does the
* localization related entity conversion during DOM to XCFW tree parsing,
* this class is needed only because GMXML would otherwise turn any & character
* it finds into a &amp; entity.
*
* @lib XCFW.lib
* @since Series 60 3.1
*/
class CXCFWEntityConverter: public CMDXMLEntityConverter
	{
public:
	/**
	 * Constructor
	 */
	IMPORT_C CXCFWEntityConverter();

	/** Destructor. */
	IMPORT_C virtual ~CXCFWEntityConverter();

	/**
	 * From CMDXMLEntityConverter
	 * Outputs a block of text to a composer with offending characters replaced by
	 * entity references.
	 * @param aComposer the composer to be used for output
	 * @param aTextToConvert The text to be converted.
	 * @return Returns KErrNone if succcessful or a file write error
	 * @exception can Leave due to OOM
	 */
	IMPORT_C TInt OutputComposedTextL( CMDXMLComposer* aComposer, const TDesC& aTextToConvert );

protected:
	/**
	 * From CMDXMLEntityConverter
	 * DTD Specific entity to text converter
	 * @param aTextToConvert Text to be converted - replacement text goes
	 * out in the same
	 * @return Returns KErrNone if successful or no entity found
	 * @return Returns KErrXMLBadEntity if malformed entity found
	 */
	TInt DTDEntityToText(TDes& aTextToConvert);

	};


#endif
