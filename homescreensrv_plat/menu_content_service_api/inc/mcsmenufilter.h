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
* Description:  Definition of menu item filter criteria
*
*/


#ifndef __MCSMENUFILTER_H__
#define __MCSMENUFILTER_H__

#include "mcsdef.h"
#include <e32base.h>

class RWriteStream;
class RReadStream;

/**
 *  Menu item filter criteria.
 *  Allows querying items filtered by flags, type and attributes.
 *  Wildcards are not supported (neither in attributes, nor in
 *  type).
 *  @lib mcsmenu.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CMenuFilter ): public CBase
    {

public:

    /**
    * Destructor.
    * @since S60 v5.0
    * @capability None.
    * @throws None.
    * @panic None.
    */
    IMPORT_C virtual ~CMenuFilter();

    /**
    * Two phased constructor.
    * @since S60 v5.0
    * @return The created object.
    * @capability None.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C static CMenuFilter* NewL();

    /**
    * Two phased constructor.
    * @since S60 v5.0
    * @return The created object.
    * @capability None.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C static CMenuFilter* NewLC();

// getters

    /**
    * Get flag filter criteria: these flags must be on.
    * @since S60 v5.0
    * @return Flags that must be on.
    * @capability None.
    * @throws None.
    * @panic None.
    */
    IMPORT_C TUint32 FlagsOn() const;

    /**
    * Get flag filter criteria: these flags must be off.
    * @since S60 v5.0
    * @return Flags that must be off.
    * @capability None.
    * @throws None.
    * @panic None.
    */
    IMPORT_C TUint32 FlagsOff() const;

    /**
    * Get type filter criteria.
    * @since S60 v5.0
    * @return Type filter.
    * @capability None.
    * @throws None.
    * @panic None.
    */
    IMPORT_C TPtrC Type() const;

    /**
    * Get number of attribute filter criteria.
    * @since S60 v5.0
    * @return Number of attribute filter criteria.
    * @capability None.
    * @throws None.
    * @panic None.
    */
    IMPORT_C TInt NumAttributes() const;

    /**
    * Get attribute filter criteria by index. (Attribute must be defined and
    * have a certain value, or it must be undefined).
    * @since S60 v5.0
    * @param aIndex Index of attribute filter criteria.
    * @param aAttrName Attribute name is returned here.
    * @param aAttrValue Attribute value (if defined) is returned here.
    * @return ETrue if attribute aAttrName must be defined and having value
    * aAttrValue. EFalse if attribute aAttrName must NOT be defined.
    * @capability None.
    * @throws None.
    * @panic None.
    */
    IMPORT_C TBool GetAttribute
        ( TInt aIndex, TPtrC& aAttrName, TPtrC& aAttrValue ) const;

// setters

    /**
    * Reset filter (==pass all).
    * @since S60 v5.0
    * @capability None.
    * @throws None.
    * @panic None.
    */
    IMPORT_C void Reset();

    /**
    * Set on and off flag filter criteria.
    * @since S60 v5.0
    * @param aFlagsOn These flags must be on. (0=="flag on" filter off)
    * @param aFlagsOff These flags must be off. (0=="flag off" filter off)
    * @capability None.
    * @throws None.
    * @panic None.
    */
    IMPORT_C void SetFlags( TUint32 aFlagsOn, TUint32 aFlagsOff );

    /**
    * Set type filter criteria.
    * @since S60 v5.0
    * @param aType Type filter. (empty==type filter off)
    * @capability None.
    * @throws None.
    * @panic None.
    */
    IMPORT_C void SetType( const TDesC& aType );

    /**
    * Add/replace attribute filter criteria.
    * The attribute must exist and have the given value.
    * @since S60 v5.0
    * @param aAttrName Attribute name. Previous attribute filter with
    * the same name is replaced.
    * aAttrValue Attribute must have this value.
    * @capability None.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C void HaveAttributeL
        ( const TDesC& aAttrName, const TDesC& aAttrValue );

    /**
    * Add/replace attribute filter criteria.
    * The attribute must not exist.
    * @since S60 v5.0
    * @param aAttrName Attribute name. Previous attribute filter with
    * the same name is replaced.
    * @capability None.
    * @throws System-wide error codes if an error occurs.
    * @panic None.
    */
    IMPORT_C void DoNotHaveAttributeL( const TDesC& aAttrName );

    /**
    * Remove attribute filter criteria.
    * @since S60 v5.0
    * @param aAttrName Attribute name. Previous attribute filter with
    * the same name (if any) is removed.
    * @capability None.
    * @throws None.
    * @panic None.
    */
    IMPORT_C void RemoveAttribute( const TDesC& aAttrName );

// not exported public

    /**
    * Externalize object.
    * @param aStream Stream to externalize into.
    */
    void ExternalizeL( RWriteStream& aStream ) const;

    /**
    * Internalize object.
    * @param aStream Stream to externalize from.
    */
    void InternalizeL( RReadStream& aStream );

private:

    /**
    * Constructor.
    */
    CMenuFilter();

    /**
    * Second-phase constructor.
    */
    void ConstructL();

private:    // data

    class TData;
    TData* iData; ///< Private data. Own.

    };

#endif // __MCSMENUFILTER_H__
