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
* Description:  Default content object definition
*
*/



#ifndef CGECODEFAULTOBJECT_H
#define CGECODEFAULTOBJECT_H

//  INCLUDES
#include <e32std.h>
#include <badesca.h>
#include "gecoobjectbase.h"
#include "gecoattributeprovider.h"

// CLASS DECLARATION
/**
*  Default implementation for CGECOObjectBase
*  Implements basic descriptor type arrays for storing object attributes.
*
*  @lib XCFW_hs_hs.lib
*  @since Series 60 3.1
*/
class CGECODefaultObject: public CGECOObjectBase, public MGECOAttributeProvider
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor
        * @param aTypeIdentifier Object identifier (e.g. XML tag with namespace)
        */
        IMPORT_C static CGECODefaultObject* NewL( const TDesC& aTypeIdentifier );
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CGECODefaultObject();

    public: // From base classes


    public: // new functions
        
        /**
        * Returns value for given attribute 
        * @since Series 60 3.1
        * @param aAttrName Attribute to search for
        * @param aAttrValue Attribute value returned
        * @return Error code
        */
        IMPORT_C TInt GetAttribute( const TDesC& aAttrName, TPtrC& aAttrValue );

        /**
        * Returns value and localization status for given attribute
        * @since Series 60 3.1
        * @param aAttrName Attribute to search for
        * @param aAttrValue Attribute value returned
        * @param aIsLocalized Attribute localization status returned
        * @return Error code
        */
        IMPORT_C TInt GetAttribute( const TDesC& aAttrName, TPtrC& aAttrValue, 
            TBool& aIsLocalized );

        /**
        * Adds new attribute to this object
        * Attribute's localization status will be EFalse.
        * @since Series 60 3.1
        * @param aAttrName New attribute name
        * @param aAttrValue New attribute value
        */
        IMPORT_C void SetAttributeL( const TDesC& aAttrName,
            const TDesC& aAttrValue );

        /**
        * Adds new attribute to this object
        * @since Series 60 3.1
        * @param aAttrName New attribute name
        * @param aAttrValue New attribute value
        * @param aIsLocalized New attribute's localization status
        */
        IMPORT_C void SetAttributeL( const TDesC& aAttrName, 
            const TDesC& aAttrValue, 
            TBool aIsLocalized );

        /**
        * Removes an attribute from this object
        * @since Series 60 3.1
        * @param aAttrName Attribute to be removed
        * @return Error code
        */
        IMPORT_C TInt RemoveAttribute( const TDesC& aAttrName ); 

        /**
        * Sets object's text data. 
        * This function will set the localization status for text to EFalse
        * @param aText New text for the object
        */
        IMPORT_C void SetTextL( const TDesC& aText );

        /**
        * Sets object's text data and localization status
        * @since Series 60 3.1
        * @param aText New text for the object
        * @param aIsLocalized text data's localization status
        */
        IMPORT_C void SetTextL( const TDesC& aText, TBool aIsLocalized );

        /**
        * Returns possible text data from this object
        * @since Series 60 3.1
        * @param aText Text data returned
        * @return Error code, e.g. KErrNotFound if object has no text data
        */
        IMPORT_C TInt GetText( TPtrC& aText );

        /**
        * Removes object's text data
        * @since Series 60 3.1
        */
        IMPORT_C void RemoveText();
        
    public: // From base classes
    

        /**
        * From CGECOObjectBase: Returns Object type identifier for this object
        */
        IMPORT_C const TDesC& TypeIdentifier();

        /**
        * From MGECOAttributeProvider. Returns number of attributes
        */
        IMPORT_C TInt NumAttributes();
        
        /**
        * From MGECOAttributeProvider. Returns name, value and localization 
        * status for attribute in given index.
        */
        IMPORT_C void AttributeDetailsL( const TInt aIndex, TPtrC& aAttrName, 
            TPtrC& aAttrValue, TBool& aIsLocalized );
            
        /**
        * From MGECOAttributeProvider. Returns name, value  
        * for attribute in given index.
        */
        IMPORT_C void AttributeDetailsL( const TInt aIndex, TPtrC& aAttrName, 
            TPtrC& aAttrValue);
            
        /**
        * From MGECOAttributeProvider. Returns node / data object text 
        * and and localization status for the text. 
        */
        IMPORT_C void TextDetailsL( TPtrC& aText, TBool& aIsLocalized );

        /**
        * From MGECOAttributeProvider. Returns ETrue if object has text data
        */
        IMPORT_C TBool HasTextData();

    
    protected:  // New functions
        
        /**
        * C++ default constructor.
        */
        IMPORT_C CGECODefaultObject();
        
        /**
        * 2nd phase constructor
        */
        IMPORT_C void BaseConstructL( const TDesC& aTypeIdentifier );
        
    protected:
        //Object identifier (e.g. XML tag name)
        HBufC*          iTypeIdentifier;
        
        //Object text (e.g. <xmltag>TEXT</xmltag>)
        //this should be set to NULL if object has no text data, only
        //attributes
        HBufC*          iText;

        //object text localization status
        TBool           iTextLocStatus;
        
        //Array holding attribute names for this object
        CDesCArraySeg*  iAttrNames;

        //Array holding attribute values for this object
        CDesCArraySeg*  iAttrValues;

        //Array holding localization status per attribute
        RArray<TBool>   iAttrLocStatus;
    };

#endif      // CGECODEFAULTOBJECT_H
            
// End of File
