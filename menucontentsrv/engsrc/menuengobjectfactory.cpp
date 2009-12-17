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

#include "menuengobjectfactory.h"
#include "menuengobject.h"
#include "menuengflags.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuEngObjectFactory::~CMenuEngObjectFactory
// ---------------------------------------------------------
//
CMenuEngObjectFactory::~CMenuEngObjectFactory()
    {
    }

// ---------------------------------------------------------
// CMenuEngObjectFactory::NewL
// ---------------------------------------------------------
//
CMenuEngObjectFactory* CMenuEngObjectFactory::NewL( CMenuEng& aEng )
    {
    CMenuEngObjectFactory* factory =
        new (ELeave) CMenuEngObjectFactory( aEng );
    return factory;
    }

// ---------------------------------------------------------
// CMenuEngObjectFactory::CMenuEngObjectFactory
// ---------------------------------------------------------
//
CMenuEngObjectFactory::CMenuEngObjectFactory( CMenuEng& aEng )
: iEng( aEng )
    {
    }

// ---------------------------------------------------------
// CMenuEngObjectFactory::GetContentObjectAndSetContextL
// ---------------------------------------------------------
//
CGECOObjectBase* CMenuEngObjectFactory::GetContentObjectAndSetContextL
( const TDesC& aTypeIdentifier )
    {
    // Notes:
    // Here we create an object for all nodes, even for the ones we don't know.
    // (We can't check the type because of extensibility).
    // This is a problem, as not all XML nodes are part of the menu structure,
    // for example the "appshell:data" is not.
    //
    // Is it a solution to use an *exclusion* list of types here??
    CMenuEngObject* object = CMenuEngObject::NewL( iEng, aTypeIdentifier );
    SetContext( object );
    return object;
    }

// ---------------------------------------------------------
// CMenuEngObjectFactory::InitializeObjectL
// ---------------------------------------------------------
//
void CMenuEngObjectFactory::InitializeObjectL
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
                // "id" -> internal attribute, convert to integer.
                context->SetId( MenuEngId::AsInt( value ) );
                }
            else if ( KMenuAttrFlags() == name )
                {
                // "flags" -> internal attribute, convert to integer.
                context->SetFlags( MenuEngFlags::AsInt( value ), ETrue );
                CGECODefaultObject* defaultContext = (CGECODefaultObject*)context;
                defaultContext->RemoveAttribute(KMenuAttrFlags);
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
// CMenuEngObjectFactory::SetContext
// ---------------------------------------------------------
//
TInt CMenuEngObjectFactory::SetContext( CGECOObjectBase* aContext )
    {
    iContext = aContext;
    return KErrNone;
    }

// ---------------------------------------------------------
// CMenuEngObjectFactory::NumAttributes
// ---------------------------------------------------------
//
TInt CMenuEngObjectFactory::NumAttributes()
    {
    TInt count = 0;
    if ( iContext )
        {
        count = 1; // We always have the ID.
        CMenuEngObject* context = (CMenuEngObject*)iContext;
        count += context->NumAttributes();
        if ( context->Flags( ETrue ) )
            {
            // Flags, if any, are written to XML as an attribute
            // named "flags".
            // The object cannot have an attribute with that name.
            count++;
            }
        }
    return count;
    }

// ---------------------------------------------------------
// CMenuEngObjectFactory::AttributeDetailsL
// ---------------------------------------------------------
//
void CMenuEngObjectFactory::AttributeDetailsL(
    const TInt aIndex,
    TPtrC& aAttrName, 
    TPtrC& aAttrValue,
    TBool& aIsLocalized )
    {
    if ( iContext )
        {
        CMenuEngObject* context = (CMenuEngObject*)iContext;
        if ( aIndex < context->NumAttributes() )
            {
            // External attributes retrieved normally, from the object.
            (void)context->GetAttribute
                ( aIndex, aAttrName, aAttrValue, aIsLocalized );
            }
        else if ( aIndex == context->NumAttributes() )
            {
            // "id" -> internal attribute, convert to string.
            MenuEngId::AsString( context->Id(), iBuf );
            aAttrName.Set( KMenuAttrId() );
            aAttrValue.Set( iBuf );
            aIsLocalized = EFalse;
            }
        else
            {
            // "flags" -> internal attribute, convert to string.
            __ASSERT_DEBUG( context->Flags( ETrue ), User::Invariant() );
            MenuEngFlags::AsString( context->Flags( ETrue ), iBuf );
            aAttrName.Set( KMenuAttrFlags() );
            aAttrValue.Set( iBuf );
            aIsLocalized = EFalse;
            }
        }
    }

// ---------------------------------------------------------
// CMenuEngObjectFactory::AttributeDetailsL
// ---------------------------------------------------------
//
void CMenuEngObjectFactory::AttributeDetailsL(
    const TInt aIndex,
    TPtrC& aAttrName, 
    TPtrC& aAttrValue )
    {
    CGECODefaultObjectFactory::AttributeDetailsL( aIndex, aAttrName, aAttrValue);
    }

// ---------------------------------------------------------
// CMenuEngObjectFactory::HasTextData
// ---------------------------------------------------------
//
TBool CMenuEngObjectFactory::HasTextData()
    {
    return EFalse;
    }

// ---------------------------------------------------------
// CMenuEngObjectFactory::TextDetailsL
// ---------------------------------------------------------
//
void CMenuEngObjectFactory::TextDetailsL
( TPtrC& /*aText*/, TBool& /*aIsLocalized*/ )
    {
    }

//  End of File  
