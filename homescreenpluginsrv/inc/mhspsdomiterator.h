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
* Description:  Interface class for dom iterators
*
*/



#ifndef Mhsps_DOM_ITERATOR_H
#define Mhsps_DOM_ITERATOR_H


// FORWARD DECLARATIONS
class ChspsDomNode;


// CLASS DECLARATION

/**
*  Mixin class to let us define different iterators for 
*  different traversal policies.
*
*  @lib hspsdomdocument.lib
*  @since S60 5.0
*  @ingroup group_hspsdom
*/
class MhspsDomIterator
    {
   public:  // Destructor
        
        /**
        * Destructor.
        * 
        * @since S60 5.0
        */
        virtual ~MhspsDomIterator(){}

    public: // Adding and removing
    	
    	/**
        * Get first i.e root node.
        * 
        * @since S60 5.0
        */
        virtual ChspsDomNode* First()=0;
    	
        /**
        * Get next node.
        * 
        * @since S60 5.0
        */
        virtual ChspsDomNode* NextL()=0;        
        
        /**
         * Get next sibling.
         * 
         * @since S60 5.0
        */
        virtual ChspsDomNode* NextSibling( ChspsDomNode& aNode )=0;
        
    };

#endif      // Mhsps_DOM_ITERATOR_H  
            
// End of File
