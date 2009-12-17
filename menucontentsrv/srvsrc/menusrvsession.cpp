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
#include "menusrvsession.h"
#include "menusrvstream.h"
#include "menusrvoperation.h"
#include "menusrvobjectfilter.h"
#include "mcsmenunotifier.h" // for the notifier events
#include "menusrvnotifier.h"
#include "menusrvbuf.h"
#include "menumsg.h"
#include "menusrvdef.h"
#include "mcsmenuitem.h"
#include "mcsmenufilter.h"
#include "menuutil.h"
#include "menueng.h"
#include "menusrveng.h"
#include "menuengobject.h"
#include <s32buf.h>
#include <s32strm.h>


const TInt KParamIndAttrName = 1;
const TInt KParamIndAttrEx = 2;
const TInt KParamIndAttrVal = 3;
const TInt KParamIndBuf = 2;
const TInt KParamIndStream = 3;
const TInt KParamIndOp = 3;
const TInt KParamIndNotif = 3;

const TInt KGetListDescriptorPos = 0;
const TInt KGetListReturnPos = 1;

//const TInt KIconSize = 74;


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuSrvSession::~CMenuSrvSession
// ---------------------------------------------------------
//
CMenuSrvSession::~CMenuSrvSession()
    {
    delete iNotifierIx;
    delete iStreamIx;
    delete iOperationIx;
    iMenuSrv.RemoveContainer( iObjectCon );
    if ( iSharedEng )
        {
        iSharedEng->RemoveSession( this ); // Ownership release.
        }
    }

// ---------------------------------------------------------
// CMenuSrvSession::NewL
// ---------------------------------------------------------
//
CMenuSrvSession* CMenuSrvSession::NewL( CMenuSrv& aMenuSrv )
    {
    CMenuSrvSession* sess = new (ELeave) CMenuSrvSession( aMenuSrv );
    CleanupStack::PushL( sess );
    sess->ConstructL();
    CleanupStack::Pop( sess );
    return sess;
    }

// ---------------------------------------------------------
// CMenuSrvSession::CMenuSrvSession
// ---------------------------------------------------------
//
CMenuSrvSession::CMenuSrvSession( CMenuSrv& aMenuSrv )
: iMenuSrv( aMenuSrv )
    {
    }

// ---------------------------------------------------------
// CMenuSrvSession::ConstructL
// ---------------------------------------------------------
//
void CMenuSrvSession::ConstructL()
    {
    iObjectCon = iMenuSrv.NewContainerL();
    iNotifierIx = CObjectIx::NewL();
    iStreamIx = CObjectIx::NewL();
    iOperationIx = CObjectIx::NewL();
    }

// ---------------------------------------------------------
// CMenuSrvSession::CountResources
// ---------------------------------------------------------
//
TInt CMenuSrvSession::CountResources()
    {
    return iObjectCon->Count();
    }

// ---------------------------------------------------------
// CMenuSrvSession::ServiceL
// ---------------------------------------------------------
//
void CMenuSrvSession::ServiceL( const RMessage2& aMessage )
    {
    if( !ServiceCapabilityTestL( aMessage ) )
    	{
        // The rest of the functions does not work with a dead engine.
        if ( iEngineDead )
            {
            User::Leave( KErrDisconnected );
            }
        
        if( !ServiceCapReadL( aMessage ) )
        	{
        	if( !ServiceCapWriteL( aMessage) )
        		{
    			//Function was not serviced
        		aMessage.Complete( KErrNotSupported ); // Sync request -> complete it now.
        		}
        	}
    	}
    }

