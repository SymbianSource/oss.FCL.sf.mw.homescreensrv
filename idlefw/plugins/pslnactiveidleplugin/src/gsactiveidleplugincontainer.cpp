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


#include "gsactiveidleplugincontainer.h"

// ---------------------------------------------------------------------------
// CGSActiveIdlePluginContainer::ConstructL()
//
// Symbian OS two phased constructor.
// ---------------------------------------------------------------------------
//
void CGSActiveIdlePluginContainer::ConstructL(const TRect& aRect)
{
    CreateWindowL(); // Makes the control a window-owning control
    SetRect(aRect);
    ActivateL();
}

// ---------------------------------------------------------------------------
// CGSActiveIdlePluginContainer::~CGSActiveIdlePluginContainer()
//
// Destructor.
// ---------------------------------------------------------------------------
//
CGSActiveIdlePluginContainer::~CGSActiveIdlePluginContainer()
{
}

CGSActiveIdlePluginContainer::CGSActiveIdlePluginContainer()
{
}

void CGSActiveIdlePluginContainer::SizeChanged()
{
}

TInt CGSActiveIdlePluginContainer::CountComponentControls() const
{
    return 0;
}

CCoeControl* CGSActiveIdlePluginContainer::ComponentControl(TInt /*aIndex*/ ) const
{
    return NULL;
}

// End of File.
