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

#ifndef __MENUENGOBJECTFACTORY_H__
#define __MENUENGOBJECTFACTORY_H__

#include "mcsdef.h"

#include <e32base.h>
#include <gecodefaultobjectfactory.h>

// FORWARD DECLARATION

class CMenuEng;

/**
* Menu Engine Object factory.
*/
NONSHARABLE_CLASS( CMenuEngObjectFactory ): public CGECODefaultObjectFactory
    {

public:     // construction

    /**
    * Destructor.
    */
    virtual ~CMenuEngObjectFactory();

    /**
    * Two-phased constructor.
    * @param aEng Engine.
    * @return The created object.
    */
    static CMenuEngObjectFactory* NewL( CMenuEng& aEng );

protected:  // construction

    /**
    * Constructor.
    * @param aEng Engine.
    */
    CMenuEngObjectFactory( CMenuEng& aEng );

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
    TBuf<KMenuMaxAttrValueLen> iBuf; ///< Conversion buffer (flags -> string).

    };

#endif // __MENUENGOBJECTFACTORY_H__
