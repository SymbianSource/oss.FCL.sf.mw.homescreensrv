/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of menu item attribute
*
*/


#ifndef __MENUITEMATTR_H__
#define __MENUITEMATTR_H__

#include <e32base.h>

class RWriteStream;
class RReadStream;

/**
 *  Menu item attribute.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMenuItemAttr ): public CBase
    {

public:

    /**
    * Destructor.
    */
    virtual ~CMenuItemAttr();

    /**
    * Two-phased constructor.
    * @param aName Attribute name.
    * @return The created object.
    */
    static CMenuItemAttr* NewL( const TDesC& aName );

    /**
    * Two-phased constructor.
    * @param aName Attribute name.
    * @return The created object.
    */
    static CMenuItemAttr* NewLC( const TDesC& aName );

    /**
    * Two-phased constructor (create and internalize).
    * @aStream Stream.
    * @return The created object.
    */
    static CMenuItemAttr* NewLC( RReadStream& aStream );

    /**
    * Get name.
    * @return Name.
    */
    TPtrC Name() const { return iName; }

    /**
    * Get value.
    * @return Value, or NULL if attribute is not set.
    */
    HBufC* Value() const { return iValue; }

    /**
    * Set value. Sets changed status as appropriate.
    * @param aValue Value, or NULL if attribute is not set (==remove attr).
    * Ownership taken.
    */
    void SetValue( HBufC* aValue );

    /**
    * Get changed status.
    * @return ETrue if changed.
    */
    TBool Changed() const { return iChanged; }

    /**
    * Set changed status.
    * @param Changed status.
    */
    void SetChanged( TBool aChanged ) { iChanged = aChanged; }

    /**
    * Externalize to stream.
    * @param aStream Stream to externalize to.
    */
    void ExternalizeL( RWriteStream& aStream ) const;

    /**
    * Internalize from stream.
    * @param aStream Stream to externalize from.
    */
    void InternalizeL( RReadStream& aStream );

protected:

    /**
    * Second phased constructor.
    * @param aName Attribute name.
    */
    void ConstructL( const TDesC& aName );

private:    // data
    
    RBuf iName; ///< Name. Own.
    HBufC* iValue; ///< Value (or NULL). Own.
    TBool iChanged; ///< ETrue if changed.

    };

/**
 * Attribute array.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( RMenuItemAttrArray ) : public RPointerArray<CMenuItemAttr>
    {

public:

    /**
    * Find attribute by name.
    * @param aName Name.
    * @return Index of attribute, or KErrNotFound.
    */
    TInt Find( const TDesC& aName ) const;

    /**
    * Count changed attributes.
    * @return Number of changed attributes.
    */
    TInt CountChanged() const;

    /**
    * Clear changed status for all attributes.
    */
    void ClearChanged();

    /**
    * Externalize changed items to stream.
    * @param aStream Stream to externalize to.
    */
    void ExternalizeChangesL( RWriteStream& aStream ) const;

    /**
    * Externalize all items to stream.
    * @param aStream Stream to externalize to.
    */
    void ExternalizeL( RWriteStream& aStream ) const;

    /**
    * Internalize from stream.
    * @param aStream Stream to externalize from.
    */
    void InternalizeL( RReadStream& aStream );

    };

/**
* Push a ResetAndDestroy() on the cleanup stack.
* @param aArray Array.
*/
void CleanupResetAndDestroyPushL( RMenuItemAttrArray& aArray );

#endif // __MENUITEMATTR_H__
