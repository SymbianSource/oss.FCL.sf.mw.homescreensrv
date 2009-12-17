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
* Description:  Class definitions for XCFW Tree
*
*/



#ifndef CXCFWTREE_H
#define CXCFWTREE_H

//  INCLUDES
#include <s32std.h>
//#include <gecoobjectbase.h>
#include "xcfwnodeinterface.h"

// FORWARD DECLARATIONS
class CGECOObjectBase;
class CXCFWNode;


// DATA TYPES
typedef RPointerArray<MXCFWNode> RNodeArray;
typedef RPointerArray<CXCFWNode> RInternalNodeArray;

// CLASS DECLARATION
/**
*  XCFW Tree interface
*  Provides access to tree functions
*
*  @lib XCFW.lib
*  @since Series 60 3.1
*/
class MXCFWTree
    {
    public:
    /**
    * Adds new node to content tree. Returns reference to added node. 
    * Three overloads are provided to allow use without parent when creating 
    * root, or use without insertbefore reference to allow adding nodes as 
    * last child of the given parent.
    * aData ownership is transferred at the beginning of the method!
    * @since Series60 3.1
    * @param aData The CXCFWObjectParser (derived) object to be added to tree
    * @param aParent Parent node for the added node
    * @param aInsertBefore Reference node - new node will be inserted before 
    *  this node in the siblings of given parent.
    * @return pointer to newly added node
    */
    virtual MXCFWNode* AddNodeL( CGECOObjectBase* aData, MXCFWNode* aParent, 
        MXCFWNode* aInsertBefore ) = 0;
    virtual MXCFWNode* AddNodeL( CGECOObjectBase* aData, 
        MXCFWNode* aParent ) = 0;
    virtual MXCFWNode* AddNodeL( CGECOObjectBase* aData ) = 0;

    /**
    * Returns child nodes of the given parentm from tree
    * @since Series60 3.1
    * @param aParent Parent node whose child nodes will be returned
    * @param aNodeList Reference to array where child nodes are added
    */
    virtual void GetChildNodesL( MXCFWNode* aParent, 
        RNodeArray& aNodeList ) = 0;
    
    /**
    * Returns nodes of certain type from tree
    * @since Series60 3.1
    * @param aType Node type identifier that corresponds to a XML tag
    * @param aNodeList Reference to array where found nodes are added
    * @param aParent Parent node where the search is started from
    * @param aRecursive Determines if search is done recursively 
    *   (finds also grandchildren)
    */
    virtual void GetNodesOfTypeL( const TDesC& aType, RNodeArray& aNodeList, 
        MXCFWNode* aParent, TBool aRecursive ) = 0;

    /**
    * Moves a node to another position in tree
    * @since Series60 3.1
    * @param aNodeToMove Node that is to be repositioned
    * @param aNewParent New parent for node to be repositioned
    * @param aInsertBefore New node will be inserted before this node 
    *  among the given parents children
    */ 
    virtual void MoveNodeL( MXCFWNode* aNodeToMove, MXCFWNode* aNewParent, 
        MXCFWNode* aInsertBefore ) = 0;
    
    /**
    * Returns all tree nodes in an array of MXCFWNode pointers
    * NOTE: node order does not necessarily 
    * reflect the current order in tree, as nodes may have been moved.
    * @since Series60 3.1
    * @return Array of MXCFWNode pointers
    */
    virtual RNodeArray& Nodes() = 0;
    
    /**
    * Removes a node from the tree
    * @since Series60 3.1
    * @param aNodeToRemove Node to be removed from tree
    */
    virtual void RemoveNodeL( MXCFWNode* aNodeToRemove ) = 0;

    
    /**
    * Returns root node of the tree
    * @since Series60 3.1
    * @return Tree root
    */
    virtual MXCFWNode* Root() = 0;

    /**
    * Sets tree lock status
    * If tree is locked, nodes cannot be added/moved/removed
    * @param aLockStatus sets the lock status
    */
    virtual void SetLocked( TBool aLockStatus ) = 0;

    /**
    * Returns tree lock status
    * @return ETrue, if tree structure is locked
    */
    virtual TBool IsLocked() = 0;

    /**
    * Returns name of localization DTD file (by default without path)
    * @return name of localization DTD file, if set
    */
    virtual const TDesC& DTDName() = 0;

    /**
    * Sets localization dtd name for this content tree
    * This information is used when saving the tree if 
    * no DTD is specified in SaveL command
    * @param aName DTD file name (by default without path)
    */
    virtual void SetDTDNameL( const TDesC& aName ) = 0;

    };


