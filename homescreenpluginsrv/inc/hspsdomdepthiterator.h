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
* Description:  Depth iterator walks the dom tree by depth first.
*
*/



#ifndef hsps_DOM_DEPTH_ITERATOR_H
#define hsps_DOM_DEPTH_ITERATOR_H

//  INCLUDES
#include <e32base.h>
#include "mhspsdomiterator.h"


// FORWARD DECLARATIONS
class ChspsDomNode;


// CLASS DECLARATION

/**
 * Depth iterator walks through the dom tree using the depth first
 * principle.
 *
 * @lib hspsdomdocument.lib
 * @since S60 5.0
 * @ingroup group_hspsdom
*/
class ChspsDomDepthIterator: public CBase, public MhspsDomIterator
    {
   public:  // Destructor
       /**
       * Symbian constructor
       * 
       * @since S60 5.0 
       * @param aRootNode   Root node.
       */        
       IMPORT_C static ChspsDomDepthIterator* NewL( ChspsDomNode& aRootNode );

       /**
        * Destructor.
        */
       IMPORT_C virtual ~ChspsDomDepthIterator();

    public: // Adding and removing    	
    	/**
        * From MhspsDomIterator.
        */
        IMPORT_C ChspsDomNode* First();
    	
    	/**
        * From MhspsDomIterator.
        */
        IMPORT_C ChspsDomNode* NextL();
        
        /**
        * From MhspsDomIterator.
        */
        IMPORT_C ChspsDomNode* NextSibling( ChspsDomNode& aNode );
        
   private:
        /**
        * C++ default constructor
        */
        ChspsDomDepthIterator( ChspsDomNode& aRootNode );
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(); 
        
        /**
        * Push node to stack.
        * @param aNode Node to push 
        */
        void PushL( ChspsDomNode& aNode );
        
        /**
        * Pop node from the stack.
        * @return Pointer to node
        */
        ChspsDomNode* Pop();
        
   private:
        // Follow tree hierarchy with stack
        RPointerArray<ChspsDomNode> iDepthLevel;
        // First node
        ChspsDomNode* iFirst;
        //Current node
        ChspsDomNode* iCurrent;
                   
    };

#endif      // hsps_DOM_DEPTH_ITERATOR_H  
            
// End of File
