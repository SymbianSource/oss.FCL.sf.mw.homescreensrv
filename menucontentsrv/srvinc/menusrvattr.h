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


#ifndef __MENUSRVMATTR_H__
#define __MENUSRVMATTR_H__

#include <e32base.h>


/**
 *  Menu srv attribute.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMenuSrvAttr ): public CBase
    {

public:

    /**
    * Destructor.
    */
    virtual ~CMenuSrvAttr();

    /**
    * Two-phased constructor.
    * @param aName Attribute name.
    * @return The created object.
    */
    static CMenuSrvAttr* NewL( const TDesC& aName );

    /**
    * Two-phased constructor.
    * @param aName Attribute name.
    * @return The created object.
    */
    static CMenuSrvAttr* NewLC( const TDesC& aName );

    /**
    * Get name.
    * @return Name.
    */
    TPtrC Name() const { return iName; }

    /**
    * Get value.
    * @return Value, or NULL if attribute is not set.
    */
    TPtrC Value() const { return iValue; }

    /**
    * Get id.
    * @return Value.
    */
    TInt Id() const { return iId; }    
    
    /**
    * Set value. Sets changed status as appropriate.
    * @param aValue Value, or NULL if attribute is not set (==remove attr).
    * Ownership taken.
    */
    void SetValueL( const TDesC& aName );

    /**
    * Set changed status.
    * @param Changed status.
    */
    void SetId( TInt aId ) { iId = aId; }

protected:

    /**
    * Second phased constructor.
    * @param aName Attribute name.
    */
    void ConstructL( const TDesC& aName );

private:    // data
    
    RBuf iName; ///< Name. Own.
    RBuf iValue; ///< Value (or NULL). Own.
    TInt iId; 

    };

/**
 * Attribute array.
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( RMenuSrvAttrArray ) : public RPointerArray<CMenuSrvAttr>
    {

public:

  /**
    * Find attribute by name.
    * @param aId Id.
    * @param aName Name.
    * @param aAttr Attribute value.
    * @return TBool ,ETrue if attribute was found
    */
    TBool Find( TInt aId, const TDesC& aName, TDes& aAttrVal  );
    
    TBool Exist( TInt aId, const TDesC& aName );
 
    /**
    * Remove attributes by id.
    * @param aId Id.
    */ 
    void RemoveId( TInt aId );
    
    void RemoveAttribute( TInt aId, const TDesC& aAttrName );
    
    /**
    * Remove not exist items.
    */
    //void RemoveNotExistItems();

  };

/**
* Push a ResetAndDestroy() on the cleanup stack.
* @param aArray Array.
*/
void CleanupResetAndDestroyPushL( RMenuSrvAttrArray& aArray );

#endif // __MENUITEMATTR_H__
