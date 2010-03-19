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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline CCaHandlerPlugin* CCaHandlerPlugin::NewL( TUid aImplementationUid )
    {
    TAny* ptr = REComSession::CreateImplementationL( aImplementationUid,
            _FOFF( CCaHandlerPlugin, iDtor_ID_Key ) );
    return reinterpret_cast<CCaHandlerPlugin*> ( ptr );
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline CCaHandlerPlugin::CCaHandlerPlugin()
    {
    }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
inline CCaHandlerPlugin::~CCaHandlerPlugin()
    {
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }
