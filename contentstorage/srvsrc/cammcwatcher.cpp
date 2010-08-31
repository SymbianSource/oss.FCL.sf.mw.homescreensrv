/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include <driveinfo.h>

#include "cammcwatcher.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCaWidgetMmcWatcher::NewL
// Second phase constructor
// ---------------------------------------------------------
//
EXPORT_C CCaMmcWatcher* CCaMmcWatcher::NewL( RFs& aFs,
        MMmcWatcherCallback* aObserver )
    {
    CCaMmcWatcher* self = NewLC( aFs, aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
// CCaWidgetMmcWatcher::NewLC
// Second phase constructor
// ---------------------------------------------------------
//
EXPORT_C CCaMmcWatcher* CCaMmcWatcher::NewLC( RFs& aFs,
        MMmcWatcherCallback* aObserver )
    {
    CCaMmcWatcher* self = new (ELeave) CCaMmcWatcher( aFs,
            aObserver );
    CleanupStack::PushL ( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CCaWidgetMmcWatcher::~CCaWidgetMmcWatcher
// Destructor
// ---------------------------------------------------------
//
CCaMmcWatcher::~CCaMmcWatcher()
    {
    Cancel();
    }

// ---------------------------------------------------------
// CCaWidgetMmcWatcher::CCaWidgetMmcWatcher
// Default constructor
// ---------------------------------------------------------
//
CCaMmcWatcher::CCaMmcWatcher( RFs& aFs,
        MMmcWatcherCallback* aObserver )
    : CActive( CActive::EPriorityStandard ),
    iFs(aFs),
    iObserver(aObserver)
    {
    CActiveScheduler::Add(this);
    }

// ---------------------------------------------------------
// CCaWidgetMmcWatcher::ConstructL
// default Symbian OS constructor
// ---------------------------------------------------------
//
void CCaMmcWatcher::ConstructL()
    {
    WaitForChangeL();
    }

// ---------------------------------------------------------
// CCaWidgetMmcWatcher::WaitForChangeL
// Request notification for disk change
// ---------------------------------------------------------
//
void CCaMmcWatcher::WaitForChangeL()
    {
    TInt mmcDrive;
    User::LeaveIfError( DriveInfo::GetDefaultDrive(
        DriveInfo::EDefaultRemovableMassStorage, mmcDrive ) );
    TDriveName mmcDriveName( TDriveUnit( mmcDrive ).Name() );
    iFs.NotifyChange( ENotifyEntry, iStatus, mmcDriveName );
    SetActive();
    }

// ---------------------------------------------------------
// CCaWidgetMmcWatcher::DoCancel
// ---------------------------------------------------------
//
void CCaMmcWatcher::DoCancel()
    {
    iFs.NotifyChangeCancel();
    }

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT (error is ignored)
// ---------------------------------------------------------------------------
// CCaWidgetMmcWatcher::RunError
// ---------------------------------------------------------------------------
//
TInt CCaMmcWatcher::RunError( TInt /*aError*/ )
    {
    // No need to do anything
    return KErrNone;
    }
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT

#ifdef COVERAGE_MEASUREMENT
#pragma CTC SKIP
#endif //COVERAGE_MEASUREMENT (memory card removal)
// ---------------------------------------------------------
// CCaWidgetMmcWatcher::RunL
// ---------------------------------------------------------
//
void CCaMmcWatcher::RunL()
    {
    TInt status( iStatus.Int() );
    WaitForChangeL();
    if ( status >= KErrNone )
        {
        iObserver->MmcChangeL();
        }
    }
#ifdef COVERAGE_MEASUREMENT
#pragma CTC ENDSKIP
#endif //COVERAGE_MEASUREMENT
