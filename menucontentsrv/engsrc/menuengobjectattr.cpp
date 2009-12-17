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

#include <s32strm.h>
#include "menuengobjectattr.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuEngObjectAttr::~CMenuEngObjectAttr
// ---------------------------------------------------------
//
CMenuEngObjectAttr::~CMenuEngObjectAttr()
    {
    iName.Close();
    iValue.Close();
    }

// ---------------------------------------------------------
// CMenuEngObjectAttr::NewL
// ---------------------------------------------------------
//
CMenuEngObjectAttr* CMenuEngObjectAttr::NewL( const TDesC& aName )
    {
    CMenuEngObjectAttr* attr = NewLC( aName );
    CleanupStack::Pop( attr );
    return attr;
    }

// ---------------------------------------------------------
// CMenuEngObjectAttr::NewLC
// ---------------------------------------------------------
//
CMenuEngObjectAttr* CMenuEngObjectAttr::NewLC( const TDesC& aName )
    {
    CMenuEngObjectAttr* attr = new (ELeave) CMenuEngObjectAttr();
    CleanupStack::PushL( attr );
    attr->ConstructL( aName );
    return attr;
    }

// ---------------------------------------------------------
// CMenuEngObjectAttr::CMenuEngObjectAttr
// ---------------------------------------------------------
//
CMenuEngObjectAttr::CMenuEngObjectAttr()
	{
	}

// ---------------------------------------------------------
// CMenuEngObjectAttr::ConstructL
// ---------------------------------------------------------
//
void CMenuEngObjectAttr::ConstructL( const TDesC& aName )
    {
    iName.CreateL( aName );
    }

// ---------------------------------------------------------
// CMenuEngObjectAttr::SetValueL
// ---------------------------------------------------------
//
void CMenuEngObjectAttr::SetValueL( const TDesC& aValue )
    {
    iValue.Close();
    iValue.CreateL( aValue );
    }

// ---------------------------------------------------------
// RMenuEngObjectAttrArray::Find
// ---------------------------------------------------------
//
TInt RMenuEngObjectAttrArray::Find( const TDesC& aName ) const
    {
    for ( TInt i = 0; i < Count(); i++ )
        {
        if ( !aName.Compare( operator[]( i )->Name() ) )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

//  End of File  
