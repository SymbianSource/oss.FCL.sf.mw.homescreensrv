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
* Description:  XCFW tree node interface
*
*/



#ifndef MXCFWNODEINTERFACE_H
#define MXCFWNODEINTERFACE_H

//  INCLUDES
#include <s32std.h>

// FORWARD DECLARATIONS
class CGECOObjectBase;

// CLASS DECLARATION
/*
*
*  Node interface for accessing data and relative nodes
*
*  @lib XCFW.lib
*  @since Series 60 3.1
*/
class MXCFWNode
    {
    public:
    /**
    * Returns parent of this node
    * @since Series 60 3.1
    * @return Parent node
    */
    virtual MXCFWNode* Parent() = 0;

    /**
    * Returns first child of this node
    * @since Series 60 3.1
    * @return First child node
    */
    virtual MXCFWNode* FirstChild() = 0;

    /**
    * Returns last child of this node
    * @since Series 60 3.1
    * @return Last child node
    */
    virtual MXCFWNode* LastChild() = 0;

    /**
    * Returns next sibling of this node
    * @since Series 60 3.1
    * @return Next sibling node
    */
    virtual MXCFWNode* NextSibling() = 0;

    /**
    * Returns previous sibling of this node
    * @since Series 60 3.1
    * @return Previous sibling node
    */
    virtual MXCFWNode* PrevSibling() = 0;

    /**
    * Returns Object parser from this node
    * @since Series 60 3.1
    * @return Object parser instance
    */
    virtual CGECOObjectBase* Data() = 0;        
    };


#endif      // MXCFWNODEINTERFACE_H   
            
// End of File
