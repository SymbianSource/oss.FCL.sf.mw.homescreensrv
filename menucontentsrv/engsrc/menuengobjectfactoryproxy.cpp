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

#include "menuengobjectfactoryproxy.h"
#include "menuengobjectfactory.h"
#include "menuenglegacyobjectfactory.h"
#include "menuengobject.h"

_LIT( KLegacyTypeData, "appshell:data" ); ///< Legacy data type.
_LIT( KMenuAttrVersion, "version" ); ///< Version attribute.
LOCAL_D const TInt KSmallestMajorVersion = 5;

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMenuEngObjectFactoryProxy::~CMenuEngObjectFactoryProxy
// ---------------------------------------------------------
//
CMenuEngObjectFactoryProxy::~CMenuEngObjectFactoryProxy()
    {
    delete iLegacyFactory;
    delete iObjFactory;
    }

// ---------------------------------------------------------
// CMenuEngObjectFactoryProxy::NewL
// ---------------------------------------------------------
//
CMenuEngObjectFactoryProxy* CMenuEngObjectFactoryProxy::NewL( CMenuEng& aEng )
    {
    CMenuEngObjectFactoryProxy* factory =
        new (ELeave) CMenuEngObjectFactoryProxy( aEng );
    CleanupStack::PushL( factory );
    factory->ConstructL();
    CleanupStack::Pop( factory );
    return factory;
    }

// ---------------------------------------------------------
// CMenuEngObjectFactoryProxy::CMenuEngObjectFactoryProxy
// ---------------------------------------------------------
//
CMenuEngObjectFactoryProxy::CMenuEngObjectFactoryProxy( CMenuEng& aEng )
: iEng( aEng )
    {
    }

// ---------------------------------------------------------
// CMenuEngObjectFactoryProxy::ConstructL
// ---------------------------------------------------------
//
void CMenuEngObjectFactoryProxy::ConstructL()
    {
    iObjFactory = CMenuEngObjectFactory::NewL( iEng );
    // iLegacyFactory is created only when needed
    }

// ---------------------------------------------------------
// CMenuEngObjectFactoryProxy::SupportLegacyFormat
// ---------------------------------------------------------
//
void CMenuEngObjectFactoryProxy::SupportLegacyFormat( TBool aSupport )
    {
    iSupportLegacyFormat = aSupport;
    }

// ---------------------------------------------------------
// CMenuEngObjectFactoryProxy::SupportLegacyFormat
// ---------------------------------------------------------
//
TBool CMenuEngObjectFactoryProxy::IsLegacyFormat()
    {
    return iParsingInLegacyFormat;
    }


// ---------------------------------------------------------
// CMenuEngObjectFactoryProxy::Reset
// ---------------------------------------------------------
//
void CMenuEngObjectFactoryProxy::Reset()
    {
    iSupportLegacyFormat = EFalse;
    delete iLegacyFactory;
    iLegacyFactory = 0;
    iParsingInLegacyFormat = EFalse;
    }

// ---------------------------------------------------------
// CMenuEngObjectFactoryProxy::GetContentObjectAndSetContextL
// Note: due to XCFW no hierarcy is seen from the factory, 
// thus it is ensured that only one 'data' node can exist.
// ---------------------------------------------------------
//
CGECOObjectBase* CMenuEngObjectFactoryProxy::GetContentObjectAndSetContextL
( const TDesC& aTypeIdentifier )
    {
    CGECOObjectBase* object = 0;
    if( iParsingInLegacyFormat )
        {
        __ASSERT_DEBUG( iLegacyFactory, User::Invariant() );
        object = iLegacyFactory->GetContentObjectAndSetContextL( aTypeIdentifier );
        }
    else
        {
        // If there is a legacy 'data' node, it is always saved with new prefix.
        TPtrC type( aTypeIdentifier );
        if( iSupportLegacyFormat && aTypeIdentifier == KLegacyTypeData )
            {
            type.Set( KMenuTypeData );
            }
        object = iObjFactory->GetContentObjectAndSetContextL( type );
        }
    // Do not call SetContext( object ); because it forwards the call!
    iContext = object;
    return object;
    }

