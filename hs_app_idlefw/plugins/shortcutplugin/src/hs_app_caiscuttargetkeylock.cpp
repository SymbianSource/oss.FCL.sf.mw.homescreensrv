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
* Description:  Class for key lock
*
*/


#include "hs_app_caiscuttargetkeylock.h"
#include <featmgr.h>

// Amount of attempts to try to connect to server.
const TInt KTriesToConnectServer(3);

// Delay between retries to connect.
const TInt KTimeBeforeRetryingServerConnection(50000);

// ----------------------------------------------------------------------------
// CAiScutKeyLock::CAiScutKeyLock
// ----------------------------------------------------------------------------
//
CAiScutTargetKeyLock::CAiScutTargetKeyLock(CAiScutEngine& aEngine, TShortcutType aType)
    : CAiScutTarget(aEngine, aType)
{
}

// ----------------------------------------------------------------------------
// CAiScutKeyLock::ConstructL
// ----------------------------------------------------------------------------
//
void CAiScutTargetKeyLock::ConstructL(const TDesC& aTarget)
{
    iDefinition = aTarget.AllocL();

    TInt err(KErrGeneral);
    TInt thisTry(0);

    // Try connect successfully with server limited a number of times
    err = iKeyLock.Connect();
    while ((err != KErrNone) && (thisTry++ < KTriesToConnectServer))
        {
        User::After(KTimeBeforeRetryingServerConnection);
        err = iKeyLock.Connect();
        }
    User::LeaveIfError(err);

}

// ----------------------------------------------------------------------------
// CAiScutKeyLock::NewL
// ----------------------------------------------------------------------------
//
CAiScutTargetKeyLock* CAiScutTargetKeyLock::NewL(
    CAiScutEngine& aEngine, TShortcutType aType, const TDesC& aTarget)
{
    CAiScutTargetKeyLock* self = new (ELeave) CAiScutTargetKeyLock(aEngine, aType);

    CleanupStack::PushL(self);
    self->ConstructL(aTarget);
    CleanupStack::Pop(self);

    return self;
}

// ----------------------------------------------------------------------------
// CAiScutKeyLock::~CAiScutKeyLock
// ----------------------------------------------------------------------------
//
CAiScutTargetKeyLock::~CAiScutTargetKeyLock()
{
    delete iDefinition;
    iKeyLock.Close();
}

// ---------------------------------------------------------------------------
// Returns the shortcut definition string
// ---------------------------------------------------------------------------
//
TPtrC CAiScutTargetKeyLock::Definition() const
{
    return iDefinition ? TPtrC(*iDefinition) : TPtrC();
}


// ---------------------------------------------------------------------------
// Returns the shortcut target caption.
// ---------------------------------------------------------------------------
//
TInt CAiScutTargetKeyLock::GetCaption(TPtrC& aDes, TAiScutAppTitleType /*aTitleType*/) const
{
    aDes.Set(KNullDesC());
    return 0;
}


// ---------------------------------------------------------------------------
// Returns the shortcut target icon.
// ---------------------------------------------------------------------------
//
TInt CAiScutTargetKeyLock::GetIcon(CGulIcon*& aIcon) const
{
    aIcon = NULL;
    return 0;
}


// -----------------------------------------------------------------------------
// Checks if the target is accessible
// -----------------------------------------------------------------------------
//
TBool CAiScutTargetKeyLock::IsAccessibleL(TInt /*aCheckType*/)
{
    return ETrue;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void CAiScutTargetKeyLock::LaunchL()
{
    EnableKeyLock();
}

// ---------------------------------------------------------------------------
// Return application uid this target launches.
// ---------------------------------------------------------------------------
//
TUid CAiScutTargetKeyLock::AppUid() const
{
    return KNullUid;
}


// ----------------------------------------------------------------------------
// CAiScutTargetKeyLock::EnableKeyLock
// ----------------------------------------------------------------------------
//
void CAiScutTargetKeyLock::EnableKeyLock()
{
    iKeyLock.EnableKeyLock();
}


// ----------------------------------------------------------------------------
// CAiScutTargetKeyLock::IsKeyLockEnabled
// ----------------------------------------------------------------------------
//
TBool CAiScutTargetKeyLock::IsKeyLockEnabled()
{
    return iKeyLock.IsKeyLockEnabled();
}

//  End of File.
