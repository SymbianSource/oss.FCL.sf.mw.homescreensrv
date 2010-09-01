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

#include "timeout.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CTimeout::NewL()
// ---------------------------------------------------------
//
CTimeout* CTimeout::NewL( TInt aPriority, TCallBack aCallBack )
    {
    CTimeout* timeout = new (ELeave) CTimeout( aPriority, aCallBack );
    CleanupStack::PushL( timeout );
    timeout->ConstructL();
    CleanupStack::Pop( timeout );
    return timeout;
    }

// ---------------------------------------------------------
// CTimeout::~CTimeout()
// ---------------------------------------------------------
//
CTimeout::~CTimeout()
    {
    Cancel();
    }

// ---------------------------------------------------------
// CTimeout::RunL()
// ---------------------------------------------------------
//
void CTimeout::RunL()
    {
#ifdef _DEBUG
    TBool again =
#endif /* def _DEBUG */
    iCallBack.CallBack();
    // Assert that this is a "once-only" callback.
    __ASSERT_DEBUG( !again, User::Invariant() );
    }

// ---------------------------------------------------------
// CTimeout::RunError()
// ---------------------------------------------------------
//
TInt CTimeout::RunError( TInt /*aError*/ )
    {
    // Ignore the error (what else could we do?).
    return KErrNone;
    }

// ---------------------------------------------------------
// CTimeout::CTimeout()
// ---------------------------------------------------------
//
CTimeout::CTimeout( TInt aPriority, TCallBack aCallBack ) : 
		CTimer( aPriority ), 
		iCallBack( aCallBack )
    {
    CActiveScheduler::Add( this );
    }

//  End of File  
