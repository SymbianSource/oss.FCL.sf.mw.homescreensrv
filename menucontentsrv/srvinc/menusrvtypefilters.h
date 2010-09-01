/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The API supports item information that is not supported by
*                the MCS server itself
*
*/
#ifndef MENUSRVTYPEFILTERS_H_
#define MENUSRVTYPEFILTERS_H_


#include "menuengfilter.h"
#include "menuengobject.h"

/**
* Type filter: match objects by type.
*/
class TMenuSrvTypeFilter: public MMenuEngFilter
    {

public:     // from MMenuEngFilter

    /**
    * Filter test function.
    * @param aObject Object.
    * @return ETrue if aObject matches this filter.
    */
    TBool MatchesObject( const CMenuEngObject& aObject ) const;

public:     // new methods

    /**
    * Set type filter.
    * @param aType Type.
    */
    void SetType( const TDesC& aType )
        { iType.Set( aType ); }

private:    // data

    TPtrC iType; ///< Type filter.

    };

/**
* Type + attribute filter: match objects by type
* and attribute with a specific value.
*/
class TMenuSrvTypeAttrFilter: public TMenuSrvTypeFilter
    {

public:     // from MMenuEngFilter

    /**
    * Filter test function.
    * @param aObject Object.
    * @return ETrue if aObject matches this filter.
    */
    TBool MatchesObject( const CMenuEngObject& aObject ) const;

public:     // new methods

    /**
    * Set attribute filter.
    * @param aName Attribute name.
    * @param aValue Attribute value.
    */
    void SetAttr( const TDesC& aName, const TDesC& aValue )
        { iAttrName.Set( aName ); iAttrValue.Set( aValue ); }

private:    // data

    TPtrC iAttrName; ///< Attr name.
    TPtrC iAttrValue; ///< Attr value.

    };

/**
* Type + attribute filter: match objects by type
* and attribute with a specific value.
*/
class TMenuSrvAttrFilter: public MMenuEngFilter
    {

public:     // from MMenuEngFilter

    /**
    * Filter test function.
    * @param aObject Object.
    * @return ETrue if aObject matches this filter.
    */
    TBool MatchesObject( const CMenuEngObject& aObject ) const;

public:     // new methods

    /**
    * Set attribute filter.
    * @param aName Attribute name.
    * @param aValue Attribute value.
    */
    void SetAttr( const TDesC& aName, const TDesC& aValue )
        { iAttrName.Set( aName ); iAttrValue.Set( aValue ); }

private:    // data

    TPtrC iAttrName; ///< Attr name.
    TPtrC iAttrValue; ///< Attr value.
    };

/**
* Type + attribute filter: match objects by type
* and attribute with a specific value.
*/
class TMenuSrvTypeAttrExistsFilter: public TMenuSrvTypeFilter
    {

public:     // from MMenuEngFilter

    /**
    * Filter test function.
    * @param aObject Object.
    * @return ETrue if aObject matches this filter.
    */
    TBool MatchesObject( const CMenuEngObject& aObject ) const;

public:     // new methods

    /**
    * Set attribute filter.
    * @param aName Attribute name.
    */
    void SetAttr( const TDesC& aName )
        { iAttrName.Set( aName ); }

private:    // data

    TPtrC iAttrName; ///< Attr name.

    };

/**
* Type + attribute filter: match objects by type
* and attribute with a specific value.
*/
class TMenuSrvAttrExistsFilter: public TMenuSrvTypeFilter
    {

public:     // from MMenuEngFilter

    /**
    * Filter test function.
    * @param aObject Object.
    * @return ETrue if aObject matches this filter.
    */
    TBool MatchesObject( const CMenuEngObject& aObject ) const;

public:     // new methods

    /**
    * Set attribute filter.
    * @param aName Attribute name.
    */
    void SetAttr( const TDesC& aName )
        { iAttrName.Set( aName ); }

private:    // data

    TPtrC iAttrName; ///< Attr name.

    };

/**
* Hidden attr filter for application items
*/
class TMenuSrvHiddenAppFilter: public MMenuEngFilter
    {

public:     // from MMenuEngFilter

    /**
    * Filter test function.
    * @param aObject Object.
    * @return ETrue if aObject matches this filter.
    */
    TBool MatchesObject( const CMenuEngObject& aObject ) const;

    };

#endif /* MENUSRVTYPEFILTERS_H_ */
