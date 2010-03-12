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
* Description:  Content Publisher Server Session
 *
*/


// INCLUDE FILES
#include <s32mem.h>
// for CleanupResetAndDestroyPushL
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <liwgenericparam.h>

#include "cpserversession.h"
#include "cpliwmap.h"
#include "cpublisherregistrymap.h"
#include "cpsecuritypolicy.h"
#include "cpserverdef.h"
#include "cpdebug.h"
#include "cpserver.h"
#include "cpserverdatamanager.h"
#include "cpactionhandlerthread.h"
#include "cpnotificationhandler.h"


using namespace LIW;

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CCPServerSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCPServerSession* CCPServerSession::NewL( TPointersForSession& aPasser )
    {
    CCPServerSession* self = CCPServerSession::NewLC( aPasser );
    CleanupStack::Pop( self ) ;
    return self;
    }

// -----------------------------------------------------------------------------
// CCPServerSession::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCPServerSession* CCPServerSession::NewLC( TPointersForSession& aPasser )
    {
    CCPServerSession* self = new ( ELeave ) CCPServerSession();
    CleanupStack::PushL( self );
    self->ConstructL( aPasser ) ;
    return self;
    }

// -----------------------------------------------------------------------------
// CCPServerSession::~CCPServerSession
// Destructor.
// -----------------------------------------------------------------------------
//
CCPServerSession::~CCPServerSession()
    {
    //remove notification handler from an array of sessions in data manager
    if ( iNotificationHandler && iDataManager )
        {
        iDataManager->RemoveObserver( iNotificationHandler );
        }
    if ( isRegister && iNotificationHandler )
        {
        iNotificationHandler->ErrorComplete( KErrCancel );
        }
    if ( iServer )
        {
        iServer->RemoveSession( );
        }
    delete iNotificationHandler;
    delete iParamList;
    }

// -----------------------------------------------------------------------------
// CCPServerSession::CCPServerSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCPServerSession::CCPServerSession() :
    CSession2()
    {

    }

// -----------------------------------------------------------------------------
// CCPServerSession::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCPServerSession::ConstructL( TPointersForSession& aPasser )
    {
    CP_DEBUG( _L8("CCPServerSession::ConstructL()") );
    iDataManager = aPasser.iDataManager;
    iServer = aPasser.iServer;
    iActionHandlerThread = aPasser.iActionHandlerThread;
    iServer->AddSession( );
    isRegister = EFalse;
    }

