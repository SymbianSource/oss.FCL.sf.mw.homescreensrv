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
* Description:  Implementation of GECODefaultObject
*
*/


// INCLUDE FILES
#include    "gecodefaultobject.h"
#include    "xcfwpanic.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGECODefaultObject::CGECODefaultObject
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CGECODefaultObject::CGECODefaultObject()
    {
    }


// -----------------------------------------------------------------------------
// CGECODefaultObject::NewL
// Two phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CGECODefaultObject* CGECODefaultObject::NewL(
    const TDesC& aTypeIdentifier )
    {
    CGECODefaultObject* self = new( ELeave ) CGECODefaultObject() ;
    
    CleanupStack::PushL( self );
    self->BaseConstructL( aTypeIdentifier );
    CleanupStack::Pop( self );

    return self;    
    }

// -----------------------------------------------------------------------------
// CGECODefaultObject::BaseConstructL
// Initializes default object data arrays and type identifier.
// -----------------------------------------------------------------------------
//
EXPORT_C void CGECODefaultObject::BaseConstructL( const TDesC& aTypeIdentifier ) 
    {
    const TInt KDefaultGranularity = 3;
    iTypeIdentifier = aTypeIdentifier.AllocL();    
    iAttrNames = new ( ELeave ) CDesCArraySeg( KDefaultGranularity );
    iAttrValues = new ( ELeave ) CDesCArraySeg( KDefaultGranularity );
    }

    
// Destructor
EXPORT_C CGECODefaultObject::~CGECODefaultObject()
    {
    delete iTypeIdentifier;
    delete iAttrNames;
    delete iAttrValues;
    delete iText;
    iAttrLocStatus.Close();
    }


// -----------------------------------------------------------------------------
// CGECODefaultObject::Identifier
// return object TypeIdentifier
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC& CGECODefaultObject::TypeIdentifier()
    {

    __ASSERT_PANIC_DEBUG( iTypeIdentifier, 
        EXCFWPanicNoTypeIdentifier );
        
    return *iTypeIdentifier;    

    }


// -----------------------------------------------------------------------------
// CGECODefaultObject::GetAttribute
// Returns attribute value for given attrib. name
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGECODefaultObject::GetAttribute( 
    const TDesC& aAttrName, 
    TPtrC& aAttrValue )
    {

    TInt error = KErrNotFound;
    const TInt count = iAttrNames->Count();    

    for ( TInt i = 0; i < count; i++ )
        {
        if ( iAttrNames->MdcaPoint( i ) == aAttrName )
            {
            error = KErrNone;
            aAttrValue.Set( iAttrValues->MdcaPoint( i ) );                
            }
        }
    
    return error;

    
    }
// -----------------------------------------------------------------------------
// CGECODefaultObject::GetAttribute
// Returns attribute value and localization status for given attrib. name
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGECODefaultObject::GetAttribute( 
    const TDesC& aAttrName, 
    TPtrC& aAttrValue, 
    TBool& aIsLocalized)
    {
        
    TInt error = KErrNotFound;
    const TInt count = iAttrNames->Count();
    
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iAttrNames->MdcaPoint( i ) == aAttrName )
            {
            error = KErrNone;
            aAttrValue.Set( iAttrValues->MdcaPoint( i ) );
            aIsLocalized = iAttrLocStatus[i];
            }
        }
    
    return error;


    }

// -----------------------------------------------------------------------------
// CGECODefaultObject::SetAttributeL
// This function adds new attribute with Localization status EFalse
// -----------------------------------------------------------------------------
//
EXPORT_C void CGECODefaultObject::SetAttributeL(
    const TDesC& aAttrName, 
    const TDesC& aAttrValue )
    {
    SetAttributeL( aAttrName, aAttrValue, EFalse );
    }

// -----------------------------------------------------------------------------
// CGECODefaultObject::SetAttributeL
// New attribute is added to object data arrays. Value and localization status
// addition is TRAPped to make sure that all arrays have identical item count
// at all times.
// -----------------------------------------------------------------------------
//
EXPORT_C void CGECODefaultObject::SetAttributeL(
    const TDesC& aAttrName, 
    const TDesC& aAttrValue, 
    TBool aIsLocalized )
    {

    iAttrNames->AppendL( aAttrName );
    TInt err = KErrNone;

    TRAP( err, iAttrValues->AppendL( aAttrValue ) );
    if ( err != KErrNone )
        {
        //if attribute value addition failed, remove name and leave
        TInt lastindex = iAttrNames->Count() - 1;
        iAttrNames->Delete( lastindex );
        User::Leave( err );
        }
    else
        {
        TRAP( err, iAttrLocStatus.AppendL( aIsLocalized ) );
        if ( err != KErrNone )
            {
            //if loc status addition failed, remove name and leave
            TInt lastindex = iAttrNames->Count() - 1;
            iAttrNames->Delete( lastindex );
            iAttrValues->Delete( lastindex );                
            }
        }
    }


