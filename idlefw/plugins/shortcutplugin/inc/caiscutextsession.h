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
* Description:  Shortcut plug-in xSP extension session class.
*
*/


#ifndef CAISCUTEXTSESSION_H
#define CAISCUTEXTSESSION_H

#include <e32base.h>

class MAiScutExtMessageHandler;

/**
 *  Server session class for xSP extensions.
 *
 *  Handles IPC between server and client. Commands and data are passed to
 *  observer.
 *
 *  @since S60 v3.2
 */
class CAiScutExtSession : public CSession2
    {
    public:
        /**
         * Constructor
         * @param aObserver Reference to observer
         * @since S60 v3.2
         */
        CAiScutExtSession( MAiScutExtMessageHandler& aObserver );

        /**
         * Destructor
         * @since S60 v3.2
         */
        ~CAiScutExtSession();

    private: // From CSession2
        void ServiceL( const RMessage2& aMessage );
        void DispatchMessageL( const RMessage2& aMessage );

    private: // New functions
        void SetTargetDefinitionL( const RMessage2& aMessage );
        void SetPopupTextL( const RMessage2& aMessage );
        void ResetPopupTextL( const RMessage2& aMessage );
        void SetIconL( const RMessage2& aMessage );
        void ResetIconL( const RMessage2& aMessage );
        void IsInShortcutsL( const RMessage2& aMessage );
        void IssuePutInShortcutsL( const RMessage2& aMessage );

        void PanicClient( const RMessage2& aMessage, TInt aPanicCode ) const;

    private: // data

        /**
         * Reference to observer
         * Ref.
         */
        MAiScutExtMessageHandler& iObserver;
        
        /**
         * Target definition
         * Own.
         */
        HBufC* iTargetDefinition;
    };

#endif // CAISCUTEXTSESSION_H

// End of File.
