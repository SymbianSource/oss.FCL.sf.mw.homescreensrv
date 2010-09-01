/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/

#ifndef __MENUENGVISITOR_H__
#define __MENUENGVISITOR_H__

// INCLUDES

#include <e32base.h>

// FORWARD DECLARATION

class CMenuEngObject;

// CLASS DECLARATION

/**
* Menu Engine hierarchical visitor interface.
* Object tree depth-first traversal.
*/
NONSHARABLE_CLASS( MMenuEngVisitor )
    {

public:     // new methods

    /**
    * Enter folder.
    * @param aFolder Entering this folder.
    * @return ETrue to visit children of this folder. VisitLeaveL() will
    *         be called after children of this folder are visited.
    *         EFalse to skip children of this folder. VisitLeaveL() will
    *         be called next.
    */
    virtual TBool VisitEnterL( CMenuEngObject& /*aFolder*/ )    
        { return ETrue; };

    /**
    * Leave folder.
    * @param aFolder Leaving this folder.
    * @return ETrue to visit next sibling of this folder.
    *         EFalse to stop visiting items at this level.
    */
    virtual TBool VisitLeaveL( CMenuEngObject& /*aFolder*/ )    
        { return ETrue; };

    /**
    * Visit leaf node.
    * @param aLeaf Leaf node.
    * @return ETrue to visit next sibling of this leaf.
    *         EFalse to stop visiting items at this level.
    */
    virtual TBool VisitL( CMenuEngObject& /*aLeaf*/ )
        { return ETrue; };

    };

#endif // __MENUENGVISITOR_H__
