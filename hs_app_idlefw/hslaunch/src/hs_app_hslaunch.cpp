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
 * Description: application class.                 
 *
 */

#include <e32property.h>
#include "hs_app_hslaunch.h"

// ========================= DECLARATIONS ==================================
_LIT( KHsExeName, "hs_app_homescreen.exe" );
_LIT( KHsProcessName, "Home screen" );
const TInt KSleepOnRetry = 250000; // 250ms
const TUid KPSCategoryUid = TUid::Uid( 0x200286E3 );
const TInt KPSCrashCountKey = 1;
_LIT_SECURITY_POLICY_C1( KPSReadPolicy, ECapabilityReadDeviceData );
_LIT_SECURITY_POLICY_C1( KPSWritePolicy, ECapabilityWriteDeviceData );

// ========================= LOCAL FUNCTIONS ==================================

void RunAppL()
    {
    // Construct active scheduler.
    CActiveScheduler* activeScheduler = new ( ELeave )CActiveScheduler();
    CleanupStack::PushL( activeScheduler );
    CActiveScheduler::Install( activeScheduler );        
    
    // Construct app.
    CHsLaunch* app = CHsLaunch::NewL();
    CleanupStack::PushL( app );        
    
    app->Activate();
    
    // Signal startup.
    RProcess::Rendezvous(KErrNone);
    
    // Start active scheduler.
    CActiveScheduler::Start();
    
    User::LeaveIfError( app->ApplicationReturnValue() );
    
    // Cleanup.
    CleanupStack::PopAndDestroy( app );
    CleanupStack::PopAndDestroy( activeScheduler );
    }

