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


#include "caiscutshortcutinfo.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutShortcutInfo::CAiScutShortcutInfo(TInt aId)
    : iId(aId)
{
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
void CAiScutShortcutInfo::ConstructL(const TDesC& aTarget)
{
    iTarget = aTarget.AllocL();
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutShortcutInfo* CAiScutShortcutInfo::NewL(TInt aId, const TDesC& aTarget)
{
    CAiScutShortcutInfo* self = new(ELeave) CAiScutShortcutInfo(aId);
    CleanupStack::PushL(self);
    self->ConstructL(aTarget);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
CAiScutShortcutInfo::~CAiScutShortcutInfo()
{
    delete iTarget;
}

// -----------------------------------------------------------------------------
// Returns the shortcut id.
// -----------------------------------------------------------------------------
//
TInt32 CAiScutShortcutInfo::Id() const
{
    return iId;
}

// -----------------------------------------------------------------------------
// Returns the shortcut target string.
// -----------------------------------------------------------------------------
//
const TDesC& CAiScutShortcutInfo::Target()
    {
    if( iTarget )
        return *iTarget;
    else
        return KNullDesC;
    }

// Eof
