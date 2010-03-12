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
* Description:  Content publisher Server
*
*/

#include <mw/memorymanager.h>
#include <liwcommon.h>
#include <ecom/ecom.h>
#include <fbs.h>

#include "cpublisherregistrymap.h"
#include "cpserver.h"
#include "cpglobals.h"
#include "cpdebug.h"
#include "cpserversession.h"
#include "cpserverdatamanager.h"
#include "cpactionhandlerthread.h"


// Policy Server
// ============================================================================
const TUint KServerPolicyRangeCount = 3;
const TInt KServerPolicyRanges[KServerPolicyRangeCount] =
    {
    0, // range 0 -- ECpServerExecuteAction - 1
    ECpServerGetListSize, // range ECpServerGetListSize -- ECpServerExecuteAction - 1
    ECpServerExecuteAction
    // range ECpServerExecuteAction -- inf        
    };

const TUint8 KServerPolicyElementsIndex[KServerPolicyRangeCount] =
    {
    0, // applies to 0th range
    1, // applies to 1st range
    CPolicyServer::EAlwaysPass
    };

const CPolicyServer::TPolicyElement KServerPolicyElements[] =
    {
        {
        _INIT_SECURITY_POLICY_C1(ECapability_None), CPolicyServer::EFailClient
        },
        {
        _INIT_SECURITY_POLICY_C1(ECapability_None), CPolicyServer::EFailClient
        }
    };

const CPolicyServer::TPolicy KServerPolicy =
    {
    CPolicyServer::EAlwaysPass, // specifies all connect attempts should pass
    KServerPolicyRangeCount,
    KServerPolicyRanges,
    KServerPolicyElementsIndex, 
    KServerPolicyElements
    };
// ============================================================================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCPServer::CCPServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCPServer::CCPServer( TInt aPriority ) :
    CPolicyServer( aPriority, KServerPolicy, EUnsharableSessions)
    {
    }

// -----------------------------------------------------------------------------
// CCPServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCPServer::ConstructL()
    {
    CP_DEBUG( _L8("CCPServer::ConstructL()" ) );
    StartL(KCPServerName);
    iBURListener = CCPServerBURListener::NewL(this);
    iCountSession = 0;
    iBURLock = iBURListener->CheckBUR();
    iDataManager = CCPDataManager::NewL(iBURLock);
    iActionHandlerThread = CCPActionHandlerThread::NewL();
    if (!iBURLock)
        {
        TRAP_IGNORE( DeactivatePublishersL() );
        }
    }

