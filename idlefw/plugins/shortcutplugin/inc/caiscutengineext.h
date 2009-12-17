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
* Description:  Shortcut plug-in engine extension
*
*/


#ifndef CAISCUTENGINEEXT_H
#define CAISCUTENGINEEXT_H

#include <e32base.h>

#include "caiscutengine.h"
#include "maiscutextmessagehandler.h"
#include "caiscutshortcutext.h"

class CAiScutPlugin;
class CAiScutExtServer;
class CAiScutExtDataModel;

/**
 *  The class extends CAiScutEngine by handling events from
 *  xSP extension server.
 *
 *  @since S60 v3.2
 */
class CAiScutEngineExt : public CAiScutEngine,
    public MAiScutExtMessageHandler
    {
    public:
        /**
         * Factory function
         * @see CAiScutEngine
         * @since S60 v3.2
         */
        static CAiScutEngineExt* NewL( CAiScutPlugin& aPlugin );

        /**
         * Destructor
         * @since S60 v3.2
         */
        ~CAiScutEngineExt();

    private:
        /**
         * Constructor
         * @see CAiScutEngine
         * @since S60 v3.2
         */
        CAiScutEngineExt( CAiScutPlugin& aPlugin );

        void ConstructL();

    public:  // New functions
        /**
         * Publishes specific shortcut
         * @since S60 v3.2
         */
        void CheckAccessAndPublish( CAiScutShortcut& aShortcut );

    private: // From CAiScutEngine
        void HandleAiEventL( TInt aEvent, const TDesC& aParam );
        void ResumeL( TBool aPublishAll, TAiTransitionReason aReason );

    private: // From MAiScutExtMessageHandler
        void HandleSetPopupLineArrayL( const TDesC& aDefinition, CDesCArray* aLineArray );
        void HandleResetPopupLineArrayL( const TDesC& aDefinition );
        void HandleSetIconL( const TDesC& aDefinition, CGulIcon* aIcon );
        void HandleResetIconL( const TDesC& aDefinition );
        TBool HandleIsInShortcutsL( const TDesC& aDefinition ) const;
        void HandleIssuePutInShortcutsL( const TDesC& aDefinition );

    private: // New functions
        /**
         * Finds a shortcut of which id matches with given.
         * @param aId Target id in hexadecimal string format
         * @return Pointer to shortcut or NULL if not found
         */
        CAiScutShortcutExt* FindShortcutById( const TDesC& aId ) const;

        /**
         * Iterates shortcuts and sets new extension data to matching
         * ones.
         * @param aDefinition Definition
         * @param aAiScutExtData Pointer to extension data or NULL
         * @return ETrue if matching shortcut(s) found
         */
        TBool PopulateExtData( const TDesC& aDefinition,
            const MAiScutExtData* aAiScutExtData );

    private: // data
        /**
         * xSP extension server instance
         * Own.
         */
        CAiScutExtServer* iAiScutExtServer;

        /**
         * Extension data model
         * Own.
         */
        CAiScutExtDataModel* iExtDataModel;
    };

#endif // CAISCUTENGINEEXT_H

// End of File.
