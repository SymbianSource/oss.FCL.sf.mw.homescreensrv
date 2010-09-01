/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of menu handler plugin interface
*
*/

// -----------------------------------------------------------------------------
// CMenuHandlerPlugin::NewL
// -----------------------------------------------------------------------------
//
inline CMenuHandlerPlugin* CMenuHandlerPlugin::NewL
( TUid aImplementationUid, RMenu& aMenu )
    {
    TAny* ptr = REComSession::CreateImplementationL( 
                aImplementationUid,
                _FOFF( CMenuHandlerPlugin, iDtor_ID_Key ),
                &aMenu );
    return reinterpret_cast<CMenuHandlerPlugin*>( ptr );
    }

// -----------------------------------------------------------------------------
// CMenuHandlerPlugin::CMenuHandlerPlugin
// -----------------------------------------------------------------------------
//
inline CMenuHandlerPlugin::CMenuHandlerPlugin( RMenu &aMenu )
: CMenuHandler( aMenu )
    {
    }

// -----------------------------------------------------------------------------
// CMenuHandlerPlugin::~CMenuHandlerPlugin
// -----------------------------------------------------------------------------
//
inline CMenuHandlerPlugin::~CMenuHandlerPlugin()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }
