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
#include "mcsdef.h"

#include "mcsmenu.h"
#include "menusrvdef.h"
#include "menumsg.h"
#include "menubuf.h"
#include "menuutil.h"
#include "menuclientoperation.h"
#include "menuhandlereng.h"
#include "mcsmenufilter.h"

#include <s32strm.h>
#include <e32debug.h>

// ================= LOCAL FUNCTIONS =======================

/**
* Start the server executable.
* @return Error code.
*/
LOCAL_C TInt StartServer()
    {
    TInt err( KErrNone );
    RProcess server;
    err = server.Create( KMenuSrvExe, KNullDesC, EOwnerThread );
    if ( !err )
        {
        TRequestStatus status;
        server.Rendezvous( status );
        if ( status != KRequestPending )
            {
            server.Kill( 0 ); // Abort startup.
            }
        else
            {
            server.Resume(); // Logon OK - start the server.
            }
        User::WaitForRequest( status ); // Wait for start or death.
        // We can't use the 'exit reason' if the server panicked as this
        // is the panic 'reason' and may be '0' which cannot be distinguished
        // from KErrNone.
        err = (server.ExitType() == EExitPanic) ? KErrGeneral : status.Int();
        server.Close();
        }
    return err;
    }

// ================= MEMBER FUNCTIONS =======================

NONSHARABLE_CLASS( RMenu::TData )
    {
public:
    // Transfer buffer. Size must be big enough to accomodate any object that
    // will go through it: attribute, TMenuItem
    TUint16 iBuf[KMenuMaxAttrValueLen];
    // Handler engine.
    CMenuHandlerEng* iHandler;
    };

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RMenu::Close
// ---------------------------------------------------------
//
EXPORT_C void RMenu::Close()
    {
    RSessionBase::Close();
    if ( iData )
        {
        delete iData->iHandler; iData->iHandler = NULL;
        delete iData; iData = NULL;
        }
    }

// ---------------------------------------------------------
// RMenu::OpenL
// ---------------------------------------------------------
//
EXPORT_C void RMenu::OpenL( const TDesC& aName )
    {
    __ASSERT_DEBUG( !iData, User::Invariant() );
    User::LeaveIfError( ConnectSession() );
    CleanupClosePushL( *this ); // Atomcic connect + local data + construct.
    iData = new (ELeave) RMenu::TData();
    iData->iHandler = NULL;
    // We have a connected blank session, now construct it.
    // It would be good to have a CreateSession which leaves us at least one
    // IpcArg, so the name could be sent to session creation directly.
    // Since having a fully constructed session needs this extra IPC, the
    // server side session had to be cluttered with ASSERTS. (Almost all
    // messages need to check that the session is fully constructed.)
    //
    // The reason for this is a design issue (we use the session as an object,
    // and not just as a communication channel. But normally there is only one
    // object used, so why complicate things for the client?
    User::LeaveIfError
        ( SendReceive( EMenuConstructSession, TIpcArgs( &aName ) ) );
    CleanupStack::Pop( this );
    }

// ---------------------------------------------------------
// RMenu::RootFolderL
// ---------------------------------------------------------
//
EXPORT_C TInt RMenu::RootFolderL()
    {
    __ASSERT_DEBUG( iData, User::Invariant() );
    TPckgBuf<TInt> pckg;
    User::LeaveIfError( SendReceive( EMenuRootFolder, TIpcArgs( &pckg ) ) );
    return pckg();
    }

// ---------------------------------------------------------
// RMenu::GetItemsL
// ---------------------------------------------------------
//
EXPORT_C void RMenu::GetItemsL(
        RArray<TMenuItem>& aItemArray,
        TInt aFolder,
        const CMenuFilter* aFilter /*=NULL*/,
        TBool aRecursive /*=EFalse*/ )
    {
    __ASSERT_DEBUG( iData, User::Invariant() );
    if ( aFilter )
        {
        // Filtered query, 2+ more IPC.
        // The buffer is opened empty, used to send the filter
        // then the result is received into it.
        RMenuBuf buf;
        User::LeaveIfError( buf.Open( *this ) );
        CleanupClosePushL( buf );
        RWriteStream stream( &buf );
        aFilter->ExternalizeL( stream );
        stream.CommitL();
        TIpcArgs args( aFolder, aRecursive, buf.SubSessionHandle() );
        User::LeaveIfError( SendReceive( EMenuGetItemsFiltered, args ) );
        RReadStream rs( &buf );
        MenuUtil::InternalizeL( aItemArray, rs );
        CleanupStack::PopAndDestroy( &buf );
        }
    else
        {
        // Unfiltered query.
        // The buffer already contains the query result when Open() returns.
        RMenuBuf buf;
        TIpcArgs args( aFolder, aRecursive );
        User::LeaveIfError( buf.Open( *this, EMenuGetItems, args ) );
        CleanupClosePushL( buf );
        RReadStream stream( &buf );
        MenuUtil::InternalizeL( aItemArray, stream );
        CleanupStack::PopAndDestroy( &buf );
        }
    }

