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

#ifndef __MENUENGOBJECTFACTORYPROXY_H__
#define __MENUENGOBJECTFACTORYPROXY_H__

#include <e32base.h>
#include <gecodefaultobjectfactory.h>

// FORWARD DECLARATION

class CMenuEng;
class CMenuEngObjectFactory;
class CMenuEngLegacyObjectFactory;

/**
* Menu Engine Object factory proxy. 
* Redirects factory methods to the appropriate concrete factory when 
* initializing objects. Objects are initialized to the new format.
* Note! Saving of the objects is always done in the new format.
*/
NONSHARABLE_CLASS( CMenuEngObjectFactoryProxy ): public CGECODefaultObjectFactory
    {

public:     // construction

    /**
    * Destructor.
    */
    virtual ~CMenuEngObjectFactoryProxy();

    /**
    * Two-phased constructor.
    * @param aEng Engine.
    * @return The created object.
    */
    static CMenuEngObjectFactoryProxy* NewL( CMenuEng& aEng );

public:     // New methods

    /**
    * Set support for legacy format.
    * @param aSupport New setting for legacy format support.
    */
    void SupportLegacyFormat( TBool aSupport );
    
    /**
    * Get legacy format status.
    * @retrun legacy format
    */
    TBool IsLegacyFormat();

    /**
    * Reset the proxy's state.
    */
    void Reset();

protected:  // construction

    /**
    * Constructor.
    * @param aEng Engine.
    */
    CMenuEngObjectFactoryProxy( CMenuEng& aEng );

    /**
    * Second phase constructor.
    */
    void ConstructL();

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
    CMenuEngObjectFactory* iObjFactory; ///< Owned.
    TBool iSupportLegacyFormat; ///< Legacy format supported?
    CMenuEngLegacyObjectFactory* iLegacyFactory; ///< Owned. Factory for legacy xml format.
    TBool iParsingInLegacyFormat; ///< Is the context legacy type?

    };

#endif // __MENUENGOBJECTFACTORYPROXY_H__
