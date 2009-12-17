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
* Description:  AI Shortcut xSP Extension API
*
*/


#ifndef RAISCUTEXTSERV_H
#define RAISCUTEXTSERV_H

#include <e32std.h>
#include <bamdesca.h>

class CGulIcon;

/**
 *  Active Idle Shortcut Plugin extension client side API.
 *  @since S60 v3.2
 */
class RAiScutExtServ : public RSessionBase
{
    public:
        /**
         * Connects a client to the AiScut extension server.
         * Target definition is generated automatically from process' uid 3.
         * @return KErrNone, if successful, otherwise one of the other
         * system-wide error codes.
         */
        IMPORT_C TInt Connect();

        /**
         * Connects a client to the AiScut extension server.
         * @param aTargetDefinition Target definition string.
         * @return KErrNone, if successful, otherwise one of the other
         * system-wide error codes.
         */
        IMPORT_C TInt Connect( const TDesC& aTargetDefinition );

        /**
         * @return The client side version number.
         */
        IMPORT_C TVersion Version() const;

        /**
         * Updates a pop-up text box.
         * @param aPopupTextLines Array of lines displayed in pop-up text box.
         * @return KErrNone, if successful, otherwise one of the other system-wide error codes.
         */
        IMPORT_C TInt UpdatePopupTextL( const MDesCArray& aPopupTextLines );

        /**
         * Resets a pop-up text box.
         * @return KErrNone, if successful, otherwise one of the other system-wide error codes.
         */
        IMPORT_C TInt ResetPopupText();

        /**
         * Updates a shortcut icon.
         * @param aIcon Icon to be shown in shortcut.
         * @return KErrNone, if successful, otherwise one of the other system-wide error codes.
         */
        IMPORT_C TInt UpdateIconL( const CGulIcon& aIcon );

        /**
         * Resets a shortcut icon. Default icon will be shown.
         * @return KErrNone, if successful, otherwise one of the other system-wide error codes.
         */
        IMPORT_C TInt ResetIcon();

        /**
         * @return ETrue if the shortcut is in shortcuts bar.
         * @return KErrNone, if successful, otherwise one of the other system-wide error codes.
         */
        IMPORT_C TInt IsInShortcuts( TBool& aIsInShortcuts ) const;

        /**
         * Launches AiScutPlugin General Settings view.
         * @return KErrNone, if successful, otherwise one of the other system-wide error codes.
         */
        IMPORT_C TInt IssuePutInShortcuts();
};

#endif // RAISCUTEXTSERV_H

// End of File.
