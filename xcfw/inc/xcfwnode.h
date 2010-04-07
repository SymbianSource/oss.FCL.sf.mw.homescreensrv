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
* Description:  Class definition of XCFW tree node 
*
*/



#ifndef CXCFWNODE_H
#define CXCFWNODE_H

//  INCLUDES
#include <s32std.h>
#include "xcfwnodeinterface.h"
#include "gecoobjectbase.h"

// FORWARD DECLARATIONS

/**
*  Node interface for setting data and relative nodes
*  This interface is not exported, CXCFWTree is the only user. 
*
*  @lib XCFW_hs_hs.lib
*  @since Series 60 3.1
*/
class CXCFWNode : public CBase, public MXCFWNode
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aData Node data object
        */
        static CXCFWNode* NewL( CGECOObjectBase* aData );

        /**
        * Destructor.
        */
        virtual ~CXCFWNode();

    public: // New functions
        
        /**
        * Sets parent for this node
        * @since Series 60 3.1
        * @param aNode New parent for this node
        */
        void SetParent( MXCFWNode* aNode );

        /**
        * Sets first child node for this node
        * @since Series 60 3.1
        * @param aNode New first child for this node
        */
        void SetFirstChild( MXCFWNode* aNode );

        /**
        * Sets last child for this node
        * @since Series 60 3.1
        * @param aNode New last child for this node
        */
        void SetLastChild( MXCFWNode* aNode );

        /**
        * Sets next sibling for this node
        * @since Series 60 3.1
        * @param aNode New next sibling for this node
        */
        void SetNextSibling( MXCFWNode* aNode );

        /**
        * Sets previous sibling for this node
        * @since Series 60 3.1
        * @param aNode New previous sibling for this node
        */
        void SetPrevSibling( MXCFWNode* aNode );
        
        /**
        * Sets content to this node
        * @since Series 60 3.1
        * @param aData New content object to be stored in this node.
        *  Note: Old data pointer should be deleted first.
        */
        void SetData( CGECOObjectBase* aData );

    public: // Functions from base classes
        
        /**
        * From MXCFWNode. Returns parent node for this node
        */
        IMPORT_C MXCFWNode* Parent();

        /**
        * From MXCFWNode. Returns first child node for this node
        */
        IMPORT_C MXCFWNode* FirstChild();

        /**
        * From MXCFWNode. Returns last child node for this node
        */
        IMPORT_C MXCFWNode* LastChild();

        /**
        * From MXCFWNode. Returns next sibling node for this node
        */
        IMPORT_C MXCFWNode* NextSibling();

        /**
        * From MXCFWNode. Returns previous sibling node for this node
        */
        IMPORT_C MXCFWNode* PrevSibling();

        /**
        * From MXCFWNode. Returns Data object for this node
        */
        IMPORT_C CGECOObjectBase* Data();        
        
    private:

        /**
        * C++ default constructor.
        */
        CXCFWNode();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( CGECOObjectBase* aData );

    private:    // Data
        //Content object parser. Owned
        CGECOObjectBase*    iData;
        
        //reference to parent node. Not owned
        MXCFWNode*            iParent;

        //reference to first child node. Not owned
        MXCFWNode*            iFirstChild;

        //reference to last child node. Not owned
        MXCFWNode*            iLastChild;

        //reference to previous sibling node. Not owned
        MXCFWNode*            iPrevSibling;

        //reference to next sibling node. Not owned
        MXCFWNode*            iNextSibling;

    };

#endif      // CXCFWNODE_H   
            
// End of File