// -----------------------------------------------------------------------------
// CGECODefaultObject::TextDetails
// Returns objects text data if any
// -----------------------------------------------------------------------------
//
EXPORT_C void CGECODefaultObject::TextDetailsL( 
    TPtrC& aText,
    TBool& aIsLocalized )
    {
    if ( iText )
        {
        aText.Set( iText->Des() );
        aIsLocalized = iTextLocStatus;
        }
    else
        {
        User::Leave( KErrNotFound );            
        }
    }


// -----------------------------------------------------------------------------
// CGECODefaultObject::NumAttributes
// Returns number of attributes from internal array
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGECODefaultObject::NumAttributes()
    {
    return iAttrNames->Count();
    }

// -----------------------------------------------------------------------------
// CGECODefaultObject::AttributeDetailsL
// Returns details of the attribute in given index (MGECOAttributeProvider IF)
// -----------------------------------------------------------------------------
//
EXPORT_C void CGECODefaultObject::AttributeDetailsL(
    const TInt aIndex, 
    TPtrC& aAttrName, 
    TPtrC& aAttrValue, 
    TBool& aIsLocalized)
    {
    
    if ( aIndex >= 0 && iAttrNames->Count() > aIndex )
        {
        aAttrName.Set( iAttrNames->MdcaPoint( aIndex ) );
        aAttrValue.Set( iAttrValues->MdcaPoint( aIndex ) );
        aIsLocalized = iAttrLocStatus[aIndex];
        }
    else
        {
        User::Leave( KErrNotFound );            
        }

    }

// -----------------------------------------------------------------------------
// CGECODefaultObject::AttributeDetailsL
// Returns details of the attribute in given index (MGECOAttributeProvider IF)
// -----------------------------------------------------------------------------
//
EXPORT_C void CGECODefaultObject::AttributeDetailsL(
    const TInt aIndex, 
    TPtrC& aAttrName, 
    TPtrC& aAttrValue)
    {
    
    if ( aIndex >= 0 && iAttrNames->Count() > aIndex )
        {
        aAttrName.Set( iAttrNames->MdcaPoint( aIndex ) );
        aAttrValue.Set( iAttrValues->MdcaPoint( aIndex ) );
        }
    else
        {
        User::Leave( KErrNotFound );            
        }

    }

// -----------------------------------------------------------------------------
// CGECODefaultObject::RemoveAttribute
// Removes an attribute from the internal attribute name array. 
// Value + localization status from same position are deleted as well.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGECODefaultObject::RemoveAttribute( 
    const TDesC& aAttrName ) 
    {

    TInt error = KErrNotFound;
    
    const TInt index = iAttrNames->Count() - 1;
    
    for ( TInt i = index; i >= 0; i-- )

        {
        if ( iAttrNames->MdcaPoint( i ) == aAttrName )
            {
            error = KErrNone;
            iAttrNames->Delete( i );
            iAttrValues->Delete( i );
            iAttrLocStatus.Remove( i );
            }
        }
    
    return error;
    }


// -----------------------------------------------------------------------------
// CGECODefaultObject::SetText
// Sets the object's text data
// -----------------------------------------------------------------------------
//
EXPORT_C void CGECODefaultObject::SetTextL( 
    const TDesC& aText )
    {
    SetTextL( aText, EFalse );
    }

// -----------------------------------------------------------------------------
// CGECODefaultObject::SetText
// Sets the object's text data and corresponding localization status
// -----------------------------------------------------------------------------
//
EXPORT_C void CGECODefaultObject::SetTextL( 
    const TDesC& aText, 
    TBool aIsLocalized )
    {
    delete iText;
    iText = NULL;
    iText = aText.AllocL();
    iTextLocStatus = aIsLocalized;
    }

// -----------------------------------------------------------------------------
// CGECODefaultObject::HasTextData
// Returns ETrue if object has text data
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CGECODefaultObject::HasTextData()
    {
    TBool ret = EFalse;
    if ( iText )
        {
        ret = ETrue;            
        }
    return ret;
    }


// -----------------------------------------------------------------------------
// CGECODefaultObject::GetText
// Returns the text data for this object
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CGECODefaultObject::GetText( TPtrC& aText )
    {
    TInt ret = KErrNotFound;
    if ( iText )
        {
        ret = KErrNone;
        aText.Set( *iText );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CGECODefaultObject::RemoveText
// Removes possible text data from the object
// -----------------------------------------------------------------------------
//
EXPORT_C void CGECODefaultObject::RemoveText()        
    {
    delete iText;
    iText = NULL;
    iTextLocStatus = EFalse;
    }

//  End of File  
