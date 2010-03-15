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
* Description:  Represents the entire xmluiml specific xml and css data. 
*
*/



// INCLUDE FILES
#include    "hspsdomdocument.h"
#include    "hspsdomnode.h"
#include    "hspsdomlist.h"
#include    "hspsdomstringpool.h"
#include    <s32mem.h>

    
// ============================ MEMBER FUNCTIONS ===============================    
// -----------------------------------------------------------------------------
// ChspsDomDocument::CloneL()
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomDocument* ChspsDomDocument::CloneL()
    {
    ChspsDomDocument* clone = new (ELeave) ChspsDomDocument;
    CleanupStack::PushL( clone );
    
    clone->iDomStringPool = iDomStringPool->CloneL();
    if ( iRootNode )
        {
        clone->iRootNode = iRootNode->CloneL( *clone->iDomStringPool, ETrue );
        }
    
    CleanupStack::Pop( clone );
    return clone;
    }
    
// -----------------------------------------------------------------------------
// ChspsDomDocument::CreateElementNSL
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomNode* ChspsDomDocument::CreateElementNSL( 
    const TDesC8& aName, 
    const TDesC8& aNamespace )
    {
    return ChspsDomNode::NewL( aName, aNamespace, *iDomStringPool );
    }
// -----------------------------------------------------------------------------
// ChspsDomDocument::ChspsDomDocument
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsDomDocument::ChspsDomDocument()
    {
    }

// -----------------------------------------------------------------------------
// ChspsDomDocument::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsDomDocument::ConstructL()
    {
    iDomStringPool = ChspsDomStringPool::NewL();
    }

// -----------------------------------------------------------------------------
// ChspsDomDocument::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomDocument* ChspsDomDocument::NewL()
    {
    ChspsDomDocument* self = new( ELeave ) ChspsDomDocument;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
    }
// -----------------------------------------------------------------------------
// ChspsDomDocument::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomDocument* ChspsDomDocument::NewL( 
    RReadStream& aStream )
    {
    ChspsDomDocument* self = new( ELeave ) ChspsDomDocument;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    aStream >> *self;
    CleanupStack::Pop(self);

    return self;
    }
// -----------------------------------------------------------------------------
// ChspsDomDocument::NewL
// Constructs ChspsDomDocument from streamed HBufC8.
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomDocument* ChspsDomDocument::NewL( 
    const HBufC8* aBufStream )
    {
    RDesReadStream readStream( *aBufStream );
    CleanupClosePushL( readStream );
    ChspsDomDocument* self = ChspsDomDocument::NewL( readStream );
    CleanupStack::PopAndDestroy( &readStream );
    return self;   
    }

// Destructor
EXPORT_C ChspsDomDocument::~ChspsDomDocument()
    {
    if ( iRootNode )
        {
        delete iRootNode;
        }
    
    if ( iDomStringPool )
        {
        delete iDomStringPool;
        }
    }

// -----------------------------------------------------------------------------
// ChspsDomDocument::DomNodeCount
// -----------------------------------------------------------------------------
//
EXPORT_C TInt ChspsDomDocument::DomNodeCount() const
    {
    TInt count( 0 );
    if ( iRootNode )
        {
        count = iRootNode->DescendantCount();
        }
    return count;
    }

// -----------------------------------------------------------------------------
// ChspsDomDocument::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsDomDocument::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream << *iDomStringPool;
        
    if ( iRootNode )
        {
        aStream.WriteInt8L( ETrue );    //Root node exist
        aStream << *iRootNode;
        }
    else
        {
        aStream.WriteInt8L( EFalse );
        }    
    }
    
// -----------------------------------------------------------------------------
// ChspsDomDocument::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsDomDocument::InternalizeL( RReadStream& aStream )
    {
    iDomStringPool->Reset();
    iDomStringPool->InternalizeL( aStream );    
    
    if ( iRootNode )
        {
        delete iRootNode;
        iRootNode = NULL;
        }
    
    TBool rootNodeExist( aStream.ReadInt8L() );
    if ( rootNodeExist )
        {    
        iRootNode = ChspsDomNode::NewL( aStream, *iDomStringPool );
        }
    }          

// -----------------------------------------------------------------------------
// ChspsDomDocument::SetRootNode
// -----------------------------------------------------------------------------
//
EXPORT_C void ChspsDomDocument::SetRootNode( ChspsDomNode* aRootNode )
    {
    iRootNode = aRootNode;
    iRootNode->SetParent( NULL );
    }          

// -----------------------------------------------------------------------------
// ChspsDomDocument::RootNode
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomNode* ChspsDomDocument::RootNode() const
    {
    return iRootNode;
    }          

// -----------------------------------------------------------------------------
// ChspsDomDocument::LastNode
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomNode* ChspsDomDocument::LastNode() const
    {
    ChspsDomNode* last = NULL;
    if ( iRootNode )
        {
        last = iRootNode;
        ChspsDomNode* tmp = iRootNode;
        while( tmp )
            {
            last = tmp;
            tmp = static_cast<ChspsDomNode*>( last->ChildNodes().Last() );
            }
        }
    return last;
    }          

// -----------------------------------------------------------------------------
// ChspsDomDocument::Size
// -----------------------------------------------------------------------------
//
EXPORT_C TInt ChspsDomDocument::Size() const
    {
   	TInt size( 1 ); //Root node information takes one byte
    size += iDomStringPool->Size();
        
    if ( iRootNode )
        {
        
        size += iRootNode->Size();
        }
    
    return size;
    }

// -----------------------------------------------------------------------------
// ChspsDomDocument::MarshallL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* ChspsDomDocument::MarshallL()
    {
    TInt dataLength = Size();
    HBufC8* writeBuf = HBufC8::NewLC( dataLength );
    TPtr8 p( writeBuf->Des() );
    RDesWriteStream writeStream( p );     //stream over the buffer
    CleanupClosePushL( writeStream );
    writeStream << *this; //Stream object
    CleanupStack::PopAndDestroy( &writeStream );
    CleanupStack::Pop( writeBuf );
    return writeBuf;
    }

// -----------------------------------------------------------------------------
// ChspsDomDocument::StringPool
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomStringPool& ChspsDomDocument::StringPool() const
    {
    return *iDomStringPool;
    }

//  End of File  