// -----------------------------------------------------------------------------
// CCPServer::NewL
// Two-phased constructor.
// -------------------------------------- ---------------------------------------
//
CCPServer* CCPServer::NewLC()
    {
    CP_DEBUG( _L8("CCPServer::NewLC()" ) );
    CCPServer* self = new( ELeave ) CCPServer( EPriorityNormal );
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// -----------------------------------------------------------------------------
// CCPServer::~CCPServer()
// Destructor.
// -------------------------------------- ---------------------------------------
//    
CCPServer::~CCPServer()
    {
    CP_DEBUG( _L8("CCPServer::~CCPServer()" ) );
    for ( TInt i(0); i< iNotifications.Count( ); i++ )
        {
        iNotifications[i]->Close( );
        }
    iNotifications.Close( );    
    delete iDataManager;
    delete iActionHandlerThread;
    delete iBURListener;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPServer::RemoveSession()
    {
    CP_DEBUG( _L8("CCPServer::RemoveSession()" ) );
    iCountSession--;
    if ( !iCountSession )
        {
        Stop( );
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
void CCPServer::AddSession()
    {
    CP_DEBUG( _L8("CCPServer::AddSession()" ) );
    iCountSession++;
    }

// ---------------------------------------------------------------------------
// CCPServer::HandleBUREventL
// ---------------------------------------------------------------------------
//
void CCPServer::HandleBUREventL( TBURStatus aStatus )
    {
    CP_DEBUG( _L8("CCPServer::HandleBUREventL()" ) );
    if ( (aStatus == EBURStatusBackup ) || (aStatus == EBURStatusRestore ) )
        {
        iBURLock = ETrue;
        iDataManager->CloseDatabase( );
        }
    else
        {
        iDataManager->OpenDatabaseL( );
        iBURLock = EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CCPServer::GetLock
// ---------------------------------------------------------------------------
//
TBool CCPServer::GetLock()
    {
    return iBURLock;
    }

// -----------------------------------------------------------------------------
// CCPServer::GetNotifications
// -----------------------------------------------------------------------------
//
RPointerArray<CLiwDefaultList>& CCPServer::GetNotifications( ) 
    {
    return iNotifications;
    }


// -----------------------------------------------------------------------------
// CCPServer::NewSessionL
// Create new session.
// -----------------------------------------------------------------------------
//
CSession2* CCPServer::NewSessionL( const TVersion& /*aVersion*/,
    const RMessage2& /*aMessage*/) const
    {
    CP_DEBUG( _L8("CCPServer::NewSessionL()" ) );
    TPointersForSession passer;
    passer.iDataManager = iDataManager;
    passer.iServer = const_cast<CCPServer*>(this);
    passer.iActionHandlerThread = iActionHandlerThread;
    return CCPServerSession::NewL( passer );
    }

// -----------------------------------------------------------------------------
// CCPServer::RunError
// RunError is called when RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CCPServer::RunError( TInt /*aError*/)
    {
    ReStart( );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCPServer::PanicClient
// Panic client.
// -----------------------------------------------------------------------------
//
void CCPServer::PanicClient( const RMessage2& aMessage, TCPServerPanic aPanic )
    {
    aMessage.Panic( KCPServerName, aPanic );
    }

// -----------------------------------------------------------------------------
// CCPServer::PanicServer
// Panic server.
// -----------------------------------------------------------------------------
//
void CCPServer::PanicServer( TCPServerPanic aPanic )
    {
    User::Panic( KCPServerName, aPanic );
    }


// -----------------------------------------------------------------------------
// CCPServer::SendNotificationL
// -----------------------------------------------------------------------------
//
void CCPServer::AppendNotificationL( CCPLiwMap* aMap ) 
    {
    aMap->InsertL( KType, TLiwVariant( KPublisher ) );
    aMap->InsertL( KOperation, TLiwVariant( KOperationUpdate ) );
    CLiwDefaultList* list = CLiwDefaultList::NewLC( );
    list->AppendL( TLiwVariant( aMap ) );
    iNotifications.AppendL( list );
    CleanupStack::Pop( list );
    }

// -----------------------------------------------------------------------------
// CCPServer::Stop
// Stop serwer
// -----------------------------------------------------------------------------
//
void CCPServer::Stop()
    {
    CP_DEBUG( _L8("CCPServer::Stop()" ) );
    CActiveScheduler::Stop( );
    }

// -----------------------------------------------------------------------------
// CCPServer::ThreadStart(void)
// Starts server.
// -----------------------------------------------------------------------------
//
TInt CCPServer::ThreadStart( void )
    {
    __UHEAP_MARK;
	// Create the server, if one with this name does not already exist.
	TFindServer findCountServer( KCPServerName );
	TFullName name;
	
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
    if ( !trapCleanup )
        {
        PanicServer( ECPServerSrvCreateServer );
        }
    CActiveScheduler* activeScheduler = new CActiveScheduler;
    if ( !activeScheduler )
        {
        PanicServer( ECPServerSrvCreateServer );
        }
    CActiveScheduler::Install( activeScheduler );

    // Need to check that the server exists.
    if ( findCountServer.Next( name ) != KErrNone )
        {
        User::RenameThread( KCPServerName );

        TRAPD( err, CreateAndRunServerL() );
        if ( err != KErrNone )
            {
            PanicServer( ECPServerSrvCreateServer );
            }
        }
    
    delete activeScheduler;
    delete trapCleanup;
    __UHEAP_MARKEND;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CCPServer::CreateAndRunServerL(void)
// Creates and runs server.
// -----------------------------------------------------------------------------
//
void CCPServer::CreateAndRunServerL( void )
    {
    // Construct server
    //
    CCPServer* server = CCPServer::NewLC( );
    // Initialisation complete, now signal the client
    //
    RProcess::Rendezvous( KErrNone );
    CActiveScheduler::Start( );    
    CleanupStack::PopAndDestroy( server );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//	
void CCPServer::DeactivatePublishersL()
    {
    CLiwGenericParamList* publishers = CLiwGenericParamList::NewLC( );
    CCPLiwMap* map = CPublisherRegistryMap::NewLC( );
    iDataManager->GetActivePublishersL( publishers );
    TInt pos( 0 );
    while( pos != KErrNotFound )
        {
        publishers->FindFirst( pos, KListMap );

        if( pos != KErrNotFound )
            {
            TLiwVariant variant = ( *publishers )[pos++].Value( );
	        map->Reset( );
            variant.Get( *map );
            
	        // update flag value in the database
	        TRAP_IGNORE( ResetAndUpdateFlagL( map ) );

            //append update notification
            TRAP_IGNORE( AppendNotificationL( map ) );

         	variant.Reset( );
            }
        }
    CleanupStack::PopAndDestroy( map );
    CleanupStack::PopAndDestroy( publishers );
    }

// -----------------------------------------------------------------------------
// CCPServer::ResetAndUpdateFlagL
// Resets Activate flag and update item in the DB
// -----------------------------------------------------------------------------
//	
void CCPServer::ResetAndUpdateFlagL( CCPLiwMap* aMap )
    {
   	TInt32 flag(0);
    if( !aMap->GetProperty( KFlag, flag ))
        {
        User::Leave(KErrNotFound);
        }
    aMap->Remove( KFlag );
    flag &= 0xFFFFFFFE;
    aMap->InsertL( KFlag, TLiwVariant( flag ) );                
    iDataManager->AddDataL( *aMap );
    }

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// E32Main entry point.
// Returns: KErrNone
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    RAllocator* iAllocator = MemoryManager::SwitchToFastAllocator();
    
    //Get the return value (needs to call CloseFastAllocator() before return)
    TInt iReturnValue = CCPServer::ThreadStart( );
    MemoryManager::CloseFastAllocator(iAllocator);
    return iReturnValue;  
    }

// End of File
