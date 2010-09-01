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
* Description:  Implementation for XCFWEntityConverter
*
*/


#include <f32file.h>
#include <stringpool.h>

#include <gmxmlconstants.h>
#include <gmxmlcomposer.h>
#include "xcfwentityconverter.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXCFWEntityConverter::CXCFWEntityConverter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CXCFWEntityConverter::CXCFWEntityConverter()
	{
	}

EXPORT_C CXCFWEntityConverter::~CXCFWEntityConverter()
	{
	}

// -----------------------------------------------------------------------------
// CXCFWEntityConverter::OutputComposedTextL
// Function outputs the given text "as is" to CMDXMLComposer
// This is needed, because the default entity converter from GMXML would
// otherwise turn all & characters into &amp; entities.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CXCFWEntityConverter::OutputComposedTextL( 
	CMDXMLComposer* aComposer, 
	const TDesC& aTextToConvert)
	{
    aComposer->OutputDataL( aTextToConvert );
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CXCFWEntityConverter::DTDEntityToText
// Just return KErrNone, dtd=>text conversion is done during DOM=>tree parsing
// -----------------------------------------------------------------------------
//
TInt CXCFWEntityConverter::DTDEntityToText(TDes& /*aTextToConvert*/)
    {
    return KErrNone;        
    }


// End Of File
