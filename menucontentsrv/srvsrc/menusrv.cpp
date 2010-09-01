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

// INCLUDE FILES

#include "menusrv.h"
#include "menusrvdef.h"
#include "menusrvsession.h"
#include "timeout.h"
#include "menumsg.h"
#include "menuutil.h"
#include "menusrveng.h"


// CONSTANTS

// Custom check is applied to all IPCs. As IPC ids contain not only the ids
// but other information is embadded into them.
    
LOCAL_D const TInt KRangeCount = 1;

LOCAL_D const TInt KSecurityRanges[KRangeCount] = 
    {
    EMenuNullFunction,
    };
    
LOCAL_D const TUint8 SecurityRangesPolicy[KRangeCount] =
    {
    CPolicyServer::ECustomCheck
    };

LOCAL_D const CPolicyServer::TPolicy KPolicy =
    {
    CPolicyServer::EAlwaysPass,
    KRangeCount,
    KSecurityRanges,
    SecurityRangesPolicy,
    NULL,
    };

// ==================== LOCAL FUNCTIONS ====================

/**
* Stop the Active Scheduler.
* @param aPtr Not used.
* @return KErrNone.
*/
LOCAL_C TInt StopScheduler( TAny* /*aPtr*/ )
    {
    // Called by the exit timer, after all clients disconnected (plus a small
    // delay). Stop the scheduler, this will enable he thread exit.
    CActiveScheduler::Stop();
    return KErrNone;
    }

/**
* Create a server.
* @param Pointer to created server (if created) returned here.
* @return Error code.
*/
LOCAL_C TInt CreateServer( CMenuSrv*& aServer )
    {
    // The TRAP is not working in the same stack frame where the
    // CTrapCleanup was created. This is why we need this function.
    TRAPD( err, aServer = CMenuSrv::NewL() );
    return err;
    }

// ==================== GLOBAL FUNCTIONS ====================

// ---------------------------------------------------------
// RunMenuServer
// ---------------------------------------------------------
//
EXPORT_C TInt RunMenuServer()
    {
    __UHEAP_MARK;
    CTrapCleanup* trapCleanup = NULL;
    CActiveScheduler* activeScheduler = NULL;
    CMenuSrv* server = NULL;

    TInt err = User::RenameThread( KMenuSrvName );
    if ( !err )
        {
        // Create a trap cleanup, make and install an active scheduler.
        err = KErrNoMemory;
        trapCleanup = CTrapCleanup::New();
        if ( trapCleanup )
            {
            activeScheduler = new CActiveScheduler();
            if ( activeScheduler )
                {
                CActiveScheduler::Install( activeScheduler );
                err = CreateServer( server );   // Not pushed (no leaving).
                if ( !err )
                    {
                    err = server->Start( KMenuSrvName );
                    }
                }
            else
            	{
            	err = KErrNoMemory;
            	}
            }
        }
    // Let the caller know how it went.
    RProcess::Rendezvous( err );
    if ( !err )
        {
        CActiveScheduler::Start(); // Start off. Exit timer will stop it.
        }
    CActiveScheduler::Install( NULL );
    delete activeScheduler;
    delete server;
    delete trapCleanup;
    __UHEAP_MARKEND;
    return err;
    }

// ==================== MEMBER FUNCTIONS ====================

// ---------------------------------------------------------
// CMenuSrv::NewL
// ---------------------------------------------------------
//
CMenuSrv* CMenuSrv::NewL()
    {
    CMenuSrv* srv = new (ELeave) CMenuSrv();
    CleanupStack::PushL( srv );
    srv->ConstructL();
    CleanupStack::Pop( srv );
    return srv;
    }

// ---------------------------------------------------------
// CMenuSrv::~CMenuSrv
// ---------------------------------------------------------
//
CMenuSrv::~CMenuSrv()
    {
    // Cancel requests and delete all sessions first.
    // Base class would do it for us but that's too late - our sessions
    // call the server back (SessionClosed, RemoveContainer, etc.).
    Cancel();
    CSession2* session;
    iSessionIter.SetToFirst();
    while ( NULL != (session = iSessionIter++) )
        {
        delete session;
        }
    // Here we should have no objects that are dependent on us.
    delete iObjectConIx; // This kills iEngines too.
    delete iExitTimer;
    }

