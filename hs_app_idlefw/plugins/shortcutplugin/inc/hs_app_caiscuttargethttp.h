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
* Description:  Class for http shortcut target.
*
*/


#ifndef CAISCUTTARGETHTTP_H
#define CAISCUTTARGETHTTP_H

#include "hs_app_caiscuttarget.h"

/**
 * Command line parameter for browser to open an url
 */
_LIT( KOpenUrlParam, "4 " );

/**
 *  Implements an http shortcut target
 *
 *  Handles parsing and launching an http shortcut target
 *
 *  @since S60 v3.2
 */
class CAiScutTargetHttp : public CAiScutTarget
{

public:

    /**
     * First phase constructor
     *
     * @since S60 v3.2
     * @param aEngine Reference to the plug-in engine
     * @param aTarget The shortcut definition string
     */
    static CAiScutTargetHttp* NewL(
        CAiScutEngine& aEngine, TShortcutType aType, const TAiScutParser& aParser );

    virtual ~CAiScutTargetHttp();

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

    /**
     * Return additional UID this target refers to. In 
     * URL case this is a unique identifer created from the URL
     * 
     * @since S60 v3.2
     */
    TUid AdditionalUid() const;
protected:

private:

    CAiScutTargetHttp( CAiScutEngine& aEngine, TShortcutType aType );

    void ConstructL( const TAiScutParser& aParser );

private:  // data

    /**
     * Target caption, part of the http address or
     * custom title given with the URL string
     */
    mutable HBufC*       iCaption;

    /**
     * Holds the shortcut definition string
     * Owned.
     */
    HBufC*              iDefinition;
    
    /**
     * Checksum for the URL
     */
    TUid                iChecksum;

};

#endif // CAISCUTTARGETHTTP_H

// End of File.
