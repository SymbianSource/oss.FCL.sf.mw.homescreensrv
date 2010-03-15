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
* Description:  Primary datatype for the entire Document Object Model.
*
*/



// INCLUDE FILES
#include    "hspsdomnode.h"
#include    "hspsdomattribute.h"
#include    "hspsdomlist.h"
#include    "hspsdomstringpool.h"

// LOCAL CONSTANTS AND MACROS
const TInt KNotDefined = -1;
_LIT8( KIdAttr, "id" );
_LIT8( KRefAttr, "ref" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChspsDomNode::ChspsDomNode
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsDomNode::ChspsDomNode( ChspsDomStringPool& aStringPool ):
    iStringPool( aStringPool ), 
    iNodeId( KNotDefined )
    {
    }
    

// -----------------------------------------------------------------------------
// ChspsDomNode::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsDomNode::ConstructL(
    const TDesC8& aName,
    const TDesC8& aNS )
    {
    iNameRef = iStringPool.AddStringL( aName );
    iNSRef = iStringPool.AddStringL( aNS );
    Construct2L();
    }

// -----------------------------------------------------------------------------
// ChspsDomNode::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsDomNode::ConstructL(
    const TInt aName,
    const TInt aNS )
    {
    iNameRef = aName;
    iNSRef = aNS;
    Construct2L();
    }

// -----------------------------------------------------------------------------
// ChspsDomNode::Construct2L 
// -----------------------------------------------------------------------------
//
void ChspsDomNode::Construct2L()
    {
    iChildList = ChspsDomList::NewL( ChspsDomList::ENodeList, iStringPool );
    iAttributeList = ChspsDomList::NewL( ChspsDomList::EAttributeList, iStringPool );
    }

