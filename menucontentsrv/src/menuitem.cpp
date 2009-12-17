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

#include "mcsmenuitem.h"
#include "menuitemattr.h"
#include "menubuf.h"
#include "mcsmenu.h"
#include "menuclientoperation.h"
#include "menucompletedoperation.h"


NONSHARABLE_CLASS( CMenuItem::TData )
    {

public:     // construction

    TData( RMenu& aMenu ): iMenu( aMenu ) {}

public:     // data

    RMenu& iMenu;
    TMenuItem iHdr;
    TUint32 iOrigFlags;
    RMenuItemAttrArray iAttrCache;
    TInt iInsertBefore;

    };

// ================= LOCAL FUNCTIONS =======================

/**
* Check if this is a valid attribute name. Leave with KErrArgument if not.
* We don't accept whitespace or exotic characters anywhere, and also
* forbid setting reserved attribute "flags".
* The Engine also checks this, but it's more friendly to leave on client
* side too - no point in attempting something that we know will fail.
*/
LOCAL_C void CheckAttrNameL( const TDesC& aAttrName )
    {
    if ( !aAttrName.Length() )
        {
        User::Leave( KErrArgument );
        }
    if ( KMenuAttrFlags() == aAttrName || KMenuAttrId() == aAttrName )
        {
        // Internal attributes, disallow.
        User::Leave( KErrArgument );
        }
    for ( TInt i = 0; i < aAttrName.Length(); i++ )
        {
        const TChar c = aAttrName[i];
        // Allowed set of characters
        if ( !c.IsAlphaDigit() &&
             '_' != c &&
             '-' != c &&
             ':' != c )
            {
            User::Leave( KErrArgument );
            }
        }
    }


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// TMenuItem::ExternalizeL
// ---------------------------------------------------------
//
void TMenuItem::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iId );
    aStream.WriteInt32L( iParent );
    aStream.WriteUint32L( iFlags );
    aStream.WriteInt32L( iType.Length() );
    aStream.WriteL( iType );
    }