// ---------------------------------------------------------
// CMenuSrvSession::ServiceCapabilityTestL
// Handles functions with test capability
// ---------------------------------------------------------
//
TBool CMenuSrvSession::ServiceCapabilityTestL( const RMessage2& aMessage )
	{
	// These debug functions work even with a dead engine.
	TBool serviced = ETrue;
	switch( aMessage.Function() )
	    {
	    case EMenuResourceMarkStart:
	        {
	        ResourceCountMarkStart();
	        aMessage.Complete( KErrNone );
	        break;
	        }
	
	    case EMenuResourceMarkEnd:
	        {
	        ResourceCountMarkEnd( aMessage );
	        aMessage.Complete( KErrNone );
	        break;
	        }
	
	    case EMenuResourceCount:
	        {
	        aMessage.Complete( CountResources() );
	        break;
	        }
	
	    case EMenuSetHeapFailure:
	        {
	        User::__DbgSetAllocFail
	            (
	            RAllocator::EUser,
	            STATIC_CAST( RAllocator::TAllocFail, aMessage.Int0() ),
	            aMessage.Int1()
	            );
	        aMessage.Complete( KErrNone );
	        break;
	        }
	
	    default:
	        {
	        // Not a debug function.
	        serviced = EFalse;
	        break;
	        }
	    }
	return serviced;
	}

