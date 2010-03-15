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
* Description:  Represent string pool for dom strings
*
*/



// INCLUDE FILES
#include    "hspsdomstringpool.h"

// ============================ LOCAL FUNCTIONS ================================

// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// ChspsDomStringPool::ChspsDomStringPool
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsDomStringPool::ChspsDomStringPool( const TBool aAllowDuplicates ) :
    iAllowDuplicates( aAllowDuplicates )
    {
    }

// -----------------------------------------------------------------------------
// ChspsDomStringPool::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsDomStringPool::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// ChspsDomStringPool::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChspsDomStringPool* ChspsDomStringPool::NewL( const TBool aAllowDuplicates )
    {
    ChspsDomStringPool* self =
            new( ELeave ) ChspsDomStringPool( aAllowDuplicates );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }    


// -----------------------------------------------------------------------------
// ChspsDomStringPool::NewL
// Two-phased stream constructor.
// -----------------------------------------------------------------------------
//
ChspsDomStringPool* ChspsDomStringPool::NewL( RReadStream& aStream,
        const TBool aAllowDuplicates )
    {
    ChspsDomStringPool* self = new( ELeave ) ChspsDomStringPool( aAllowDuplicates );
    CleanupStack::PushL( self );
    aStream >> *self;
    CleanupStack::Pop(self);

    return self;
    }    
    
// -----------------------------------------------------------------------------
// ChspsDomStringPool::~ChspsDomStringPool
// Destructor
// -----------------------------------------------------------------------------
//
ChspsDomStringPool::~ChspsDomStringPool()
    {
    iStringPool.ResetAndDestroy();
    iStringPoolOptimizer.Close();
    }

// -----------------------------------------------------------------------------
// ChspsDomStringPool::CloneL
// -----------------------------------------------------------------------------
//
ChspsDomStringPool* ChspsDomStringPool::CloneL()
    {    
    ChspsDomStringPool* clone = NULL;    
    if( iAllowDuplicates )
        {
        clone = ChspsDomStringPool::NewL( ETrue );
        }
    else
        {
        clone = ChspsDomStringPool::NewL( EFalse );
        }    
    CleanupStack::PushL( clone );
    
    TInt count( iStringPool.Count() );
    for ( TInt i = 0; i < count; i++ )
        {    
        HBufC8* tmp = iStringPool[i]->Des().AllocLC();
        clone->DoAddStringL( tmp );
        CleanupStack::Pop( tmp );
        }
    CleanupStack::Pop( clone );
    return clone;
    }

// -----------------------------------------------------------------------------
// ChspsDomStringPool::Reset
// -----------------------------------------------------------------------------
//
void ChspsDomStringPool::Reset()
    {    
    if( iStringPool.Count() > 0 )
        {
        iStringPool.ResetAndDestroy();        
        }
    
    if( iStringPoolOptimizer.Count() > 0 )
        {
        iStringPoolOptimizer.Reset();
        }
    }

// -----------------------------------------------------------------------------
// ChspsDomStringPool::AddStringL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt ChspsDomStringPool::AddStringL( const TDesC8& aString )
    {   
    TInt index = iStringPoolOptimizer.GetIndex( aString );

    if( index == KErrNotFound )
        {
        HBufC8* string = aString.AllocLC();
        index = DoAddStringL( string );        
        CleanupStack::Pop( string );        
        }    
    
    return index;    
    }

// -----------------------------------------------------------------------------
// ChspsDomStringPool::AddStringL
// -----------------------------------------------------------------------------
//
TInt ChspsDomStringPool::AddStringL( HBufC8* aString )
    {   
    if( !aString )
        {
        User::Leave( KErrArgument );
        }
    
    TInt index = iStringPoolOptimizer.GetIndex( *aString );    
    
    if( index == KErrNotFound )
        {
        index = DoAddStringL( aString );       
        }
    else
        {
        delete aString;
        }
    
    return index;    
    }

// -----------------------------------------------------------------------------
// ChspsDomStringPool::AddStringL
// -----------------------------------------------------------------------------
//
void ChspsDomStringPool::AddAllL( ChspsDomStringPool& aStringPool )
    {
    const TInt count = aStringPool.Count();
    for( TInt i = 0; i < count; i++ )
        {
        AddStringL( aStringPool.String( i ) );
        }    
    }

// -----------------------------------------------------------------------------
// ChspsDomStringPool::String
// -----------------------------------------------------------------------------
//
const TDesC8& ChspsDomStringPool::String( const TInt aStringRef )
    {       
    if( aStringRef >= 0 && aStringRef < iStringPool.Count() )
        {
        return (*iStringPool[ aStringRef ]);
        }
    else
        {
        return KNullDesC8;
        }
    }

// -----------------------------------------------------------------------------
// ChspsDomStringPool::Size
// -----------------------------------------------------------------------------
//
TInt ChspsDomStringPool::Size() const
    {
    TInt size( 0 );
    
    TInt count( iStringPool.Count() );
    for ( TInt i=0; i<count; i++ )
        {
        size += sizeof(TInt16);     //Length
        size++;                     //HBufC control mark
        size++;                     //HBufC control mark
        size += iStringPool[i]->Size();  //Buffer sixe in bytes     
        }
    return size;    
    }

// -----------------------------------------------------------------------------
// ChspsDomStringPool::Count
// -----------------------------------------------------------------------------
//
TInt ChspsDomStringPool::Count() const
    {
    return iStringPool.Count();
    }

// -----------------------------------------------------------------------------
// ChspsDomStringPool::ExternalizeL
// -----------------------------------------------------------------------------
//
void ChspsDomStringPool::ExternalizeL( RWriteStream& aStream ) const
    {
    TInt count( iStringPool.Count() );
    aStream.WriteInt16L( count );
    
    for ( TInt i=0; i<count; i++ )
        {
        aStream.WriteInt16L( iStringPool[i]->Length() );
        aStream << *iStringPool[i];      
        }
    }

// -----------------------------------------------------------------------------
// ChspsDomStringPool::InternalizeL
// -----------------------------------------------------------------------------
//
void ChspsDomStringPool::InternalizeL( RReadStream& aStream )
    {
    TInt len(0);
    TInt16 count ( aStream.ReadInt16L() );    

    for ( TInt i=0; i<count; i++ )
        {
        len = aStream.ReadInt16L();
        HBufC8* tmp = HBufC8::NewLC( aStream, len );
        AddStringL( tmp ); // OWNERSHIP TRANSFERRED!        
        CleanupStack::Pop( tmp );
        }
    }

// -----------------------------------------------------------------------------
// ChspsDomStringPool::DoAddStringL
// -----------------------------------------------------------------------------
//
TInt ChspsDomStringPool::DoAddStringL( HBufC8* aNewString )
    {
    if( !aNewString )
        {
        User::Leave( KErrArgument );
        }       
    
    TInt index = iStringPool.Count();
    
    if( !iAllowDuplicates )
        {
        ThspsDomStringPoolOptimizerEntry tmp( index, *aNewString );
        iStringPoolOptimizer.AddEntryL( tmp );
        }
    
    iStringPool.AppendL( aNewString );       
        
    return index;
    }

//  End of File  
