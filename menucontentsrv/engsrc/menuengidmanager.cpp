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

#include "menuengidmanager.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuEngIdManager::~CMenuEngIdManager
// ---------------------------------------------------------
//
CMenuEngIdManager::~CMenuEngIdManager()
    {
    iIds.Close();
    }

// ---------------------------------------------------------
// CMenuEngIdManager::CMenuEngIdManager
// ---------------------------------------------------------
//
CMenuEngIdManager::CMenuEngIdManager()
    {
    }

// ---------------------------------------------------------
// CMenuEngIdManager::AllocL
// ---------------------------------------------------------
//
void CMenuEngIdManager::AllocL( TInt& aId )
    {
    TInt id = 0;
    TInt startSeed = iSeed;
    do
        {
        iSeed++;
        if ( iSeed == startSeed )
            {
            // No more free IDs. This happens when we used ALL possible
            // integer numbers. (4GB of memory used for the ID-s only!)
            // Happy coverage testing! ;)
            User::Leave( KErrNoMemory );
            }
        if ( KErrNotFound == iIds.FindInOrder( iSeed ) )
            {
            id = iSeed;
            }
        }
    while ( !id );
    iIds.InsertInOrderL( id );
    aId = id;
    }

// ---------------------------------------------------------
// CMenuEngIdManager::AddL
// ---------------------------------------------------------
//
void CMenuEngIdManager::AddL( TInt aId )
    {
    if ( aId && KErrNotFound == iIds.FindInOrder( aId ) )
        {
        iIds.InsertInOrderL( aId );
        }
    }

// ---------------------------------------------------------
// CMenuEngIdManager::Remove
// ---------------------------------------------------------
//
void CMenuEngIdManager::Remove( TInt aId )
    {
    TInt i = iIds.FindInOrder( aId );
    if ( KErrNotFound != i )
        {
        iIds.Remove( i );
        }
    }

//  End of File  
