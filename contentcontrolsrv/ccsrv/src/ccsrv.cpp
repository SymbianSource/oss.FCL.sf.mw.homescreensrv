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

// System include files

// User include files
#include "ccsrv.h"
#include "ccsrvsession.h"
#include "ccresource.h"
#include "ccsrvapi.h"


// Local constants

// Home screen content control server name
_LIT( KCcSrvName,"ccserver" );

// Home screen content control server policy
//Total number of ranges
const TUint KCcRangeCount = 2;

//Definition of the ranges of IPC numbers
const TInt KCcRanges[KCcRangeCount] = 
        {
        ECcIPCFunctionBase,
        ECcNotSupported 
        }; 

// Policy actions for each of the above ranges
const TUint8 KCcPolicyAction[KCcRangeCount] = 
        {
        0,
        CPolicyServer::ENotSupported
        };

// Requested capabilities
const CPolicyServer::TPolicyElement KCcCapability[] = 
        {
        {_INIT_SECURITY_POLICY_C2( ECapabilityReadUserData, ECapabilityWriteUserData ), CPolicyServer::EFailClient}
        };

const CPolicyServer::TPolicy KCcPolicy =
        {
        0,
        KCcRangeCount,                                 
        KCcRanges,
        KCcPolicyAction,
        KCcCapability
        };

// -----------------------------------------------------------------------------
// RunServerL()
// -----------------------------------------------------------------------------
//
static void RunServerL()
    {
    User::LeaveIfError(User::RenameThread(KCcSrvName));
    CActiveScheduler* s=new(ELeave) CActiveScheduler;
    CleanupStack::PushL(s);
    CActiveScheduler::Install(s);
    CCcSrv::NewLC();
    RProcess::Rendezvous(KErrNone);
    CActiveScheduler::Start();
    CleanupStack::PopAndDestroy( 2, s );
    }

