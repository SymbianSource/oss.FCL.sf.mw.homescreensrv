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

#include "menuenglegacyobjectfactory.h"
#include "menuengobject.h"
#include "menuengflags.h"
#include "mcsmenuitem.h"

// Legacy types
_LIT( KLegacyTypeData, "appshell:data" );
_LIT( KLegacyTypeApp, "appshell:application" );
_LIT( KLegacyTypeUrl, "appshell:url" );
_LIT( KLegacyTypeFolder, "appshell:folder" );

// Legacy attributes.
_LIT( KLegacyAttrLockDelete, "lock_delete" );
_LIT( KLegacyAttrLockName, "lock_name" );
_LIT( KLegacyAttrLockIcon, "lock_icon" );
_LIT( KLegacyAttrLockMoveInto, "lock_moveinto" );
_LIT( KLegacyAttrLockItem, "lock_item" );

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuEngLegacyObjectFactory::~CMenuEngLegacyObjectFactory
// ---------------------------------------------------------
//
CMenuEngLegacyObjectFactory::~CMenuEngLegacyObjectFactory()
    {
    }

// ---------------------------------------------------------
// CMenuEngLegacyObjectFactory::NewL
// ---------------------------------------------------------
//
CMenuEngLegacyObjectFactory* CMenuEngLegacyObjectFactory::NewL( CMenuEng& aEng )
    {
    CMenuEngLegacyObjectFactory* factory =
        new (ELeave) CMenuEngLegacyObjectFactory( aEng );
    return factory;
    }

// ---------------------------------------------------------
// CMenuEngLegacyObjectFactory::CMenuEngLegacyObjectFactory
// ---------------------------------------------------------
//
CMenuEngLegacyObjectFactory::CMenuEngLegacyObjectFactory( CMenuEng& aEng )
: iEng( aEng )
    {
    }

// ---------------------------------------------------------
// CMenuEngLegacyObjectFactory::IsSupportedType
// ---------------------------------------------------------
//
TBool CMenuEngLegacyObjectFactory::IsSupportedType( const TDesC& aLegacyTypeIdentifier ) const
    {
    TBool ret = EFalse;
    if( aLegacyTypeIdentifier.Compare( KLegacyTypeData ) == 0 ||
        aLegacyTypeIdentifier.Compare( KLegacyTypeApp ) == 0 || 
        aLegacyTypeIdentifier.Compare( KLegacyTypeUrl ) == 0 || 
        aLegacyTypeIdentifier.Compare( KLegacyTypeFolder ) == 0 )
        {
        ret = ETrue;
        }
    return ret;
    }

// ---------------------------------------------------------
// CMenuEngLegacyObjectFactory::ConvertedType
// ---------------------------------------------------------
//
TPtrC CMenuEngLegacyObjectFactory::ConvertedType( const TDesC& aLegacyTypeIdentifier ) const
    {
    TPtrC ret( KNullDesC );
    if( aLegacyTypeIdentifier.Compare( KLegacyTypeData ) == 0 )
        {
        ret.Set( KMenuTypeData );
        }
    else if( aLegacyTypeIdentifier.Compare( KLegacyTypeApp ) == 0 )
        {
        ret.Set( KMenuTypeApp );
        }
    else if ( aLegacyTypeIdentifier.Compare( KLegacyTypeUrl ) == 0 )
        {
        ret.Set( KMenuTypeUrl );
        }
    else if ( aLegacyTypeIdentifier.Compare( KLegacyTypeFolder ) == 0 )
        {
        ret.Set( KMenuTypeFolder );
        }
    return ret;
    }

// ---------------------------------------------------------
// CMenuEngLegacyObjectFactory::GetContentObjectAndSetContextL
// ---------------------------------------------------------
//
CGECOObjectBase* CMenuEngLegacyObjectFactory::GetContentObjectAndSetContextL
( const TDesC& aTypeIdentifier )
    {
    CMenuEngObject* object = 0;
    if ( !IsSupportedType( aTypeIdentifier ) )
        {
        User::Leave( KErrNotSupported );
        }
    object = CMenuEngObject::NewL( iEng, ConvertedType( aTypeIdentifier ) );
    SetContext( object );
    return object;
    }

