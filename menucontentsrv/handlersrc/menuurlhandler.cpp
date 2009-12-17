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
#include "menuurlhandler.h"
#include "menuuninstalloperation.h"
#include "menucompletedoperation.h"
#include "mcsmenuitem.h"
#include "mcsdef.h"

#include <w32std.h>
#include <apgtask.h>
#include <apgcli.h>
#include <AknTaskList.h>
#include <SchemeHandler.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuUrlHandler::~CMenuUrlHandler
// ---------------------------------------------------------
//
CMenuUrlHandler::~CMenuUrlHandler()
    {
    }

// ---------------------------------------------------------
// CMenuUrlHandler::NewL
// ---------------------------------------------------------
//
CMenuUrlHandler* CMenuUrlHandler::NewL( RMenu &aMenu )
    {
    CMenuUrlHandler* handler = new (ELeave) CMenuUrlHandler( aMenu );
    CleanupStack::PushL( handler );
    handler->ConstructL();
    CleanupStack::Pop( handler );
    return handler;
    }

// ---------------------------------------------------------
// CMenuUrlHandler::CMenuUrlHandler
// ---------------------------------------------------------
//
CMenuUrlHandler::CMenuUrlHandler( RMenu &aMenu )
: CMenuHandlerPlugin( aMenu )
    {
    }

// ---------------------------------------------------------
// CMenuUrlHandler::ConstructL
// ---------------------------------------------------------
//
void CMenuUrlHandler::ConstructL()
    {
    BaseConstructL();
    }

// ---------------------------------------------------------
// CMenuUrlHandler::SupportsType
// ---------------------------------------------------------
//
TBool CMenuUrlHandler::SupportsType( const TDesC& aType )
    {
    if ( !aType.Compare( KMenuTypeUrl() ) )
        {
        return ETrue;
        }

    return EFalse;
    }

// ---------------------------------------------------------
// CMenuUrlHandler::HandleCommandL
// ---------------------------------------------------------
//
CMenuOperation* CMenuUrlHandler::HandleCommandL(
        CMenuItem& aItem,
        const TDesC8& aCommand,
        const TDesC8& /*aParams*/,
        TRequestStatus& aStatus )
    {
    // Check if item type is correct
    if( aItem.Type() != KMenuTypeUrl() )
        {
        User::Leave( KErrNotSupported );
        }

    if( !aCommand.Compare( KMenuCmdOpen() ) )
        {
        // Get URL attribute 'url'
        TBool attrExists = ETrue;
        TPtrC url = aItem.GetAttributeL( KMenuAttrUrl(), attrExists );

        if( !attrExists )
            {
            User::Leave( KErrCorrupt );
            }

        LaunchUrlL( url );

        return CMenuCompletedOperation::NewL
                    ( iMenu, CActive::EPriorityStandard, aStatus, KErrNone );
        }

    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CMenuUrlHandler::LaunchUrlL
// ---------------------------------------------------------
//

void CMenuUrlHandler::LaunchUrlL( const TDesC& aUrl )
    {
    CSchemeHandler* urlHandler = CSchemeHandler::NewL( aUrl );
    if( urlHandler )
        {
        CleanupStack::PushL( urlHandler );
        urlHandler->HandleUrlStandaloneL();
        CleanupStack::PopAndDestroy( urlHandler );
        }
    }