// -----------------------------------------------------------------------------
// E32Main
// Server process entry-point
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    __UHEAP_MARK;
    CTrapCleanup* cleanup=CTrapCleanup::New();
    TInt r=KErrNoMemory;
    if (cleanup)
        {
        TRAP(r,RunServerL());
        delete cleanup;
        }
    __UHEAP_MARKEND;
    return r;
    }

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// CCcSrv::NewLC
// -----------------------------------------------------------------------------
//
CServer2* CCcSrv::NewLC()
    {
    CCcSrv* self=new ( ELeave ) CCcSrv;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------
// CCcSrv::ConstructL()
// -----------------------------------------------------------------------
//
void CCcSrv::ConstructL()
    {
    StartL( KCcSrvName );
    }

// -----------------------------------------------------------------------
// CCcSrv::CCcSrv()
// -----------------------------------------------------------------------
//
CCcSrv::CCcSrv()
    :CPolicyServer( 0,KCcPolicy,ESharableSessions )
    ,iLastSessionId( 0 )
    {
    }

// -----------------------------------------------------------------------
// CCcSrv::~CCcSrv()
// -----------------------------------------------------------------------
//
CCcSrv::~CCcSrv()
    {
    iSessions.Close();
    iProviders.Close();
    iObservers.Close();
    }

// -----------------------------------------------------------------------------
// CCcSrv::NewSessionL
// -----------------------------------------------------------------------------
//
CSession2* CCcSrv::NewSessionL(
    const TVersion&,
    const RMessage2& /* aMessage */ ) const
    {
    return new ( ELeave ) CCcSrvSession();
    }

// -----------------------------------------------------------------------------
// CCcSrv::AddSessionL
// -----------------------------------------------------------------------------
//
void CCcSrv::AddSessionL( CCcSrvSession* aSession )
    {
    // Find next available session id
    while( ETrue )
        {
        iLastSessionId = iLastSessionId + 1;
        TBool found( EFalse );
        for ( TUint32 i = 0; i < iSessions.Count() && !found; i++ )
            {
            if ( iSessions[ i ]->Id() == iLastSessionId )
                {
                found = ETrue;
                }
            }
        if ( !found )
            {
            break;
            }
        }
    
    aSession->SetId( iLastSessionId );
    iSessions.AppendL( aSession );
    }

// -----------------------------------------------------------------------------
// CCcSrv::DropSession
// -----------------------------------------------------------------------------
//
void CCcSrv::DropSession( CCcSrvSession* aSession )
    {
    // Remove possible provider
    for ( TUint32 i = 0; i < iProviders.Count(); i++ )
        {
        if ( iProviders[ i ].iSession->Id() == aSession->Id() )
            {
            iProviders.Remove( i );
            break;
            }
        }
    // Unregister possible observations and remove observer
    for ( TUint32 i = 0; i < iObservers.Count(); )
        {
        if ( iObservers[ i ].iObserver == aSession->Id() )
            {
            for ( TUint32 j = 0; j < iProviders.Count(); j++ )
                {
                if ( iObservers[ i ].iProviderId == iProviders[ j ].iId )
                    {
                    TRAP_IGNORE( iProviders[ j ].iSession->
                        UnregisterObserverSessionL( aSession->Id() ) );
                    }
                }
            iObservers.Remove( i );
            }
        else
            {
            // Get next observer
            i++;
            }
        }
    // Remove session
    for ( TUint32 i = 0; i < iSessions.Count(); i++ )
        {
        if ( iSessions[ i ]->Id() == aSession->Id() )
            {
            iSessions.Remove( i );
            break;
            }
        }
    
    if ( iSessions.Count() == 0 )
        {
        // Last session dropped -> stop server
        CActiveScheduler::Stop();
        }
    }

// -----------------------------------------------------------------------------
// CCcSrv::RegisterProviderSessionL
// -----------------------------------------------------------------------------
//
void CCcSrv::RegisterProviderL( 
    TUint32 aProvider,
    CCcSrvSession* aSession )
    {
    for ( TUint32 i = 0; i < iProviders.Count(); i++ )
        {
        if ( iProviders[ i ].iId == aProvider )
            {
            User::Leave( KErrAlreadyExists );
            }
        }
    CCcSrv::TCcProvider provider;
    provider.iId = aProvider;
    provider.iSession = aSession;
    iProviders.Append( provider );
    
    // Register possible active observers
    for ( TUint32 i = 0; i < iObservers.Count(); i++ )
        {
        if ( iObservers[ i ].iProviderId == aProvider )
            {
            provider.iSession->RegisterObserverSessionL( iObservers[ i ].iObserver );
            }
        }
    
    }

// -----------------------------------------------------------------------------
// CCcSrv::RegisterObserverL
// -----------------------------------------------------------------------------
//
void CCcSrv::RegisterObserverL( 
    TUint32 aProvider,
    CCcSrvSession* aSession )
    {
    for ( TUint32 i = 0; i < iProviders.Count(); i++ )
        {
        if ( iProviders[ i ].iId == aProvider )
            {
            iProviders[ i ].iSession->RegisterObserverSessionL( aSession->Id() );
            break;
            }
        }
    CCcSrv::TCcObserver observer;
    observer.iProviderId = aProvider;
    observer.iObserver = aSession->Id();
    iObservers.Append( observer );
    }

// -----------------------------------------------------------------------------
// CCcSrv::ResolveProviderAddressL
// -----------------------------------------------------------------------------
//
void CCcSrv::ResolveProviderAddressL( 
    TUint32 aProvider,
    TUint32& aAddress )
    {
    TBool found( EFalse );
    for ( TUint32 i = 0; i < iProviders.Count() && !found; i++ )
        {
        if ( iProviders[ i ].iId == aProvider )
            {
            aAddress = iProviders[ i ].iSession->Id();
            found = ETrue;
            }
        }
    if ( !found )
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CCcSrv::GetTrId
// -----------------------------------------------------------------------------
//
TUint32 CCcSrv::GetTrId()
    {
    iLastTrId++;
    return iLastTrId;
    }

// -----------------------------------------------------------------------------
// CCcSrv::SendMsgL
// -----------------------------------------------------------------------------
//
void CCcSrv::SendMsgL( 
    CCcSrvMsg& aMessage )
    {
    TBool found( EFalse );
    for ( TUint32 i = 0; i < iSessions.Count() && !found; i++ )
        {
        if ( iSessions[ i ]->Id() == aMessage.Receiver() )
            {
            iSessions[ i ]->ReceiveMsgL( aMessage );
            found = ETrue;
            }
        }
    if ( !found )
        {
        User::Leave( KErrNotFound );
        }
    }

// End of file
