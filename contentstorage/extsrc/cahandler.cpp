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
 * Description:  ?Description
 *
 */

#include "cahandler.h"

// ======== MEMBER FUNCTIONS ========

NONSHARABLE_CLASS( CCaHandler::TData )
    {
    // Future-proofing only.
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C CCaHandler::~CCaHandler()
    {
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C CCaHandler::CCaHandler()
    {
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C void CCaHandler::BaseConstructL()
    {
    // Future-proofing only.
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C TBool CCaHandler::SupportsType( const TDesC& /*aType*/)
    {
    return EFalse;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C void CCaHandler::HandleCommandL( CCaInnerEntry& /*aEntry*/,
        const TDesC8& /*aCommand*/)
    {
    User::Leave( KErrNotSupported );
    }
