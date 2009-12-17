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
* Description:  Class for new message shortcut target.
*
*/


#ifndef CAISCUTTARGETNEWMSG_H
#define CAISCUTTARGETNEWMSG_H

#include <msvstd.h>

#include "caiscuttarget.h"

class CGulIcon;

/**
 *  Implements a new message or new email shortcut target
 *
 *  Handles parsing and launching a new message shortcut target
 *
 *  @since S60 v3.2
 */
class CAiScutTargetNewMsg : public CAiScutTarget
{

public:

    /**
     * First phase constructor
     *
     * @since S60 v3.2
     * @param aEngine Reference to the plug-in engine
     * @param aParser Shortcut definition parser
     */
    static CAiScutTargetNewMsg* NewL(
        CAiScutEngine& aEngine, TShortcutType aType, const TAiScutParser& aParser);

    virtual ~CAiScutTargetNewMsg();

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

    CAiScutTargetNewMsg(CAiScutEngine& aEngine, TShortcutType aType);

    void ConstructL(const TAiScutParser& aParser);

    /**
     *
     */
    void GetCaptionL(TAiScutAppTitleType aTitleType) const;

    /**
     * Leaving version of GetIcon.
     */
    void GetIconL(CGulIcon*& aIcon) const;
    
    void ShowErrorNote();

private:  // data

    /**
     * Application uid.
     */
    TUid                iAppUid;

    /**
     * Possible view uid.
     */
    TUid                iViewUid;

    /**
     * Shortcut caption.
     * Own.
     */
    mutable HBufC*      iCaption;

    /**
     * Shortcut short caption.
     * Own.
     */
    mutable HBufC*      iShortCaption;

    /**
     * Holds the shortcut definition string
     * Owned.
     */
    HBufC*              iDefinition;

    /**
     * Message type
     */
    TUid                iMtm;
};

#endif // CAISCUTTARGETNEWMSG_H

// End of File.
