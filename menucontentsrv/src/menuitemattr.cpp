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
#include "menuitemattr.h"

// ================= LOCAL FUNCTIONS =======================

/**
* Cleanup support method. Call ResetAndDestroy() on the array.
* @param RMenuItemArray* as TAny*
*/
LOCAL_C void ResetAndDestroy( TAny* aArray )
    {
    ((RMenuItemAttrArray*)aArray)->ResetAndDestroy();
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuItemAttr::~CMenuItemAttr
// ---------------------------------------------------------
//
CMenuItemAttr::~CMenuItemAttr()
    {
    iName.Close();
    delete iValue;
    }

// ---------------------------------------------------------
// CMenuItemAttr::NewL
// ---------------------------------------------------------
//
CMenuItemAttr* CMenuItemAttr::NewL( const TDesC& aName )
    {
    CMenuItemAttr* attr = NewLC( aName );
    CleanupStack::Pop( attr );
    return attr;
    }

// ---------------------------------------------------------
// CMenuItemAttr::NewLC
// ---------------------------------------------------------
//
CMenuItemAttr* CMenuItemAttr::NewLC( const TDesC& aName )
    {
    CMenuItemAttr* attr = new (ELeave) CMenuItemAttr();
    CleanupStack::PushL( attr );
    attr->ConstructL( aName );
    return attr;
    }

// ---------------------------------------------------------
// CMenuItemAttr::NewLC
// ---------------------------------------------------------
//
CMenuItemAttr* CMenuItemAttr::NewLC( RReadStream& aStream )
    {
    CMenuItemAttr* attr = new (ELeave) CMenuItemAttr();
    CleanupStack::PushL( attr );
    attr->InternalizeL( aStream );
    return attr;
    }

// ---------------------------------------------------------
// CMenuItemAttr::ConstructL
// ---------------------------------------------------------
//
void CMenuItemAttr::ConstructL( const TDesC& aName )
    {
    iName.CreateL( aName );
    }

// ---------------------------------------------------------
// CMenuItemAttr::SetValue
// ---------------------------------------------------------
//
void CMenuItemAttr::SetValue( HBufC* aValue )
    {
    if ( iValue )
        {
        if ( aValue && *iValue == *aValue )
            {
            // String to same string: no change.
            delete aValue;
            }
        else
            {
            // String to other string or NULL: change.
            iChanged = ETrue;
            delete iValue;
            iValue = aValue;
            }
        }
    else
        {
        if ( aValue )
            {
            // NULL to string: change.
            iChanged = ETrue;
            iValue = aValue;
            }
        else
            {
            // NULL to NULL: no change.
            }
        }
    }

// ---------------------------------------------------------
// CMenuItemAttr::ExternalizeL
// ---------------------------------------------------------
//
void CMenuItemAttr::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iName.Length() );
    aStream.WriteL( iName );
    if ( iValue )
        {
        aStream.WriteInt32L( iValue->Length() );
        aStream.WriteL( *iValue );
        }
    else
        {
        aStream.WriteInt32L( KErrNotFound );
        }
    }

// ---------------------------------------------------------
// CMenuItemAttr::InternalizeL
// ---------------------------------------------------------
//
void CMenuItemAttr::InternalizeL( RReadStream& aStream )
    {
    iName.Close();
    delete iValue; iValue = NULL;
    TInt len;
    len = aStream.ReadInt32L();
    iName.CreateL( len );
    aStream.ReadL( iName, len );
    len = aStream.ReadInt32L();
    if ( len >= 0 )
        {
        iValue = HBufC::NewL( len );
        TPtr ptr( iValue->Des() );
        aStream.ReadL( ptr, len );
        }
    }

// ---------------------------------------------------------
// RMenuItemAttrArray::Find
// ---------------------------------------------------------
//
TInt RMenuItemAttrArray::Find( const TDesC& aName ) const
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

// ---------------------------------------------------------
// RMenuItemAttrArray::CountChanged
// ---------------------------------------------------------
//
TInt RMenuItemAttrArray::CountChanged() const
    {
    TInt count = 0;
    for ( TInt i = 0; i < Count(); i++ )
        {
        if ( operator[]( i )->Changed() )
            {
            count++;
            }
        }
    return count;
    }

// ---------------------------------------------------------
// RMenuItemAttrArray::ClearChanged
// ---------------------------------------------------------
//
void RMenuItemAttrArray::ClearChanged()
    {
    for ( TInt i = 0; i < Count(); i++ )
        {
        operator[]( i )->SetChanged( EFalse );
        }
    }

// ---------------------------------------------------------
// RMenuItemAttrArray::ExternalizeChangesL
// ---------------------------------------------------------
//
void RMenuItemAttrArray::ExternalizeChangesL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( CountChanged() );
    for ( TInt i = 0; i < Count(); i++ )
        {
        const CMenuItemAttr* attr = operator[]( i );
        if ( attr->Changed() )
            {
            attr->ExternalizeL( aStream );
            }
        }
    }

// ---------------------------------------------------------
// RMenuItemAttrArray::ExternalizeL
// ---------------------------------------------------------
//
void RMenuItemAttrArray::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( Count() );
    for ( TInt i = 0; i < Count(); i++ )
        {
        operator[]( i )->ExternalizeL( aStream );
        }
    }

// ---------------------------------------------------------
// RMenuItemAttrArray::InternalizeL
// ---------------------------------------------------------
//
void RMenuItemAttrArray::InternalizeL( RReadStream& aStream )
    {
    TInt count = aStream.ReadInt32L();
    for ( TInt i = 0; i < count; i++ )
        {
        CMenuItemAttr* attr = CMenuItemAttr::NewLC( aStream );
        AppendL( attr );
        CleanupStack::Pop( attr );
        }
    }

// ---------------------------------------------------------
// CleanupResetAndDestroyPushL
// ---------------------------------------------------------
//
void CleanupResetAndDestroyPushL( RMenuItemAttrArray& aArray )
    {
    CleanupStack::PushL( TCleanupItem( ResetAndDestroy, &aArray ) );
    }

//  End of File  
