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
* Description:  
*
*/


#include "mcsoperationmanager.h"


// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
RMcsOperationManager::~RMcsOperationManager()
	{
	}

// ---------------------------------------------------------------------------
// destructor
// ---------------------------------------------------------------------------
void RMcsOperationManager::Close()
	{
    iOperations.ResetAndDestroy();
	}


// ---------------------------------------------------------------------------
// default constructor
// ---------------------------------------------------------------------------
RMcsOperationManager::RMcsOperationManager(  )				
	{
	}

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void RMcsOperationManager::AddL(  CActive* aOperation )
    {
    User::LeaveIfError( iOperations.Append( aOperation ) );
    }

// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
void RMcsOperationManager::Remove( CActive* aOperation )
    {
    TInt ptrIndex( iOperations.Find( aOperation ) );
    if ( KErrNotFound != ptrIndex )
        {
        iOperations.Remove( ptrIndex );
        }
    }
