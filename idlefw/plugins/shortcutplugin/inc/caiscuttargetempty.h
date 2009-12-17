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
* Description:  Empty target
*
*/


#ifndef CAISCUTEMPTY_H
#define CAISCUTEMPTY_H

#include "caiscuttarget.h"

/**
 *  Empty target.
 *
 *  @since S60 v3.2
 */
class CAiScutTargetEmpty : public CAiScutTarget
{
public:     // Constructors and destructor

    /**
     * Two-phased constructor.
     * @param
     * @return new instance.
     */
    static CAiScutTargetEmpty* NewL(
        CAiScutEngine& aEngine, TShortcutType aType, const TDesC& aTarget );

    /**
     * Destructor.
     */
    virtual ~CAiScutTargetEmpty();

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

private: // New methods

    /**
     * C++ default constructor.
     */
    CAiScutTargetEmpty( CAiScutEngine& aEngine, TShortcutType aType );

    /**
     * By default Symbian OS constructor is private.
     */
    void ConstructL( const TDesC& aTarget );

private:  // Data

    /**
     * Holds the shortcut definition string
     * Owned.
     */
    HBufC*              iDefinition;

};

#endif

// End of File.
