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
* Description:  Class for messaging view shortcut target.
*
*/


#ifndef CAISCUTTARGETMESSAGINGVIEW_H
#define CAISCUTTARGETMESSAGINGVIEW_H

#include <msvstd.h>

#include "caiscuttarget.h"

class CGulIcon;

/**
 *  Implements a mailbox shortcut target
 *
 *  Handles parsing and launching a mailbox shortcut target
 *
 *  @since S60 v3.2
 */
class CAiScutTargetMessagingView : public CAiScutTarget
{

public:

    /**
     * First phase constructor
     *
     * @since S60 v3.2
     * @param aEngine Reference to the plug-in engine
     * @param aParser Shortcut definition parser
     */
    static CAiScutTargetMessagingView* NewL(
        CAiScutEngine& aEngine, TShortcutType aType, const TAiScutParser& aParser);

    virtual ~CAiScutTargetMessagingView();

    /**
     * Returns the shortcut definition string
     *
     * @since S60 v3.2
     * @return Shortcut definition string
     */
    TPtrC Definition() const;

    /**
     * See base class
     */
    TInt GetCaption(TPtrC& aDes, TAiScutAppTitleType aTitleType) const;

    /**
	 * See base class
     */
    TInt GetIcon(CGulIcon*& aIcon) const;

    /**
     * See base class
     */
    TBool IsAccessibleL(TInt aCheckType);

    /**
     * Launches the shortcut
     *
     * @since S60 v3.2
     */
    void LaunchL();

    /**
     * Return application uid this target launches.
     *
     * @since S60 v3.2
     */
    TUid AppUid() const;

protected:

private:

    CAiScutTargetMessagingView(CAiScutEngine& aEngine, TShortcutType aType);

    void ConstructL(const TAiScutParser& aParser);

    /**
     * Tries to find a view id
     *
     * @since S60 v3.2
     * @return Mailbox id
     */
    TMsvId FindViewIdL();

    /**
     * Leaving version of GetIcon.
     */
    void GetIconL(CGulIcon*& aIcon) const;

private:  // data

    /**
     * View name. Contains the remote mailbox name or the messaging view name
     * Own.
     */
    HBufC* iViewName;

    /**
     * Holds the shortcut definition string
     * Owned.
     */
    HBufC* iDefinition;

};

#endif // CAISCUTTARGETMESSAGINGVIEW_H

// End of File.