// ---------------------------------------------------------
// CMenuEngLegacyObjectFactory::InitializeObjectL
// ---------------------------------------------------------
//
void CMenuEngLegacyObjectFactory::InitializeObjectL
( MGECOAttributeProvider& aAttributeProvider )
    {
    if ( iContext )
        {
        CMenuEngObject* context = (CMenuEngObject*)iContext;
        TPtrC name;
        TPtrC value;
        TBool localized;
        context->Reset();
        const TInt count = aAttributeProvider.NumAttributes();
        for ( TInt i = 0; i < count; i++ )
            {
            aAttributeProvider.AttributeDetailsL( i, name, value, localized );
            if ( KMenuAttrId() == name ) 
                {
                // Legacy xml format does not support 'id'. Skip it.
                }
            else if ( KMenuAttrFlags() == name )
                {
                // Legacy xml format does not support 'flags'. Skip it.
                }

            // Collect attributes that are handled as flags
            else if ( KLegacyAttrLockDelete() == name )
                {
                context->SetFlags( TMenuItem::ELockDelete, ETrue );
                }
            else if ( KLegacyAttrLockName() == name )
                {
                context->SetFlags( TMenuItem::ELockName, ETrue );
                }
            else if ( KLegacyAttrLockIcon() == name )
                {
                context->SetFlags( TMenuItem::ELockIcon, ETrue );
                }
            else if ( KLegacyAttrLockMoveInto() == name )
                {
                context->SetFlags( TMenuItem::ELockMoveInto, ETrue );
                }
            else if ( KLegacyAttrLockItem() == name )
                {
                context->SetFlags( TMenuItem::ELockItem, ETrue );
                }

            else
                {
                // Other attributes stored normally, in the object.
                context->SetInitialAttributeL( name, value, localized );
                }
            }
        }
    }

// ---------------------------------------------------------
// CMenuEngLegacyObjectFactory::SetContext
// ---------------------------------------------------------
//
TInt CMenuEngLegacyObjectFactory::SetContext( CGECOObjectBase* aContext )
    {
    iContext = aContext;
    return KErrNone;
    }

// ---------------------------------------------------------
// CMenuEngLegacyObjectFactory::NumAttributes
// ---------------------------------------------------------
//
TInt CMenuEngLegacyObjectFactory::NumAttributes()
    {
    // Do not support saving in legacy format - panic in debug build!
    __ASSERT_DEBUG( EFalse, User::Invariant() );
    return 0;
    }

// ---------------------------------------------------------
// CMenuEngLegacyObjectFactory::AttributeDetailsL
// ---------------------------------------------------------
//
void CMenuEngLegacyObjectFactory::AttributeDetailsL(
    const TInt /*aIndex*/,
    TPtrC& /*aAttrName*/, 
    TPtrC& /*aAttrValue*/,
    TBool& /*aIsLocalized*/ )
    {
    // Do not support saving in legacy format - panic in debug build!
    __ASSERT_DEBUG( EFalse, User::Invariant() );
    }

// ---------------------------------------------------------
// CMenuEngLegacyObjectFactory::AttributeDetailsL
// ---------------------------------------------------------
//
void CMenuEngLegacyObjectFactory::AttributeDetailsL(
    const TInt /*aIndex*/,
    TPtrC& /*aAttrName*/, 
    TPtrC& /*aAttrValue*/ )
    {
    // Do not support saving in legacy format - panic in debug build!
    __ASSERT_DEBUG( EFalse, User::Invariant() );
    }

// ---------------------------------------------------------
// CMenuEngLegacyObjectFactory::HasTextData
// ---------------------------------------------------------
//
TBool CMenuEngLegacyObjectFactory::HasTextData()
    {
    return EFalse;
    }

// ---------------------------------------------------------
// CMenuEngLegacyObjectFactory::TextDetailsL
// ---------------------------------------------------------
//
void CMenuEngLegacyObjectFactory::TextDetailsL
( TPtrC& /*aText*/, TBool& /*aIsLocalized*/ )
    {
    }

//  End of File  
