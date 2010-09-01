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
* Description:  Depth iterator walks the dom tree using depth first principle.
*
*/



// INCLUDE FILES
#include    "hspsdomdepthiterator.h"
#include    "hspsdomlist.h"
#include    "hspsdomnode.h"

// ============================ LOCAL FUNCTIONS ===============================
 

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ChspsDomDepthIterator::ChspsDomDepthIterator
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
ChspsDomDepthIterator::ChspsDomDepthIterator( ChspsDomNode& aRootNode  ):
    iFirst(&aRootNode), iCurrent(&aRootNode)
    {
    }

// -----------------------------------------------------------------------------
// ChspsDomDepthIterator::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void ChspsDomDepthIterator::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// ChspsDomDepthIterator::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomDepthIterator* ChspsDomDepthIterator::NewL( ChspsDomNode& aRootNode )
    {
    ChspsDomDepthIterator* self = new( ELeave ) ChspsDomDepthIterator( aRootNode );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// Destructor
EXPORT_C ChspsDomDepthIterator::~ChspsDomDepthIterator()
    {
    iDepthLevel.Close();
    }

// -----------------------------------------------------------------------------
// ChspsDomDepthIterator::First
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomNode* ChspsDomDepthIterator::First()
    {
    return iFirst;
    }
    
// -----------------------------------------------------------------------------
// ChspsDomDepthIterator::Next
// -----------------------------------------------------------------------------
//
EXPORT_C ChspsDomNode* ChspsDomDepthIterator::NextL()
    {
    ChspsDomNode* sibling = NULL; 
    ChspsDomNode* firstChild = NULL;     
    
    //Initialise iCurrent if the whole tree has been walked through 
    if (!iCurrent) 
        {
        iCurrent = iFirst;
        }
        
    firstChild = static_cast<ChspsDomNode*>( iCurrent->ChildNodes().First() );
        
    // Current node has childs left
    if (firstChild)
        {
        //Keep count of depth level
        PushL( *iCurrent );
        iCurrent = firstChild;
        }
    else //If current node has siblings left
        {
        sibling = NextSibling(*iCurrent);
        if(sibling)
            {
            iCurrent = sibling;    
            }
        else //Current node don't have any childs or siblings left
            {
            // Reverse the tree by moving up in hierarchy
            // Move up one level in hierarchy and check if siblings exists or we are in a 
            // root level
            TBool stop( EFalse );
            while( !stop && iDepthLevel.Count() )  
                {
                iCurrent = Pop();                   //Reach the previous level
                sibling = NextSibling( *iCurrent ); //Check if siblings exist
                if ( sibling && iDepthLevel.Count() )
                    {
                    iCurrent = sibling;
                    stop = ETrue;
                    }
                else                                //We didn't find any siblings
                    {
                    iCurrent = NULL;
                    }
                }    
            }
        }
    return iCurrent;
    }

// -----------------------------------------------------------------------------
// NextSibling Find next sibling.
// Returns NULL if there is no siblings left or there is no parent i.e this is a
// root node
// Returns: ChspsDomNode* Pointer to the node or NULL
//          
// -----------------------------------------------------------------------------
//

EXPORT_C ChspsDomNode* ChspsDomDepthIterator::NextSibling( ChspsDomNode& aNode )
    {
    ChspsDomNode* left = NULL;
    ChspsDomNode* parent = aNode.Parent();
    if ( parent )
        {
        ChspsDomList& nodeList = parent->ChildNodes();
        TInt currentIndex( nodeList.ItemIndex( aNode ) );
        TInt nextIndex( ++currentIndex );
        TInt length( nodeList.Length() );
        if ( nextIndex < length )  
            {
            left = static_cast<ChspsDomNode*>( nodeList.Item( nextIndex ) );
            }
        }
    return left;     
    }

// -----------------------------------------------------------------------------
// ChspsDomDepthIterator::PushL
// -----------------------------------------------------------------------------
//
void ChspsDomDepthIterator::PushL( ChspsDomNode& aNode )
    {
    iDepthLevel.AppendL( &aNode );
    }

// -----------------------------------------------------------------------------
// ChspsDomDepthIterator::Pop
// -----------------------------------------------------------------------------
//
ChspsDomNode* ChspsDomDepthIterator::Pop()
    {
    ChspsDomNode* pop = NULL;
    TInt count( iDepthLevel.Count() );
    if ( count )
        {
        pop = iDepthLevel[ count-1 ];
        iDepthLevel.Remove( count-1 );
        }
    return pop;    
    }
    
//  End of File  
