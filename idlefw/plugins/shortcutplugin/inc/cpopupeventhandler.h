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
* Description:  Shortcut plug-in Popup event handler class
*
*/


#ifndef CPOPUPEVENTHANDLER_H
#define CPOPUPEVENTHANDLER_H

#include <e32base.h>
#include <badesca.h>

#include "PopupFSM.h"
#include "MPopupFSMActions.h"

class MPopupEventHandlerActions;

/**
 *
 *  The class handles Popup events (set, reset, gain/lost focus, etc.) by
 *  delegating them to automatically generated state machine class which in 
 *  turn decides what actions the events cause in different states.
 *
 *  This class has bi-directional friend relationship to CAiScutShortcutExt
 *  in order to establish concrete actions while decoupling state machine
 *  related functions out of CAiScutShortcutExt.
 *
 *  @since S60 v3.2
 */
class CPopupEventHandler : public CActive, 
    public MPopupFSMActions
    {
    public:
        /**
         * Factory function
         * @param aPopupEventHandlerActions actions interface
         * @since S60 v3.2
         */
        static CPopupEventHandler* NewL( 
            MPopupEventHandlerActions& aPopupEventHandlerActions );

        /**
         * Factory function
         * @param aPopupEventHandlerActions actions interface
         * @since S60 v3.2
         */
        static CPopupEventHandler* NewLC( 
            MPopupEventHandlerActions& aPopupEventHandlerActions );

        /**
         * Destructor
         * @since S60 v3.2
         */
        ~CPopupEventHandler();

    private:

        /**
         * Constructor
         * @param aPopupEventHandlerActions actions interface
         * @since S60 v3.2
         */
        CPopupEventHandler( 
            MPopupEventHandlerActions& aPopupEventHandlerActions );

        void ConstructL();

    public:  // New functions
        /**
         * Accessor for state machine
         * @return Reference to state machine instance
         * @since S60 v3.2
         */
        TPopupFSM& PopupFSM();

        TBool PublishPopup() const;
        TBool PopupVisible() const;
        
        /**
         * @return Is publishing needed
         */
        TBool PublishCaption() const;

        /**
         * @return Is caption visible
         */
        TBool CaptionVisible() const;
        
    private:    // From CActive
        void DoCancel();
        void RunL();

    private: // From MPopupFSMActions
        void CompleteSelf();
        void CancelRequest();
        void StartShortTimer();
        void StartLongTimer();
        void IssuePublishPopup();
        void IssueCleanPopup();
        void IssuePublishCaption();
        void IssueCleanCaption();

    private: // data
        /**
         * Actions interface
         * Ref.
         */
        MPopupEventHandlerActions& iPopupEventHandlerActions;

        /**
         * State machine instance
         * Own.
         */
        TPopupFSM iPopupFSM;

        /**
         * Timeout timer
         * Own.
         */
        RTimer iTimer;

        TBool iPublishPopup;
        TBool iPopupVisible;

        TBool iPublishCaption;
        TBool iCaptionVisible;
    };

#endif // CPOPUPEVENTHANDLER_H

// End of File.
