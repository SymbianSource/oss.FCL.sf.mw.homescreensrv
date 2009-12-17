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


// INCLUDE FILES

#include <s32strm.h>
#include "mcssuiteobjectattr.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSuiteObjectAttr::~CSuiteObjectAttr
// ---------------------------------------------------------
//
CSuiteObjectAttr::~CSuiteObjectAttr()
    {
    iName.Close();
    iValue.Close();
    }

// ---------------------------------------------------------
// CSuiteObjectAttr::NewL
// ---------------------------------------------------------
//
CSuiteObjectAttr* CSuiteObjectAttr::NewL( const TDesC& aName )
    {
    CSuiteObjectAttr* attr = NewLC( aName );
    CleanupStack::Pop( attr );
    return attr;
    }

// ---------------------------------------------------------
// CSuiteObjectAttr::NewLC
// ---------------------------------------------------------
//
CSuiteObjectAttr* CSuiteObjectAttr::NewLC( const TDesC& aName )
    {
    CSuiteObjectAttr* attr = new (ELeave) CSuiteObjectAttr();
    CleanupStack::PushL( attr );
    attr->ConstructL( aName );
    return attr;
    }

// ---------------------------------------------------------
// CSuiteObjectAttr::CSuiteObjectAttr
// ---------------------------------------------------------
//
CSuiteObjectAttr::CSuiteObjectAttr()
	{
	}

// ---------------------------------------------------------
// CSuiteObjectAttr::ConstructL
// ---------------------------------------------------------
//
void CSuiteObjectAttr::ConstructL( const TDesC& aName )
    {
    iName.CreateL( aName );
    }

// ---------------------------------------------------------
// CSuiteObjectAttr::SetValueL
// ---------------------------------------------------------
//
void CSuiteObjectAttr::SetValueL( const TDesC& aValue )
    {
    iValue.Close();
    iValue.CreateL( aValue );
    }

// ---------------------------------------------------------
// RSuiteAttrArray::Find
// ---------------------------------------------------------
//
TInt RSuiteAttrArray::Find( const TDesC& aName ) const
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