// ---------------------------------------------------------
// CMenuSrvSession::ServiceCapReadWriteL
// Handles functions with read device data capability
// ---------------------------------------------------------
//
TBool CMenuSrvSession::ServiceCapReadL( const RMessage2& aMessage )
	{
	TBool serviced = ETrue;
	switch( aMessage.Function() )
        {
		case EMenuConstructSession:
            {
            __ASSERT_ALWAYS( !iSharedEng, PanicClientL( aMessage ) );
            RBuf name;
            name.CreateL( aMessage.GetDesLengthL( 0 ) );
            CleanupClosePushL( name );
            aMessage.ReadL( 0, name );
            // Get the shared engine and open it.
            CMenuSrvEng* sharedEng = iMenuSrv.GetEngineL( name );
            CleanupClosePushL( *sharedEng );
            sharedEng->AddSessionL( this ); // Ownership transfer.
            // No leaving after this point.
            CleanupStack::Pop( sharedEng );
            iSharedEng = sharedEng;
            CleanupStack::PopAndDestroy( &name );
            break;
            };

        case EMenuRootFolder:
            {
            __ASSERT_ALWAYS( iSharedEng, PanicClientL( aMessage ) );
            TPckgBuf<TInt> root;
            TInt id;
            iSharedEng->Engine().RootFolderL( id );
            root() = id;
            aMessage.WriteL( 0, root );
            break;
            }

        case EMenuStreamClose:
            {
            TInt handle = aMessage.Int3();
            StreamL( handle ); // Check that handle is valid.
            iStreamIx->Remove( handle );
            break;
            };

        case EMenuStreamOpen:
            {
            NewStreamL( *(HMenuSrvBuf::NewL()), aMessage );
            break;
            }

        case EMenuStreamRead:
            {
            TInt handle = aMessage.Int3();
            TInt len;
            StreamL( handle ).ReadL( aMessage, len );
            break;
            }

        case EMenuStreamWrite:
            {
            TInt handle = aMessage.Int3();
            StreamL( handle ).WriteL( aMessage );
            break;
            }

        case EMenuGetHdr:
            {
            __ASSERT_ALWAYS( iSharedEng, PanicClientL( aMessage ) );
            TInt id = aMessage.Int0();
            TPckgBuf<TMenuItem> hdr;
            iSharedEng->Engine().GetItemL( id, hdr() );
            aMessage.WriteL( 1, hdr );
            break;
            }

        case EMenuGetItems:
            {
            __ASSERT_ALWAYS( iSharedEng, PanicClientL( aMessage ) );
            HMenuSrvBuf* streamBuf = HMenuSrvBuf::NewLC();
            RWriteStream stream( streamBuf );
            TInt folder = aMessage.Int0();
            TBool recursive = aMessage.Int1();
            RArray<TMenuItem> items;
            CleanupClosePushL( items );
            iSharedEng->Engine().GetItemsL( items, folder, NULL, recursive );
            MenuUtil::ExternalizeL( items, stream );
            stream.CommitL();
            CleanupStack::PopAndDestroy( &items );
            CleanupStack::Pop( streamBuf );
            NewStreamL( *(stream.Sink()), aMessage );
            break;
            }

        case EMenuGetRunningApps:
            {
            __ASSERT_ALWAYS( iSharedEng, PanicClientL( aMessage ) );
            HMenuSrvBuf* streamBuf = HMenuSrvBuf::NewLC();
            RWriteStream stream( streamBuf );
            RArray<TUid> items;
            CleanupClosePushL( items );
            iSharedEng->GetRunningAppsL( items );
            MenuUtil::ExternalizeL( items, stream );
            stream.CommitL();
            CleanupStack::PopAndDestroy( &items );
            CleanupStack::Pop( streamBuf );
            NewStreamL( *(stream.Sink()), aMessage );
            break;
            }            
            
        case EMenuGetItemsFiltered:
            {
            __ASSERT_ALWAYS( iSharedEng, PanicClientL( aMessage ) );
            TInt folder = aMessage.Int0();
            TBool recursive = aMessage.Int1();
            TInt bufHandle = aMessage.Int2();
            RArray<TMenuItem> items;
            CleanupClosePushL( items );
            MStreamBuf& buf = StreamL( bufHandle ).Host();
            RReadStream rs( &buf );
            CMenuFilter* filter = CMenuFilter::NewLC();
            filter->InternalizeL( rs );
            TMenuSrvObjectFilter engFilter( *filter );
            iSharedEng->Engine().GetItemsL
                ( items, folder, &engFilter, recursive );
            CleanupStack::PopAndDestroy( filter );
            RWriteStream stream( &buf );
            MenuUtil::ExternalizeL( items, stream );
            stream.CommitL();
            CleanupStack::PopAndDestroy( &items );
            break;
            }
            
        case EMenuGetListSize:
            {
            __ASSERT_ALWAYS( iSharedEng, PanicClientL( aMessage ) );
            TInt deslen = aMessage.GetDesLength( KGetListDescriptorPos );
            HBufC8* buffer = HBufC8::NewLC( deslen );
            TPtr8 tempDes = buffer->Des( );
            aMessage.Read( KGetListDescriptorPos, tempDes );
            TInt size = iSharedEng->GetListSizeL( *buffer );
            TPckg<TInt> sizeDes( size );
            aMessage.WriteL( KGetListReturnPos, sizeDes );
            CleanupStack::PopAndDestroy( buffer );
            break;
            }   

        case EMenuGetListData:
            {
            __ASSERT_ALWAYS( iSharedEng, PanicClientL( aMessage ) );
            aMessage.WriteL( KGetListDescriptorPos, 
            		iSharedEng->GetListDataL() );
            iSharedEng->CloseOutputBuffer();
            break;
            }              
                        
            
        case EMenuItemGetAttribute:
            {
            __ASSERT_ALWAYS( iSharedEng, PanicClientL( aMessage ) );
            TInt id = aMessage.Int0();
            RBuf name;
            name.CreateL( aMessage.GetDesLengthL( KParamIndAttrName ) );
            CleanupClosePushL( name );
            aMessage.ReadL( KParamIndAttrName, name );
            TPckgBuf<TBool> attrExistsBuf;
            TBool attrExists(EFalse);
        	RBuf attrVal;
        	attrVal.CleanupClosePushL();
        	attrVal.CreateL(KMenuMaxAttrValueLen);
        	iSharedEng->GetAttributeL( id, name, attrExists, attrVal);
            attrExistsBuf() = attrExists;
            aMessage.WriteL( KParamIndAttrEx, attrExistsBuf );
            
            if ( attrExists )
                {
                aMessage.WriteL( KParamIndAttrVal, attrVal );
                }
        	CleanupStack::PopAndDestroy( &attrVal );
            CleanupStack::PopAndDestroy( &name );
            break;
            }
            
        case EMenuItemGetAttributeList:
            {
            __ASSERT_ALWAYS( iSharedEng, PanicClientL( aMessage ) );
            TInt id = aMessage.Int0();
            TInt bufHandle = aMessage.Int2();
            HMenuSrvBuf* streamBuf = HMenuSrvBuf::NewLC();
            RWriteStream ws( streamBuf );
            RArray<TPtrC> attributeList;
        	CleanupClosePushL(attributeList);
            
            iSharedEng->GetAttributeListL( id, attributeList );

            for ( TInt ndx = 0; ndx < attributeList.Count(); ndx++ )
                {
                TPtrC name;
                name.Set( attributeList[ndx] );
                ws.WriteInt32L( name.Length() );
                ws.WriteL( name );
                }
            ws.WriteInt32L( 0 );
            ws.CommitL();

        	CleanupStack::PopAndDestroy(&attributeList);
            CleanupStack::Pop( streamBuf );
            NewStreamL( *(ws.Sink()), aMessage );

            break;
            }

        case EMenuNotifierClose:
            {
            TInt handle = aMessage.Int3();
            NotifierL( handle ); // Check that handle is valid.
            iNotifierIx->Remove( handle );
            break;
            };

        case EMenuNotifierOpen:
            {
            NewNotifierL( aMessage );
            break;
            }

        case EMenuNotifierNotify:
            {
            TInt handle = aMessage.Int3();
            CMenuSrvNotifier& notifier( NotifierL( handle ) );
            notifier.NotifyL( aMessage.Int0(), aMessage.Int1(), aMessage );
            // No leaving after this point!
            return ETrue; // Async request -> the object will complete it.
            }

        case EMenuNotifierNotifyCancel:
            {
            TInt handle = aMessage.Int3();
            NotifierL( handle ).Cancel();
            break;
            }
            
	    default:
	        {
	        serviced = EFalse;
	        break;
	        }
        }
	if( serviced )
		{
		aMessage.Complete( KErrNone );// Sync request -> complete it now.
		}
	return serviced;
	}