// -----------------------------------------------------------------------------
// CCPServerSession::ServiceL
// Handle client requests.
// -----------------------------------------------------------------------------
//
void CCPServerSession::ServiceL( const RMessage2& aMessage )
    {
    CP_DEBUG( _L8("CCPServerSession::ServiceL()" ) );
    TBool serverLocked = GetServerLock( aMessage );
    if ( serverLocked )
        {
        aMessage.Complete( KErrLocked );
        }
    else
        {
        TInt err(KErrNone);
        TBool panicedClient(EFalse);
        TRAP( err , DispatchMessageL( aMessage, panicedClient ) );
        if ( (!(aMessage.Function( ) == ECpServerRegisterObserver ) 
            || err == KErrInUse) && !panicedClient )
            {
            aMessage.Complete( err );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCPServerSession::ServiceL
// Handle client requests.
// -----------------------------------------------------------------------------
//
void CCPServerSession::DispatchMessageL( const RMessage2& aMessage, TBool& aPanicedClient )
    {
    CP_DEBUG( _L8("CCPServerSession::DispatchMessageL()" ) );
    switch ( aMessage.Function( ) )
        {
        case ECpServerAddData:
            AddDataL( aMessage );
            break;
        case ECpServerAddDataNonPersistent:
            AddDataNonPersistentL( aMessage );
            break;
        case ECpServerGetListSize:
            GetListSizeL( aMessage );
            break;
        case ECpServerGetListData:
            GetListDataL( aMessage );
            break;
        case ECpServerRemoveData:
            RemoveDataL( aMessage );
            break;
        case ECpServerRegisterObserver:
            RegisterObserverL( aMessage );
            break;
        case ECpServerAddObserver:
            AddObserverL( aMessage );
            break;
        case ECpServerRemoveObserver:
            RemoveObserverL( aMessage );
            break;
        case ECpServerUnRegisterObserver:
            UnregisterObserverL( );
            break;
        case ECpServerGetChangeInfoData:
            GetChangeInfoDataL( aMessage );
            break;
        case ECpServerExecuteAction:
            ExecuteActionL( aMessage );
            break;
        default:
            iServer->PanicClient( aMessage, ECPServerBadRequest );
            aPanicedClient = ETrue;
            break;
        }
    }

// -----------------------------------------------------------------------------
// CCPServerSession::AddDataL
// --------------- --------------------------------------------------------------
//
void CCPServerSession::AddDataL( const RMessage2& aMessage )
    {
    CP_DEBUG( _L8("CCPServerSession::AddDataL()" ) );
    CCPLiwMap* map = UnpackFromClientLC( aMessage );  
    map->SetSecurityL( aMessage );
    TBool activateSupport = map->ActivateActionSupport( );
    TUint id( 0 );
    id = iDataManager->AddDataL( *map );
    TPckg<TUint> idData(id);
    aMessage.WriteL( KReturnPosition, idData );
	if(activateSupport)
	    {
	    CLiwDefaultList* list = CLiwDefaultList::NewLC( );
	    // execute activation or deactivation action for publisher
	    TRAP_IGNORE( GetAndExecuteActionL( map, list ) );
	    TRAP_IGNORE( SendNotificationL( map, list ) );
	    CleanupStack::PopAndDestroy( list );
	    }
    CleanupStack::PopAndDestroy( map );
    }

// -----------------------------------------------------------------------------
// CCPServerSession::AddDataNonPersistentL
// --------------- --------------------------------------------------------------
//
void CCPServerSession::AddDataNonPersistentL( const RMessage2& aMessage )
    {
    CP_DEBUG( _L8("CCPServerSession::AddDataL()" ) );
    CCPLiwMap* map = UnpackFromClientLC( aMessage );
    map->SetSecurityL( aMessage );
    TUint id( 0 );
    TPckg<TUint> idData(id);
    iDataManager->AddNonPersistentDataL( map );
    aMessage.WriteL( KReturnPosition, idData );
    CleanupStack::PopAndDestroy( map );
    }
// -----------------------------------------------------------------------------
// CCPServerSession::GetListL
// --------------- --------------------------------------------------------------
//
void CCPServerSession::GetListSizeL( const RMessage2& aMessage )
    {
    CP_DEBUG( _L8("CCPServerSession::GetListSizeL()" ) );
    CCPLiwMap* map = UnpackFromClientLC( aMessage );
    map->SetSecurityL( aMessage );
    delete iParamList;
    iParamList = NULL;
    iParamList = CLiwGenericParamList::NewL( );
    iDataManager->GetListL( *map, *iParamList );
    TPckg<TInt> sizeDes(iParamList->Size( ) );
    aMessage.WriteL( KReturnPosition, sizeDes );
    CleanupStack::PopAndDestroy( map );
    }

// -----------------------------------------------------------------------------
// CCPServerSession::GetListDataL
// --------------- --------------------------------------------------------------
//
void CCPServerSession::GetListDataL( const RMessage2& aMessage )
    {
    CP_DEBUG( _L8("CCPServerSession::GetListDataL()" ) );
    if( iParamList )
        {
        ExternalizeAndWriteToClientL( aMessage, iParamList );
        delete iParamList;
        }
    iParamList = NULL;
    }

// -----------------------------------------------------------------------------
// CCPServerSession::RemoveDataL
// --------------- --------------------------------------------------------------
//
void CCPServerSession::RemoveDataL( const RMessage2& aMessage )
    {
    CP_DEBUG( _L8("CCPServerSession::RemoveDataL()" ) );
    CCPLiwMap* map = UnpackFromClientLC( aMessage );
    map->SetSecurityL( aMessage );
    iDataManager->RemoveDataL( *map );
    CleanupStack::PopAndDestroy( map );
    }

// -----------------------------------------------------------------------------
// CCPServerSession::ExecuteActionL
// --------------- --------------------------------------------------------------
//
void CCPServerSession::ExecuteActionL( const RMessage2& aMessage )
    {
    TInt error(KErrNone);
    CP_DEBUG( _L8("CCPServerSession::ExecuteActionSizeL()" ) );

    TUint options = static_cast<TUint>( aMessage.Int2() ); // 2 == KOptionsPosition

    CCPLiwMap* map = UnpackFromClientLC( aMessage );
    CLiwGenericParamList* paramList = CLiwGenericParamList::NewLC( );
    CLiwDefaultList* list = CLiwDefaultList::NewLC();
    error = iDataManager->GetActionL( *map, *paramList, list );
    //we notify apart from action execution result. So in fact
    //notification means there was an attempt to execute action
    if ( !( options & KDisableNotification ) )
        {
        iDataManager->HandleChangeL( list );
        }
    User::LeaveIfError( error );
    ExecuteL( *paramList );    
    CleanupStack::PopAndDestroy( list );
    CleanupStack::PopAndDestroy( paramList );
    CleanupStack::PopAndDestroy( map );
    }

// -----------------------------------------------------------------------------
// CCPServerSession::ExecuteL
// --------------- --------------------------------------------------------------
//
void CCPServerSession::ExecuteL(
        const CLiwGenericParamList& aActionParams )
    {
    for ( TInt i = 0; i < aActionParams.Count(); i++ )
    	{
		TLiwGenericParam param;
 	    param.PushL();
    	aActionParams.AtL( i, param );
    	CLiwGenericParamList* singleAction = CLiwGenericParamList::NewLC();
    	singleAction->AppendL( param );
    	TRAP_IGNORE( iActionHandlerThread->ExecuteL( *singleAction ) );
    	CleanupStack::PopAndDestroy( singleAction );
		CleanupStack::PopAndDestroy( &param );
    	}
    }

// -----------------------------------------------------------------------------
// CCPServerSession::RegisterObserverL
// --------------- --------------------------------------------------------------
//
void CCPServerSession::RegisterObserverL( const RMessage2& aMessage )
    {
    CP_DEBUG( _L8("CCPServerSession::RegisterObserverL()" ) );
    if ( !isRegister )
        {
        if (aMessage.Int3() != KErrAlreadyExists &&
            aMessage.GetDesLength( KDescriptorPosition) < KErrNone )
            {
            iServer->PanicClient( aMessage, ECPServerBadRequest );
            User::Leave( KErrGeneral );
            }
        if ( !iNotificationHandler )
            {
            iNotificationHandler = CCPNotificationHandler::NewL(
                                               iServer->GetNotifications());
            iDataManager->AddObserverL( iNotificationHandler );
            }
        iNotificationHandler->SaveMessageL( aMessage );
        isRegister = ETrue;
        }
    else
        {
        User::Leave( KErrInUse );
        }
    }

// -----------------------------------------------------------------------------
// CCPServerSession::AddObserverL
// --------------- --------------------------------------------------------------
//
void CCPServerSession::AddObserverL( const RMessage2& aMessage )
    {
    CP_DEBUG( _L8("CCPServerSession::AddObserverL()" ) );
    if( iNotificationHandler )
        {
        iNotificationHandler->AddObserverL( aMessage );
        }
    }

// -----------------------------------------------------------------------------
// CCPServerSession::RemoveObserverL
// --------------- --------------------------------------------------------------
//
void CCPServerSession::RemoveObserverL( const RMessage2& aMessage )
    {
    CP_DEBUG( _L8("CCPServerSession::RemoveObserverL()" ) );
    if( iNotificationHandler )
        {
        iNotificationHandler->RemoveObserverL( aMessage );
        }
    }


// -----------------------------------------------------------------------------
// CCPServerSession::UnregisterObserverL
// --------------- --------------------------------------------------------------
//
void CCPServerSession::UnregisterObserverL()
    {
    CP_DEBUG( _L8("CCPServerSession::UnregisterObserverL()" ) );
    if ( isRegister )
        {
        //remove notification handler from an array of sessions in data manager
        iDataManager->RemoveObserver( iNotificationHandler );
        //reset filter for this session
        iNotificationHandler->ErrorComplete( KErrCancel );
        delete iNotificationHandler;
        iNotificationHandler = NULL;
        isRegister = EFalse;
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CCPServerSession::GetChangeInfoDataL
// -----------------------------------------------------------------------------
//
void CCPServerSession::GetChangeInfoDataL( const RMessage2& aMessage )
    {
    CP_DEBUG( _L8("CCPServerSession::GetChangeInfoData()" ) );
    if( iNotificationHandler )
        {
        isRegister = EFalse;
        ExternalizeAndWriteToClientL( aMessage,
            iNotificationHandler->GetPointerToChangeInfoList( ) );
        iNotificationHandler->Reset( );
        }
    }

// -----------------------------------------------------------------------------
// CCPServerSession::ExternalizeAndWriteToClient
// -----------------------------------------------------------------------------
//
void CCPServerSession::ExternalizeAndWriteToClientL(
    const RMessage2& aMessage, const CLiwGenericParamList* outParamList )
    {
    CP_DEBUG( _L8("CCPServerSession::ExternalizeAndWriteToClientL()" ) );
    HBufC8* buf = HBufC8::NewLC( outParamList->Size( ) );
    TPtr8 des = buf->Des( );
    RDesWriteStream writeStream(des);
    CleanupClosePushL( writeStream );
    outParamList->ExternalizeL( writeStream );
    writeStream.CommitL( );
    aMessage.WriteL( KDescriptorPosition, des );
    CleanupStack::PopAndDestroy( &writeStream );
    CleanupStack::PopAndDestroy( buf );
    }

// -----------------------------------------------------------------------------
// 
// -----------------------------------------------------------------------------
//
CCPLiwMap* CCPServerSession::UnpackFromClientLC( const RMessage2& aMessage )
    {
    CP_DEBUG( _L8("CCPServerSession::UnpackFromClientLC()") );
    TInt deslen = aMessage.GetDesLengthL( KDescriptorPosition );
    HBufC8* buffer = HBufC8::NewLC( deslen );
    TPtr8 tempDes = buffer->Des( );
    aMessage.Read( KDescriptorPosition, tempDes );
    RDesReadStream datastrm( *buffer);
    CleanupClosePushL( datastrm );
    CCPLiwMap* inParamList = CCPLiwMap::NewL( datastrm );
    CleanupStack::PopAndDestroy( &datastrm );
    CleanupStack::PopAndDestroy( buffer );
    inParamList->PushL( );
    return inParamList;
    }
    
// -----------------------------------------------------------------------------
// CCPServerSession::SendNotificationL
// -----------------------------------------------------------------------------
//
void CCPServerSession::SendNotificationL( CCPLiwMap* aMap, 
		CLiwDefaultList* aNotificationList ) 
    {
    RBuf8 trigger;
    trigger.CleanupClosePushL();
    aMap->GetPropertyL( KActionTrigger, trigger );
    if( ( trigger == KActivateTrigger ) || ( trigger == KDeactivateTrigger ) )
        {
        iDataManager->HandleChangeL( aNotificationList );
        }
    CleanupStack::PopAndDestroy( &trigger );
    }

// -----------------------------------------------------------------------------
// CCPServerSession::GetAndExecuteActionL
// --------------- --------------------------------------------------------------
//
void CCPServerSession::GetAndExecuteActionL( CCPLiwMap* aMap,
		CLiwDefaultList* aNotificationList, TBool aInsertTrigger )
    {
    if (aInsertTrigger)
        {
        aMap->InsertL( KActionTrigger, TLiwVariant( KActivateTrigger ) ); 
        }
    CLiwGenericParamList* paramList = CLiwGenericParamList::NewLC();
    iDataManager->GetActionL( *aMap, *paramList, aNotificationList );
    iActionHandlerThread->ExecuteL( *paramList );    
    CleanupStack::PopAndDestroy( paramList );
    }
 
// -----------------------------------------------------------------------------
// CCPServerSession::GetServerLock
// --------------- --------------------------------------------------------------
//
TBool CCPServerSession::GetServerLock( const RMessage2& aMessage )
	{
	// Allways allow to unregister
	return ( (aMessage.Function() != ECpServerUnRegisterObserver) 
			&& iServer->GetLock() ); 
			
	}
// End of File
