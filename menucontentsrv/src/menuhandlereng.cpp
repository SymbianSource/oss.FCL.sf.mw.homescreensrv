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

#include "menuhandlereng.h"
#include "mcsmenuhandler.h"
#include "mcsmenuhandlerplugin.h"
#include "mcsmenuhandlerplugin.hrh"
#include "mcsdef.h"
#include "mcsmenuitem.h"
#include "menuuid.hrh"

// ================= MEMBER FUNCTIONS =======================

/**
* Cleanupstack support method, calls RImplInfoPtrArray::ResetAndDestroy()
* of the passed argument (array).
* @param aArray Array.
*/
LOCAL_C void ResetAndDestroyArray( TAny* aArray )
    {
    ((RImplInfoPtrArray*)aArray)->ResetAndDestroy();
    }


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuHandlerEng::~CMenuHandlerEng
// ---------------------------------------------------------
//
CMenuHandlerEng::~CMenuHandlerEng()
    {
    iHandlers.ResetAndDestroy();
    }

// ---------------------------------------------------------
// CMenuHandlerEng::NewL
// ---------------------------------------------------------
//
CMenuHandlerEng* CMenuHandlerEng::NewL( RMenu& aMenu )
    {
    CMenuHandlerEng* eng = new (ELeave) CMenuHandlerEng( aMenu );
    return eng;
    }

// ---------------------------------------------------------
// CMenuHandlerEng::CMenuHandlerEng
// ---------------------------------------------------------
//
CMenuHandlerEng::CMenuHandlerEng( RMenu& aMenu ): iMenu( aMenu )
    {
    }

// ---------------------------------------------------------
// CMenuHandlerEng::HandleCommandL
// ---------------------------------------------------------
//
CMenuOperation* CMenuHandlerEng::HandleCommandL(
        CMenuItem& aItem,
        const TDesC8& aCommand,
        const TDesC8& aParams,
        TRequestStatus& aStatus )
    {
    // Currently, we load handlers on demand and never unload them.
    // As there aren't many types of items, this seems appropriate.
    CMenuHandler* handler = FindHandler( aItem.Type() );
    if ( !handler )
        {
        handler = LoadHandlerL( aItem.Type() );
        }
    if ( handler )
        {
        return handler->HandleCommandL( aItem, aCommand, aParams, aStatus );
        }
    User::Leave( KErrNotSupported );
    return NULL;
    }

// ---------------------------------------------------------
// CMenuHandlerEng::FindHandler
// ---------------------------------------------------------
//
CMenuHandler* CMenuHandlerEng::FindHandler( const TDesC& aType )
    {
    for ( TInt i = 0; i < iHandlers.Count(); i++ )
        {
        CMenuHandler* handler = iHandlers[i];
        if ( handler->SupportsType( aType ) )
            {
            return handler;
            }
        }
    return NULL;
    }

// ---------------------------------------------------------
// CMenuHandlerEng::LoadHandlerL
// ---------------------------------------------------------
//
CMenuHandler* CMenuHandlerEng::LoadHandlerL( const TDesC& aType )
    {
    CMenuHandlerPlugin* handler = NULL;
    TBuf8<KMenuMaxTypeLen> type;
    type.Copy( aType );
    TEComResolverParams resolverParams;
    resolverParams.SetDataType( type );
    RImplInfoPtrArray implInfoArray;
    CleanupStack::PushL( TCleanupItem( ResetAndDestroyArray, &implInfoArray ) );
    REComSession::ListImplementationsL
        ( TUid::Uid( MENU_HANDLER_IF_UID ), resolverParams, implInfoArray );
        
    __ASSERT_DEBUG( implInfoArray.Count() <= 1, User::Invariant() );
    
    if ( implInfoArray.Count() != 0 )
        {
        TUid implUid = implInfoArray[ 0 ]->ImplementationUid();
        handler = CMenuHandlerPlugin::NewL( implUid, iMenu );
        CleanupStack::PushL( handler );
        // Here we check if this is the handler we need. Currently we only do
        // a sanity check for the handler (it should support the registered
        // types!), but later this can be extended with better support query.
        // E.g. support for type AND command, etc.
        if ( handler->SupportsType( aType ) )
            {
            iHandlers.AppendL( handler );
            CleanupStack::Pop( handler );
            }
        else
            {
            CleanupStack::PopAndDestroy( handler );
            handler = NULL;
            }
        }

    CleanupStack::PopAndDestroy( &implInfoArray );
    return handler;
    }
