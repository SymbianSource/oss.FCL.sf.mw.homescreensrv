/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
#ifndef POPUPFSM_H
#define POPUPFSM_H

//  EXTERNAL INCLUDES
#include <e32base.h>

//  FORWARD DECLARATIONS
class TPopupFSM;
class MPopupFSMActions;

//  CLASS DEFINITIONS
/**
 * Popup state machine
 */
class TPopupFSM
    {
    /**
     * Base class for states
     */
    class TPopupStateBase
        {
        public:     // New methods
            virtual void HandleForeground( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            virtual void HandleUpdate( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            virtual void HandleRequestCompleted( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            virtual void HandleReset( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            virtual void HandleBackground( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            virtual void HandleGotFocus( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            virtual void HandleLostFocus( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
        };

    /**
     * State class TNotSetOffFocus
     */
    class TNotSetOffFocus : public TPopupStateBase
        {
        protected:  // Methods derived from TPopupState
            void HandleUpdate( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            void HandleGotFocus( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            void HandleForeground( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
        };

    /**
     * State class TSetOffFocus
     */
    class TSetOffFocus : public TPopupStateBase
        {
        protected:  // Methods derived from TPopupState
            void HandleGotFocus( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            void HandleReset( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
        };

    /**
     * State class TGettingFocus
     */
    class TGettingFocus : public TPopupStateBase
        {
        protected:  // Methods derived from TPopupState
            void HandleRequestCompleted( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            void HandleReset( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            void HandleLostFocus( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
        };

    /**
     * State class TLosingFocus
     */
    class TLosingFocus : public TPopupStateBase
        {
        protected:  // Methods derived from TPopupState
            void HandleRequestCompleted( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            void HandleReset( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            void HandleGotFocus( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
        };

    /**
     * State class TNotSetOnFocus
     */
    class TNotSetOnFocus : public TPopupStateBase
        {
        protected:  // Methods derived from TPopupState
            void HandleUpdate( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            void HandleLostFocus( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            void HandleBackground( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            void HandleForeground( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
        };

    /**
     * State class TVisible
     */
    class TVisible : public TPopupStateBase
        {
        protected:  // Methods derived from TPopupState
            void HandleUpdate( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            void HandleReset( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            void HandleLostFocus( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            void HandleRequestCompleted( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            void HandleBackground( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
        };

    /**
     * State class TNotVisible
     */
    class TNotVisible : public TPopupStateBase
        {
        protected:  // Methods derived from TPopupState
            void HandleUpdate( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            void HandleReset( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            void HandleLostFocus( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
        };

    /**
     * State class TBackgroundNotSetOnFocus
     */
    class TBackgroundNotSetOnFocus : public TPopupStateBase
        {
        protected:  // Methods derived from TPopupState
            void HandleForeground( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            void HandleUpdate( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
        };

    /**
     * State class TBackgroundSetOnFocus
     */
    class TBackgroundSetOnFocus : public TPopupStateBase
        {
        protected:  // Methods derived from TPopupState
            void HandleReset( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
            void HandleForeground( TPopupFSM* aPopupFSM, MPopupFSMActions& aPopupFSMActions );
        };


    public:    // Constructors
        TPopupFSM( MPopupFSMActions& aPopupFSMActions );

    public:     // New methods
        void HandleForeground();
        void HandleUpdate();
        void HandleRequestCompleted();
        void HandleReset();
        void HandleBackground();
        void HandleGotFocus();
        void HandleLostFocus();

    private:     // New methods
        void SetState( TPopupStateBase* aNewState );

    private:    // Data
        //Ref:
        TPopupStateBase* iCurrentState;
        MPopupFSMActions& iPopupFSMActions;

        //Own:
        TNotSetOffFocus iNotSetOffFocus;
        TSetOffFocus iSetOffFocus;
        TGettingFocus iGettingFocus;
        TLosingFocus iLosingFocus;
        TNotSetOnFocus iNotSetOnFocus;
        TVisible iVisible;
        TNotVisible iNotVisible;
        TBackgroundNotSetOnFocus iBackgroundNotSetOnFocus;
        TBackgroundSetOnFocus iBackgroundSetOnFocus;
    private:    // Friend class definitions
        friend class TNotSetOffFocus;
        friend class TSetOffFocus;
        friend class TGettingFocus;
        friend class TLosingFocus;
        friend class TNotSetOnFocus;
        friend class TVisible;
        friend class TNotVisible;
        friend class TBackgroundNotSetOnFocus;
        friend class TBackgroundSetOnFocus;
    };

#endif      //  POPUPFSM_H

// End of file
