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
* Description:  BS Server - implemenration of CServer2 
 *
*/


#include <eikenv.h>
#include <eikappui.h>
#include <apgwgnam.h>
#include <e32base.h>

#include "bsengineglobals.h"
#include "bsengine.h"
#include "bsserversession.h"
#include "bsdebug.h"
#include "bsserver.h"

const TInt idsArrayGranularity = 30;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CBSServer::CBSServer( TInt aPriority ) :
    CServer2(aPriority)
    {
    }

// -----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBSServer::ConstructL()
    {
    DEBUG(("Start BS Server" ));
    iEngine = CBSEngine::NewL( );
    StartL( KBSEngineName );
    }

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -------------------------------------- ---------------------------------------
//
CBSServer* CBSServer::NewLC()
    {
    CBSServer* self = new( ELeave ) CBSServer( EPriorityNormal );
    CleanupStack::PushL( self );
    self->ConstructL( );
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor.
// -------------------------------------- ---------------------------------------
//
CBSServer::~CBSServer()
    {
    DEBUG(("Destroy BS Server" ));
    delete iEngine;
    }

// -----------------------------------------------------------------------------
// Create new session.
// -----------------------------------------------------------------------------
//
CSession2* CBSServer::NewSessionL( const TVersion& aVersion,
    const RMessage2& /*aMessage*/ ) const
    {
    // Check we're the right version
    if ( !User::QueryVersionSupported( TVersion( KBSEngineMajorVersionNumber,
        KBSEngineMinorVersionNumber, KBSEngineBuildVersionNumber ), aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }

    return CBSServerSession::NewL( ( CBSServer* ) this );
    }

// -----------------------------------------------------------------------------
// Increment sessions.
// -----------------------------------------------------------------------------
//
void CBSServer::IncrementSessions()
    {
    iSessionCount++;
    }

// -----------------------------------------------------------------------------
// Decrement sessions.
// -----------------------------------------------------------------------------
//
void CBSServer::DecrementSessions()
    {
    iSessionCount--;
    }
// -----------------------------------------------------------------------------
// RunError is called when RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CBSServer::RunError( TInt aError )
    {
    DEBUG(("CBSServer::RunError %d", aError ));
    if ( aError == KErrBadDescriptor )
        {
        PanicClient( Message( ), EBSEngineBadDescriptor );
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
// Panic client.
// -----------------------------------------------------------------------------
//
void CBSServer::PanicClient( const RMessage2& aMessage, TBSEnginePanic aPanic )
    {
    aMessage.Panic( KBSEngineName, aPanic );
    }

// -----------------------------------------------------------------------------
// Panic server.
// -----------------------------------------------------------------------------
//
void CBSServer::PanicServer( TBSEnginePanic aPanic )
    {
    User::Panic( KBSEngineName, aPanic );
    }

// -----------------------------------------------------------------------------
// Create and start the server.
// -----------------------------------------------------------------------------
//
void CBSServer::ThreadFunctionL()
    {
    CBSAppUi* ui = new (ELeave) CBSAppUi;
    ui->ConstructL( );

    // Construct our server        
    CBSServer* server = CBSServer::NewLC( );

    // set engine to AppUI to get notification about focus changes
    ui->iEngine = &(server->Engine( ));

    RProcess::Rendezvous( KErrNone );

    // Start handling requests
    CActiveScheduler::Start( );

    CleanupStack::PopAndDestroy( server );

    ui->PrepareToExit( );

    }
// -----------------------------------------------------------------------------
// Create and start the server.
// -----------------------------------------------------------------------------
//
TInt CBSServer::ThreadFunction()
    {
    __UHEAP_MARK;

    User::RenameThread( KBSEngineThreadName );

    CBSEikonEnv* env = new CBSEikonEnv;
    __ASSERT_ALWAYS( env, PanicServer( EBSEngineEIkonEnv ) );

    TRAPD(error, env->ConstructL(););
    __ASSERT_ALWAYS( !error, PanicServer( EBSEngineEIkonEnv ) );

    TRAPD( err, ThreadFunctionL() );
    if ( err != KErrNone )
        {
        PanicServer( EBSEngineSrvCreateServer );
        }

    env->DestroyEnvironment( );

    __UHEAP_MARKEND;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Getter of engine
// -----------------------------------------------------------------------------
//
CBSEngine& CBSServer::Engine()
    {
    return *iEngine;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CBSServer::CBSEikonEnv::DestroyEnvironment()
    {
    CEikonEnv::DestroyEnvironment( );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CBSServer::CBSEikonEnv::ConstructL()
    {
    CEikonEnv::ConstructL( EFalse ); 
    SetAutoForwarding(ETrue); 
    User::SetPriorityControl(EFalse);
    }
// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CBSServer::CBSAppUi::~CBSAppUi()
    {
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CBSServer::CBSAppUi::HandleWsEventL( const TWsEvent& aEvent,
    CCoeControl* /*aDestination*/)
    {
    if ( aEvent.Type( ) == EEventFocusGroupChanged 
        || aEvent.Type( ) == EEventWindowGroupsChanged )
        {

        TInt windowsGroupID = iCoeEnv->WsSession().GetFocusWindowGroup( );
        CApaWindowGroupName* apaWGName = CApaWindowGroupName::NewLC(
            iCoeEnv->WsSession( ), windowsGroupID );
        TUid uid = apaWGName->AppUid( );
        CleanupStack::PopAndDestroy( apaWGName );
        // if UID is 0, ignore event. 
        // Next event should bring correct information
        if ( uid.iUid )
            {
            iEngine->HandleFocusChangeL( uid );
            }
        }
    else if( aEvent.Type( ) == EEventKeyDown ) 
        {
        iEngine->ApplicationKeyWasPressed( );
        ForwardEventL( aEvent );     
        }
    else if( aEvent.Type( ) == EEventKeyUp )
        {
        ForwardEventL( aEvent );     
        }
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CBSServer::CBSAppUi::ForwardEventL( const TWsEvent& aEvent )
    {
    CArrayFixFlat<TInt>* idArray = 
                    new (ELeave) CArrayFixFlat<TInt>( idsArrayGranularity );
    iCoeEnv->WsSession().WindowGroupList( idArray );
    
    TInt ownId = iCoeEnv->RootWin().Identifier();
    for( TInt x(0); x < idArray->Count(); x++ )
        {
        if( (*idArray)[x] != ownId )
            {
            iCoeEnv->WsSession().SendEventToWindowGroup( 
                                                    (*idArray)[x], aEvent );
            }
        }
    delete idArray;
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
void CBSServer::CBSAppUi::ConstructL()
    {
    CEikAppUi::BaseConstructL( ENoAppResourceFile | ENoScreenFurniture );
    //get notifications about focus change events    
    iCoeEnv->RootWin().EnableFocusChangeEvents( );
    //get notifications about windows group change events    
    iCoeEnv->RootWin().EnableGroupChangeEvents( );
    //disable notifications about layout change
    iCoeEnv->RootWin().DisableScreenChangeEvents();
    //get notifications about application key event
    iCoeEnv->RootWin().CaptureKeyUpAndDowns( EStdKeyApplication0, 0, 0 );
    }

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// E32Main item point.
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    return CBSServer::ThreadFunction( );
    }

// End of File

