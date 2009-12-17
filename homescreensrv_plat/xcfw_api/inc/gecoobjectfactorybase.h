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
* Description:  Base content object factory interface.
*
*/



#ifndef CGECOOBJECTFACTORYBASE_H
#define CGECOOBJECTFACTORYBASE_H

// INCLUDES
#include <e32base.h>
#include "gecoattributeprovider.h" //MGECOAttributeProvider

// FORWARD DECLARATIONS
class CGECOObjectBase;

// CLASS DECLARATION

/**
* Abstract base class for object factories. 
* All object factories derive from this class.
*
* @lib XCFW.lib
* @since Series 60 3.1
*/
class CGECOObjectFactoryBase: public CBase,
                              public MGECOAttributeProvider
    {
    public: //new functions

        /**
        * Get object parser
        * @param aTypeIdentifier Object type identifier
        * @return CGECOObjectBase derived object. Caller takes ownership.
        */
        virtual CGECOObjectBase* GetContentObjectAndSetContextL(
            const TDesC& aTypeIdentifier ) = 0;

        /**
        * Initializes the current object with attribute provider data
        * @param aAttributeProvider Attribute provider for data initialization
        */
        virtual void InitializeObjectL(
            MGECOAttributeProvider& aAttributeProvider ) = 0;

        /**
        * Set context object for initialize and preparesave operations
        * @param aContext Object to use in InitializeL operations
        * @return Error code, e.g. when the factory does not know this object.
        */
        virtual TInt SetContext( CGECOObjectBase* aContext ) = 0;
        

    public: //from base classes
        
        /**
        * From MGECOAttributeProvider. Returns number of attributes
        */
        virtual TInt NumAttributes() = 0;
        
        /**
        * From MGECOAttributeProvider. Returns name, value and localization 
        * status for attribute in given index.
        */
        virtual void AttributeDetailsL( const TInt aIndex, TPtrC& aAttrName, 
            TPtrC& aAttrValue, TBool& aIsLocalized ) = 0;

        /**
        * From MGECOAttributeProvider. Returns name and value  
        * for attribute in given index.
        */
        virtual void AttributeDetailsL( const TInt aIndex, TPtrC& aAttrName, 
            TPtrC& aAttrValue) = 0;

        /**
        * From MGECOAttributeProvider. Returns ETrue if object / tag 
        * has text data
        */
        virtual TBool HasTextData() = 0;
            
        /**
        * From MGECOAttributeProvider. Returns XML node / content object 
        * text data if any
        */
        virtual void TextDetailsL(TPtrC& aText, TBool& aIsLocalized) = 0;
        

    protected: //data
        //Current factory context. Not owned.
        CGECOObjectBase*             iContext;
    };
    

#endif //CGECOOBJECTFACTORYBASE_H
            
// End of File
