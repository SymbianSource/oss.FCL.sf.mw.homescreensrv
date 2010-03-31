/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef C_STRPARSER_H
#define C_STRPARSER_H

#include <e32base.h>
#include <utf.h>
#include "aistrparser.h"

// CLASS DECLARATION

/**
*  @ingroup group_aiutils
* 
*  String Parser
*
*  @since S60 3.2
*/
NONSHARABLE_CLASS(CStrParser) : 
    public CBase, public MAiStrParser
	{
	
public:
	
    static CStrParser* NewL();
    
private:

    CStrParser();
    
    void ConstructL();
    
    void Release();
    	  
	TInt ParseInt( TInt32& aResult, const TDesC8& aSourceString );

	HBufC16* CopyToBufferL( HBufC16* aTargetBuffer, const TDesC16& aSourceText );

	HBufC16* CopyToBufferL( HBufC16* aTargetBuffer, const TDesC8& aSourceText );

	HBufC8* CopyToBufferL( HBufC8* aTargetBuffer, const TDesC8& aSourceText );

	HBufC8* CopyToBufferL( HBufC8* aTargetBuffer, const TDesC16& aSourceText );

    };

#endif // C_STRPARSER_H





