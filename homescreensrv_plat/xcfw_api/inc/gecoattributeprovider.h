/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class definition for attribute access interface
*
*/


#ifndef CGECOATTRIBUTEPROVIDER_H
#define CGECOATTRIBUTEPROVIDER_H

#include <e32std.h>


//CLASS DESCRIPTION
/**
*  Attribute provider interface that content object factories must implement
*  to allow generic access to attribute names/values/localisation reference ids
*  and possible object text data in string format.
*
*  @lib XCFW_hs_hs.lib
*  @since Series 60 3.1
*/
class MGECOAttributeProvider
    {
    public:

    /**
    * Returns number of attributes for this object / tag
    * @since Series 60 3.1
    * @return Number of attributes for this object / tag
    */
    virtual TInt NumAttributes() = 0;

    /**
    * Returns name, value and localization status of an attribute
    * @since Series 60 3.1
    * @param aIndex Index of attribute to get. 
    * @param aAttributeName Attribute name returned
    * @param aAttributeValue Attribute value returned
    * @param aIsLocalized Attribute localization status returned
    */
    virtual void AttributeDetailsL(const TInt aIndex, TPtrC& aAttrName, 
        TPtrC& aAttrValue, TBool& aIsLocalized) = 0;

    /**
    * Returns name and value of an attribute
    * @since Series 60 3.1
    * @param aIndex Index of attribute to get. 
    * @param aAttributeName Attribute name returned
    * @param aAttributeValue Attribute value returned
    */
    virtual void AttributeDetailsL(const TInt aIndex, TPtrC& aAttrName, 
        TPtrC& aAttrValue) = 0;

    /**
    * Returns ETrue if object / tag has text data
    * @since Series 60 3.1
    * @return ETrue if object / tag has text data
    */
    virtual TBool HasTextData() = 0;
        
    /**
    * Returns XML node / content object text data if any
    * @since Series 60 3.1
    * @param aText Text data returned
    * @param aIsLocalized Textdata localization status returned
    */
    virtual void TextDetailsL(TPtrC& aText, TBool& aIsLocalized) = 0;

    };


#endif //CGECOATTRIBUTEPROVIDER_H