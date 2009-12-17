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
* Description:  Shortcut plug-in shortcut extensions
*
*/


#ifndef CAISCUTSHORTCUTEXT_H
#define CAISCUTSHORTCUTEXT_H

#include <e32base.h>
#include <badesca.h>
#include <aicontentpublisher.h>

#include "caiscutshortcut.h"
#include "mpopupeventhandleractions.h"

class CPopupEventHandler;
class MAiScutExtData;

/**
 *
 *  The class extends CAiScutShortcut by offering means to store
 *  changeable popup text lines. The class overrides content publishing
 *  functions from base class.
 *
 *  @since S60 v3.2
 */
class CAiScutShortcutExt : public CAiScutShortcut,
    public MPopupEventHandlerActions
    {
    public:
        /**
         * Factory function
         * @see CAiScutShortcut
         * @since S60 v3.2
         */
        static CAiScutShortcutExt* NewL( TInt aId, const TDesC& aTarget,
            CAiScutEngine& aEngine );

        /**
         * Factory function
         * @see CAiScutShortcut
         * @since S60 v3.2
         */
        static CAiScutShortcutExt* NewLC( TInt aId, const TDesC& aTarget,
            CAiScutEngine& aEngine );

        /**
         * Destructor
         * @since S60 v3.2
         */
        ~CAiScutShortcutExt();

    private:
        CAiScutShortcutExt( TInt aId, CAiScutEngine& aEngine );
        void ConstructL( const TDesC& aTarget );

    public:  // New functions
        /**
         * @return Definition string of active target
         * @since S60 v3.2
         */
        TPtrC ActiveTargetDefinition() const;

        /**
         * Handles events (e.g. gain/lost focus) from AI framework
         * @param aEvent Event
         * @since S60 v3.2
         */
        void HandleAIEventL( TInt aEvent );

        /**
         * Handles Resumed event
         * @param aResumeReason Resume reason
         * @since S60 v3.2
         */
        void HandleResumed( TAiTransitionReason aResumeReason );

        /**
         * Set extension data
         * @param aAiScutExtData Pointer to new extension data
         * @since S60 v3.2
         */
        void SetAiScutExtData( const MAiScutExtData* aAiScutExtData );


    private: // From CAiScutShortcut
        void PublishL( MAiPropertyExtension& aPlugin, 
            MAiContentObserver& aObserver );

        TInt PublishCaption( MAiPropertyExtension& aPlugin, 
            MAiContentObserver& aObserver, TInt aCaptionContentId) const;

        void PublishPopupText( MAiPropertyExtension& aPlugin, 
            MAiContentObserver& aObserver ) const;
        
        TInt PublishIcon( MAiPropertyExtension& aPlugin,
            MAiContentObserver& aObserver, TInt aIconContentId );

    private:    // From MPopupEventHandlerActions
        void IssuePublishShortcut();
        
    private: // data

        /**
         * Extension data
         * Ref.
         */
        const MAiScutExtData* iAiScutExtData;
        
        /**
         * Flag that tells whether to publish or clean the popup text box.
         * The flag is controlled by iPopupEventHandler
         * Own.
         */
        //TBool iPublishLineArray;

        /**
         * Handler of various events, includes state machine
         * Own.
         */
        CPopupEventHandler* iPopupEventHandler;
        
        /**
         * Pointer to previous popup line array. This is used detect changes
         * in data.
         * Ref.
         */
        const MDesCArray* iPreviousPopupLineArray;

        /**
         * Pointer to previous icon. This is used detect changes
         * in data.
         * Ref.
         */
        const CGulIcon* iPreviousIcon;
        
        /**
         * Has the icon changed
         */
        TBool iIconChanged;
        
        /**
         * Has the text changed
         */
        TBool iTextChanged;

    };

#endif // CAISCUTSHORTCUTEXT_H

// End of File.
