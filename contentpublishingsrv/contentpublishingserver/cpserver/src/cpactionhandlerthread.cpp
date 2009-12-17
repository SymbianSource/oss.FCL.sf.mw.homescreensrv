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

#include "cpactionhandlerthread.h"
#include "cpserverdef.h"
#include "cpserveractionmanager.h"
#include "cpglobals.h"

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CPExecutionThreadFunction( TAny* aParam )
    {
    ASSERT( aParam );
    CCPActionHandlerThread* actionHandlerThread =
            reinterpret_cast<CCPActionHandlerThread*>( aParam );
    return actionHandlerThread->ThreadFunction();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPActionHandlerThread* CCPActionHandlerThread::NewL()
    {
    CCPActionHandlerThread* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPActionHandlerThread* CCPActionHandlerThread::NewLC()
    {
    CCPActionHandlerThread* self = new ( ELeave ) CCPActionHandlerThread;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPActionHandlerThread::~CCPActionHandlerThread()
    {
    if ( ExecutionThreadRunning() && iActionExecutorAO )
        {
        // stop action execution thread
        
        // request notification when action execution thread terminates
        TRequestStatus exitStatus;
        iActionExecThread.Logon( exitStatus );
        
        // passing NULL as action parameters will cause the action executor to
        // stop active scheduler in command execution thread
        iActionExecutorAO->SetActionParams( NULL );
        // ask action executor to execute the action
        TRequestStatus* actionExecutorAOStatus =
            iActionExecutorAO->RequestStatus();
        iActionExecThread.RequestComplete( actionExecutorAOStatus, KErrNone );
        
        // wait for the action execution thread to terminate
        User::WaitForRequest( exitStatus );
        }
    iActionExecThread.Close();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPActionHandlerThread::ExecuteL(
        const CLiwGenericParamList& aActionParams )
    {
    StartExecutionThreadIfNeededL();
    
    ASSERT( iActionExecutorAO );
    iActionExecutorAO->SetActionParams( &aActionParams );
    
    // create rendezvous so that we are notified when action
    // execution finishes
    TRequestStatus actionExecutionStatus = KRequestPending;
    iActionExecThread.Rendezvous( actionExecutionStatus );
    
    // request action execution by completing request for the AO running
    // in the action execution thread 
    TRequestStatus* actionExecutorAOStatus =
            iActionExecutorAO->RequestStatus();
    iActionExecThread.RequestComplete( actionExecutorAOStatus, KErrNone );
    
    // wait until action execution finishes
    User::WaitForRequest( actionExecutionStatus );

    TInt err = actionExecutionStatus.Int();
    // please note that checking actionExecutionStatus is not a reliable way of
    // determining whether everything is OK because if plugin panics with 
    // reason (panic number) KErrNone, actionExecutionStatus will contain KErrNone
    if ( !ExecutionThreadRunning() )
        {
        // action execution thread died,
        // iActionExecutorAO pointer is no longer valid
        iActionExecutorAO = NULL;
        if ( err >= 0 )
            {
            err = KErrDied;
            }
        }
    else
        {
        iActionExecutorAO->SetActionParams( NULL );
        }
    
    User::LeaveIfError( err );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
 void CCPActionHandlerThread::StartExecutionThreadIfNeededL()
    {
    if ( !ExecutionThreadRunning() )
        {
        TBuf<KThreadNameLimit> threadName;
        if ( iActionExecThreadStarted )
            {
            iActionExecThread.Close();
            }
        
        // create action execution thread
        threadName.Format( KCPServerPluginThreadName, ++iThreadNum );
        TInt err = iActionExecThread.Create( threadName,
                CPExecutionThreadFunction,
                KDefaultStackSize,
                KCPServerPluginThreadMinHeapSize,
                KCPServerPluginThreadMaxHeapSize,
                reinterpret_cast<TAny*>( this ) );
        User::LeaveIfError( err );
        
        // create rendezvous so that we are notified when the thread
        // has started
        TRequestStatus threadStatus;
        iActionExecThread.Rendezvous( threadStatus );
        
        // start the action execution thread
        iActionExecThread.Resume();
        
        // wait for the thread to start
        User::WaitForRequest( threadStatus );
        
        User::LeaveIfError( threadStatus.Int() );
        
        iActionExecThreadStarted = ETrue;
        ASSERT( ExecutionThreadRunning() );
        }
    }

// ---------------------------------------------------------------------------
// RThread::ExitType() returns EExitPending if the thread is alive or has not
// been started yet. There is a member variable, iActionExecThreadStarted
// which tells if the thread was started or not. Combining the information
// from the member variable and RThread::ExitType() allows us to determine if
// the thread is running (alive).
// ---------------------------------------------------------------------------
//
 TBool CCPActionHandlerThread::ExecutionThreadRunning()
    {
    TBool actionExecThreadDidNotDie =
            iActionExecThread.ExitType() == EExitPending;

    TBool threadRunning =
            iActionExecThreadStarted && actionExecThreadDidNotDie;
    
    // if the thread is alive, it must have created an active object for
    // executing actions in plugins; verify that it is the case
    ASSERT( !threadRunning || ( threadRunning && iActionExecutorAO ));
    
    return threadRunning;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
 CCPActionHandlerThread::CCPActionHandlerThread()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPActionHandlerThread::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TInt CCPActionHandlerThread::ThreadFunction()
    {
    __UHEAP_MARK;

    CServerEikonEnv* env = new CServerEikonEnv;
    if ( !env )
        {
        return KErrNoMemory;
        }
    CServerAppUi* ui = new CServerAppUi;
    if ( !ui )
        {
        delete env;
        return KErrNoMemory;
        }
    
    TRAPD( err,
        env->ConstructL();
        env->DisableExitChecks( ETrue );
        ui->ConstructL();

        iActionExecutorAO = CActionExecutorAO::NewL();
        
        // notify the main thread that action execution thread
        // has successfully started
        RThread::Rendezvous( KErrNone );

        CActiveScheduler::Start();
    );

    delete iActionExecutorAO;
    iActionExecutorAO = NULL;

    ui->PrepareToExit();
    env->DestroyEnvironment();
    
    __UHEAP_MARKEND;
    
    return err;
    }

// -----------------------------------------------------------------------------
// CContentHarvesterEikonEnv::DestroyEnvironment
// 
// -----------------------------------------------------------------------------
//
void CCPActionHandlerThread::CServerEikonEnv::DestroyEnvironment()
    {
    CEikonEnv::DestroyEnvironment( );
    }

// -----------------------------------------------------------------------------
// CContentHarvesterEikonEnv::ConstructL
// 
// -----------------------------------------------------------------------------
//
void CCPActionHandlerThread::CServerEikonEnv::ConstructL()
    {
    CEikonEnv::ConstructL( EFalse );
    SetAutoForwarding( ETrue );
    User::SetPriorityControl( EFalse );
    }
// -----------------------------------------------------------------------------
// CContentHarvesterAppUi::~CContentHarvesterAppUi
// 
// -----------------------------------------------------------------------------
//
CCPActionHandlerThread::CServerAppUi::~CServerAppUi()
    {
    }

// ---------------------------------------------------------------------------
// CContentHarvesterAppUi::ConstructL
// 
// ---------------------------------------------------------------------------
//
void CCPActionHandlerThread::CServerAppUi::ConstructL()
    {
    CEikAppUi::BaseConstructL( ENoAppResourceFile | ENoScreenFurniture );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPActionHandlerThread::CActionExecutorAO*
CCPActionHandlerThread::CActionExecutorAO::NewL()
    {
    CActionExecutorAO* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPActionHandlerThread::CActionExecutorAO*
CCPActionHandlerThread::CActionExecutorAO::NewLC()
    {
    CActionExecutorAO* self = new ( ELeave ) CActionExecutorAO();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPActionHandlerThread::CActionExecutorAO::~CActionExecutorAO()
    {
    delete iActionManager;
    }
        
// ---------------------------------------------------------------------------
// Any code executed in this method must not leave!
// This way we avoid implementing RunError while making sure that 
// Rendezvous will always be called
// ---------------------------------------------------------------------------
//
void CCPActionHandlerThread::CActionExecutorAO::RunL()
    {
    if( iActionParams )
        {
        // execute action
        TRAPD( err, iActionManager->ExecuteL( *iActionParams ) );
        
        // notify the main thread that action execution has finished
        RThread::Rendezvous( err );
    
        // wait until another action execution is requested
        iStatus = KRequestPending;
        SetActive();
        }
    else
        {
        CActiveScheduler::Stop();
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPActionHandlerThread::CActionExecutorAO::DoCancel()
    {
    TRequestStatus* myRequestStatus = &iStatus;
    User::RequestComplete( myRequestStatus, KErrCancel );
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
TRequestStatus* CCPActionHandlerThread::CActionExecutorAO::RequestStatus()
    {
    return &iStatus;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPActionHandlerThread::CActionExecutorAO::SetActionParams(
        const CLiwGenericParamList* aActionParams )
    {
    iActionParams = aActionParams;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CCPActionHandlerThread::CActionExecutorAO::CActionExecutorAO()
    : CActive( CActive::EPriorityLow )
    {
    CActiveScheduler::Add( this );

    // wait until action execution is requested
    iStatus = KRequestPending;
    SetActive();
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPActionHandlerThread::CActionExecutorAO::ConstructL()
    {
    iActionManager = CCPActionManager::NewL();
    }