// ---------------------------------------------------------
// RMenu::GetRunningAppsL
// ---------------------------------------------------------
//
EXPORT_C  void RMenu::GetRunningAppsL( RArray<TUid>& aArray )
	{
    RMenuBuf buf;
    TIpcArgs args( 0 );
    User::LeaveIfError( buf.Open( *this, EMenuGetRunningApps, args ) );
    CleanupClosePushL( buf );
    RReadStream stream( &buf );
    MenuUtil::InternalizeL( aArray, stream );
    CleanupStack::PopAndDestroy( &buf );
	}
    

// ---------------------------------------------------------
// RMenu::RemoveL
// ---------------------------------------------------------
//
EXPORT_C CMenuOperation* RMenu::RemoveL( TInt aId, TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( iData, User::Invariant() );
    CMenuClientOperation* op = new (ELeave) CMenuClientOperation
        ( *this, CActive::EPriorityStandard, aStatus );
    CleanupStack::PushL( op );
    op->RemoveL( aId );
    CleanupStack::Pop( op );
    return op;
    }

// ---------------------------------------------------------
// RMenu::MoveToFolderL
// ---------------------------------------------------------
//
EXPORT_C CMenuOperation* RMenu::MoveToFolderL(
        const RArray<TInt>& aItems,
        TInt aFolder,
        TInt aMoveBefore,
        TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( iData, User::Invariant() );
    RMenuBuf buf;
    User::LeaveIfError( buf.Open( *this ) );
    CleanupClosePushL( buf );
    RWriteStream stream( &buf );
    MenuUtil::ExternalizeL( aItems, stream );
    stream.CommitL();
    CMenuClientOperation* op = new (ELeave) CMenuClientOperation
        ( *this, CActive::EPriorityStandard, aStatus );
    CleanupStack::PushL( op );
    op->MoveToFolderL( buf, aFolder, aMoveBefore );
    CleanupStack::Pop( op );
    CleanupStack::PopAndDestroy( &buf );
    return op;
    }

// ---------------------------------------------------------
// RMenu::ReorderL
// ---------------------------------------------------------
//
EXPORT_C CMenuOperation* RMenu::ReorderL
( TInt aId, TInt aMoveBefore, TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( iData, User::Invariant() );
    CMenuClientOperation* op = new (ELeave) CMenuClientOperation
        ( *this, CActive::EPriorityStandard, aStatus );
    CleanupStack::PushL( op );
    op->ReorderL( aId, aMoveBefore );
    CleanupStack::Pop( op );
    return op;
    }

// ---------------------------------------------------------
// RMenu::ResourceMark
// ---------------------------------------------------------
//
EXPORT_C void RMenu::ResourceMark()
    {
    SendReceive( EMenuResourceMarkStart );
    }

// ---------------------------------------------------------
// RMenu::ResourceCheck
// ---------------------------------------------------------
//
EXPORT_C void RMenu::ResourceCheck()
    {
    SendReceive( EMenuResourceMarkEnd );
    }

// ---------------------------------------------------------
// RMenu::ResourceCount
// ---------------------------------------------------------
//
EXPORT_C TInt RMenu::ResourceCount()
    {
    return SendReceive( EMenuResourceCount );
    }

#ifdef _DEBUG

// ---------------------------------------------------------
// RMenu::__DbgSetAllocFail
// ---------------------------------------------------------
//
EXPORT_C void RMenu::__DbgSetAllocFail
( RAllocator::TAllocFail aType, TInt aRate )
    {
    TInt type = STATIC_CAST( TInt, aType );
    SendReceive( EMenuSetHeapFailure, TIpcArgs( type, aRate ) );
    }

