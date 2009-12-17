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
#include <mcsmenuutils.h>
#include "menulinkhandler.h"
#include "menuuninstalloperation.h"
#include "mcsdef.h"
#include "mcsmenuitem.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuLinkHandler::~CMenuLinkHandler
// ---------------------------------------------------------
//
CMenuLinkHandler::~CMenuLinkHandler()
    {
    }

// ---------------------------------------------------------
// CMenuLinkHandler::NewL
// ---------------------------------------------------------
//
CMenuLinkHandler* CMenuLinkHandler::NewL( RMenu &aMenu )
    {
    CMenuLinkHandler* handler = new (ELeave) CMenuLinkHandler( aMenu );
    CleanupStack::PushL( handler );
    handler->ConstructL();
    CleanupStack::Pop( handler );
    return handler;
    }

// ---------------------------------------------------------
// CMenuLinkHandler::CMenuLinkHandler
// ---------------------------------------------------------
//
CMenuLinkHandler::CMenuLinkHandler( RMenu &aMenu )
: CMenuHandlerPlugin( aMenu )
    {
    }

// ---------------------------------------------------------
// CMenuLinkHandler::ConstructL
// ---------------------------------------------------------
//
void CMenuLinkHandler::ConstructL()
    {
    BaseConstructL();
    }

// ---------------------------------------------------------
// CMenuLinkHandler::SupportsType
// ---------------------------------------------------------
//
TBool CMenuLinkHandler::SupportsType( const TDesC& aType )
    {
    if ( !aType.Compare( KMenuTypeLink ) )
        {
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CMenuLinkHandler::HandleCommandL
// ---------------------------------------------------------
//
CMenuOperation* CMenuLinkHandler::HandleCommandL(
        CMenuItem& aItem,
        const TDesC8& aCommand,
        const TDesC8& aParams,
        TRequestStatus& aStatus )
    {
    if ( aCommand == KMenuCmdOpen && aItem.Type() == KMenuTypeLink )
        {
        TBool attrExists = EFalse;
        TPtrC val = aItem.GetAttributeL( KMenuAttrRefId, attrExists );
        if ( attrExists )
            {
            // "ref_id" defined, get item and forward command.
            TUint refId = 0;
            MenuUtils::GetTUint( val, refId );            
            CMenuItem* item = CMenuItem::OpenL( iMenu, refId );
            CleanupStack::PushL( item );
            CMenuOperation* op = item->HandleCommandL
                ( aCommand, aParams, aStatus ); // Not pushed, no leaving.
            CleanupStack::PopAndDestroy( item );
            return op;
            }
        else
            {
            // "ref_id" missing from aItem.
            User::Leave( KErrCorrupt );
            return NULL;
            }
        }
    else if ( aCommand == KMenuCmdRemove && aItem.Type() == KMenuTypeLink )
        {
        return CMenuUninstallOperation::NewL
            ( iMenu, CActive::EPriorityStandard, aStatus, aItem );
        }

    User::Leave ( KErrNotSupported );
    return NULL;
    }

// End of file
