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
* Description:  Implementation of XCFW Content tree
*
*/



// INCLUDE FILES
#include    "xcfwtree.h"
#include    "xcfwnode.h"
#include    "xcfwpanic.h"

// MACROS
//Helper macro to cast MXCFWNode pointers to CXCFWNode
#define WRAPNODE( aNode ) static_cast<CXCFWNode*>( aNode )

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CXCFWTree::CXCFWTree
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CXCFWTree::CXCFWTree()
    {
    }

// -----------------------------------------------------------------------------
// CXCFWTree::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CXCFWTree::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CXCFWTree::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CXCFWTree* CXCFWTree::NewL()
    {
    CXCFWTree* self = new( ELeave ) CXCFWTree;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
EXPORT_C CXCFWTree::~CXCFWTree()
    {
    //delete dtd name buffer
    delete iDTDName;
    
    //delete node array    
    iNodeList.ResetAndDestroy();
    iNodeList.Close();
    
    //reset node array for client
    iNodeListForClient.Reset();
    iNodeListForClient.Close();
    
    //Null root pointer (owned in the iNodeList array)
    iRoot = NULL;
    }


// -----------------------------------------------------------------------------
// CXCFWTree::AddNodeL
// this overload is for adding node under certain parent before certain child
// -----------------------------------------------------------------------------
//
EXPORT_C MXCFWNode* CXCFWTree::AddNodeL(
    CGECOObjectBase* aData, 
    MXCFWNode* aParent, 
    MXCFWNode* aInsertBefore )
    {
    CleanupStack::PushL( aData );
    __ASSERT_LEAVE( aParent && aData && aInsertBefore &&
        aData->TypeIdentifier() != iRoot->Data()->TypeIdentifier() &&
        aInsertBefore->Parent() == aParent, KErrArgument );

    __ASSERT_LEAVE( !iLocked, KErrAccessDenied );
    
    //Create new node and add to arrays.
    CXCFWNode* nodeToAdd = CXCFWNode::NewL( aData );
    CleanupStack::Pop( aData );
    CleanupStack::PushL( nodeToAdd );
    User::LeaveIfError( iNodeList.Append( nodeToAdd ) );
    CleanupStack::Pop( nodeToAdd );

    //update node references
    nodeToAdd->SetParent( aParent );
    nodeToAdd->SetNextSibling( aInsertBefore );
    nodeToAdd->SetPrevSibling( aInsertBefore->PrevSibling() );
        
    //if "insert before" -node has previous sibling, update that
    if ( aInsertBefore->PrevSibling() )
        {
        WRAPNODE( aInsertBefore->PrevSibling() )->SetNextSibling( nodeToAdd );
        }

    //set previous sibling
    WRAPNODE( aInsertBefore )->SetPrevSibling( nodeToAdd );            

    if( aParent->FirstChild() == aInsertBefore )
        {
        WRAPNODE( aParent )->SetFirstChild( nodeToAdd );            
        }

    //Set client list dirty => will be updated on next Nodes() request.
    iClientListDirty = ETrue;
    
    return nodeToAdd;

    }

// -----------------------------------------------------------------------------
// CXCFWTree::AddNodeL
// this overload is for adding node under certain parent as last child
// -----------------------------------------------------------------------------
//
EXPORT_C MXCFWNode* CXCFWTree::AddNodeL(
    CGECOObjectBase* aData, 
    MXCFWNode* aParent )
    {
    CleanupStack::PushL( aData );
    //Legality checks
    __ASSERT_LEAVE( aParent && aData && 
         aData->TypeIdentifier() != iRoot->Data()->TypeIdentifier(),
         KErrArgument );

    __ASSERT_LEAVE( !iLocked, KErrAccessDenied );


    //Create new node and add to arrays
    CXCFWNode* nodeToAdd = CXCFWNode::NewL( aData );
    CleanupStack::Pop( aData );
    CleanupStack::PushL( nodeToAdd );
    User::LeaveIfError( iNodeList.Append( nodeToAdd ) );
    CleanupStack::Pop( nodeToAdd );

    //set parent for new node
    nodeToAdd->SetParent( aParent );

    //if parent has a last child
    if( aParent->LastChild() )
        {
        nodeToAdd->SetPrevSibling( aParent->LastChild() );
        WRAPNODE( aParent->LastChild() )->SetNextSibling( nodeToAdd );
        }
    else
        {
        WRAPNODE( aParent )->SetFirstChild( nodeToAdd );
        }

    //update last child of the parent        
    WRAPNODE( aParent )->SetLastChild( nodeToAdd );        

    //Set client list dirty => will be updated on next Nodes() request.
    iClientListDirty = ETrue;

    return nodeToAdd;        
    }

// -----------------------------------------------------------------------------
// CXCFWTree::AddNodeL
// this overload is for adding root node only
// -----------------------------------------------------------------------------
//
EXPORT_C MXCFWNode* CXCFWTree::AddNodeL(
    CGECOObjectBase* aData )
    {
    CleanupStack::PushL( aData );
    //if we already have a root, this is illegal
    __ASSERT_LEAVE( iRoot==NULL, KErrAlreadyExists );

    __ASSERT_LEAVE( !iLocked, KErrAccessDenied );

    //Create root node and add to arrays
    CXCFWNode* nodeToAdd = CXCFWNode::NewL( aData );
    CleanupStack::Pop( aData );
    CleanupStack::PushL( nodeToAdd );
    User::LeaveIfError( iNodeList.Append( nodeToAdd ) );
    CleanupStack::Pop( nodeToAdd );

    //Set internal root member.
    iRoot = nodeToAdd;
    nodeToAdd = NULL;

    //Set client list dirty => will be updated on next Nodes() request.
    iClientListDirty = ETrue;

    return iRoot;
    }

// -----------------------------------------------------------------------------
// CXCFWTree::GetChildNodesL
// Put child nodes of the given parent to the given array
// -----------------------------------------------------------------------------
//
EXPORT_C void CXCFWTree::GetChildNodesL(
    MXCFWNode* aParent, 
    RNodeArray& aNodeList)
    {
    
    __ASSERT_LEAVE( aParent != NULL, KErrArgument );
    
    MXCFWNode* pointer = aParent->FirstChild();
    while ( pointer )
        {
        User::LeaveIfError( aNodeList.Append( pointer ) );
        pointer = pointer->NextSibling();
        }

    }

// -----------------------------------------------------------------------------
// CXCFWTree::GetNodesOfTypeL
// Put nodes that have dataobject of given type to given array.
// In case of recursion, pre-order recursion is used
// -----------------------------------------------------------------------------
//
EXPORT_C void CXCFWTree::GetNodesOfTypeL( 
    const TDesC& aType, 
    RNodeArray& aNodeList, 
    MXCFWNode* aParent, 
    TBool aRecursive )
    {

    __ASSERT_LEAVE( aParent != NULL, KErrArgument );

    MXCFWNode* pointer = aParent->FirstChild();
    
    while ( pointer )
        {
        if ( pointer->Data()->TypeIdentifier().Compare( aType ) == 0 )
            {
            User::LeaveIfError( aNodeList.Append( pointer ) );                
            }
        if ( aRecursive )
            {
            GetNodesOfTypeL( aType, aNodeList, pointer, aRecursive );                
            }
        pointer = pointer->NextSibling();
        }

    }

// -----------------------------------------------------------------------------
// CXCFWTree::MoveNodeL
// Moves a node in tree and updates the relative nodes in both original and
// new location.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXCFWTree::MoveNodeL(
    MXCFWNode* aNodeToMove, 
    MXCFWNode* aNewParent, 
    MXCFWNode* aInsertBefore )
    {

    // assure parameter legality
    __ASSERT_LEAVE( aNodeToMove && aNewParent && 
        aNodeToMove != iRoot && aInsertBefore != iRoot &&
        aNodeToMove != aNewParent,
        KErrArgument );            

    __ASSERT_LEAVE( !iLocked, KErrAccessDenied );


    //if "insert before" node is child node of the given parent, move is illegal
    if ( aInsertBefore && aInsertBefore->Parent() != aNewParent )
        {
        User::Leave( KErrArgument );            
        }
    
    MXCFWNode* check = aNewParent->Parent();
    while ( check )
        {
        //if user tries to move a node under itself (it is among the parents 
        // of the given new parent) we'll leave
        if ( check == aNodeToMove )
            {
            User::Leave( KErrArgument );
            }
        check = check->Parent();
        }
    check = NULL;
    
    //first update old parent if necessary
    if ( aNodeToMove->Parent()->FirstChild() == aNodeToMove )
        {
        WRAPNODE( aNodeToMove->Parent() )->SetFirstChild( 
                                aNodeToMove->NextSibling() );            
        }
    if ( aNodeToMove->Parent()->LastChild() == aNodeToMove )
        {
        WRAPNODE( aNodeToMove->Parent() )->SetLastChild(
                                   aNodeToMove->PrevSibling() );            
        }
    
    //update old siblings
    if ( aNodeToMove->PrevSibling() )
        {
        WRAPNODE( aNodeToMove->PrevSibling())->SetNextSibling(
                                aNodeToMove->NextSibling() );            
        }
    
    if ( aNodeToMove->NextSibling() )
        {
        WRAPNODE( aNodeToMove->NextSibling() )->SetPrevSibling( 
                                aNodeToMove->PrevSibling() );            
        }
        
    //set new parent:
    WRAPNODE( aNodeToMove )->SetParent( aNewParent );
    
    //update new siblings
    if ( aInsertBefore )
        {

        //if "insert before" -node has a sibling before it, we update that
        if ( aInsertBefore->PrevSibling() )
            {
            WRAPNODE( aInsertBefore->PrevSibling() )->SetNextSibling(
                                aNodeToMove );                
            }
        else 
        //no previous sibling => insertbefore is the first child of new parent
            {
            WRAPNODE( aNewParent )->SetFirstChild( aNodeToMove );                
            }
        WRAPNODE( aNodeToMove )->SetPrevSibling( aInsertBefore->PrevSibling() );
        WRAPNODE( aNodeToMove )->SetNextSibling( aInsertBefore );
        WRAPNODE( aInsertBefore )->SetPrevSibling( aNodeToMove );
        }
    else
        {
        //if new parent had children
        if ( aNewParent->LastChild() )
            {
            WRAPNODE( aNewParent->LastChild() )->SetNextSibling( aNodeToMove );
            WRAPNODE( aNodeToMove )->SetPrevSibling( aNewParent->LastChild() );
            WRAPNODE( aNewParent )->SetLastChild( aNodeToMove );
            WRAPNODE( aNodeToMove )->SetNextSibling(NULL);
            }
        else //no children before
            {
            WRAPNODE( aNewParent )->SetFirstChild( aNodeToMove );
            WRAPNODE( aNewParent )->SetLastChild( aNodeToMove );
            WRAPNODE( aNodeToMove )->SetPrevSibling( NULL );
            WRAPNODE( aNodeToMove )->SetNextSibling( NULL );            
            }
        }

    //Set client list dirty => will be updated on next Nodes() request.
    iClientListDirty = ETrue;

    }
        
