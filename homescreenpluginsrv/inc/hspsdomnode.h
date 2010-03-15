/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef hsps_DOM_NODE_H
#define hsps_DOM_NODE_H

//  INCLUDES
#include <e32base.h>
#include <s32strm.h>
#include "mhspsdomlistitem.h"

// FORWARD DECLARATIONS
enum TContentType
    {
    EEmpty = 0,
    EAny,
    EPCData
    };

class ChspsDomList;
class ChspsDomStringPool;
class ChspsNode;

// CLASS DECLARATION

/**
 * Class represents a single node in the document tree.
 *
 * @lib hspsdomdocument.lib
 * @since S60 5.0
 * @ingroup group_hspsdom
 */
class ChspsDomNode : public CBase, public MhspsDomListItem
    {

    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * 
        * @since S60 5.0
        * @param aName          Name of node.
        * @param aNS            Namespace.
        * @param aStringPool    Attached string pool.
        */
        static ChspsDomNode* NewL( 
            const TDesC8& aName, 
            const TDesC8& aNS,
            ChspsDomStringPool& aStringPool );

        /**
        * Two-phased constructor.
        * 
        * @since S60 5.0
        * @param aName          Name of node (string pool index).
        * @param aNS            Namespace (string pool index).
        * @param aStringPool    Attached string pool.
        */
        static ChspsDomNode* NewL( 
            const TInt aName, 
            const TInt aNS,
            ChspsDomStringPool& aStringPool );        
        
        /**
        * Two-phased stream constructor.
        * 
        * @since S60 5.0
        * @param aStream        Source stream.
        * @param aStringPool    Attached string pool.
        */
        static ChspsDomNode* NewL( RReadStream& aStream, ChspsDomStringPool& aStringPool );
        
        /**
        * Destructor.
        */
        virtual ~ChspsDomNode();
      
    public: // New functions
        /**
        * Makes a clone from this node and it's child nodes.
        * 
        * @since S60 5.0
        * @param aStringPool    A new string pool.
        * @param aFastClone     If ETrue, then fast mode cloning is used. 
        * 
        * @return Pointer to a clone node. Caller has the ownership.
        */
        IMPORT_C ChspsDomNode* CloneL( ChspsDomStringPool& aStringPool,
                                       const TBool aFastClone = EFalse );
        
        /**
        * Makes a clone only from this node.
        * 
        * @since S60 5.0
        * @param aStringPool A new string pool.
        * @return Pointer to a clone node. Caller has the ownership.
        */
        IMPORT_C ChspsDomNode* CloneWithoutKidsL( ChspsDomStringPool& aStringPool );
        
        /**
        * Contructs a ref node from this node and its child's.
        * 
        * @since S60 5.0
        * @return Pointer to a ref node. Caller has the ownership.
        */
        IMPORT_C ChspsDomNode* CreateRefNodeL();
         
        /**
        * Get the namespace of this node.
        *  
        * @since S60 5.0
        * @return Pointer to the namespace buffer.
        */
        IMPORT_C const TDesC8& Namespace();
        
        /**
        * Node's attributes exist in a list.
        * 
        * @since S60 5.0
        * @return Reference to a attribute list. 
        */
        IMPORT_C ChspsDomList& AttributeList()const;
                
        /**
        * Node need to know it's parent.
        * 
        * @since S60 5.0
        * @param aParent Pointer to this node parent.
        */
        IMPORT_C void SetParent( ChspsDomNode* aParent );
        
        /**
        * Node's parent is enquered e.g to traverse the document.
        * 
        * @since S60 5.0
        * @return Pointer to a parent node.
        */
        IMPORT_C ChspsDomNode* Parent() const;
        
        /**
        * Node id is for a uiengine use.
        * 
        * @since S60 5.0
        * @param aNodeId Unique identifier.
        */
        IMPORT_C void SetNodeId( const TInt aNodeId );

        /**
        * Get the node unique identifier.
        * 
        * @since S60 5.0
        * @return Node unique identifier.
        */
        IMPORT_C TInt NodeId() const;

        /**
        * Get the node unique identifier.
        * 
        * @since S60 5.0
        * @return Node unique identifier.
        */
        IMPORT_C TInt ItemIndex( const MhspsDomListItem& aItem )const;

        /**
        * Access to child nodes.
        * 
        * @since S60 5.0
        * @return Pointer to a list of child nodes.
        */
        IMPORT_C ChspsDomList& ChildNodes();
        
        /**
        * Child nodes of this node are needed to be able to 
        * traverse the document.
        * 
        * @since S60 5.0
        * @param aNode Node owns it's childs.
        */
        IMPORT_C void AddChildL( ChspsDomNode* aNode );

         /**
        * Child nodes of this node are needed to be able to 
        * traverse the document.
        * 
        * @since S60 5.0
        * @param aNode Node owns it's childs.
        * @param aIndex Index in list where the node is added.
        */
        IMPORT_C void AddChildL( ChspsDomNode* aNode, TInt aIndex );
       
        /**
        * Removes node from the child list and frees memory allocated  
        * for a child node and it descendants.
        * 
        * @since S60 5.0
        * @param aNode Node to be released.
        */
        IMPORT_C void DeleteChild( ChspsDomNode* aNode );
        
        /**
        * Replaces a child node with the other node. Memory allocated for 
        * the node is freed.
        *    
        * @since S60 5.0
        * @param aNode Node to be replaced.
        * @param aNewNode New node.
        */
        IMPORT_C void ReplaceChildL( ChspsDomNode* aNode, ChspsDomNode* aNewNode );
        
        /**
        * Get the character data.
        * 
        * @since S60 5.0
        * @return Reference to a buffer.
        */
        IMPORT_C const TDesC8& PCData();
        
        /**
        * Append PCData into node. Thus it is possible to have
        * "<root>a <first/> c <second/> e </root>" PCData.
        * 
        * @since S60 5.0
        * @param aPCData UTF-8 encoded parsed character data.
        */
        IMPORT_C void AppendPCDataL( const TDesC8& aPCData );
        
        /**
        * Set parsed character data.
        * 
        * @since S60 5.0
        * @param aPCData UTF-8 encoded parsed character data.
        */
        IMPORT_C void SetPCDataL( const TDesC8& aPCData );
        
        /**
        * Set content type.
        * 
        * @since S60 5.0
        * @return content type.
        */
        IMPORT_C const TContentType& ContentType();
        
        /**
        * Get content type.
        * 
        * @since S60 5.0
        * @param aContentType content type.
        */
        IMPORT_C void SetContentType( const TContentType& aContentType );
        
        /**
        * Recursive function to count all child nodes belonging
        * to this node. 
        * 
        * @since S60 5.0
        * @return This node and it's all childs count.
        */
        IMPORT_C TInt DescendantCount() const;
        
        /**
        * Get reference to the dom's string pool.
        * 
        * @since S60 5.0
        * @return Reference to string pool.
        */
        IMPORT_C ChspsDomStringPool& StringPool() const;
        
        /**
        * Mark this node as a reference node.
        * 
        * @since S60 5.0
        * @param aRefNode ETrue if node is a reference node.
        */
        IMPORT_C void SetRefNode( TBool aRefNode=ETrue );
        
        /**
        * Check if this node is referring to some global node.
        * 
        * @since S60 5.0
        * @return ETrue if this is a reference node.
        */
        IMPORT_C TBool IsRefNode() const;
        
        /**
        * Deletes attributes when they are not needed anymore.
        * I.e. UIEngine has copied them into property list.
        * 
        * @since S60 5.0
        */
        IMPORT_C void DeleteAttributeList();
        
        /**
        * Returns value of given attribute.
        * 
        * @param aAttribute Name of the attribute.
        * @since S60 5.0
        * @return Value of an attribute.
        */
        IMPORT_C const TDesC8& AttributeValue(const TDesC8& aAttribute) const;
   
   public: //From MhspsDomListItem
                
        /**
        * Documented in ChspsDomListItem::Size
        */
        TInt Size() const;
        
        /**
        * Documented in ChspsDomListItem::ExternalizeL
        */
        void ExternalizeL( RWriteStream& aStream ) const;
        
        /**
        * Documented in ChspsDomListItem::InternalizeL
        */
        void InternalizeL( RReadStream& aStream );
        
        
        /**
        * Documented in ChspsDomListItem::Name
        */
        IMPORT_C const TDesC8& Name();       
            
    public:          
    
        /**
        * Sets pointer to associated layout node.
        * 
        * @since S60 5.0
        * @param aNode Pointer to associated layout node.
        */
        IMPORT_C void SetLayoutNode(ChspsNode* aNode);
        
        /**
        * Returns pointer to associated layout node.
        * 
        * @since S60 5.0
        * @return Pointer to associated layout node.
        */
        IMPORT_C ChspsNode* LayoutNode();
    private:
        /**
        * C++ default constructor.
        */
        ChspsDomNode( ChspsDomStringPool& aStringPool );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC8& aName, const TDesC8& aNS );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TInt aName, const TInt aNS );        

        /**
        * Construct members
        */
        void Construct2L();        
        
    private:    // Data
        
        //iNameRef refers to a name string in a string pool 
        TInt                iNameRef;
        
        //iNSRef refers to a namespace string in a string pool 
        TInt                iNSRef;
        
        //iStringPool is used to resolve strings based on references
        ChspsDomStringPool&   iStringPool;
        
        //Type of the elements content
        TContentType        iContentType;
        
        //Parsed character data
        HBufC8*             iPCData;
        
        //Node's parent pointer 
        ChspsDomNode*         iParentNode;
        
        //Node unique identifier
        TInt                iNodeId;
        
        //Node's childs are collected into a list. Childs are owned by node
        ChspsDomList*         iChildList;
        
        //Node is the owner of the attributes
        ChspsDomList*         iAttributeList;
                
        //Flag is set if this node is reference node
        TBool               iRefNode;
         
        // Not owned
        ChspsNode*            iLayoutNode;
    };

#endif      // hsps_DOM_NODE_H 
            
// End of File
