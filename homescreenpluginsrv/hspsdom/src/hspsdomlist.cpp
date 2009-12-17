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
* Description:  List class to hold ChspsDomList objects.
*
*/



// INCLUDE FILES
#include    "hspsdomlist.h"
#include    "hspsdomnode.h"
#include    "hspsdomattribute.h"
#include    "hspsdomstringpool.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChspsDomList::ChspsDomList
// C++ parameter constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsDomList::ChspsDomList( TListType aListType, ChspsDomStringPool& aStringPool ):
    iListType( aListType ),
    iList( 1 ),
    iStringPool( aStringPool )
    {
    }
// -----------------------------------------------------------------------------
// ChspsDomList::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsDomList::ConstructL()
    {
   
    }

// -----------------------------------------------------------------------------
// ChspsDomList::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChspsDomList* ChspsDomList::NewL( TListType aListType, ChspsDomStringPool& aStringPool )
    {
    ChspsDomList* self = new( ELeave ) ChspsDomList( aListType, aStringPool );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
// -----------------------------------------------------------------------------
// ChspsDomList::NewL
// Two-phased stream constructor.
// -----------------------------------------------------------------------------
//
ChspsDomList* ChspsDomList::NewL( 
    RReadStream& aStream, 
    ChspsDomStringPool& aStringPool )
    {
    ChspsDomList* self = new( ELeave ) ChspsDomList( ENodeList, aStringPool );
    
    CleanupStack::PushL( self );
   
    aStream >> *self;
    CleanupStack::Pop( self );

    return self;
    }
    
// Destructor
ChspsDomList::~ChspsDomList()
    {
    iList.ResetAndDestroy();    
    }

// -----------------------------------------------------------------------------
// ChspsDomList::Reset
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsDomList::Reset()
    {
    iList.Reset();
    }
    
// -----------------------------------------------------------------------------
// ChspsDomList::AddItemL
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsDomList::AddItemL( MhspsDomListItem* aItem )
    {
    iList.AppendL( aItem );
    }
    
// -----------------------------------------------------------------------------
// ChspsDomList::AddItemL
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsDomList::AddItemL( MhspsDomListItem* aItem, TInt aIndex )
    {
    iList.InsertL( aItem, aIndex );
    }

// -----------------------------------------------------------------------------
// ChspsDomList::RemoveItem
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsDomList::RemoveItem( TInt aIndex )
    {
    iList.Remove( aIndex);
    iList.Compress();
    }
// -----------------------------------------------------------------------------
// ChspsDomList::DeleteItem
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsDomList::DeleteItem( TInt aIndex )
    {
    delete iList[ aIndex ];
    iList.Remove( aIndex);
    iList.Compress();
    }

// -----------------------------------------------------------------------------
// ChspsDomList::RemoveItem
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsDomList::RemoveItem( MhspsDomListItem* aItem ) 
    {
    MhspsDomListItem* tmp = NULL;
    TInt count( iList.Count() );
    TBool stop( EFalse );
    for ( TInt i=0; i<count && !stop; i++ )    
        {
        tmp = iList[i];
        if ( tmp == aItem )
            {
            iList.Remove( i );
            iList.Compress();
            stop = ETrue;
            }
        }
    }

// -----------------------------------------------------------------------------
// ChspsDomList::DeleteItem
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsDomList::DeleteItem( MhspsDomListItem* aItem ) 
    {
    MhspsDomListItem* tmp = NULL;
    TInt count( iList.Count() );
    TBool stop( EFalse );
    for ( TInt i=0; i<count && !stop ; i++ )    
        {
        tmp = iList[i];
        if ( tmp == aItem )
            {
            delete iList[i];
            iList.Remove( i );
            iList.Compress();
            stop = ETrue;
            }
        }
    }    
// -----------------------------------------------------------------------------
// ChspsDomList::Item
// -----------------------------------------------------------------------------
//    
EXPORT_C MhspsDomListItem* ChspsDomList::Item( TInt aIndex )const
    {
    
    return iList[ aIndex ];
    }    
// -----------------------------------------------------------------------------
// ChspsDomList::Length
// -----------------------------------------------------------------------------
//
EXPORT_C TInt ChspsDomList::Length() const
    {
   
    return iList.Count();    
    }
// -----------------------------------------------------------------------------
// ChspsDomList::First
// -----------------------------------------------------------------------------
//
EXPORT_C MhspsDomListItem* ChspsDomList::First()
    {
    if ( iList.Count() )
        {
        return iList[ 0 ];
        }
    else
        {
        return NULL;
        }
    }    
    
// -----------------------------------------------------------------------------
// ChspsDomList::Last
// -----------------------------------------------------------------------------
//
EXPORT_C MhspsDomListItem* ChspsDomList::Last()
    {
    TInt count( iList.Count() );
    if ( count )
        {
        return  iList[ count-1 ];
        }
    else
        {
        return NULL;
        }
   
    }    
    

// -----------------------------------------------------------------------------
// ChspsDomList::FindByName
// -----------------------------------------------------------------------------
//
EXPORT_C MhspsDomListItem* ChspsDomList::FindByName( const TDesC8& aName )
    {
    MhspsDomListItem* tmp = NULL;
    TInt count( iList.Count() );
    for ( TInt i=0; i<count; i++ )    
        {
        tmp = iList[ i ];
        
        if ( aName.Compare( tmp->Name() ) == 0 )
            {
            return tmp;
            }
        }
    return NULL; 
    }
        
// -----------------------------------------------------------------------------
// ChspsDomList::ItemIndex
// -----------------------------------------------------------------------------
//
EXPORT_C TInt ChspsDomList::ItemIndex( const MhspsDomListItem& aItem ) const
    {
    
    MhspsDomListItem* tmp = NULL;
    TInt count( iList.Count() );
    for ( TInt i=0; i<count; i++ )    
        {
        tmp = iList[ i ];
        if ( tmp == &aItem )
            {
            return i;
            }
        }
    return KErrNotFound;   
    }

// -----------------------------------------------------------------------------
// ChspsDomList::StringPool
// -----------------------------------------------------------------------------
//        
EXPORT_C ChspsDomStringPool& ChspsDomList::StringPool() const
    {
    return iStringPool;
    }
    
// -----------------------------------------------------------------------------
// ChspsDomList::Size
// -----------------------------------------------------------------------------
//
TInt ChspsDomList::Size() const
    {
    TInt size = 0;
    
    size += sizeof(TInt8);      //Type
    size += sizeof(TInt32);     //Item count
   
    TInt count( iList.Count() );
    for ( TInt i=0; i<count; i++ )    
        {
        size += iList[ i ]->Size();
        }
    return size;
    }
// -----------------------------------------------------------------------------
// ChspsDomList::ExternalizeL
// -----------------------------------------------------------------------------
//
void ChspsDomList::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt8L( iListType );
    
   
    TInt count( iList.Count() );
    aStream.WriteInt32L( count );
    
    
    for ( TInt i=0; i<count; i++ )    
        {
        aStream << *iList[ i ];
        }    
    }

// -----------------------------------------------------------------------------
// ChspsDomList::InternalizeL
// -----------------------------------------------------------------------------
//
void ChspsDomList::InternalizeL( RReadStream& aStream )
    {
    iListType = static_cast<ChspsDomList::TListType>( aStream.ReadInt8L() );    
    
    TInt count ( aStream.ReadInt32L() );
    for ( TInt i=0; i<count; i++ )
        {
        MhspsDomListItem* item = NULL;
        
        switch( iListType )
        	{
        	case ChspsDomList::ENodeList:
    			{
    			item = ChspsDomNode::NewL( aStream, iStringPool );
    			}
        	break;
        	
        	case ChspsDomList::EAttributeList:
    			{
    			item = ChspsDomAttribute::NewL( aStream, iStringPool );
    			}
        	break;
        	default:
        		User::Leave(KErrArgument);
        		break;
        	}
        CleanupDeletePushL( item );	
        iList.AppendL( item );
        CleanupStack::Pop( item );
        }
    }



//  End of File  
