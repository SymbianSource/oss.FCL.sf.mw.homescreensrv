/*
* Copyright (c) 2005,2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Represent single xmluiml attribute
*
*/



// INCLUDE FILES
#include    "hspsdomattribute.h"
#include    "hspsdomstringpool.h"

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// ChspsDomAttribute::ChspsDomAttribute
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsDomAttribute::ChspsDomAttribute( ChspsDomStringPool& aStringPool ):
    iStringPool( aStringPool ),
    iNameRef( KErrNotFound ),
    iValueRef( KErrNotFound )
    {
    }

// -----------------------------------------------------------------------------
// ChspsDomAttribute::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsDomAttribute::ConstructL( const TDesC8& aName )
    {
    iNameRef = iStringPool.AddStringL( aName );
    }

// -----------------------------------------------------------------------------
// ChspsDomAttribute::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsDomAttribute::ConstructL( const TInt aName )
    {
    iNameRef = aName;
    }

// -----------------------------------------------------------------------------
// ChspsDomAttribute::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomAttribute* ChspsDomAttribute::NewL( 
    const TDesC8& aName,
    ChspsDomStringPool& aStringPool )
    {
    ChspsDomAttribute* self = new( ELeave ) ChspsDomAttribute( aStringPool );
    
    CleanupStack::PushL( self );
    self->ConstructL( aName );
    CleanupStack::Pop( self );

    return self;
    }    

// -----------------------------------------------------------------------------
// ChspsDomAttribute::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomAttribute* ChspsDomAttribute::NewL( 
        const TInt aName,
        ChspsDomStringPool& aStringPool )
    {
    ChspsDomAttribute* self = new( ELeave ) ChspsDomAttribute( aStringPool );
    
    CleanupStack::PushL( self );
    self->ConstructL( aName );
    CleanupStack::Pop( self );

    return self;
    } 

// -----------------------------------------------------------------------------
// ChspsDomAttribute::NewL
// Two-phased stream constructor.
// -----------------------------------------------------------------------------
//
ChspsDomAttribute* ChspsDomAttribute::NewL( 
    RReadStream& aStream, 
    ChspsDomStringPool& aStringPool )
    {
    ChspsDomAttribute* self = new( ELeave ) ChspsDomAttribute( aStringPool );
    CleanupStack::PushL( self );
    aStream >> *self;
    CleanupStack::Pop( self );

    return self;
    }    
    
// -----------------------------------------------------------------------------
// ChspsDomAttribute::~ChspsDomAttribute
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomAttribute::~ChspsDomAttribute()
    {
    }

// -----------------------------------------------------------------------------
// ChspsDomAttribute::CloneL
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomAttribute* ChspsDomAttribute::CloneL()
    {
    ChspsDomAttribute* clone = new (ELeave)ChspsDomAttribute( iStringPool );
    clone->iNameRef = iNameRef;
    clone->iValueRef = iValueRef;
    return clone;
    }

// -----------------------------------------------------------------------------
// ChspsDomAttribute::CloneL
// -----------------------------------------------------------------------------
//
ChspsDomAttribute* ChspsDomAttribute::CloneL( ChspsDomStringPool& aStringPool,
                                              const TBool aFastClone )
    {        
    ChspsDomAttribute* clone = NULL;
    
    if( aFastClone )
        {
        clone = ChspsDomAttribute::NewL( iNameRef, aStringPool );
        }
    else
        {    
        const TDesC8& name = iStringPool.String( iNameRef );
        clone = ChspsDomAttribute::NewL( name, aStringPool );
        }
    
    CleanupStack::PushL( clone );
    
    if ( iValueRef > KErrNotFound )
        {
        if( aFastClone )
            {
            clone->SetValueL( iValueRef );
            }
        else
            {
            const TDesC8& value = iStringPool.String( iValueRef );
            clone->SetValueL( value );
            }
        }
    
    CleanupStack::Pop( clone );    
    
    return clone;
    
    }
    
// -----------------------------------------------------------------------------
// ChspsDomAttribute::Name
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& ChspsDomAttribute::Name()
    {
    return iStringPool.String( iNameRef );
    }
    
// -----------------------------------------------------------------------------
// ChspsDomAttribute::NameStringPoolIndex
// -----------------------------------------------------------------------------
//        
EXPORT_C TInt16 ChspsDomAttribute::NameStringPoolIndex()const
    {
    return iNameRef;    
    }    
// -----------------------------------------------------------------------------
// ChspsDomAttribute::Value
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& ChspsDomAttribute::Value()
    {
    if ( iValueRef > KErrNotFound )
        {
        return iStringPool.String( iValueRef );
        }
    return KNullDesC8;
    }
    
// -----------------------------------------------------------------------------
// ChspsDomAttribute::ValueStringPoolIndex
// -----------------------------------------------------------------------------
//        
EXPORT_C TInt16 ChspsDomAttribute::ValueStringPoolIndex()const
    {
    return iValueRef;    
    }    
// -----------------------------------------------------------------------------
// ChspsDomAttribute::SetValueL
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsDomAttribute::SetValueL( const TDesC8& aValue )
    {
    iValueRef = iStringPool.AddStringL( aValue );
    }

// -----------------------------------------------------------------------------
// ChspsDomAttribute::SetValueL
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsDomAttribute::SetValueL( const TInt aValue )
    {
    iValueRef = aValue;
    }
   
// -----------------------------------------------------------------------------
// ChspsDomAttribute::Size
// -----------------------------------------------------------------------------
//
TInt ChspsDomAttribute::Size() const
    {
    TInt size( 0 );
    
    size += sizeof(TInt16); // iNameRef     
      
    size += sizeof(TInt16); // iValueRef
    
    return size;    
    }

// -----------------------------------------------------------------------------
// ChspsDomAttribute::ExternalizeL
// -----------------------------------------------------------------------------
//
void ChspsDomAttribute::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt16L( iNameRef );
    aStream.WriteInt16L( iValueRef );
    }

// -----------------------------------------------------------------------------
// ChspsDomAttribute::InternalizeL
// -----------------------------------------------------------------------------
//
void ChspsDomAttribute::InternalizeL( RReadStream& aStream )
    {
    iNameRef = aStream.ReadInt16L();
    iValueRef = aStream.ReadInt16L();
    }
//  End of File  
