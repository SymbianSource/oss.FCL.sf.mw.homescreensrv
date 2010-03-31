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
* Description:  Class for bookmark shortcut target.
*
*/


#ifndef CAISCUTTARGETBKM_H
#define CAISCUTTARGETBKM_H

#include "hs_app_caiscuttarget.h"

/**
 *  Implements an bookmark shortcut target
 *
 *  Handles parsing and launching an bookmark shortcut target
 *
 *  @since S60 v3.2
 */
class CAiScutTargetBkm : public CAiScutTarget
{

public:

    /**
     * First phase constructor
     *
     * @since S60 v3.2
     * @param aEngine Reference to the plug-in engine
     * @param aParser Shortcut definition parser
     */
    static CAiScutTargetBkm* NewL(
        CAiScutEngine& aEngine, TShortcutType aType, const TAiScutParser& aParser);

    virtual ~CAiScutTargetBkm();

    /**
     * Returns the shortcut definition string
     *
     * @since S60 v3.2
     * @return Shortcut definition string
     */
    TPtrC Definition() const;

    /**
     * Returns the shortcut target caption.
     * Caption can be either a descriptor or a resource id.
     * If the return value has a non-zero value, it is the resource id.
     * Zero indicates that the descriptor reference parameter is used.
     * Default implementation returns KErrNotSupported;
     *
     * Getting the target caption and icon utilizes the "lazy evaluation" idiom
     * which means they are created only when they are first needed. This is because
     * there are (atleast) two scenarios where they are never needed:
     * 1. The active theme does not support shortcut captions or icons.
     * 2. Shortcuts launched using LaunchByValue are never published.
     *
     * @since S60 v3.2
     * @param aDes On return, the descriptor if used
     * @return The resource id or zero to indicate that the descriptor is used.
     */
    TInt GetCaption(TPtrC& aDes, TAiScutAppTitleType aTitleType) const;

    /**
     * Returns the shortcut target icon.
     * Icon can be either an icon pointer or a resource id.
     * If the return value has a non-zero value, it is the resource id.
     * Zero indicates that the icon pointer is used.
     * Default implementation returns KErrNotSupported;
     *
     * Uses the "lazy evaluation" idiom to get the icon. See Caption()
     *
     * @since S60 v3.2
     * @param aIcon On return, the icon pointer if used
     * @return The resource id or zero to indicate that the icon pointer is used.
     */
    TInt GetIcon(CGulIcon*& aIcon) const;

    /**
     * Checks if the shortcut target is accessible
     *
     * @since S60 v3.2
     * @param aCheckType Specifies which kind of shortcuts should check its access
     * @return ETrue if accessible, EFalse if not
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
    
    /**
     * Return an additional id this target may use when 
     * launching. Default implementation return TUid(-1)
     *
     * @since S60 v3.2
     */
    TUid AdditionalUid() const;

protected:

private:

    CAiScutTargetBkm(CAiScutEngine& aEngine, TShortcutType aType);

    void ConstructL(const TAiScutParser& aParser);

    /**
     *
     */
    void GetCaptionL() const;

private:  // data

    /**
     * Bookmark uid.
     */
    TUid                iBkmUid;

    /**
     * Shortcut caption.
     * Own.
     */
    mutable HBufC*      iCaption;

    /**
     * Holds the shortcut definition string.
     * Own.
     */
    HBufC*              iDefinition;

};

#endif // CAISCUTTARGETAPP_H

// End of File.