// ---------------------------------------------------------
// CMenuSrvSession::ServiceCapWriteL
// Handles functions with write device data capability
// ---------------------------------------------------------
//
TBool CMenuSrvSession::ServiceCapWriteL( const RMessage2& aMessage )
	{
	TBool serviced = ETrue;
	switch( aMessage.Function() )
        {
        case EMenuOperationClose:
            {
            TInt handle = aMessage.Int3();
            OperationL( handle ); // Check that handle is valid.
            iOperationIx->Remove( handle );
            break;
            };

        case EMenuOperationCreateRemove:
            {
            __ASSERT_ALWAYS( iSharedEng, PanicClientL( aMessage ) );
            CMenuSrvOperation* op = new (ELeave) CMenuSrvRemoveOperation
                ( iSharedEng->Engine(), aMessage.Int0() );
            AddOperationL( op, aMessage ); // Ownership taken before adding.
            break;
            }

        case EMenuOperationCreateMoveToFolder:
            {
            __ASSERT_ALWAYS( iSharedEng, PanicClientL( aMessage ) );
            TInt bufHandle = aMessage.Int0();
            MStreamBuf& buf = StreamL( bufHandle ).Host();
            RReadStream rs( &buf );
            CMenuSrvOperation* op = CMenuSrvMoveToFolderOperation::NewL
                ( iSharedEng->Engine(), rs, aMessage.Int1(), aMessage.Int2() );
            AddOperationL( op, aMessage ); // Ownership taken before adding.
            break;
            }

        case EMenuOperationCreateReorder:
            {
            __ASSERT_ALWAYS( iSharedEng, PanicClientL( aMessage ) );
            CMenuSrvOperation* op = new (ELeave) CMenuSrvReorderOperation
                ( iSharedEng->Engine(), aMessage.Int0(), aMessage.Int1() );
            AddOperationL( op, aMessage ); // Ownership taken before adding.
            break;
            }

        case EMenuOperationCreateAdd:
            {
            __ASSERT_ALWAYS( iSharedEng, PanicClientL( aMessage ) );
            TBuf<KMenuMaxTypeLen> type;
            __ASSERT_ALWAYS( aMessage.GetDesLength( 0 ) <= type.MaxLength(), \
                User::Leave( KErrBadDescriptor ) ); // Panic the client.
            aMessage.ReadL( 0, type );
            TInt bufHandle = aMessage.Int1();
            MStreamBuf& buf = StreamL( bufHandle ).Host();
            RReadStream rs( &buf );
            CMenuSrvAddOperation* op = CMenuSrvAddOperation::NewL
                ( iSharedEng->Engine(), type, rs );
            CleanupStack::PushL( op ); // Leaving calls before AddOperationL.
            // Send the ID back. Object not yet added to engine!
            TPckgBuf<TInt> id;
            id() = op->ObjectId();
            __ASSERT_DEBUG( id(), User::Invariant() ); // We must have the ID already!
            aMessage.WriteL( 2, id );
            CleanupStack::Pop( op );
            AddOperationL( op, aMessage ); // Ownership taken before adding.
            break;
            }

        case EMenuOperationCreateUpdate:
            {
            __ASSERT_ALWAYS( iSharedEng, PanicClientL( aMessage ) );
            TInt bufHandle = aMessage.Int1();
            MStreamBuf& buf = StreamL( bufHandle ).Host();
            RReadStream rs( &buf );
            CMenuSrvOperation* op = CMenuSrvUpdateOperation::NewL
                ( iSharedEng->Engine(), aMessage.Int0(), rs );
            AddOperationL( op, aMessage ); // Ownership taken before adding.
            break;
            }

        case EMenuOperationCancel:
            {
            TInt handle = aMessage.Int3();
            OperationL( handle ).Cancel();
            break;
            }

        case EMenuOperationStart:
            {
            TInt handle = aMessage.Int3();
            OperationL( handle ).StartL( aMessage );
            // No leaving after this point!
            return ETrue; // Async request -> the object will complete it.
            }
        default:
            {
            serviced = EFalse;
            break;
            }
        }
	if( serviced )
		{
		aMessage.Complete( KErrNone );// Sync request -> complete it now.
		}
	return serviced;
	}

