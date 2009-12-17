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



#ifndef CGECODEFAULTOBJECTFACTORY_H
#define CGECODEFAULTOBJECTFACTORY_H

//  INCLUDES
#include <e32std.h>
#include "gecoattributeprovider.h" //MGECOAttributeProvider
#include "gecoobjectfactorybase.h"

// CLASS DECLARATION

/**
* Generic content object factory. All object factories derive from this class.
*
* @lib XCFW.lib
* @since Series 60 3.1
*/
class CGECODefaultObjectFactory: public CGECOObjectFactoryBase
    {
    public: //constructors and destructor
        /**
        /* Two-phased constructor
        */
        IMPORT_C static CGECODefaultObjectFactory* NewL();

        //destructor
        virtual ~CGECODefaultObjectFactory();

    public: //new functions

        /**
        * Get object parser
        * @param aTypeIdentifier Object type identifier
        * @return CGECOObjectBase derived object. Caller takes ownership.
        */
        IMPORT_C CGECOObjectBase* GetContentObjectAndSetContextL(
            const TDesC& aTypeIdentifier );

        /**
        * Initializes the current object with attribute provider data
        * @param aAttributeProvider Attribute provider for data initialization
        */
        IMPORT_C void InitializeObjectL(
            MGECOAttributeProvider& aAttributeProvider );

        /**
        * Set context object for initialize and preparesave operations
        * @param aContext Object to use in InitializeL operations
        * @return Error code, e.g. when the factory does not know this object.
        */
        IMPORT_C TInt SetContext( CGECOObjectBase* aContext );
        

    public: //from base classes
    
        /**
        * From MGECOAttributeProvider. Returns number of attributes
        */
        IMPORT_C TInt NumAttributes();
        
        /**
        * From MGECOAttributeProvider. Returns name, value and localization status
        * for attribute in given index.
        */
        IMPORT_C void AttributeDetailsL( const TInt aIndex, TPtrC& aAttrName, 
            TPtrC& aAttrValue, TBool& aIsLocalized);
                                
        /**
        * From MGECOAttributeProvider. Returns name and value 
        * for attribute in given index.
        */
        IMPORT_C void AttributeDetailsL( const TInt aIndex, TPtrC& aAttrName, 
            TPtrC& aAttrValue);
                                
        /**
        * From MGECOAttributeProvider. Returns node / data object text data
        * and localization status
        */
        IMPORT_C void TextDetailsL( TPtrC& aText, TBool& aIsLocalized );

        /**
        * From MGECOAttributeProvider. Returns ETrue if object has text data
        */
        IMPORT_C TBool HasTextData();


    protected: //new functions

        /**
        * C++ default constructor.
        */
        IMPORT_C CGECODefaultObjectFactory();
            
    private: //new function

        /**
        * 2nd phase constructor
        */
        void ConstructL();
        
    private: //data

    };
    

#endif //CGECODEFAULTOBJECTFACTORY_H
            
// End of File