// ---------------------------------------------------------
// TMenuItem::InternalizeL
// ---------------------------------------------------------
//
void TMenuItem::InternalizeL( RReadStream& aStream )
    {
    iId = aStream.ReadInt32L();
    iParent = aStream.ReadInt32L();
    iFlags = aStream.ReadUint32L();
    TInt len = aStream.ReadInt32L();
    aStream.ReadL( iType, len );
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuItem::~CMenuItem
// ---------------------------------------------------------
//
EXPORT_C CMenuItem::~CMenuItem()
    {
    if ( iData )
        {
        iData->iAttrCache.ResetAndDestroy();
        delete iData;
        }
    }

// ---------------------------------------------------------
// CMenuItem::CreateL
// ---------------------------------------------------------
//
EXPORT_C CMenuItem* CMenuItem::CreateL
( RMenu& aMenu, const TDesC& aType, TInt aFolder, TInt aInsertBefore )
    {
    if ( KMenuTypeData() == aType )
        {
        // Internal type, disallow.
        User::Leave( KErrArgument );
        }
    // Item created locally, no IPC.
    CMenuItem* item = NewLC( aMenu );
    item->iData->iHdr.SetType( aType );
    item->iData->iHdr.SetParent( aFolder );
    item->iData->iOrigFlags = item->iData->iHdr.Flags();
    item->iData->iInsertBefore = aInsertBefore;
    CleanupStack::Pop( item );
    return item;
    }

// ---------------------------------------------------------
// CMenuItem::OpenL
// ---------------------------------------------------------
//
EXPORT_C CMenuItem* CMenuItem::OpenL( RMenu& aMenu, TInt aId )
    {
    // Get header from server, IPC needed.
    return OpenL( aMenu, aMenu.GetHdrL( aId ) );
    }

// ---------------------------------------------------------
// CMenuItem::OpenL
// ---------------------------------------------------------
//
EXPORT_C CMenuItem* CMenuItem::OpenL( RMenu& aMenu, const TMenuItem& aHdr )
    {
    // We accept the existing header, no IPC needed.
    CMenuItem* item = NewLC( aMenu );
    item->iData->iHdr = aHdr;
    item->iData->iOrigFlags = item->iData->iHdr.Flags();
    CleanupStack::Pop( item );
    return item;
    }

// ---------------------------------------------------------
// CMenuItem::CMenuItem
// ---------------------------------------------------------
//
CMenuItem::CMenuItem()
    {
    }

// ---------------------------------------------------------
// CMenuItem::NewLC
// ---------------------------------------------------------
//
CMenuItem* CMenuItem::NewLC( RMenu& aMenu )
    {
    CMenuItem* item = new (ELeave) CMenuItem();
    CleanupStack::PushL( item );
    item->iData = new (ELeave) CMenuItem::TData( aMenu );
    return item;
    }

// ---------------------------------------------------------
// CMenuItem::Id
// ---------------------------------------------------------
//
EXPORT_C TInt CMenuItem::Id() const
    {
    return iData->iHdr.Id();
    }

// ---------------------------------------------------------
// CMenuItem::Parent
// ---------------------------------------------------------
//
EXPORT_C TInt CMenuItem::Parent() const
    {
    return iData->iHdr.Parent();
    }

// ---------------------------------------------------------
// CMenuItem::Flags
// ---------------------------------------------------------
//
EXPORT_C TUint32 CMenuItem::Flags() const
    {
    return iData->iHdr.Flags();
    }

// ---------------------------------------------------------
// CMenuItem::SetFlags
// ---------------------------------------------------------
//
EXPORT_C void CMenuItem::SetFlags( TUint32 aMask, TBool aOn )
    {
    if ( aOn )
        {
        iData->iHdr.SetFlags( iData->iHdr.Flags() | aMask );
        }
    else
        {
        iData->iHdr.SetFlags( iData->iHdr.Flags() & ~aMask );
        }
    }

// ---------------------------------------------------------
// CMenuItem::Type
// ---------------------------------------------------------
//
EXPORT_C TPtrC CMenuItem::Type() const
    {
    return iData->iHdr.Type();
    }

// ---------------------------------------------------------
// CMenuItem::GetAttributeL
// ---------------------------------------------------------
//
EXPORT_C TPtrC CMenuItem::GetAttributeL
( const TDesC& aAttrName, TBool& aAttrExists )
    {
    CheckAttrNameL( aAttrName );
    CMenuItemAttr* attr = NULL;
    TInt i = iData->iAttrCache.Find( aAttrName );
    if ( KErrNotFound == i )
        {
        attr = CMenuItemAttr::NewLC( aAttrName );
        HBufC* value;
        if( iData->iHdr.Id() )
            {
            value = iData->iMenu.GetAttributeL( Id(), aAttrName );
            }
           else
            {
            value = NULL;
            }

        attr->SetValue( value ); // Takes ownership.
        attr->SetChanged( EFalse ); // New in cache -> not changed.
        iData->iAttrCache.AppendL( attr );
        CleanupStack::Pop( attr ); // Now owned by the cache.
        }
    else
        {
        attr = iData->iAttrCache[i];
        }
    __ASSERT_DEBUG( attr, User::Invariant() ); // Should be cached by now.
    if ( attr->Value() )
        {
        aAttrExists = ETrue;
        return *attr->Value();
        }
    aAttrExists = EFalse;
    return KNullDesC();
    }

// ---------------------------------------------------------
// CMenuItem::GetAttributeL
// ---------------------------------------------------------
//
EXPORT_C void CMenuItem::GetAttributeListL( RArray<TAttributeName>& aList )
    {
    iData->iMenu.GetAttributeListL( Id(), aList );
    }

// ---------------------------------------------------------
// CMenuItem::SetAttributeL
// ---------------------------------------------------------
//
EXPORT_C void CMenuItem::SetAttributeL
( const TDesC& aAttrName, const TDesC& aAttrValue )
    {
    CheckAttrNameL( aAttrName );
    CMenuItemAttr* attr = NULL;
    TInt i = iData->iAttrCache.Find( aAttrName );
    if ( KErrNotFound == i )
        {
        attr = CMenuItemAttr::NewLC( aAttrName );
        attr->SetChanged( ETrue ); // Changed.
        iData->iAttrCache.AppendL( attr );
        CleanupStack::Pop( attr ); // Now owned by the cache.
        }
    else
        {
        attr = iData->iAttrCache[i];
        }
    __ASSERT_DEBUG( attr, User::Invariant() ); // Should be cached by now.
    attr->SetValue( aAttrValue.AllocL() ); // Sets changed bit as needed.
    }

// ---------------------------------------------------------
// CMenuItem::RemoveAttributeL
// ---------------------------------------------------------
//
EXPORT_C void CMenuItem::RemoveAttributeL( const TDesC& aAttrName )
    {
    CheckAttrNameL( aAttrName );
    CMenuItemAttr* attr = NULL;
    TInt i = iData->iAttrCache.Find( aAttrName );
    if ( KErrNotFound == i )
        {
        attr = CMenuItemAttr::NewLC( aAttrName );
        attr->SetChanged( ETrue ); // Changed.
        iData->iAttrCache.AppendL( attr );
        CleanupStack::Pop( attr ); // Now owned by the cache.
        }
    else
        {
        attr = iData->iAttrCache[i];
        }
    __ASSERT_DEBUG( attr, User::Invariant() ); // Should be cached by now.
    attr->SetValue( NULL ); // Sets changed bit as needed.
    }

// ---------------------------------------------------------
// CMenuItem::SaveL
// ---------------------------------------------------------
//
EXPORT_C CMenuOperation* CMenuItem::SaveL( TRequestStatus& aStatus )
    {
    if ( !Changed() )
        {
        return CMenuCompletedOperation::NewL
            ( iData->iMenu, CActive::EPriorityStandard, aStatus, KErrNone );
        }
    if ( Id() )
        {
        return UpdateL( aStatus );
        }
    return AddL( aStatus );
    }

// ---------------------------------------------------------
// CMenuItem::HandleCommandL
// ---------------------------------------------------------
//
EXPORT_C CMenuOperation* CMenuItem::HandleCommandL(
        const TDesC8& aCommand,
        const TDesC8& aParams,
        TRequestStatus& aStatus )
    {
    return iData->iMenu.HandleCommandL( *this, aCommand, aParams, aStatus );
    }

// ---------------------------------------------------------
// CMenuItem::AddL
// ---------------------------------------------------------
//
CMenuOperation* CMenuItem::AddL( TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( Changed(), User::Invariant() );
    __ASSERT_DEBUG( !Id(), User::Invariant() );
    RMenuBuf buf;
    User::LeaveIfError( buf.Open( iData->iMenu ) );
    CleanupClosePushL( buf );
    RWriteStream stream( &buf );
    // Flags.
    stream.WriteUint32L( iData->iHdr.Flags() );
    __ASSERT_DEBUG( 0 == iData->iOrigFlags, User::Invariant() ); // New item!
    // Attributes.
    __ASSERT_DEBUG( iData->iAttrCache.Count() == \
        iData->iAttrCache.CountChanged(), User::Invariant() ); // New item!
    iData->iAttrCache.ExternalizeL( stream );
    // Parent folder and insertion point.
    stream.WriteInt32L( Parent() );
    stream.WriteInt32L( iData->iInsertBefore );
    stream.CommitL();
    CMenuClientOperation* op = new (ELeave) CMenuClientOperation
        ( iData->iMenu, CActive::EPriorityStandard, aStatus );
    CleanupStack::PushL( op );
    TInt id = op->AddL( Type(), buf );
    __ASSERT_DEBUG( id, User::Invariant() );
    iData->iHdr.SetId( id ); // Write ID back.
    CleanupStack::Pop( op );
    CleanupStack::PopAndDestroy( &buf );
    ClearChanged();
    return op;
    }

// ---------------------------------------------------------
// CMenuItem::UpdateL
// ---------------------------------------------------------
//
CMenuOperation* CMenuItem::UpdateL( TRequestStatus& aStatus )
    {
    __ASSERT_DEBUG( Changed(), User::Invariant() );
    __ASSERT_DEBUG( Id(), User::Invariant() );
    RMenuBuf buf;
    User::LeaveIfError( buf.Open( iData->iMenu ) );
    CleanupClosePushL( buf );
    RWriteStream stream( &buf );
    // Flags and flag changes (small -> always sent).
    stream.WriteUint32L( iData->iHdr.Flags() );
    stream.WriteUint32L( iData->iOrigFlags ^ iData->iHdr.Flags() );
    // Changed attributes.
    iData->iAttrCache.ExternalizeChangesL( stream );
    stream.CommitL();
    CMenuClientOperation* op = new (ELeave) CMenuClientOperation
        ( iData->iMenu, CActive::EPriorityStandard, aStatus );
    CleanupStack::PushL( op );
    op->UpdateL( Id(), buf );
    CleanupStack::Pop( op );
    CleanupStack::PopAndDestroy( &buf );
    ClearChanged();
    return op;
    }

// ---------------------------------------------------------
// CMenuItem::Changed
// ---------------------------------------------------------
//
TBool CMenuItem::Changed() const
    {
    return !iData->iHdr.Id() || // New item
           iData->iHdr.Flags() != iData->iOrigFlags || // Changed flags
           iData->iAttrCache.CountChanged(); // Changed attributes
    }

// ---------------------------------------------------------
// CMenuItem::ClearChanged
// ---------------------------------------------------------
//
void CMenuItem::ClearChanged()
    {
    iData->iAttrCache.ClearChanged();
    iData->iOrigFlags = iData->iHdr.Flags();
    }


// ---------------------------------------------------------
// CMenuItem::RunningStatus
// ---------------------------------------------------------
//
EXPORT_C TBool CMenuItem::RunningStatusL()
    {
    TBool ret(EFalse);
    HBufC* value;
    value = iData->iMenu.GetAttributeL( Id(), KRunningStatus );
    if( value )
    	{
    	ret = ETrue;
    	}
    delete value;
    return ret;
    }


