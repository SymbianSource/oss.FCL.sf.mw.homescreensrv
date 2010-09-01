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

#include "menubuf.h"
#include "menumsg.h"
#include "mcsmenu.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RMenuBuf::Open
// ---------------------------------------------------------
//
TInt RMenuBuf::Open( RMenu& aMenu )
    {
	SetBuf( ERead | EWrite, iBuf.iData, iBuf.iData ); // Empty.
    return CreateSubSession( aMenu, EMenuStreamOpen, TIpcArgs() );
    }

// ---------------------------------------------------------
// RMenuBuf::Open
// ---------------------------------------------------------
//
TInt RMenuBuf::Open( RMenu& aMenu, TInt aFunction, TIpcArgs& aArgs )
    {
    iBuf.iLen = 0;
	SetBuf( ERead | EWrite, iBuf.iData, iBuf.iData ); // Initially empty.
    TPckg<TMenuBuf> buf( iBuf );
    aArgs.Set( 2, &buf );
    TInt err = CreateSubSession( aMenu, aFunction, aArgs );
    if ( !err && iBuf.iLen > 0 )
        {
        // If aFunction provided data, it is already here.
        SetEnd( ERead, iBuf.iData + iBuf.iLen );
        }
    return err;
    }

// ---------------------------------------------------------
// RMenuBuf::Close()
// ---------------------------------------------------------
//
void RMenuBuf::Close()
    {
    // Mind the order: TStreamBuf::Close calls SynchL -> needs an open buf.
    TStreamBuf::Close();
    CloseSubSession( EMenuStreamClose );
    }

// ---------------------------------------------------------
// RMenuBuf::UnderflowL()
// ---------------------------------------------------------
//
TInt RMenuBuf::UnderflowL( TInt /*aMaxLength*/ )
    {
    __ASSERT_DEBUG( Avail( ERead ) == 0, User::Invariant() );
    TInt len;
    IpcReadL( len );
    return len;
    }

// ---------------------------------------------------------
// RMenuBuf::OverflowL()
// ---------------------------------------------------------
//
void RMenuBuf::OverflowL()
    {
    __ASSERT_DEBUG( Avail( EWrite ) == 0, User::Invariant() );
    IpcWriteL();
    }

// ---------------------------------------------------------
// RMenuBuf::DoSynchL()
// ---------------------------------------------------------
//
void RMenuBuf::DoSynchL()
    {
    if ( Lag( ERead ) )
        {
        // Read lag unexpected, we do not support seeking.
        User::Leave( KErrNotSupported );
        }
    IpcWriteL();    // Flush write lag.
	SetBuf( ERead | EWrite, iBuf.iData, iBuf.iData );
    }

// ---------------------------------------------------------
// RMenuBuf::IpcReadL()
// ---------------------------------------------------------
//
void RMenuBuf::IpcReadL( TInt& aLength )
    {
	TPtr8 ptr( iBuf.iData, KMenuStreamBufSize );
    User::LeaveIfError( SendReceive( EMenuStreamRead, TIpcArgs( &ptr ) ) );
    SetBuf( EWrite, iBuf.iData, iBuf.iData );
    SetBuf( ERead, iBuf.iData, iBuf.iData + ptr.Length() );
    aLength = ptr.Length();
    }

// ---------------------------------------------------------
// RMenuBuf::IpcWriteL()
// ---------------------------------------------------------
//
void RMenuBuf::IpcWriteL()
    {
    if ( Lag( EWrite ) )
        {
        TPtrC8 ptr( iBuf.iData, Lag( EWrite ) );
        User::LeaveIfError
            ( SendReceive( EMenuStreamWrite, TIpcArgs( &ptr ) ) );
        }
    SetBuf( ERead, iBuf.iData, iBuf.iData );
    SetBuf( EWrite, iBuf.iData, iBuf.iData + KMenuStreamBufSize );
    }

//  End of File  