// -----------------------------------------------------------------------------
// E32Main
// entry-point
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {            
    TInt error = KErrNone;
    
    __UHEAP_MARK;    
    
    // Construct cleanup stack.
    CTrapCleanup* cleanupstack = CTrapCleanup::New();
    if( !cleanupstack )
        {
        error  = KErrNoMemory;
        }
    
    // Call next phase of startup.
    if( !error )
        {
        TRAP( error, RunAppL() );
        }
    
    // Cleanup.
    delete cleanupstack;
    cleanupstack = NULL;
    
    __UHEAP_MARKEND;
    
    return error;
    }

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CHsLaunch::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
CHsLaunch* CHsLaunch::NewL()
    {
    CHsLaunch* self = new ( ELeave ) CHsLaunch();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHsLaunch::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHsLaunch::ConstructL()
    {
    iShutdownMonitor = CHsShutdownMonitor::NewL( *this );
    iShutdownMonitor->StartMonitor();
    
    // Create property to pub/sub.    
    TInt error = RProperty::Define( KPSCrashCountKey,
                                    RProperty::EInt,
                                    KPSReadPolicy,
                                    KPSWritePolicy );    
    if( error == KErrNone )
        {
        // Init it to zero.
        error = RProperty::Set( KPSCategoryUid, KPSCrashCountKey, 0 );
        }                
    else if( error == KErrAlreadyExists )
        {
        error = KErrNone;        
        }        
    
    User::LeaveIfError( error );
    }

// -----------------------------------------------------------------------------
// CHsLaunch::CHsLaunch()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CHsLaunch::CHsLaunch() :
        CActive( EPriorityStandard ),
        iApplicationReturnValue( KErrNone )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CHsLaunch::~CHsLaunch()
// Destructor.
// -----------------------------------------------------------------------------
//
CHsLaunch::~CHsLaunch()
    {
    Cancel();
    
    delete iProcessMonitor;
    iProcessMonitor = NULL;
    
    delete iShutdownMonitor;
    iShutdownMonitor = NULL;
    }

// -----------------------------------------------------------------------------
// CHsLaunch::Activate()
// Set active object to be ran
// -----------------------------------------------------------------------------
//
void CHsLaunch::Activate()
    {   
    SetActive();
    TRequestStatus* tmp = &iStatus;
    User::RequestComplete( tmp, KErrNone );
    }

// -----------------------------------------------------------------------------
// CHsLaunch::ApplicationReturnValue()
// Get application return value
// -----------------------------------------------------------------------------
//
TInt CHsLaunch::ApplicationReturnValue()
    {
    return iApplicationReturnValue;
    }

// -----------------------------------------------------------------------------
// CHsLaunch::RunL
// -----------------------------------------------------------------------------
//
void CHsLaunch::RunL()
    {    
    // Create app or connect to existing.
    TInt processExisted = EFalse;
    
    RProcess process;
    TInt processError = process.Create( KHsExeName, KNullDesC );
    if( processError == KErrAlreadyExists )
        {        
        processError = process.Open( KHsProcessName, EOwnerProcess );
        processExisted = ETrue;
        }
    
    TInt monitorError = KErrNone;
    if( processError == KErrNone )
        {
        TRAP( monitorError, InitProcessMonitorL( process.Id() ) );
        }
    
    if( processError == KErrNone &&
        !processExisted )
        {
        // Make sure process is started even if monitor startup
        // fails. This will assure that process is not left in
        // suspended state. Resume can not be called for
        // already running process (will cause KERN-EXEC 46).
        process.Resume();
        }
                     
    process.Close();
    
    if( processError != KErrNone ||
        monitorError != KErrNone )
        {
        /* Error in this method is critical and it might cause
         * endless active scheduler loop if no active objects are
         * not triggered. Therefore shutdown. This application
         * will be restarted by startup sw. */
        ShutdownApp( KErrGeneral );
        }
    }

// -----------------------------------------------------------------------------
// CHsLaunch::DoCancel()
// From CActive.
// -----------------------------------------------------------------------------
//
void CHsLaunch::DoCancel()
    {
    if( iProcessMonitor )
        {
        iProcessMonitor->Cancel();
        }
    
    if( iShutdownMonitor )
        {
        iShutdownMonitor->Cancel();
        }
    }      

// -----------------------------------------------------------------------------
// CHsLaunch::RunError()
// From CActive.
// -----------------------------------------------------------------------------
//
TInt CHsLaunch::RunError( TInt aError )
    {
    return aError;
    } 

// -----------------------------------------------------------------------------
// CHsLaunch::ProcessEndedL()
// From MHsProcessMonitorObserver
// -----------------------------------------------------------------------------
//
void CHsLaunch::ProcessEnded( const TExitType& aExitType,
                              const TInt /*aExitReason*/,
                              const TExitCategoryName& /*aExitCategory*/ )
    {    
    // Only respond to panic. EExitTerminate and EExitKill are ignored.
    if( aExitType != EExitPanic )
        {
        return;
        }
    
    TInt crashCount = 0;
    TInt error = RProperty::Get( KPSCategoryUid,
                                 KPSCrashCountKey,
                                 crashCount );
    
    if( error == KErrNone )
        {
        crashCount++;
        error = RProperty::Set( KPSCategoryUid,
                                KPSCrashCountKey,
                                crashCount );
        }
    
    if( error == KErrNone )
        {
        User::After( KSleepOnRetry ); 
        Activate();
        }
    else
        {    
        ShutdownApp( error );
        }
    }

// -----------------------------------------------------------------------------
// CHsLaunch::ProcessMonitoringErrorL()
// From MHsProcessMonitorObserver
// -----------------------------------------------------------------------------
//
void CHsLaunch::ProcessMonitoringError( TInt aError )
    {
    /* Error in this method is critical and it might cause
     * endless active scheduler loop if no active objects are
     * not triggered. Therefore shutdown. This application
     * will be restarted by startup sw. */
    ShutdownApp( aError );
    }

// -----------------------------------------------------------------------------
// CHsLaunch::SystemShutdownEvent()
// From MHsShutdownMonitorObserver
// -----------------------------------------------------------------------------
//
void CHsLaunch::SystemShutdownEvent()
    {
    // Do not shut down app. Startup app would try to restart this app.
    // Just deactivate all active objects and wait for power off.
    if( IsActive() )
        {
        Cancel();
        }
    else
        {
        DoCancel();
        }    
    }

// -----------------------------------------------------------------------------
// CHsLaunch::InitProcessMonitorL()
// Initialize process monitor
// -----------------------------------------------------------------------------
//
void CHsLaunch::InitProcessMonitorL( const TInt aProcessId )
    {
    if( iProcessMonitor )
        {
        delete iProcessMonitor;
        iProcessMonitor = NULL;
        }
    iProcessMonitor = CHsProcessMonitor::NewL( aProcessId, *this );    
    User::LeaveIfError( iProcessMonitor->StartMonitor() );
    }

// -----------------------------------------------------------------------------
// CHsLaunch::ShutdownApp()
// Shut down application
// -----------------------------------------------------------------------------
//
void CHsLaunch::ShutdownApp( const TInt aApplicationReturnValue )
    {
    if( IsActive() )
        {
        Cancel();
        }
    else
        {
        DoCancel();
        }
    
    iApplicationReturnValue = aApplicationReturnValue;    
    CActiveScheduler::Stop();
    }

// End of File
