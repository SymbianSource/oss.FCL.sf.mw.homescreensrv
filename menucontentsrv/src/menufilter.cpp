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
#include "mcsmenufilter.h"
#include "menuitemattr.h"

#include <s32strm.h>

NONSHARABLE_CLASS( CMenuFilter::TData )
    {

public:     // data

    TUint32 iFlagsOn; ///< These flags should be on.
    TUint32 iFlagsOff; ///< These flags should be off.
    TBuf<KMenuMaxTypeLen> iType; ///< Type filter (empty == filter off).
    RMenuItemAttrArray iAttributes; ///< Attribute filter.

    };

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuFilter::~CMenuFilter
// ---------------------------------------------------------
//
EXPORT_C CMenuFilter::~CMenuFilter()
    {
    if ( iData )
        {
        iData->iAttributes.ResetAndDestroy();
        delete iData;
        }
    }

// ---------------------------------------------------------
// CMenuFilter::NewL
// ---------------------------------------------------------
//
EXPORT_C CMenuFilter* CMenuFilter::NewL()
    {
    CMenuFilter* filter = NewLC();
    CleanupStack::Pop( filter );
    return filter;
    }

// ---------------------------------------------------------
// CMenuFilter::NewLC
// ---------------------------------------------------------
//
EXPORT_C CMenuFilter* CMenuFilter::NewLC()
    {
    CMenuFilter* filter = new (ELeave) CMenuFilter();
    CleanupStack::PushL( filter );
    filter->ConstructL();
    return filter;
    }

// ---------------------------------------------------------
// CMenuFilter::CMenuFilter
// ---------------------------------------------------------
//
CMenuFilter::CMenuFilter()
    {
    }

// ---------------------------------------------------------
// CMenuFilter::ConstructL
// ---------------------------------------------------------
//
void CMenuFilter::ConstructL()
    {
    iData = new (ELeave) CMenuFilter::TData();
    Reset();
    }

// ---------------------------------------------------------
// CMenuFilter::FlagsOn
// ---------------------------------------------------------
//
EXPORT_C TUint32 CMenuFilter::FlagsOn() const
    {
    return iData->iFlagsOn;
    }

// ---------------------------------------------------------
// CMenuFilter::FlagsOff
// ---------------------------------------------------------
//
EXPORT_C TUint32 CMenuFilter::FlagsOff() const
    {
    return iData->iFlagsOff;
    }

// ---------------------------------------------------------
// CMenuFilter::Type
// ---------------------------------------------------------
//
EXPORT_C TPtrC CMenuFilter::Type() const
    {
    return iData->iType;
    }

// ---------------------------------------------------------
// CMenuFilter::NumAttributes
// ---------------------------------------------------------
//
EXPORT_C TInt CMenuFilter::NumAttributes() const
    {
    return iData->iAttributes.Count();
    }

// ---------------------------------------------------------
// CMenuFilter::GetAttribute
// ---------------------------------------------------------
//
EXPORT_C TBool CMenuFilter::GetAttribute
( TInt aIndex, TPtrC& aAttrName, TPtrC& aAttrValue ) const
    {
    CMenuItemAttr* attr = iData->iAttributes[aIndex];
    aAttrName.Set( attr->Name() );
    if ( attr->Value() )
        {
        aAttrValue.Set( *attr->Value() );
        return ETrue;
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CMenuFilter::Reset
// ---------------------------------------------------------
//
EXPORT_C void CMenuFilter::Reset()
    {
    iData->iType = KNullDesC();
    iData->iAttributes.ResetAndDestroy();
    iData->iFlagsOn = 0;
    iData->iFlagsOff = 0;
    }

// ---------------------------------------------------------
// CMenuFilter::SetFlags
// ---------------------------------------------------------
//
EXPORT_C void CMenuFilter::SetFlags( TUint32 aFlagsOn, TUint32 aFlagsOff )
    {
    iData->iFlagsOn = aFlagsOn;
    iData->iFlagsOff = aFlagsOff;
    }

// ---------------------------------------------------------
// CMenuFilter::SetType
// ---------------------------------------------------------
//
EXPORT_C void CMenuFilter::SetType( const TDesC& aType )
    {
    iData->iType = aType;
    }

// ---------------------------------------------------------
// CMenuFilter::HaveAttributeL
// ---------------------------------------------------------
//
EXPORT_C void CMenuFilter::HaveAttributeL
( const TDesC& aAttrName, const TDesC& aAttrValue )
    {
    CMenuItemAttr* attr = NULL;
    TInt i = iData->iAttributes.Find( aAttrName );
    if ( KErrNotFound == i )
        {
        attr = CMenuItemAttr::NewLC( aAttrName );
        iData->iAttributes.AppendL( attr );
        CleanupStack::Pop( attr ); // Now owned by iAttributes.
        }
    else
        {
        attr = iData->iAttributes[i];
        }
    __ASSERT_DEBUG( attr, User::Invariant() );
    attr->SetValue( aAttrValue.AllocL() );
    }

// ---------------------------------------------------------
// CMenuFilter::DoNotHaveAttributeL
// ---------------------------------------------------------
//
EXPORT_C void CMenuFilter::DoNotHaveAttributeL( const TDesC& aAttrName )
    {
    CMenuItemAttr* attr = NULL;
    TInt i = iData->iAttributes.Find( aAttrName );
    if ( KErrNotFound == i )
        {
        attr = CMenuItemAttr::NewLC( aAttrName );
        iData->iAttributes.AppendL( attr );
        CleanupStack::Pop( attr ); // Now owned by iAttributes.
        }
    else
        {
        attr = iData->iAttributes[i];
        }
    __ASSERT_DEBUG( attr, User::Invariant() );
    attr->SetValue( NULL );
    }

// ---------------------------------------------------------
// CMenuFilter::RemoveAttribute
// ---------------------------------------------------------
//
EXPORT_C void CMenuFilter::RemoveAttribute( const TDesC& aAttrName )
    {
    TInt i = iData->iAttributes.Find( aAttrName );
    if ( KErrNotFound != i )
        {
        delete iData->iAttributes[i];
        iData->iAttributes.Remove( i );
        }
    }

// ---------------------------------------------------------
// CMenuFilter::ExternalizeL
// ---------------------------------------------------------
//
void CMenuFilter::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteUint32L( iData->iFlagsOn );
    aStream.WriteUint32L( iData->iFlagsOff );
    aStream.WriteInt32L( iData->iType.Length() );
    aStream.WriteL( iData->iType );
    iData->iAttributes.ExternalizeL( aStream );
    }

// ---------------------------------------------------------
// CMenuFilter::InternalizeL
// ---------------------------------------------------------
//
void CMenuFilter::InternalizeL( RReadStream& aStream )
    {
    iData->iFlagsOn = aStream.ReadUint32L();
    iData->iFlagsOff = aStream.ReadUint32L();
    TInt len = aStream.ReadInt32L();
    aStream.ReadL( iData->iType, len );
    iData->iAttributes.ResetAndDestroy();
    iData->iAttributes.InternalizeL( aStream );
    }