// ---------------------------------------------------------
// CMenuSrvSession::ServiceError
// ---------------------------------------------------------
//
void CMenuSrvSession::ServiceError( const RMessage2& aMessage, TInt aError )
    {
    // A bad descriptor / bad handle error implies a badly programmed client,
    // so panic it; otherwise use the default handling (report the error to
    // the client).
    if ( aError == KErrBadDescriptor || aError == KErrBadHandle )
        {
        aMessage.Panic( KMenuSrvName, aError );
        }
    if ( aError == KErrNoMemory )
    	{
    	iSharedEng->CloseOutputBuffer();
    	iSharedEng->CleanAttributeCache();
    	}
    CSession2::ServiceError( aMessage, aError );
    }

// ---------------------------------------------------------
// CMenuSrvSession::EngineEvents
// ---------------------------------------------------------
//
void CMenuSrvSession::EngineEvents( TInt aFolder, TInt aEvents )
    {
    for ( TInt i = 0; i < iNotifierIx->Count(); i++ )
        {
        CMenuSrvNotifier* notif = (CMenuSrvNotifier*)(*iNotifierIx)[i];
        notif->HandleEvents( aFolder, aEvents );
        }
    }

// ---------------------------------------------------------
// CMenuSrvSession::EngineError
// ---------------------------------------------------------
//
void CMenuSrvSession::EngineError( TInt /*aErr*/ )
    {
    // Unrecoverable engine error. The engine is dead and will be deleted.
    // This session has already been removed from the shared engine's list.
    // This session became useless now; it rejects all messages and is
    // waiting to be closed.
    iSharedEng = NULL;
    iEngineDead = ETrue;
    }
