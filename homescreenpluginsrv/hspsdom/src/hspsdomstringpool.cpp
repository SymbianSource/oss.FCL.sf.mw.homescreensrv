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
// -----------------------------------------------------------------------------
// Adds string to string pool. If string doesn't appear yet, it is added to 
// the string pool and index to pool is returned. 
// @param aString String to add 
// @param aArray A pool which holds strings
// @return Index to string pool 
// -----------------------------------------------------------------------------
//
LOCAL_C TInt AddToStringPoolL( const TDesC8& aString, RPointerArray<HBufC8>& aArray )
    {
    TBool found( EFalse );
    TInt index( 0 );
     
    TInt count( aArray.Count() );
    for (; index < count && !found;  )
        {
        if ( aArray[ index ]->Des().Compare( aString ) == 0 )
            {
            found = ETrue;
            }
        else
            {
            index++;
            }    
        }
    if ( !found )
        {
        HBufC8* tmp = aString.AllocLC();
        aArray.AppendL( tmp );
        CleanupStack::Pop( tmp );
        index = aArray.Count()-1;    //Last item
        }
    return index;
    }
// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// ChspsDomStringPool::ChspsDomStringPool
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsDomStringPool::ChspsDomStringPool()
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
ChspsDomStringPool* ChspsDomStringPool::NewL()
    {
    ChspsDomStringPool* self = new( ELeave ) ChspsDomStringPool;
    
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
ChspsDomStringPool* ChspsDomStringPool::NewL( RReadStream& aStream )
    {
    ChspsDomStringPool* self = new( ELeave ) ChspsDomStringPool;
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
    }

// -----------------------------------------------------------------------------
// ChspsDomStringPool::CloneL
// -----------------------------------------------------------------------------
//
ChspsDomStringPool* ChspsDomStringPool::CloneL()
    {
    ChspsDomStringPool* clone = ChspsDomStringPool::NewL();
    CleanupStack::PushL( clone );
    
    TInt count( iStringPool.Count() );
    for ( TInt i=0; i<count; i++ )
        {
        HBufC8* tmp = iStringPool[i]->Des().AllocLC();
        clone->iStringPool.AppendL( tmp );
        CleanupStack::Pop( tmp );
        }
    CleanupStack::Pop( clone );
    return clone;
    }

// -----------------------------------------------------------------------------
// ChspsDomNode::AddStringL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt ChspsDomStringPool::AddStringL( const TDesC8& aString )
    {
    return AddToStringPoolL( aString, iStringPool );
    }

// -----------------------------------------------------------------------------
// ChspsDomNode::String
// -----------------------------------------------------------------------------
//
const TDesC8& ChspsDomStringPool::String( const TInt aStringRef )
    {
    if ( aStringRef < iStringPool.Count() )
        return (*iStringPool[ aStringRef ]);
    else
        return KNullDesC8;
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
        iStringPool.AppendL( tmp );
        CleanupStack::Pop( tmp );
        }
   
    }
//  End of File  
