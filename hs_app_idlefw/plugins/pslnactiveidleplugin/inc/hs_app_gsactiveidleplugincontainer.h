/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Psln Active Idle settings plugin.
*
*/


#ifndef C_GSACTIVEIDLEPLUGINCONTAINER_H
#define C_GSACTIVEIDLEPLUGINCONTAINER_H

#include <coecntrl.h>

/**
 *  CGSActiveIdlePluginContainer container class
 *
 *  @since S60 3.2
 */
class CGSActiveIdlePluginContainer : public CCoeControl
{
public: // Constructors and destructor

    /**
     * Symbian OS constructor.
     * @param aRect Listbox's rect.
     *
     */
    void ConstructL(const TRect& aRect);

    ~CGSActiveIdlePluginContainer();

    CGSActiveIdlePluginContainer();

// From CCoeControl

    TInt CountComponentControls() const;

    CCoeControl* ComponentControl(TInt aIndex) const;

    void SizeChanged();

};

#endif // C_GSACTIVEIDLEPLUGINCONTAINER_H

// End of File.