// -----------------------------------------------------------------------------
// CXCFWTree::Nodes
// return tree nodes as array of MXCFWNode pointers
// -----------------------------------------------------------------------------
//
EXPORT_C RNodeArray& CXCFWTree::Nodes()
    {

    //see if we should update the MXCFWNode array to reflect the new
    if ( iClientListDirty )
        {

        if ( iNodeListForClient.Count() > 0 )
            {
            iNodeListForClient.Reset();
            }

        TInt count = iNodeList.Count();
        for ( TInt i = 0; i < count; i++ )
            {
            iNodeListForClient.Append( (MXCFWNode*)iNodeList[i] );           
            }
        iClientListDirty = EFalse;
        }

    return iNodeListForClient;
    }

        
// -----------------------------------------------------------------------------
// CXCFWTree::RemoveNodeL
// Removes a node from tree by updating all relative nodes and removing from
// arrays.
// -----------------------------------------------------------------------------
//
EXPORT_C void CXCFWTree::RemoveNodeL(
    MXCFWNode* aNodeToRemove )
    {

    __ASSERT_LEAVE( aNodeToRemove != NULL, KErrArgument );

    __ASSERT_LEAVE( !iLocked, KErrAccessDenied );
    
    if ( iRoot != aNodeToRemove )
        {
        //update related nodes before removal, unless removing root node.
        if ( aNodeToRemove->PrevSibling() )
            {
            WRAPNODE( aNodeToRemove->PrevSibling() )->SetNextSibling(
                                    aNodeToRemove->NextSibling() );            
            }
        
        if ( aNodeToRemove->NextSibling() )
            {
            WRAPNODE( aNodeToRemove->NextSibling() )->SetPrevSibling( 
                                    aNodeToRemove->PrevSibling() );    
            }

        if ( aNodeToRemove->Parent()->FirstChild() == aNodeToRemove )
            {
            WRAPNODE( aNodeToRemove->Parent() )->SetFirstChild( 
                                    aNodeToRemove->NextSibling() );            
            }
        
        if ( aNodeToRemove->Parent()->LastChild() == aNodeToRemove )
            {
            WRAPNODE( aNodeToRemove->Parent() )->SetLastChild( 
                                    aNodeToRemove->PrevSibling() );            
            }
        }
    else
        {
        //root was removed.
        iRoot = NULL;            
        }

    //Set client list dirty => will be updated on next Nodes() request.
    iClientListDirty = ETrue;
    
    RemoveNodeRecursiveL( aNodeToRemove );

    }