#else /* not _DEBUG */

// ---------------------------------------------------------
// RMenu::__DbgSetAllocFail
// ---------------------------------------------------------
//
EXPORT_C void RMenu::__DbgSetAllocFail
( RAllocator::TAllocFail /*aType*/, TInt /*aRate*/ )
    {
    }

#endif /* def _DEBUG */

// ---------------------------------------------------------
// RMenu::HandleCommandL
// ---------------------------------------------------------
//
CMenuOperation* RMenu::HandleCommandL(
        CMenuItem& aItem,
        const TDesC8& aCommand,
        const TDesC8& aParams,
        TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( iData, User::Invariant() );
    if ( !iData->iHandler )
        {
        // Create handler engine on demand - handlers not loaded unless needed.
        iData->iHandler = CMenuHandlerEng::NewL( *this );
        }
    return iData->iHandler->HandleCommandL
        ( aItem, aCommand, aParams, aStatus );
    }

// ---------------------------------------------------------
// RMenu::ConnectSession
// ---------------------------------------------------------
//
TInt RMenu::ConnectSession()
    {
    TVersion version( KMenuMajorVersion, KMenuMinorVersion, KMenuBuild );
    TInt err = CreateSession( KMenuSrvName, version );
    if ( KErrNotFound == err )
        {
        err = StartServer();
        if ( !err || KErrAlreadyExists == err ) // Deal with race conditions.
            {
            err = CreateSession( KMenuSrvName, version );
            }
        }
    return err;
    }

// ---------------------------------------------------------
// RMenu::GetAttributeL
// ---------------------------------------------------------
//
HBufC* RMenu::GetAttributeL( TInt aId, const TDesC& aAttrName )
    {
    __ASSERT_DEBUG( iData, User::Invariant() );
    __ASSERT_DEBUG( sizeof( iData->iBuf ) >= 2 * KMenuMaxAttrValueLen, \
        User::Invariant() ); // Must fit into the transfer buffer.
    
    RBuf ptr;
    ptr.CleanupClosePushL();
    ptr.CreateL(KMenuMaxAttrValueLen);
    TPckgBuf<TBool> attrExists( EFalse );
    TIpcArgs args( aId, &aAttrName, &attrExists, &ptr );
    User::LeaveIfError( SendReceive( EMenuItemGetAttribute, args ) );
    
    if( attrExists() )
    	{
    	HBufC* ret = ptr.AllocL();
    	CleanupStack::PopAndDestroy( &ptr );
    	return ret;
    	}
    else
    	{
    	CleanupStack::PopAndDestroy( &ptr );
    	return NULL;
    	}
    }
    


// ---------------------------------------------------------
// RMenu::GetAttributeL
// ---------------------------------------------------------
void RMenu::GetAttributeListL( TInt aId, RArray<TAttributeName>& aList )
    {
    __ASSERT_DEBUG( iData, User::Invariant() );
    __ASSERT_DEBUG( sizeof( iData->iBuf ) >= 2 * KMenuMaxAttrValueLen, \
        User::Invariant() ); // Must fit into the transfer buffer.

    RMenuBuf buf;
    TIpcArgs args( aId );
    User::LeaveIfError( buf.Open( *this, EMenuItemGetAttributeList, args ) );
    CleanupClosePushL( buf );
    RReadStream rs( &buf );
    
    TInt len;
    TAttributeName name;
    while ( ETrue )
        {

        len = rs.ReadInt32L();

        if ( len == 0 )
            {
            break;
            }

        rs.ReadL( name, len );
        aList.AppendL( name );
        }

    CleanupStack::PopAndDestroy( &buf );
    }

// ---------------------------------------------------------
// RMenu::GetHdrL
// ---------------------------------------------------------
//
const TMenuItem& RMenu::GetHdrL( TInt aId )
    {
    __ASSERT_DEBUG( iData, User::Invariant() );
    __ASSERT_DEBUG( sizeof( iData->iBuf ) >= sizeof( TMenuItem ), \
        User::Invariant() ); // Header must fit into the transfer buffer.
    TPckg<TMenuItem> pckg( (TMenuItem&)iData->iBuf );
    TIpcArgs args( aId, &pckg );
    User::LeaveIfError( SendReceive( EMenuGetHdr, args ) );
    return pckg(); // Returned object is in the transfer buffer.
    }
