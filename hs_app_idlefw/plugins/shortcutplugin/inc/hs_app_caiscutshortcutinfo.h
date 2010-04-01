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
* Description:  Small container for shortcut info.
*
*/


#ifndef CAISCUTSHORTCUTINFO_H
#define CAISCUTSHORTCUTINFO_H

#include <e32base.h>

/**
 *  Implementation for a shortcut
 *
 *  Handles parsing and launching an application shortcut
 *
 *  @since S60 v3.2
 */
class CAiScutShortcutInfo : public CBase
{

public:

    /**
     * First phase constructor
     *
     * @since S60 v3.2
     * @param aId Shortcut id
     * @param aTarget The shortcut definition string
     */
    static CAiScutShortcutInfo* NewL(TInt aId, const TDesC& aTarget);

    virtual ~CAiScutShortcutInfo();

    /**
     * Returns the shortcut id
     *
     * @since S60 v3.2
     * @return Shortcut id
     */
    TInt32 Id() const;

    /**
     * Returns the shortcut target string.
     *
     * @since S60 v3.2
     * @return Shortcut target string
     */
    const TDesC& Target();
    
protected:

private:

    CAiScutShortcutInfo(TInt aId);

    void ConstructL(const TDesC& aTarget);

private:  // data

    /**
     * Shortcut id.
     */
    TInt32          iId;

    /**
     * Shortcut target string.
     * Own.
     */
    HBufC*  iTarget;

};

#endif // CAISCUTSHORTCUTINFO_H

// End of File.
