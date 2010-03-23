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

#include "cahandlerengine.h"
#include "cahandler.h"
#include "cahandlerplugin.h"
#include "cahandlerplugin.hrh"
#include "cadef.h"
#include "cainnerentry.h"

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Cleanupstack support method, calls RImplInfoPtrArray::ResetAndDestroy()
// of the passed argument (array).
// ---------------------------------------------------------------------------
//
LOCAL_C void ResetAndDestroyArray( TAny* aArray )
    {
    ( (RImplInfoPtrArray* ) aArray )->ResetAndDestroy();
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C CCaHandlerEngine::~CCaHandlerEngine()
    {
    iHandlers.ResetAndDestroy();
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C CCaHandlerEngine* CCaHandlerEngine::NewL()
    {
    CCaHandlerEngine* self = NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C CCaHandlerEngine* CCaHandlerEngine::NewLC()
    {
    CCaHandlerEngine* self = new ( ELeave ) CCaHandlerEngine();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
void CCaHandlerEngine::ConstructL()
    {
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
CCaHandlerEngine::CCaHandlerEngine()
    {
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
EXPORT_C void CCaHandlerEngine::HandleCommandL( CCaInnerEntry& aEntry,
        const TDesC8& aCommand )
    {
    // Currently, we load handlers on demand and never unload them.
    // As there aren't many types of items, this seems appropriate.
    CCaHandler* handler = FindHandler( aEntry.GetEntryTypeName() );
    if( !handler )
        {
        //TODO: need to change this
        if( aEntry.GetEntryTypeName() == KCaTypeApp()
                || aEntry.GetEntryTypeName() == KCaTypeWidget() )
            {
            handler = LoadHandlerL( KCaTypeApp() );
            }
        else
            {
            handler = LoadHandlerL( aEntry.GetEntryTypeName() );
            }
        }
    if( handler )
        {
        handler->HandleCommandL( aEntry, aCommand );
        }
    else
        {
        User::Leave( KErrNotFound );
        }
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
CCaHandler* CCaHandlerEngine::FindHandler( const TDesC& aType )
    {
    for( TInt i = 0; i < iHandlers.Count(); i++ )
        {
        CCaHandler* handler = iHandlers[i];
        if( handler->SupportsType( aType ) )
            {
            return handler;
            }
        }
    return NULL;
    }

// ---------------------------------------------------------
//
// ---------------------------------------------------------
//
CCaHandler* CCaHandlerEngine::LoadHandlerL( const TDesC& aType )
    {
    CCaHandlerPlugin* handler = NULL;
    TBuf8<KCaMaxTypeLen> type;
    type.Copy( aType );
    TEComResolverParams resolverParams;
    resolverParams.SetDataType( type );
    RImplInfoPtrArray implInfoArray;
    CleanupStack::PushL(
            TCleanupItem( ResetAndDestroyArray, &implInfoArray ) );
    REComSession::ListImplementationsL( TUid::Uid( HANDLER_IF_UID ),
            resolverParams, implInfoArray );

    __ASSERT_DEBUG( implInfoArray.Count() <= 1, User::Invariant() );

    if( implInfoArray.Count() != 0 )
        {
        TUid implUid = implInfoArray[0]->ImplementationUid();
        handler = CCaHandlerPlugin::NewL( implUid );
        CleanupStack::PushL( handler );
        // Here we check if this is the handler we need. Currently we only do
        // a sanity check for the handler (it should support the registered
        // types!), but later this can be extended with better support query.
        // E.g. support for type AND command, etc.
        if( handler->SupportsType( aType ) )
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