// ---------------------------------------------------------
// CMenuSrvSession::NewStreamL
// ---------------------------------------------------------
//
void CMenuSrvSession::NewStreamL
( MStreamBuf& aHost, const RMessage2& aMessage )
    {
    aHost.PushL();
    TInt len = Min( aHost.SizeL(), KMenuStreamBufSize );
    if ( len )
        {
        // We already have some data; send it.
        TPckgBuf<TMenuBuf> buf;
        aHost.ReadL( buf().iData, len );
        buf().iLen = len;
        aMessage.WriteL( KParamIndBuf, buf );
        }
    // Create the stream, it takes ownership of aHost.
    CMenuSrvStream* obj = new (ELeave) CMenuSrvStream( aHost );
    CleanupStack::Pop( &aHost );
    CleanupStack::PushL( obj );
    iObjectCon->AddL( obj );
    TInt handle = iStreamIx->AddL( obj );
    CleanupStack::Pop( obj ); // Now we manage the stream by handle.
    TPckgC<TInt> handlePckg( handle );
    TInt err = aMessage.Write( KParamIndStream, handlePckg );
    if ( err )
        {
        iStreamIx->Remove( handle );
        User::Leave( err );
        }
    }

// ---------------------------------------------------------
// CMenuSrvSession::StreamL
// ---------------------------------------------------------
//
CMenuSrvStream& CMenuSrvSession::StreamL( TInt aHandle )
    {
    CMenuSrvStream* obj = (CMenuSrvStream*)iStreamIx->AtL( aHandle );
    if ( !obj )
        {
        User::Leave( KErrBadHandle );
        }
    return *obj;
    }

// ---------------------------------------------------------
// CMenuSrvSession::AddOperationL
// ---------------------------------------------------------
//
void CMenuSrvSession::AddOperationL
( CMenuSrvOperation* aOperation, const RMessage2& aMessage )
    {
    CleanupStack::PushL( aOperation );
    iObjectCon->AddL( aOperation );
    TInt handle = iOperationIx->AddL( aOperation );
    CleanupStack::Pop( aOperation ); // Now we manage the operation by handle.
    TPckgC<TInt> handlePckg( handle );
    TInt err = aMessage.Write( KParamIndOp, handlePckg );
    if ( err )
        {
        iOperationIx->Remove( handle ); // Deletes the operation.
        User::Leave( err );
        }
    }

// ---------------------------------------------------------
// CMenuSrvSession::OperationL
// ---------------------------------------------------------
//
CMenuSrvOperation& CMenuSrvSession::OperationL( TInt aHandle )
    {
    CMenuSrvOperation* obj = (CMenuSrvOperation*)iOperationIx->AtL( aHandle );
    if ( !obj )
        {
        User::Leave( KErrBadHandle );
        }
    return *obj;
    }

// ---------------------------------------------------------
// CMenuSrvSession::NewNotifierL
// ---------------------------------------------------------
//
void CMenuSrvSession::NewNotifierL( const RMessage2& aMessage )
    {
    CMenuSrvNotifier* obj = new (ELeave) CMenuSrvNotifier;
    CleanupStack::PushL( obj );
    iObjectCon->AddL( obj );
    TInt handle = iNotifierIx->AddL( obj );
    CleanupStack::Pop( obj ); // Now we manage the notifier by handle.
    TPckgC<TInt> handlePckg( handle );
    TInt err = aMessage.Write( KParamIndNotif, handlePckg );
    if ( err )
        {
        iNotifierIx->Remove( handle );
        User::Leave( err );
        }
    }

// ---------------------------------------------------------
// CMenuSrvSession::NotifierL
// ---------------------------------------------------------
//
CMenuSrvNotifier& CMenuSrvSession::NotifierL( TInt aHandle )
    {
    CMenuSrvNotifier* obj = (CMenuSrvNotifier*)iNotifierIx->AtL( aHandle );
    if ( !obj )
        {
        User::Leave( KErrBadHandle );
        }
    return *obj;
    }

// ---------------------------------------------------------
// CMenuSrvSession::PanicClientL
// ---------------------------------------------------------
//
void CMenuSrvSession::PanicClientL( const RMessage2& aMessage )
    {
    // Client error. Panic client and leave.
    // Use an error code (KErrGeneral) which does not cause ServiceError
    // to panic the client again!
    aMessage.Panic( KMenuSrvName, KErrGeneral );
    User::Leave( KErrGeneral );
    }

//  End of File
