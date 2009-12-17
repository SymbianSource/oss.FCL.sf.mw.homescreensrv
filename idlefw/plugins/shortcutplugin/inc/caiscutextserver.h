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
* Description:  Shortcut plug-in xSP extension server class.
*
*/


#ifndef CAISCUTEXTSERVER_H
#define CAISCUTEXTSERVER_H

#include <e32base.h>
#include <w32std.h>

#include "maiscutextmessagehandler.h"
#include "aicontentpublisher.h"

class CAiScutExtServer;
class CAknMessageQueryDialog;

/**
 *
 *  Helper class for passing parameters to the callback function.
 *
 *  @since S60 v3.2
 */
class CLinkParams : public CBase
    {
    public:

        CLinkParams(CAiScutExtServer& aServer, HBufC* aTargetDefinition);

        ~CLinkParams();

    public:

        /**
         * Reference to the server object.
         * Ref.
         */
        CAiScutExtServer& iServer;

        /**
         * Application identifier.
         * Own.
         */
        HBufC* iTargetDefinition;

        /**
         * The dialog that owns the link..
         * Ref.
         */
        CAknMessageQueryDialog* iNote;
    };

/**
 *  Server class for xSP extensions.
 *
 *  Handles sessions.
 *
 *  @since S60 v3.2
 */
class CAiScutExtServer : public CServer2
    {
    public:
        /**
         * Constructor
         * @param aObserver Reference to observer
         * @since S60 v3.2
         */
        CAiScutExtServer( MAiScutExtMessageHandler& aObserver, RWsSession& aWsSession );

        /**
         * 2nd-phase constructor
         * @since S60 v3.2
         */
        void ConstructL();

        /**
         * Destructor
         * @since S60 v3.2
         */
        ~CAiScutExtServer();

    private: // From CServer2

        CSession2* NewSessionL(
            const TVersion& aVersion, const RMessage2& aMessage ) const;

    private: // New methods

        void ReadAppListL();

        void WriteAppListL();

        static TInt LinkCallBack(TAny* aParam);

        static TInt IdleCallBack(TAny* aParam);

        void DoShowTipMessageL();

    public:

        TBool HasConnectedBefore( TUid& aUid, const RMessage2& aMessage );

        void ShowTipMessage( TUid aUid );

        void ResumeL( TAiTransitionReason aReason );

    private: // data

        /**
         * Reference to observer
         * Ref.
         */
        MAiScutExtMessageHandler& iObserver;

        /**
         * Reference to Window Server session
         * Ref.
         */
        RWsSession& iWsSession;

        /**
         * List of applications we are not going to
         * show the tip dialog.
         * Own.
         */
        RArray<TUid> iUids;

        /**
         * List of applications to show the dialog.
         *
         * Own.
         */
        RArray<TUid> iShowUids;

        /**
         * Resource offset for releasing the resoruce on destruction.
         *
         * Own.
         */
        TInt iResourceOffset;

        /**
         * CIdle for the tip dialog
         *
         * Own.
         */
        CIdle* iIdle;

        /**
         * Foreground/background indication
         *
         */
        TBool iIsForeground;

        /**
         * Tip message visible indication
         *
         */
        TBool iIsTipMessageVisible;
    };

#endif // CAISCUTEXTSERVER_H

// End of File.