// -----------------------------------------------------------------------------
// CXCFWTree::RemoveFromArray
// Removes node pointer from CXCFWNode and MXCFWNode pointer arrays. 
// Deletes from CXCFWNode array as well.
// -----------------------------------------------------------------------------
//
void CXCFWTree::RemoveFromNodeList(
    MXCFWNode* aNode )
    {

    //remove node from the pointer array.
    TInt i = iNodeList.Count() - 1 ;
    for ( ;i>=0 && aNode != iNodeList[i] ; i-- ){}
    if ( i>=0 )
        {
        //delete object
        delete iNodeList[i];
        //remove from pointer arrays.
        iNodeList.Remove(i);
        }

    }

// -----------------------------------------------------------------------------
// CXCFWTree::RemoveNodeRecursiveL
// Post-order recursion is used to make sure that no orhpan nodes are left
// floating.
// -----------------------------------------------------------------------------
//
void CXCFWTree::RemoveNodeRecursiveL(
    MXCFWNode* aNode )
    {

    __ASSERT_LEAVE ( aNode != NULL, KErrArgument );

    MXCFWNode* pointer = aNode->FirstChild();
    
    while ( pointer )
        {
        MXCFWNode* temp = pointer->NextSibling();
        RemoveNodeRecursiveL( pointer );
        pointer = temp;            
        }

    RemoveFromNodeList( aNode );
    
    }


// -----------------------------------------------------------------------------
// CXCFWTree::Root
// Returns MXCFWNode pointer to root node.
// -----------------------------------------------------------------------------
//
EXPORT_C MXCFWNode* CXCFWTree::Root()
    {
    return iRoot;
    }

EXPORT_C void CXCFWTree::SetLocked( TBool aLockStatus )
    {
    iLocked = aLockStatus;
    }

EXPORT_C TBool CXCFWTree::IsLocked()
    {
    return iLocked;
    }

const TDesC& CXCFWTree::DTDName()
    {
    if ( !iDTDName )
        {
        return KNullDesC;
        }
    else
        {
        return *iDTDName;        
        }
    }

void CXCFWTree::SetDTDNameL( const TDesC& aName )
    {
    delete iDTDName;
    iDTDName = NULL;
    iDTDName = aName.AllocL();        
    }

//  End of File  
