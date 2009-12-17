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

#ifndef __MENUENGLEGACYOBJECTFACTORY_H__
#define __MENUENGLEGACYOBJECTFACTORY_H__

#include <e32base.h>
#include <gecodefaultobjectfactory.h>

// FORWARD DECLARATION

class CMenuEng;

/**
* Menu Engine Object factory for reading previous, legacy version of XML.
* Note. This class is meant only for reading! Objects are internalized 
* and saved to the new format (@see CMenuEngObjectFactory).
*/
NONSHARABLE_CLASS( CMenuEngLegacyObjectFactory ): public CGECODefaultObjectFactory
    {

public:     // construction

    /**
    * Destructor.
    */
    virtual ~CMenuEngLegacyObjectFactory();

    /**
    * Two-phased constructor.
    * @param aEng Engine.
    * @return The created object.
    */
    static CMenuEngLegacyObjectFactory* NewL( CMenuEng& aEng );

protected:  // construction

    /**
    * Constructor.
    * @param aEng Engine.
    */
    CMenuEngLegacyObjectFactory( CMenuEng& aEng );

private:    // New

    /**
    * Check support for legacy node type.
    * @param aLegacyTypeIdentifier Node type.
    * @return ETrue if aLegacyTypeIdentifier is supported by this factory.
    */
    TBool IsSupportedType( const TDesC& aLegacyTypeIdentifier ) const;

    /**
    * Get the converted (current) type for legacy node type.
    * @param aLegacyTypeIdentifier Node type.
    * @return Converted type.
    */
    TPtrC ConvertedType( const TDesC& aLegacyTypeIdentifier ) const;

public:    // from CGECOObjectFactoryBase

    /**
    * Create object and set it current.
    * @param aTypeIdentifier Object type identifier.
    * @return CGECOObjectBase derived object. Caller takes ownership.
    */
    CGECOObjectBase* GetContentObjectAndSetContextL(
        const TDesC& aTypeIdentifier );

    /**
    * Initializes the current object with attribute provider data.
    * @param aAttributeProvider Attribute provider for data initialization.
    */
    void InitializeObjectL(
        MGECOAttributeProvider& aAttributeProvider );

    /**
    * Set context object for initialize and preparesave operations.
    * @param aContext Object to use in InitializeL operations.
    * @return Error code.
    */
    TInt SetContext( CGECOObjectBase* aContext );

public:    // from MGECOAttributeProvider

    /**
    * Get number of attributes (of current object).
    * @return Number of attributes.
    */
    TInt NumAttributes();

    /**
    * Get attribute by index (of current object).
    * @param aIndex Attribute index.
    * @param aAttrName Attribute name is returned here.
    * @param aAttrValue Attribute value is returned here.
    * @param aIsLocalized Localized status is returned here.
    */
    void AttributeDetailsL(
        const TInt aIndex,
        TPtrC& aAttrName, 
        TPtrC& aAttrValue,
        TBool& aIsLocalized );

    /**
    * Get attribute by index (of current object).
    * @param aIndex Attribute index.
    * @param aAttrName Attribute name is returned here.
    * @param aAttrValue Attribute value is returned here.
    */
    void AttributeDetailsL(
        const TInt aIndex,
        TPtrC& aAttrName, 
        TPtrC& aAttrValue );

    /**
    * Check if current object has text data.
    * @return ETrue if current object has text data.
    */
    TBool HasTextData();

    /**
    * Get text data of current object.
    * @param aText Text data is returned here.
    * @param aIsLocalized Localized status is returned here.
    */
    void TextDetailsL( TPtrC& aText, TBool& aIsLocalized );


private:    // data

    CMenuEng& iEng; ///< Engine. Not owned.

    };

#endif // __MENUENGLEGACYOBJECTFACTORY_H__
