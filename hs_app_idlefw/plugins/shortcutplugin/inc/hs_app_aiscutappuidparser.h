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
* Description:  Shortcut definition parser.
*
*/


#ifndef AISCUTAPPUIDPARSER_H
#define AISCUTAPPUIDPARSER_H

// INCLUDES
#include <e32base.h>

// CLASS DECLARATIONS

/**
 * Parser class that interpretes the given text as a description of
 * application position data.
 */
class TAiScutAppUidParser
{
public: // Construction

    /**
     * Standard C++ constructor.
     * @param aData Data which will be parsed
     * @param aAppPositions This array will be filled with parsed
     *                      application positions.
     */
    TAiScutAppUidParser(const TDesC& aData, RArray<TUid>& aAppPositions);

public: // Interface

    /**
     * Parses the data that was given in constructor and fills the
     * application position array with parsed data.
     */
    void ParseL();

private: // Implementation
    /**
     * Skips the given char.
     * Skipping can be done conditionally or unconditionally.
     * If conditional skipping is used, next character in data will be
     * skipped only if it is the same as given character.
     * If unconditional skipping is used the next character in data must
     * be the given character, otherwise the data is invalid and parsing
     * will be stopped.
     * @param aChar Character that will be skipped.
     * @param aConditionalSkip If ETrue, use conditional skipping,
     *                         otherwise unconditional skipping will be
     *                         used.
     */
    void SkipChar(TChar aChar, TBool aConditionalSkip);

    /**
     * Reads app UID from data. If data doesn't contain app UID in the
     * point in which this function is called, data is considered invalid.
     * @return App UID that was read from data.
     */
    TUid ReadAppUid();

private:

    /// Ref: Application positions array that will be filled when parsing
    RArray<TUid>&   iUidArray;

    /// Lexer for data
    TLex            iLex;

    /// Is the data that has been read so far valid.
    TBool           iLexIsValid;
};

#endif // AISCUTAPPUIDPARSER_H

// End of File