/**
*  CXCFWTree implements MXCFWTree interface. It owns the actual tree nodes.
*
*  @lib XCFW.lib
*  @since Series 60 3.1
*/
class CXCFWTree : public CBase, public MXCFWTree
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        IMPORT_C static CXCFWTree* NewL();
        
        /**
        * Destructor.
        */
        IMPORT_C virtual ~CXCFWTree();

    public: // Functions from base classes

        /**
        * From MXCFWTree. Adds a node to tree.
        * aData ownership is transferred at the beginning of the method!
        */
        IMPORT_C MXCFWNode* AddNodeL( CGECOObjectBase* aData, 
            MXCFWNode* aParent, MXCFWNode* aInsertBefore );

        /**
        * From MXCFWTree. Adds a node to tree.
        * aData ownership is transferred at the beginning of the method!
        */
        IMPORT_C MXCFWNode* AddNodeL( CGECOObjectBase* aData, 
            MXCFWNode* aParent );

        /**
        * From MXCFWTree. Adds a node to tree.
        * aData ownership is transferred at the beginning of the method!
        */
        IMPORT_C MXCFWNode* AddNodeL( CGECOObjectBase* aData );

        /**
        * From MXCFWTree. Gets list of child nodes for a parent
        */
        IMPORT_C void GetChildNodesL( MXCFWNode* aParent, 
            RNodeArray& aNodeList );

        /**
        * From MXCFWTree. Gets list of certain type nodes.
        */
        IMPORT_C void GetNodesOfTypeL( const TDesC& aType, 
            RNodeArray& aNodeList, 
            MXCFWNode* aParent, 
            TBool aRecursive );

        /**
        * From MXCFWTree. Moves a node in tree to another position.
        */
        IMPORT_C void MoveNodeL( MXCFWNode* aNodeToMove, 
            MXCFWNode* aNewParent, 
            MXCFWNode* aInsertBefore );
        
        /**
        * From MXCFWTree. Returns tree nodes in an MXCFWNode pointer array
        */
        IMPORT_C RNodeArray& Nodes();
        
        /**
        * From MXCFWTree. Removes a node from tree.
        */
        IMPORT_C void RemoveNodeL( MXCFWNode* aNodeToRemove );

        /**
        * From MXCFWTree. Returns tree root node.
        */
        IMPORT_C MXCFWNode* Root();
        
        /**
        * From MXCFWTree. Sets tree structure lock status
        */        
        IMPORT_C void SetLocked( TBool aLockStatus );

        /**
        * From MXCFWTree. Returns lock status
        */
        IMPORT_C TBool IsLocked();
        
        /**
        * From MXCFWTree. Returns default DTD name for this tree's content
        */
        const TDesC& DTDName();

        /**
        * From MXCFWTree. Sets DTD name for this tree content
        */
        void SetDTDNameL( const TDesC& aName );
        
    private:

        /**
        * C++ default constructor.
        */
        CXCFWTree();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // new functions 
    
        /**
        * Removes a tree node branch starting from given node recursively
        * @since Series 60 3.1
        * @param Node to remove (whole branch will be removed)
        */
        void RemoveNodeRecursiveL( MXCFWNode* aNode) ;

        /**
        * Removes a tree node branch starting from given node recursively
        * @since Series 60 3.1
        * @param Node to remove (whole branch will be removed)
        */
        void RemoveFromNodeList( MXCFWNode* aNode );

    private:    // Data
        //tree node array for client ( MXCFWNode pointer array )
        RInternalNodeArray      iNodeList;  
        //root node (owned in the node array)
        CXCFWNode*              iRoot;
        //node list for client (array of MXCFWNodes). Populated at request.
        RNodeArray              iNodeListForClient;
        //marks client node list dirty =>needs to be updated on next request.
        TBool                   iClientListDirty;
        //Lock status. If locked, no changes to tree structure are allowed
        TBool                   iLocked;
        //DTD file name related to this content (engine needs to know this)
        HBufC*                  iDTDName;

    private:
        friend class CXCFWEngine;

    

    };

#endif      // CXCFWTREE_H   
            
// End of File
