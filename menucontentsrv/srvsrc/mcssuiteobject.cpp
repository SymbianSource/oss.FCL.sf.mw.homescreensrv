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
#include "mcssuiteobject.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSuiteObject::~CSuiteObject
// ---------------------------------------------------------
//
CSuiteObject::~CSuiteObject()
    {
    iAttributes.ResetAndDestroy();
    iFileName.Close();
    iName.Close();
    }

// ---------------------------------------------------------
// CSuiteObject::NewL
// ---------------------------------------------------------
//
CSuiteObject* CSuiteObject::NewL( const TDesC& aFileName )
    {
    CSuiteObject* attr = NewLC( aFileName );
    CleanupStack::Pop( attr );
    return attr;
    }

// ---------------------------------------------------------
// CSuiteObject::NewLC
// ---------------------------------------------------------
//
CSuiteObject* CSuiteObject::NewLC( const TDesC& aFileName )
    {
    CSuiteObject* attr = new (ELeave) CSuiteObject();
    CleanupStack::PushL( attr );
    attr->ConstructL( aFileName );
    return attr;
    }

// ---------------------------------------------------------
// CSuiteObject::CSuiteObject
// ---------------------------------------------------------
//
CSuiteObject::CSuiteObject()
	{
	}

// ---------------------------------------------------------
// CSuiteObject::ConstructL
// ---------------------------------------------------------
//
void CSuiteObject::ConstructL( const TDesC& aFileName )
    {
    iFileName.CreateL( aFileName );
    }

// ---------------------------------------------------------
// CSuiteObject::SetSuiteNameL
// ---------------------------------------------------------
//
void CSuiteObject::SetSuiteNameL( const TDesC& aName )
	{
    iName.CreateL( aName );
	}

// ---------------------------------------------------------
// CSuiteObject::SetInitialAttributeL
// ---------------------------------------------------------
//
void CSuiteObject::SetInitialAttributeL( const TDesC& aAttrName, 
		const TDesC& aAttrValue, 
		TBool aLocalized )
    {
    CSuiteObjectAttr* attr = CSuiteObjectAttr::NewLC( aAttrName );
    iAttributes.AppendL( attr );
    CleanupStack::Pop( attr );
    __ASSERT_DEBUG( attr, User::Invariant() );
    attr->SetValueL( aAttrValue );
    attr->SetLocalized( aLocalized );
    }

// ---------------------------------------------------------
// CSuiteObject::GetAttributeL
// ---------------------------------------------------------
//
void CSuiteObject::GetAttribute( const TDesC& aAttrName, 
		TBool& aAttrExists, TDes& aAttrVal )
	{
	aAttrExists = EFalse;
	TInt attrPos = iAttributes.Find(aAttrName);
	if (attrPos != KErrNotFound)
		{
		aAttrVal.Copy( iAttributes[attrPos]->Value() );
		aAttrExists = ETrue;
		}
	}

// ---------------------------------------------------------
// CSuiteObject::CloseSuiteArray
// ---------------------------------------------------------
//
void CSuiteObject::CloseSuiteArray()
	{
	iAttributes.ResetAndDestroy();
	iAttributes.Close();
	}


//  End of File  
