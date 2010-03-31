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
* Description:  Shortcut plug-in xSP extension server message handler.
*
*/


#ifndef MAISCUTEXTMESSAGEHANDLER_H
#define MAISCUTEXTMESSAGEHANDLER_H

#include <e32base.h>

class CGulIcon;

/**
 *
 *  Shortcut plug-in xSP extension server message handler.
 *
 *  @since S60 v3.2
 */
class MAiScutExtMessageHandler
    {
    public:
        /**
         * Notifies that the popup text has been changed.
         * @param aDefinition Target shortcut definition
         * @param aLineArray Pointer to array of popup line texts. Ownership
         * is transferred.
         * @since S60 v3.2
         */
        virtual void HandleSetPopupLineArrayL( const TDesC& aDefinition,
            CDesCArray* aLineArray ) = 0;
        /**
         * Notifies that the popup text has been reseted.
         * @param aDefinition Target shortcut definition
         * @since S60 v3.2
         */
        virtual void HandleResetPopupLineArrayL( const TDesC& aDefinition ) = 0;

        /**
         * Notifies that the ion has been changed.
         * @param aDefinition Target shortcut definition
         * @param aIcon Pointer to new icon. Ownership
         * is transferred.
         * @since S60 v3.2
         */
        virtual void HandleSetIconL( const TDesC& aDefinition,
            CGulIcon* aIcon ) = 0;

        /**
         * Notifies that the icon has been reseted.
         * @param aDefinition Target shortcut definition
         * @since S60 v3.2
         */
        virtual void HandleResetIconL( const TDesC& aDefinition ) = 0;
        
        /**
         * Checks whether the target is in shortcuts
         * @param aDefinition Target shortcut definition
         * @return ETrue if the target is in shortcuts
         * @since S60 v3.2
         */
        virtual TBool HandleIsInShortcutsL( const TDesC& aDefinition ) const = 0;
        
        /**
         * Launches General Settings so that the target definition is passed in.
         * @param aDefinition Target shortcut definition
         * @since S60 v3.2
         */
        virtual void HandleIssuePutInShortcutsL( const TDesC& aDefinition ) = 0;
    };

#endif // MAISCUTEXTMESSAGEHANDLER_H

// End of File.
