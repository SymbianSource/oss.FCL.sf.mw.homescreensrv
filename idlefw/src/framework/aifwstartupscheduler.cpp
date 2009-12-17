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
* Description:  AvtiveIdle2 startup scheduler
*
*/


#include "aifwstartupscheduler.h"

CAiFwStartupScheduler::CAiFwStartupScheduler()
    : iResult(KErrNone)
    {
    }

CAiFwStartupScheduler* CAiFwStartupScheduler::NewLC()
    {
    CAiFwStartupScheduler* self = new(ELeave) CAiFwStartupScheduler;
    CleanupStack::PushL(self);
    return self;
    }

CAiFwStartupScheduler::~CAiFwStartupScheduler()
    {
    }

TInt CAiFwStartupScheduler::Result()
    {
    return iResult;
    }

// All RunL leaves from active objects which execute during Active Idle
// Framework startup end up here
void CAiFwStartupScheduler::Error(TInt aError) const
    {
    // Store any error code
    if (aError != KErrNone && iResult == KErrNone)
        {
        iResult = aError;
        }
        
    // Stop the scheduler as all errors during Active Idle Framework startup
    // are fatal
    Stop();
    }

