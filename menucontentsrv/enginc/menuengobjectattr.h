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

#ifndef __MENUENGOBJECTATTR_H__
#define __MENUENGOBJECTATTR_H__

#include <e32base.h>

/**
* Menu Engine Object attribute.
*/
NONSHARABLE_CLASS( CMenuEngObjectAttr ): public CBase
    {

public:     // construction

    /**
    * Destructor.
    */
    virtual ~CMenuEngObjectAttr();

    /**
    * Two-phased constructor.
    * @param aName Name.
    * @return The created object.
    */
    static CMenuEngObjectAttr* NewL( const TDesC& aName );

    /**
    * Two-phased constructor.
    * @param aName Name.
    * @return The created object.
    */
    static CMenuEngObjectAttr* NewLC( const TDesC& aName );

protected:  // construction

    /**
     * Constructor
     */
    CMenuEngObjectAttr();
    
    /**
     * 2nd phase constructor.
     * @param aName Name.
     */
    void ConstructL( const TDesC& aName );

public:     // new methods

    /**
    * Get name.
    * @return Name.
    */
    TPtrC Name() const { return iName; }

    /**
    * Get value.
    * @return Value.
    */
    TPtrC Value() const { return iValue; }

    /**
    * Set (or replace) value.
    * @param aValue Value.
    */
    void SetValueL( const TDesC& aValue );

    /**
    * Get localized status.
    * @return ETrue if localized.
    */
    TBool Localized() const { return iLocalized; }

    /**
    * Set localized status.
    * @param aLocalized ETrue if localized.
    */
    void SetLocalized( TBool aLocalized ) { iLocalized = aLocalized; }

private:    // data
    
    RBuf iName; ///< Name. Own.
    RBuf iValue; ///< Value. Own.
    TBool iLocalized; ///< Localized status.

    };

/**
* Array of CMenuEngObjectAttr-s.
*/
NONSHARABLE_CLASS( RMenuEngObjectAttrArray )
: public RPointerArray<CMenuEngObjectAttr>
    {

public:     // new methods

    /**
    * Find attribute by name.
    * @param aName Name.
    * @return Index of attribute, or KErrNotFound.
    */
    TInt Find( const TDesC& aName ) const;

    };

#endif // __MENU_ENG_OBJECT_ATTR_H__
