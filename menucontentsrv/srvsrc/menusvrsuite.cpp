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
* Description:  Definition of menu item attribute
*
*/

#include <s32strm.h>
#include "menusvrsuite.h"
#include "mcssuiteobject.h"

// ================= LOCAL FUNCTIONS =======================

/**
* Cleanup support method. Call ResetAndDestroy() on the array.
* @param RMenuSrvSuiteArray* as TAny*
*/
LOCAL_C void ResetAndDestroy( TAny* aArray )
    {
    ((RMenuSrvSuiteArray*)aArray)->ResetAndDestroy();
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// RMenuSrvSuiteArray::FindLastSuite
// ---------------------------------------------------------
//
TInt RMenuSrvSuiteArray::FindLastSuite( const TDesC& aSuiteName )
	{
	for ( TInt i = Count()-1; i >= 0; i-- )
		{
		if( aSuiteName == operator[]( i )->SuiteName() )
			{
			return i;
			}
		}
	return KErrNotFound;
	}

// ---------------------------------------------------------
// RMenuSrvSuiteArray::FindByFile
// ---------------------------------------------------------
//
TInt RMenuSrvSuiteArray::FindByFile( const TDesC& aFileName )
	{
	for ( TInt i = Count()-1; i >= 0; i-- )
		{
		if( aFileName == operator[]( i )->FileName() )
			{
			return i;
			}
		}
	return KErrNotFound;
	}

// ---------------------------------------------------------
// RMenuSrvSuiteArray::Exist
// ---------------------------------------------------------
//
TBool RMenuSrvSuiteArray::Exist( const TDesC& aSuiteName )
	{
	return ( FindLastSuite(aSuiteName) != KErrNotFound );
	}

// ---------------------------------------------------------
// RMenuSrvSuiteArray::RemoveByFileName
// ---------------------------------------------------------
//
void RMenuSrvSuiteArray::RemoveByFileName( const TDesC& aFileName  )
	{
	for ( TInt i = 0; i < Count(); i++ )
		{
		if( aFileName == operator[]( i )->FileName() )
			{
			delete operator[]( i );
			Remove( i );
			i--;
			}
		}
	}

// ---------------------------------------------------------
// RMenuSrvSuiteArray::Append
// ---------------------------------------------------------
//
TInt RMenuSrvSuiteArray::Append(const CSuiteObject* anEntry)
	{
	TInt pos = FindLastSuite( anEntry->SuiteName() );
	if (pos != KErrNotFound)
		{
		operator[]( pos )->CloseSuiteArray();
		}
	return RPointerArray<CSuiteObject>::Append( anEntry );
	}


// ---------------------------------------------------------
// CleanupResetAndDestroyPushL
// ---------------------------------------------------------
//
void CleanupResetAndDestroyPushL( RMenuSrvSuiteArray& aArray )
	{
	CleanupStack::PushL( TCleanupItem( ResetAndDestroy, &aArray ) );
	}