// ---------------------------------------------------------
// CMenuSrv::NewContainerL
// ---------------------------------------------------------
//
CObjectCon* CMenuSrv::NewContainerL()
    {
    return iObjectConIx->CreateL();
    }

// ---------------------------------------------------------
// CMenuSrv::RemoveContainer
// ---------------------------------------------------------
//
void CMenuSrv::RemoveContainer( CObjectCon* aCon )
    {
    iObjectConIx->Remove( aCon );
    }

// ---------------------------------------------------------
// CMenuSrv::GetEngineL
// ---------------------------------------------------------
//
CMenuSrvEng* CMenuSrv::GetEngineL( const TDesC& aName )
    {
    CMenuSrvEng* eng = NULL;
    for ( TInt i = 0; i < iEngines->Count(); i++ )
        {
        eng = (CMenuSrvEng*)(*iEngines)[i];
        if ( eng->ContentName() == aName )
            {
            return eng;
            }
        }
    eng = CMenuSrvEng::NewL( *this, aName );
    CleanupClosePushL( *eng );
    iEngines->AddL( eng );
    CleanupStack::Pop( eng );
    return eng;
    }

// ---------------------------------------------------------
// CMenuSrv::EngineDeleted
// ---------------------------------------------------------
//
void CMenuSrv::EngineDeleted()
    {
    if ( 1 >= iEngines->Count() )
        {
        // Last engine is being deleted now.
        // Exit now, without delay: the engines had the timeout.
        //
        // Engine count is 1 when the engine has been created and added.
        // Engine count is 0 in case of leave in GetEngineL().
        iExitTimer->Cancel();
        CActiveScheduler* currentScheduler = CActiveScheduler::Current();
        // No more sessions; schedule self-deletion.
        if (currentScheduler)
            {
            iExitTimer->After( TTimeIntervalMicroSeconds32( 0 ) );
            }
        }
    }

// ---------------------------------------------------------
// CMenuSrv::CMenuSrv
// ---------------------------------------------------------
//
CMenuSrv::CMenuSrv()
: CPolicyServer( CActive::EPriorityStandard, KPolicy, ESharableSessions )
    {
    }

// ---------------------------------------------------------
// CMenuSrv::ConstructL
// ---------------------------------------------------------
//
void CMenuSrv::ConstructL()
    {
    iExitTimer = CTimeout::NewL
        ( CActive::EPriorityStandard, TCallBack( StopScheduler, NULL ) );
    iExitTimer->Cancel();
    iExitTimer->After( TTimeIntervalMicroSeconds32( KMenuSrvExitDelay ) );
    iObjectConIx = CObjectConIx::NewL();
    iEngines = iObjectConIx->CreateL();
    }

// ---------------------------------------------------------
// CMenuSrv::NewSessionL
// ---------------------------------------------------------
//
CSession2* CMenuSrv::NewSessionL
( const TVersion& aVersion, const RMessage2& /*aMessage*/ ) const
    {
    TVersion version( KMenuMajorVersion, KMenuMinorVersion, KMenuBuild );
    if ( !User::QueryVersionSupported( version, aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }
    CSession2* session = CMenuSrvSession::NewL( (CMenuSrv&)*this );
    iExitTimer->Cancel();   // We have a client, cancel exit (if pending).
    return session;
    }

// ---------------------------------------------------------
// CMenuSrv::CustomSecurityCheckL
// ---------------------------------------------------------
//
CPolicyServer::TCustomResult CMenuSrv::CustomSecurityCheckL
    ( const RMessage2& aMsg, TInt& /*aAction*/, TSecurityInfo& /*aMissing*/ )
    {
    TInt func = aMsg.Function();
    TCustomResult ret = EFail;
    if( func > EMenuTestCapabilityStart && 
        func < EMenuTestCapabilityEnd )
        {
        if( aMsg.HasCapability( ECapabilityAllFiles ) )
           {
           ret = EPass;
           }
        }
    else if( func > EMenuReadCapabilityStart && 
             func < EMenuReadCapabilityEnd )
        {
        if( aMsg.HasCapability( ECapabilityReadDeviceData ) )
           {
           ret = EPass;
           }
        }
    else if( func > EMenuWriteCapabilityStart && 
             func < EMenuWriteCapabilityEnd )
        {
        if( aMsg.HasCapability( ECapabilityWriteDeviceData ) )
           {
           ret = EPass;
           }
        }
    else
        {
        ;
        }
    return ret;
    }

//  End of File  