// -----------------------------------------------------------------------------
// ChspsDomNode::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChspsDomNode* ChspsDomNode::NewL(
    const TDesC8& aName,
    const TDesC8& aNS, 
    ChspsDomStringPool& aStringPool )
    {
    ChspsDomNode* self = new( ELeave ) ChspsDomNode( aStringPool );
    
    CleanupStack::PushL( self );
    self->ConstructL( aName, aNS );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// ChspsDomNode::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
ChspsDomNode* ChspsDomNode::NewL(
    const TInt aName,
    const TInt aNS, 
    ChspsDomStringPool& aStringPool )
    {
    ChspsDomNode* self = new( ELeave ) ChspsDomNode( aStringPool );
    
    CleanupStack::PushL( self );
    self->ConstructL( aName, aNS );
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// ChspsDomNode::NewL
// Two-phased stream constructor.
// -----------------------------------------------------------------------------
//
ChspsDomNode* ChspsDomNode::NewL( RReadStream& aStream, ChspsDomStringPool& aStringPool )
    {
    ChspsDomNode* self = new( ELeave ) ChspsDomNode( aStringPool );
    
    CleanupStack::PushL( self );
    
    aStream >> *self;
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
ChspsDomNode::~ChspsDomNode()
    {
    delete iChildList;
    delete iAttributeList;
    
    delete iPCData;
    }
    
// -----------------------------------------------------------------------------
// ChspsDomNode::CloneL
// Clones this node and it's child nodes. This is a recursive function.
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomNode* ChspsDomNode::CloneL( ChspsDomStringPool& aStringPool,
                                             const TBool aFastClone )
    {
    ChspsDomNode* clone = NULL;
    
    if( aFastClone )
        {
        clone = ChspsDomNode::NewL( iNameRef, iNSRef, aStringPool );
        }
    else
        {
        const TDesC8& name = iStringPool.String( iNameRef );
        const TDesC8& ns = iStringPool.String( iNSRef );        
        clone = ChspsDomNode::NewL( name, ns, aStringPool );
        }

    CleanupStack::PushL( clone );

    if ( iPCData )
        {
        clone->AppendPCDataL( *iPCData );
        }
    
    clone->iNodeId = iNodeId;
    clone->iRefNode = iRefNode;
    
    TInt childCount( iChildList->Length() );
   
    for( TInt i = 0; i < childCount; i++ )
        {
        ChspsDomNode* childClone = 
            static_cast<ChspsDomNode*>( iChildList->Item(i) )->CloneL( aStringPool, aFastClone );
        CleanupStack::PushL( childClone );
        childClone->iParentNode = clone;
        clone->iChildList->AddItemL( childClone );
        CleanupStack::Pop( childClone );
        }
    
    TInt attrCount( iAttributeList->Length() );
    for( TInt j = 0; j < attrCount; j++ )
        {
        ChspsDomAttribute* attrClone = 
            static_cast<ChspsDomAttribute*>( iAttributeList->Item(j) )->CloneL( aStringPool, aFastClone );
        CleanupStack::PushL( attrClone );
        clone->iAttributeList->AddItemL( attrClone );
        CleanupStack::Pop( attrClone );
        }
    CleanupStack::Pop( clone );
    return clone;
    }

// -----------------------------------------------------------------------------
// ChspsDomNode::CloneWithoutKidsL
// Clones only this node. This is a recursive function.
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomNode* ChspsDomNode::CloneWithoutKidsL( ChspsDomStringPool& aStringPool )
    {
    const TDesC8& name = iStringPool.String( iNameRef );
    const TDesC8& ns = iStringPool.String( iNSRef );
    
    ChspsDomNode*  clone = ChspsDomNode::NewL( name, ns, aStringPool );
    CleanupStack::PushL( clone );
    if ( iPCData )
        {
        clone->AppendPCDataL( *iPCData );
        }
    clone->iNodeId = iNodeId;
    clone->iRefNode = iRefNode;
    
    TInt attrCount( iAttributeList->Length() );
    for ( TInt j=0; j<attrCount; j++ )
        {
        ChspsDomAttribute* attrClone = 
            static_cast<ChspsDomAttribute*>( iAttributeList->Item(j) )->CloneL( aStringPool );
        CleanupStack::PushL( attrClone );
        clone->iAttributeList->AddItemL( attrClone );
        CleanupStack::Pop( attrClone );
        }
    
    CleanupStack::Pop( clone );
    return clone;
    }

// -----------------------------------------------------------------------------
// ChspsDomNode::CreateRefNodeL
// Recursive function to create referer nodes.
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomNode* ChspsDomNode::CreateRefNodeL()
    {
    const TDesC8& name = iStringPool.String( iNameRef );
    const TDesC8& ns = iStringPool.String( iNSRef );
    
    ChspsDomNode*  ref = ChspsDomNode::NewL( name, ns, iStringPool );
    CleanupStack::PushL( ref );
    
    ref->iRefNode = ETrue;
    
    TInt childCount( iChildList->Length() );
   
    for ( TInt i=0; i<childCount; i++ )
        {
        ChspsDomNode* childRef = 
            static_cast<ChspsDomNode*>( iChildList->Item(i) )->CreateRefNodeL();
        CleanupStack::PushL( childRef );
        childRef->iParentNode = ref;
        ref->iChildList->AddItemL( childRef );
        CleanupStack::Pop( childRef );
        }
    
    ChspsDomAttribute* attr = NULL;
    if ( !iRefNode )
        {
        attr = static_cast<ChspsDomAttribute*>( iAttributeList->FindByName( KIdAttr ) );
        }
    else
        {
        attr = static_cast<ChspsDomAttribute*>( iAttributeList->FindByName( KRefAttr ) );
        }
        
    if ( attr )
        {
        ChspsDomAttribute* newAttr = ChspsDomAttribute::NewL( KRefAttr, iStringPool );
        CleanupStack::PushL( newAttr );
        newAttr->SetValueL( attr->Value() );
        ref->iAttributeList->AddItemL( newAttr );
        CleanupStack::Pop( newAttr );
        }
    else
        {
        //referred node don't have an id or ref, thats not ok.
        User::Leave(KErrArgument);
        }
            
    CleanupStack::Pop( ref );
    return ref;
    }   

    
// -----------------------------------------------------------------------------
// ChspsDomNode::Name
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& ChspsDomNode::Name()
    {
    return iStringPool.String( iNameRef );
    }
        
// -----------------------------------------------------------------------------
// ChspsDomNode::Namespace
// -----------------------------------------------------------------------------
//
EXPORT_C const TDesC8& ChspsDomNode::Namespace()
    {
    return iStringPool.String( iNSRef );
    }     

// -----------------------------------------------------------------------------
// ChspsDomNode::AttributeList
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomList& ChspsDomNode::AttributeList() const
    {
    return *iAttributeList;
    }
// -----------------------------------------------------------------------------
// ChspsDomNode::SetParent
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsDomNode::SetParent( ChspsDomNode* aParent )
    {
    iParentNode = aParent;
    }
// -----------------------------------------------------------------------------
// ChspsDomNode::Parent
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomNode* ChspsDomNode::Parent()const
    {
    return iParentNode;
    }
// -----------------------------------------------------------------------------
// ChspsDomNode::AddChildL
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsDomNode::AddChildL( ChspsDomNode* aNode )
    {
    aNode->SetParent( this );
    iChildList->AddItemL( aNode );
    }

// -----------------------------------------------------------------------------
// ChspsDomNode::AddChildL
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsDomNode::AddChildL( ChspsDomNode* aNode, TInt aIndex )
    {
    aNode->SetParent( this );
    iChildList->AddItemL( aNode, aIndex );
    }

// -----------------------------------------------------------------------------
// ChspsDomNode::DeleteChild
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsDomNode::DeleteChild( ChspsDomNode* aNode )
    {
    iChildList->DeleteItem( aNode );
    }

// -----------------------------------------------------------------------------
// ChspsDomNode::ReplaceChildL
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsDomNode::ReplaceChildL( 
    ChspsDomNode* aNode,
    ChspsDomNode* aNewNode )
    {
    ChspsDomNode* swapChild = NULL;
    ChspsDomList& childList = aNode->ChildNodes();
    TInt childCount( childList.Length() );
    for ( TInt i=0; i<childCount; i++ )
        {
        swapChild = static_cast<ChspsDomNode*>( childList.Item(i) );
        aNewNode->AddChildL( swapChild );   //Let the new node adopt the child
        //cannot remove it during iterating childlist
        //childList.RemoveItem( swapChild );  //Remove it from the orginal parent
        }
    iChildList->DeleteItem( aNode ); // Delete the old child
    
    aNewNode->SetParent( this );    //Set new child
    iChildList->AddItemL( aNewNode );
    }
            
// -----------------------------------------------------------------------------
// ChspsDomNode::ChildNodes
// -----------------------------------------------------------------------------
//   
EXPORT_C ChspsDomList& ChspsDomNode::ChildNodes()
    {
    return *iChildList;
    }
// -----------------------------------------------------------------------------
// ChspsDomNode::SetNodeId
// -----------------------------------------------------------------------------
//   
EXPORT_C void ChspsDomNode::SetNodeId( const TInt aNodeId )
    {
    iNodeId = aNodeId;
    }
// -----------------------------------------------------------------------------
// ChspsDomNode::NodeId
// -----------------------------------------------------------------------------
//   
EXPORT_C TInt ChspsDomNode::NodeId() const
    {
    return iNodeId;
    }

// -----------------------------------------------------------------------------
// ChspsDomNode::ItemIndex
// -----------------------------------------------------------------------------
//   
EXPORT_C TInt ChspsDomNode::ItemIndex( const MhspsDomListItem& aItem )const
    {
    return iChildList->ItemIndex( aItem );
    }

// -----------------------------------------------------------------------------
// ChspsDomNode::AppendPCDataL
// -----------------------------------------------------------------------------
//   
EXPORT_C void ChspsDomNode::AppendPCDataL( const TDesC8& aPCData )
    {
    if ( iPCData )
        {
        iPCData = iPCData->ReAllocL( iPCData->Length() + aPCData.Length() );
        iPCData->Des().Append( aPCData );
        }
    else
        {
        iPCData = aPCData.AllocL();
        }
    }


// -----------------------------------------------------------------------------
// ChspsDomNode::PCData
// -----------------------------------------------------------------------------
//  
EXPORT_C const TDesC8& ChspsDomNode::PCData()
    {
    if ( iPCData )
        {
        return *iPCData;
        }
    return KNullDesC8;
    }

// -----------------------------------------------------------------------------
// ChspsDomNode::SetPCDataL
// -----------------------------------------------------------------------------
//  
EXPORT_C void ChspsDomNode::SetPCDataL( const TDesC8& aPCData )
    {
    if ( iPCData )
        {
        delete iPCData;
        iPCData = NULL;
        }
    iPCData = aPCData.AllocL();
    }

// -----------------------------------------------------------------------------
// ChspsDomNode::ContentType
// -----------------------------------------------------------------------------
//   
EXPORT_C const TContentType& ChspsDomNode::ContentType()
    {
    return iContentType;
    }
    
// -----------------------------------------------------------------------------
// ChspsDomNode::SetContentType
// -----------------------------------------------------------------------------
//   
EXPORT_C void ChspsDomNode::SetContentType( const TContentType& aContentType )
    {
    iContentType = aContentType;
    }

// -----------------------------------------------------------------------------
// ChspsDomNode::StringPool
// -----------------------------------------------------------------------------
//        
EXPORT_C ChspsDomStringPool& ChspsDomNode::StringPool() const
    {
    return iStringPool;
    }

// -----------------------------------------------------------------------------
// ChspsDomNode::SetRefNode
// -----------------------------------------------------------------------------
//        
EXPORT_C void ChspsDomNode::SetRefNode( TBool aRefNode )
    {
    iRefNode = aRefNode;
    }
    
// -----------------------------------------------------------------------------
// ChspsDomNode::IsRefNode
// -----------------------------------------------------------------------------
//        
EXPORT_C TBool ChspsDomNode::IsRefNode() const
    {
    return iRefNode;
    }
// -----------------------------------------------------------------------------
// ChspsDomNode::Size
// -----------------------------------------------------------------------------
//
TInt ChspsDomNode::Size() const
    {
    TInt size( 0 );
    
    size += sizeof( TInt16 ); //iNameRef
    size += sizeof( TInt16 ); //iNSRef
    size += sizeof( TBool ); //iRefNode
    if ( iPCData )
        {
        size += sizeof( TInt8 );
        size += sizeof( TInt16 );
        size += iPCData->Size();
        size++;
        }
    else
        {
        size += sizeof( TInt8 );
        }
     
    size += sizeof( TInt32 );       //For nodeId
    size += iAttributeList->Size();
    
    size += iChildList->Size();    
    
    return size;    
    }    
// -----------------------------------------------------------------------------
// ChspsDomNode::ExternalizeL
// -----------------------------------------------------------------------------
//
void ChspsDomNode::ExternalizeL( RWriteStream& aStream ) const
    {    
    aStream.WriteInt16L( iNameRef );
    aStream.WriteInt16L( iNSRef );
    aStream.WriteInt8L( iRefNode );
    
    if ( iPCData )
        {
        aStream.WriteInt8L( ETrue );
        aStream.WriteInt16L( iPCData->Length() );
        aStream << *iPCData;
        }
    else
        {
        aStream.WriteInt8L( EFalse );
        }
        
	aStream.WriteInt32L( iNodeId );
    aStream << *iChildList;
    aStream << *iAttributeList;
   }

// -----------------------------------------------------------------------------
// ChspsDomNode::InternalizeL
// -----------------------------------------------------------------------------
//
void ChspsDomNode::InternalizeL( RReadStream& aStream )
    {
    iNameRef = aStream.ReadInt16L();
    iNSRef = aStream.ReadInt16L();
    iRefNode = aStream.ReadInt8L();
     
    TInt len( 0 );
    TBool exist( aStream.ReadInt8L() );
    if ( exist )
        {
        len = aStream.ReadInt16L();
        delete iPCData;
        iPCData = NULL;
        iPCData = HBufC8::NewL( aStream, len );
        }
    
    iNodeId = aStream.ReadInt32L();
   
    iChildList = ChspsDomList::NewL( aStream, iStringPool );
    TInt count( iChildList->Length() );
    for( TInt i=0; i<count; i++ )
        {
        ChspsDomNode* node = static_cast<ChspsDomNode*>( iChildList->Item( i ) );
        if ( node )
            {
            node->SetParent( this );
            }
        else
            {
            User::Leave( KErrArgument );
            }
        }
    
    iAttributeList = ChspsDomList::NewL( aStream, iStringPool );
    }

   

// -----------------------------------------------------------------------------
// ChspsDomNode::DescendantCount
// Recursive counting function
// -----------------------------------------------------------------------------
//   
EXPORT_C TInt ChspsDomNode::DescendantCount() const
    {
    TInt count( 1 ); //Node itself
    
    TInt length( iChildList->Length() );
    for ( TInt i=0; i<length; i++ )
       {
       count += static_cast<ChspsDomNode*>( iChildList->Item( i ) )->DescendantCount(); 
       }
     
    return count;
    }                  
    
// -----------------------------------------------------------------------------
// ChspsDomNode::AttributeValue
// Returns value of "name" attribute
// -----------------------------------------------------------------------------
//   
EXPORT_C const TDesC8& ChspsDomNode::AttributeValue(const TDesC8& aAttribute) const
    {
    ChspsDomAttribute* attribute = 
        static_cast<ChspsDomAttribute*>(iAttributeList->FindByName(aAttribute));
    if(attribute)
        {
        return attribute->Value();
        }
    else
        {
        return KNullDesC8;
        }
    }
    
// -----------------------------------------------------------------------------
// ChspsDomNode::SetLayoutNode
// Sets pointer to associated layout node
// -----------------------------------------------------------------------------
//   
EXPORT_C void ChspsDomNode::SetLayoutNode(ChspsNode* aNode)
    {
    iLayoutNode = aNode;
    }
        
// -----------------------------------------------------------------------------
// ChspsDomNode::LayoutNode
// Gets pointer to associated layout node
// -----------------------------------------------------------------------------
//   
EXPORT_C ChspsNode* ChspsDomNode::LayoutNode()
    {
    return iLayoutNode;
    }
//  End of File  
