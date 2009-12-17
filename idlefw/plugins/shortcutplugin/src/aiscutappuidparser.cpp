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
* Description:  Shortcut definition parser
*
*/


#include "aiscutappuidparser.h"

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// TAiScutAppUidParser::TAiScutAppUidParser
// ---------------------------------------------------------------------------
//
TAiScutAppUidParser::TAiScutAppUidParser(const TDesC& aData, RArray<TUid>& aUidArray)
    :
    iUidArray(aUidArray),
    iLex(aData),
    iLexIsValid(EFalse)
{
}

// ---------------------------------------------------------------------------
// TAiScutAppUidParser::SkipChar
// ---------------------------------------------------------------------------
//
void TAiScutAppUidParser::SkipChar(TChar aChar, TBool aConditionalSkip)
{
    iLex.SkipSpaceAndMark();
    if (iLex.Peek() == aChar)
    {
        iLex.Inc();
    }
    else
    {
        // If not conditional skip, then the input data is invalid
        if (!aConditionalSkip)
        {
            iLexIsValid = EFalse;
        }
    }
    iLex.SkipSpaceAndMark();
}

// ---------------------------------------------------------------------------
// TAiScutAppUidParser::ReadAppUid
// ---------------------------------------------------------------------------
//
TUid TAiScutAppUidParser::ReadAppUid()
{
    TUint32 appUid;

    iLex.Mark();
    while (iLex.Peek().IsHexDigit())
    {
        iLex.Inc();
    }
    TPtrC uidToken = iLex.MarkedToken();
    TLex uidLex(uidToken);
    if (uidLex.Val(appUid, EHex) != KErrNone)
    {
        iLexIsValid = EFalse;
    }

    return TUid::Uid(appUid);
}

// ---------------------------------------------------------------------------
// TAiScutAppUidParser::ParseL
// ---------------------------------------------------------------------------
//
void TAiScutAppUidParser::ParseL()
{
    iLexIsValid = ETrue;

    while (!iLex.Eos() && iLexIsValid)
    {
        SkipChar(',', ETrue);
        TUid applicationUid = ReadAppUid();

        if (iLexIsValid)
        {
            iUidArray.AppendL(TUid(applicationUid));
        }
    }
}

// End of File
