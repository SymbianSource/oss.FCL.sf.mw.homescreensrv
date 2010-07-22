/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#include <e32property.h>
#include <startupdomainpskeys.h>
#include <flogger.h>
#include <w32std.h>

#include "tsbacksteppingactivation.h"
#include "tsbackstepping.h"

 // ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CBacksteppingActivation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTsBacksteppingActivation* CTsBacksteppingActivation::NewL(MTsWindowGroupsMonitor &monitor)
{
    RDebug::Print(_L("CBacksteppingActivation::NewL"));
    CTsBacksteppingActivation* self = new( ELeave ) CTsBacksteppingActivation(monitor);
    self->ConstructL();
    return self;
}

// -----------------------------------------------------------------------------
// CBacksteppingWaitingIdle::CBacksteppingWaitingIdle
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CTsBacksteppingActivation::CTsBacksteppingActivation(MTsWindowGroupsMonitor &monitor) 
:
    CActive(EPriorityLow),
    mMonitor(monitor)
{
    CActiveScheduler::Add(this);
}

// -----------------------------------------------------------------------------
// CBacksteppingActivation::ConstructL()
// -----------------------------------------------------------------------------
//
void CTsBacksteppingActivation::ConstructL() 
{
    User::LeaveIfError(mProperty.Attach(KPSUidStartup, KPSIdlePhase1Ok));
    iStatus = KRequestPending;
    SetActive();
    TRequestStatus* status(&iStatus);
    User::RequestComplete(status);
}

// -----------------------------------------------------------------------------
// CBacksteppingActivation::~CBacksteppingActivation
// Destructor
// -----------------------------------------------------------------------------
//
CTsBacksteppingActivation::~CTsBacksteppingActivation()
{
    Cancel();
    delete mEngine;
    mProperty.Close();
}

// -----------------------------------------------------------------------------
// CBacksteppingActivation::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CTsBacksteppingActivation::DoCancel()
{
    if ( IsActive() && mProperty.Handle() ) {
        mProperty.Cancel();
    }
}

// -----------------------------------------------------------------------------
// CBacksteppingActivation::RunL
// 
// -----------------------------------------------------------------------------
//
void CTsBacksteppingActivation::RunL()
{
    User::LeaveIfError(iStatus.Int());
    
    TInt state(~EIdlePhase1Ok);
    User::LeaveIfError(mProperty.Get(state));
    
    if (EIdlePhase1Ok == state) {
        if (0 == mEngine) {
            mEngine = CTsBackstepping::NewL(mMonitor);
        }
    } else {
        delete mEngine;
        mEngine = 0;
    }
    Subscribe();
}

// -----------------------------------------------------------------------------
// CBacksteppingActivation::RunError
// 
// -----------------------------------------------------------------------------
//
TInt CTsBacksteppingActivation::RunError(TInt error)
{
    if (KErrCancel != error) {
        Subscribe();
    }
    return KErrNone;
}

// -----------------------------------------------------------------------------
// CBacksteppingActivation::Subscribe
// 
// -----------------------------------------------------------------------------
//
void CTsBacksteppingActivation::Subscribe()
{
    mProperty.Subscribe(iStatus);
    SetActive();
}
// end of file