// ---------------------------------------------------------
// CMenuEngObjectFactoryProxy::InitializeObjectL
// ---------------------------------------------------------
//
void CMenuEngObjectFactoryProxy::InitializeObjectL
( MGECOAttributeProvider& aAttributeProvider )
    {
    // check the 'data' node's version identifier. If the version is 
    // less than 5.0, then the factory will function in legacy mode 
    // until parsing completed (see CMenuEng::HandleEngineEventL).
    CMenuEngObject* object = (CMenuEngObject*)iContext;
    if ( iSupportLegacyFormat && object->Type() == KMenuTypeData )
        {
        iParsingInLegacyFormat = EFalse; // By default
        TPtrC name;
        TPtrC value;
        TBool localized;
        const TInt count = aAttributeProvider.NumAttributes();
        for ( TInt i = 0; i < count; i++ )
            {
            aAttributeProvider.AttributeDetailsL( i, name, value, localized );
            if ( name == KMenuAttrVersion )
                {
                // Format is: <major>.<minor>
                TReal32 version( 0. );
                TLex lex( value );
                const TInt err = lex.Val( version, TChar('.') );
                if ( !err && version < KSmallestMajorVersion )
                    {
                    // Considered as legacy xml format
                    if( !iLegacyFactory )
                        {
                        iLegacyFactory = CMenuEngLegacyObjectFactory::NewL( iEng );
                        }
                    iParsingInLegacyFormat = ETrue;
                    }
                break;
                }
            }
        }

    CGECODefaultObjectFactory::InitializeObjectL(aAttributeProvider);
    if( iParsingInLegacyFormat )
        {
        __ASSERT_DEBUG( iLegacyFactory, User::Invariant() );
        iLegacyFactory->InitializeObjectL( aAttributeProvider );
        }
    else
        {
        iObjFactory->InitializeObjectL( aAttributeProvider );
        }
    }

// ---------------------------------------------------------
// CMenuEngObjectFactoryProxy::SetContext
// ---------------------------------------------------------
//
TInt CMenuEngObjectFactoryProxy::SetContext( CGECOObjectBase* aContext )
    {
    __ASSERT_DEBUG( !iLegacyFactory, User::Invariant() );
    iContext = aContext;
    return iObjFactory->SetContext( aContext );
    }

// ---------------------------------------------------------
// CMenuEngObjectFactoryProxy::NumAttributes
// ---------------------------------------------------------
//
TInt CMenuEngObjectFactoryProxy::NumAttributes()
    {
    __ASSERT_DEBUG( !iLegacyFactory, User::Invariant() );
    return iObjFactory->NumAttributes();
    }

// ---------------------------------------------------------
// CMenuEngObjectFactoryProxy::AttributeDetailsL
// ---------------------------------------------------------
//
void CMenuEngObjectFactoryProxy::AttributeDetailsL(
    const TInt aIndex,
    TPtrC& aAttrName, 
    TPtrC& aAttrValue,
    TBool& aIsLocalized )
    {
    __ASSERT_DEBUG( !iLegacyFactory, User::Invariant() );
    iObjFactory->AttributeDetailsL( aIndex, aAttrName, aAttrValue, aIsLocalized );
    }

// ---------------------------------------------------------
// CMenuEngObjectFactoryProxy::AttributeDetailsL
// ---------------------------------------------------------
//
void CMenuEngObjectFactoryProxy::AttributeDetailsL(
    const TInt aIndex,
    TPtrC& aAttrName, 
    TPtrC& aAttrValue )
    {
    __ASSERT_DEBUG( !iLegacyFactory, User::Invariant() );
    TRAPD( err, CGECODefaultObjectFactory::AttributeDetailsL(
    	aIndex,
    	aAttrName,
    	aAttrValue ) );
    if( KErrNotFound==err )
    	{
    	TBool dummy = EFalse;
    	AttributeDetailsL( aIndex, aAttrName, aAttrValue, dummy );
    	}
    }

// ---------------------------------------------------------
// CMenuEngObjectFactoryProxy::HasTextData
// ---------------------------------------------------------
//
TBool CMenuEngObjectFactoryProxy::HasTextData()
    {
    __ASSERT_DEBUG( !iLegacyFactory, User::Invariant() );
    return iObjFactory->HasTextData();
    }

// ---------------------------------------------------------
// CMenuEngObjectFactoryProxy::TextDetailsL
// ---------------------------------------------------------
//
void CMenuEngObjectFactoryProxy::TextDetailsL
( TPtrC& aText, TBool& aIsLocalized )
    {
    __ASSERT_DEBUG( !iLegacyFactory, User::Invariant() );
    iObjFactory->TextDetailsL( aText, aIsLocalized );
    }

//  End of File  
