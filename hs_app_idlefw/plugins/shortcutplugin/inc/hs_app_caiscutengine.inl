/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Inline functions for the shortcut plug-in engine
*
*/


// ---------------------------------------------------------------------------
// Returns the application architecture server session owned by the engine.
// ---------------------------------------------------------------------------
//
inline RApaLsSession& CAiScutEngine::ApaSession()
{
    return iApaSession;
}

// ---------------------------------------------------------------------------
// Returns reference to the bookmark database owned by the engine.
// ---------------------------------------------------------------------------
//
inline RFavouritesDb& CAiScutEngine::FavouritesDb()
{
    return iBookmarkDb;
}

// ---------------------------------------------------------------------------
// Returns the message server session owned by the engine.
// ---------------------------------------------------------------------------
//
inline CMsvSession* CAiScutEngine::MsvSession()
{
    return iMsvSession;
}

// ---------------------------------------------------------------------------
// Returns a pointer to view server session wrapper.
// ---------------------------------------------------------------------------
//
inline CVwsSessionWrapper* CAiScutEngine::VwsSession()
{
    return iVwsSession;
}

// ---------------------------------------------------------------------------
// Returns a pointer to the coe environment
// ---------------------------------------------------------------------------
//
inline CCoeEnv* CAiScutEngine::Env()
{
    return iEnv;
}

// End of File.
