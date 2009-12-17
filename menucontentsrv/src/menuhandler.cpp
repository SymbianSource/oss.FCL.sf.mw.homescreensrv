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
#include "mcsmenuhandler.h"

// ================= MEMBER FUNCTIONS =======================

NONSHARABLE_CLASS( CMenuHandler::TData )
    {
    // Future-proofing only.
    };

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuHandler::~CMenuHandler
// ---------------------------------------------------------
//
EXPORT_C CMenuHandler::~CMenuHandler()
    {
    }

// ---------------------------------------------------------
// CMenuHandler::CMenuHandler
// ---------------------------------------------------------
//
EXPORT_C CMenuHandler::CMenuHandler( RMenu &aMenu )
: iMenu( aMenu )
    {
    }

// ---------------------------------------------------------
// CMenuHandler::BaseConstructL
// ---------------------------------------------------------
//
EXPORT_C void CMenuHandler::BaseConstructL()
    {
    // Future-proofing only.
    }

// ---------------------------------------------------------
// CMenuHandler::SupportsType
// ---------------------------------------------------------
//
EXPORT_C TBool CMenuHandler::SupportsType( const TDesC& /*aType*/ )
    {
    return EFalse;
    }

// ---------------------------------------------------------
// CMenuHandler::HandleCommandL
// ---------------------------------------------------------
//
EXPORT_C CMenuOperation* CMenuHandler::HandleCommandL(
        CMenuItem& /*aItem*/,
        const TDesC8& /*aCommand*/,
        const TDesC8& /*aParams*/,
        TRequestStatus& /*aStatus*/ )
    {
    User::Leave( KErrNotSupported );
    return NULL;
    }
