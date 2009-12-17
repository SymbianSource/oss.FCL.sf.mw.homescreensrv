/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Content Harvester Server
 *
*/


//#include <eikenv.h>
//#include <eikappui.h>

#include <fbs.h>

#include "contentharvesterserver.h"
#include "contentharvestersession.h"
#include "contentharvesterglobals.h"
#include "contentharvesterengine.h"
#include "cpdebug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CContentHarvesterServer::CContentHarvesterServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CContentHarvesterServer::CContentHarvesterServer( TInt aPriority ) :
    CServer2(aPriority)
    {
    }

// -----------------------------------------------------------------------------
// CContentHarvesterServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CContentHarvesterServer::ConstructL()
    {
    CP_DEBUG( _L8("Start Content Harvester Server" ));
    StartL( KContentHarvesterName );
    iEngine = CContentHarvesterEngine::NewL( );
    }

// -----------------------------------------------------------------------------
// CContentHarvesterServer::NewL
// Two-phased constructor.
// -------------------------------------- ---------------------------------------
//
CContentHarvesterServer* CContentHarvesterServer::NewLC()
    {
    CContentHarvesterServer* self = new( ELeave )
    CContentHarvesterServer( EPriorityNormal );
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// Destructor
CContentHarvesterServer::~CContentHarvesterServer()
    {
    CP_DEBUG( _L8("Destroy Content Harvester Server" ));
    delete iEngine;
    }

// -----------------------------------------------------------------------------
// CContentHarvesterServer::NewSessionL
// Create new session.
// -----------------------------------------------------------------------------
//
CSession2* CContentHarvesterServer::NewSessionL( const TVersion& aVersion,
    const RMessage2& /*aMessage*/ ) const
    {
    CP_DEBUG(_L8("CContentHarvesterServer::NewSessionL" ));
    // Check we're the right version
    if ( !User::QueryVersionSupported( TVersion(
        KContentHarvesterMajorVersionNumber,
        KContentHarvesterMinorVersionNumber,
        KContentHarvesterBuildVersionNumber ), aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }
    return CContentHarvesterSession::NewL( ( CContentHarvesterServer* ) this );
    }

// -----------------------------------------------------------------------------
// CContentHarvesterServer::RunError
// RunError is called when RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CContentHarvesterServer::RunError( TInt aError )
    {
    CP_DEBUG( _L8("CContentHarvesterServer::RunError " ));
    if ( aError == KErrBadDescriptor )
        {
        PanicClient( Message( ), EContentHarvesterBadDescriptor );
        }
    else
        {
        Message().Complete( aError );
        }

    // The leave will result in an early return from CServer::RunL(), skipping
    // the call to request another message. So do that now in order to keep the
    // server running.
    ReStart( );
    // Handled the error fully
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CContentHarvesterServer::PanicClient
// Panic client.
// -----------------------------------------------------------------------------
//
void CContentHarvesterServer::PanicClient( const RMessage2& aMessage,
    TContentHarvesterPanic aPanic )
    {
    aMessage.Panic( KContentHarvesterName, aPanic );
    }

// -----------------------------------------------------------------------------
// CContentHarvesterServer::PanicServer
// Panic server.
// -----------------------------------------------------------------------------
//
void CContentHarvesterServer::PanicServer( TContentHarvesterPanic aPanic )
    {
    User::Panic( KContentHarvesterName, aPanic );
    }

// -----------------------------------------------------------------------------
// CContentHarvesterServer::ThreadFunctionL
// Create and start the server.
// -----------------------------------------------------------------------------
//
void CContentHarvesterServer::ThreadFunctionL()
    {
    RFbsSession::Connect( );
    // Construct our server        

    CContentHarvesterServer* server = CContentHarvesterServer::NewLC( );
    CP_DEBUG(_L8("Send Rendezvous" ));
    RProcess::Rendezvous( KErrNone );

    CActiveScheduler::Start( );

    CleanupStack::PopAndDestroy( server );

    RFbsSession::Disconnect( );
    }
// -----------------------------------------------------------------------------
// CContentHarvesterServer::ThreadFunctionL
// Create and start the server.
// -----------------------------------------------------------------------------
//
TInt CContentHarvesterServer::ThreadFunction()
    {
    __UHEAP_MARK;

	TFindServer findCountServer( KContentHarvesterName );
	TFullName   name;
    
	// Need to check that the server exists.
	if (findCountServer.Next(name) != KErrNone)
		{
		User::RenameThread( KContentHarvesterThreadName );
	
		CTrapCleanup* cleanupStack = CTrapCleanup::New( );
		if ( !(cleanupStack ) )
			{
			PanicServer( EContentHarvesterCreateTrapCleanup );
			}
	
		CContentHarvesterEikonEnv* env = new CContentHarvesterEikonEnv;
		__ASSERT_ALWAYS( env, PanicServer( EContentHarvesterEIkonEnv ) );
	
		CContentHarvesterAppUi* ui = new CContentHarvesterAppUi;
		__ASSERT_ALWAYS( ui, PanicServer( EContentHarvesterEIkonEnv ) );
	
		TRAPD(error, env->ConstructL(); ui->ConstructL());
		__ASSERT_ALWAYS( !error, PanicServer( EContentHarvesterEIkonEnv ) );
		
		TRAPD( err, ThreadFunctionL() );
		if ( err != KErrNone )
			{
			PanicServer( EContentHarvesterSrvCreateServer );
			}
		ui->PrepareToExit( );
		env->DestroyEnvironment( );
	
		delete cleanupStack;
		cleanupStack = NULL;
		}
	
    __UHEAP_MARKEND;

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CContentHarvesterServer::Engine
// Getter of engine
// -----------------------------------------------------------------------------
//
CContentHarvesterEngine& CContentHarvesterServer::Engine()
    {
    return *iEngine;
    }
// -----------------------------------------------------------------------------
// CContentHarvesterServer::Engine
// Getter of engine
// -----------------------------------------------------------------------------
//
void CContentHarvesterServer::Stop()
    {
    CActiveScheduler::Stop( );
    }

// -----------------------------------------------------------------------------
// CContentHarvesterEikonEnv::DestroyEnvironment
// 
// -----------------------------------------------------------------------------
//
void CContentHarvesterServer::CContentHarvesterEikonEnv::DestroyEnvironment()
    {
    CEikonEnv::DestroyEnvironment( );
    }

// -----------------------------------------------------------------------------
// CContentHarvesterEikonEnv::ConstructL
// 
// -----------------------------------------------------------------------------
//
void CContentHarvesterServer::CContentHarvesterEikonEnv::ConstructL()
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
CContentHarvesterServer::CContentHarvesterAppUi::~CContentHarvesterAppUi()
    {
    }

// -----------------------------------------------------------------------------
// CContentHarvesterAppUi::ConstructL
// 
// -----------------------------------------------------------------------------
//
void CContentHarvesterServer::CContentHarvesterAppUi::ConstructL()
    {
    CEikAppUi::BaseConstructL( ENoAppResourceFile|ENoScreenFurniture );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
//
TErrorHandlerResponse CContentHarvesterServer::CContentHarvesterAppUi::HandleError( TInt aError, 
                                            const SExtendedError& /*aExtErr*/, 
                                            TDes& /*aErrorText*/, 
                                            TDes& /*aContextText*/ )
    {
    RDebug::Printf("CContentHarvesterAppUi::HandleError %d", aError);
	if ( aError != KErrNoMemory )       
        {       
        return ENoDisplay; // we dont want to handle this error       
        }       
    return EErrorNotHandled;  
    }
	
// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// E32Main entry point.
// Returns: KErrNone
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    return CContentHarvesterServer::ThreadFunction( );
    }

// End of File

