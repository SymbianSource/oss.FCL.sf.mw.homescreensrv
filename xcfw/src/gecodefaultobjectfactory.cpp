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
* Description:  Implementation of GECODefaultObjectFactory
*
*/



// INCLUDE FILES
#include    "gecodefaultobjectfactory.h"
#include    "gecodefaultobject.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGECOObjectFactory::CGECOObjectFactory
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CGECODefaultObjectFactory::CGECODefaultObjectFactory()
    {
    }

    
// Destructor
EXPORT_C CGECODefaultObjectFactory::~CGECODefaultObjectFactory()
    {
    //Null context reference
    iContext = NULL;
    }


// -----------------------------------------------------------------------------
// CGECOObjectFactory::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CGECODefaultObjectFactory* CGECODefaultObjectFactory::NewL()
    {
    CGECODefaultObjectFactory* self = new( ELeave ) CGECODefaultObjectFactory();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;    
    }


// -----------------------------------------------------------------------------
// CGECOObjectFactory::ConstructL
// -----------------------------------------------------------------------------
//
void CGECODefaultObjectFactory::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// CGECOObjectFactory::GetContentObjectAndSetContextL
// Function instantiates a new default content object, sets context and
// returns the object.
// -----------------------------------------------------------------------------
//
EXPORT_C CGECOObjectBase* 
    CGECODefaultObjectFactory::GetContentObjectAndSetContextL(
    const TDesC& aIdentifier )
    {
    CGECODefaultObject* myObject = CGECODefaultObject::NewL( aIdentifier );
    iContext = myObject;
    return myObject;
    }



// -----------------------------------------------------------------------------
// CGECOObjectFactory::InitializeL
// Base class implementation reads attributes to default object arrays.
// -----------------------------------------------------------------------------
//
EXPORT_C void CGECODefaultObjectFactory::InitializeObjectL(
    MGECOAttributeProvider& aAttributeProvider )
    {
    if ( iContext )
        {
        //Cast context to GECODefaultObject
        CGECODefaultObject* temp = 
            reinterpret_cast<CGECODefaultObject*>( iContext );

        TInt counter = aAttributeProvider.NumAttributes() - 1;
        TPtrC name;
        TPtrC value;
        TBool islocalized = EFalse;
        while ( counter >= 0 )
            {
            aAttributeProvider.AttributeDetailsL( counter,name,value );
            temp->SetAttributeL( name, value, islocalized );
            counter--;            
            }

        if ( aAttributeProvider.HasTextData() )
            {
            aAttributeProvider.TextDetailsL( value, islocalized );
            temp->SetTextL( value, islocalized );
            }

        temp = NULL;
        }

    }


// -----------------------------------------------------------------------------
// CGECOObjectFactory::SetContext
// Sets internal context pointer. Deriving class may return an error if 
// it does not know the context by context's type identifier.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGECODefaultObjectFactory::SetContext( 
    CGECOObjectBase* aContext )
    {
    iContext = aContext;
    return KErrNone;        
    }


// -----------------------------------------------------------------------------
// CGECOObjectFactory::HasTextData
// Returns ETrue if object has text data
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CGECODefaultObjectFactory::HasTextData()
    {
    TBool ret = EFalse;
    if ( iContext )
        {
        CGECODefaultObject* temp = 
            ((CGECODefaultObject*)iContext);
        ret = temp->HasTextData( );        
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CGECOObjectFactory::TextDetails
// Returns text data details for this object
// -----------------------------------------------------------------------------
//
EXPORT_C void CGECODefaultObjectFactory::TextDetailsL(
    TPtrC& aText,
    TBool& aIsLocalized )
    {
    if ( iContext )
        {
        CGECODefaultObject* temp = 
            ((CGECODefaultObject*)iContext);
            temp->TextDetailsL( aText, aIsLocalized );        
        }
    }

// -----------------------------------------------------------------------------
// CGECOObjectFactory::NumAttributes
// Returns number of attributes for the current context
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGECODefaultObjectFactory::NumAttributes()
    {
    TInt count = 0;

    if ( iContext )
        {
        count = ((CGECODefaultObject*)iContext)->NumAttributes();
        }

    return count;
    }

// -----------------------------------------------------------------------------
// CGECODefaultObjectFactory::AttributeDetailsL
// Returns attribute details for the current context
// -----------------------------------------------------------------------------
//
EXPORT_C void CGECODefaultObjectFactory::AttributeDetailsL(
    const TInt aIndex, 
    TPtrC& aAttrName, 
    TPtrC& aAttrValue, 
    TBool& aIsLocalized )
    {
    
    if ( iContext )
        {
        CGECODefaultObject* temp = ((CGECODefaultObject*)iContext);
        temp->AttributeDetailsL( aIndex, aAttrName, aAttrValue, aIsLocalized );
        temp = NULL;
        }


    }

// -----------------------------------------------------------------------------
// CGECODefaultObjectFactory::AttributeDetailsL
// Returns attribute details for the current context
// -----------------------------------------------------------------------------
//
EXPORT_C void CGECODefaultObjectFactory::AttributeDetailsL(
    const TInt aIndex, 
    TPtrC& aAttrName, 
    TPtrC& aAttrValue)
    {
    
    if ( iContext )
        {
        CGECODefaultObject* temp = ((CGECODefaultObject*)iContext);
        temp->AttributeDetailsL( aIndex, aAttrName, aAttrValue);
        temp = NULL;
        }


    }

//  End of File  
