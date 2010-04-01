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
* Description:  Class for empty target.
*
*/


#include "hs_app_caiscuttargetempty.h"

// ----------------------------------------------------------------------------
// CAiScutEmpty::CAiScutEmpty
// ----------------------------------------------------------------------------
//
CAiScutTargetEmpty::CAiScutTargetEmpty(CAiScutEngine& aEngine, TShortcutType aType)
    : CAiScutTarget(aEngine, aType)
{
}

// ----------------------------------------------------------------------------
// CAiScutEmpty::ConstructL
// ----------------------------------------------------------------------------
//
void CAiScutTargetEmpty::ConstructL(const TDesC& aTarget)
{
    iDefinition = aTarget.AllocL();
}

// ----------------------------------------------------------------------------
// CAiScutEmpty::NewL
// ----------------------------------------------------------------------------
//
CAiScutTargetEmpty* CAiScutTargetEmpty::NewL(
    CAiScutEngine& aEngine, TShortcutType aType, const TDesC& aTarget)
{
    CAiScutTargetEmpty* self = new (ELeave) CAiScutTargetEmpty(aEngine, aType);

    CleanupStack::PushL(self);
    self->ConstructL(aTarget);
    CleanupStack::Pop(self);

    return self;
}

// ----------------------------------------------------------------------------
// CAiScutEmpty::~CAiScutEmpty
// ----------------------------------------------------------------------------
//
CAiScutTargetEmpty::~CAiScutTargetEmpty()
{
    delete iDefinition;
}

// ---------------------------------------------------------------------------
// Returns the shortcut definition string
// ---------------------------------------------------------------------------
//
TPtrC CAiScutTargetEmpty::Definition() const
{
    return iDefinition ? TPtrC(*iDefinition) : TPtrC();
}

// ---------------------------------------------------------------------------
// Returns the shortcut target caption.
// ---------------------------------------------------------------------------
//
TInt CAiScutTargetEmpty::GetCaption(TPtrC& aDes, TAiScutAppTitleType /*aTitleType*/) const
{
    aDes.Set(KNullDesC());
    return 0;
}

// ---------------------------------------------------------------------------
// Returns the shortcut target icon.
// ---------------------------------------------------------------------------
//
TInt CAiScutTargetEmpty::GetIcon(CGulIcon*& aIcon) const
{
    aIcon = NULL;
    return 0;
}

// -----------------------------------------------------------------------------
// Checks if the target is accessible
// -----------------------------------------------------------------------------
//
TBool CAiScutTargetEmpty::IsAccessibleL(TInt /*aCheckType*/)
{
    return (iType != EScutUnknown);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CAiScutTargetEmpty::LaunchL()
{
}

// ---------------------------------------------------------------------------
// Return application uid this target launches.
// ---------------------------------------------------------------------------
//
TUid CAiScutTargetEmpty::AppUid() const
{
    return KNullUid;
}

//  End of File.
