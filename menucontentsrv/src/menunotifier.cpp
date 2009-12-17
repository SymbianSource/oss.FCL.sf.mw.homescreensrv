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
#include "mcsmenunotifier.h"
#include "mcsmenu.h"
#include "menumsg.h"

NONSHARABLE_CLASS( RMenuNotifier::TData )
    {
    };

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RMenuNotifier::Close
// ---------------------------------------------------------
//
EXPORT_C void RMenuNotifier::Close()
    {
    if ( iData )
        {
        CloseSubSession( EMenuNotifierClose );
        delete iData; iData = NULL;
        }
    }

// ---------------------------------------------------------
// RMenuNotifier::Open
// ---------------------------------------------------------
//
EXPORT_C TInt RMenuNotifier::Open( RMenu& aMenu )
    {
    __ASSERT_DEBUG( !iData, User::Invariant() );
    // Creation of local data and server object is atomic.
    TInt err = KErrNone;
    iData = new RMenuNotifier::TData();
    if ( iData )
        {
        err = CreateSubSession( aMenu, EMenuNotifierOpen );
        if ( err )
            {
            delete iData; iData = NULL;
            }
        }
    else
        {
        err = KErrNoMemory;
        }
    return err;
    }

// ---------------------------------------------------------
// RMenuNotifier::Notify
// ---------------------------------------------------------
//
EXPORT_C void RMenuNotifier::Notify
( TInt aFolder, TInt aEvents, TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( iData, User::Invariant() );
    TIpcArgs args( aFolder, aEvents );
    SendReceive( EMenuNotifierNotify, args, aStatus );
    }

// ---------------------------------------------------------
// RMenuNotifier::Cancel
// ---------------------------------------------------------
//
EXPORT_C void RMenuNotifier::Cancel()
    {
    __ASSERT_DEBUG( iData, User::Invariant() );
    SendReceive( EMenuNotifierNotifyCancel );
    }
